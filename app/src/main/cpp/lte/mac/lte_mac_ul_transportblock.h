//
// Created by Vladimir Ponomarenko on 18.05.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_mac_ul_transportblock_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_MAC_UL_Transport_Block: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num SubPkt") != j.end() && j["Num SubPkt"].is_number()) {
        n_subpkt = j["Num SubPkt"].get<int>();
    } else {
        LOGD("LTE_MAC_UL_Transport_Block: Missing or invalid 'Num SubPkt'");
        j["error"] = "Missing Num SubPkt in main header";
        return 0;
    }

    //LOGD("Decoding LTE_MAC_UL_Transport_Block with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            bool break_outer_loop = false;
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LteMacULTransportBlock_SubpktHeaderFmt,
                                                     ARRAY_SIZE(LteMacULTransportBlock_SubpktHeaderFmt, Fmt),
                                                     b, offset, length, subpkt_j);

                int subpkt_id = -1, subpkt_payload_ver = -1, subpkt_size = 0, subpkt_nsample = 0;
                bool header_ok = false;

                if (header_consumed > 0 &&
                    subpkt_j.find("SubPacket ID") != subpkt_j.end() && subpkt_j["SubPacket ID"].is_number() &&
                    subpkt_j.find("Version") != subpkt_j.end() && subpkt_j["Version"].is_number() &&
                    subpkt_j.find("SubPacket Size") != subpkt_j.end() && subpkt_j["SubPacket Size"].is_number() &&
                    subpkt_j.find("Num Samples") != subpkt_j.end() && subpkt_j["Num Samples"].is_number()) {
                    subpkt_id = subpkt_j["SubPacket ID"].get<int>();
                    subpkt_payload_ver = subpkt_j["Version"].get<int>();
                    subpkt_size = subpkt_j["SubPacket Size"].get<int>();
                    subpkt_nsample = subpkt_j["Num Samples"].get<int>();
                    if (subpkt_size >= header_consumed && start_subpkt + subpkt_size <= start + length) {
                        header_ok = true;
                    }
                }

                if (!header_ok) {
                    LOGD("LTE_MAC_UL_Transport_Block: Error decoding subpacket %d header or missing fields/invalid size.", i);
                    if (subpkt_j.find("SubPacket Size") != subpkt_j.end() && subpkt_j["SubPacket Size"].is_number()) {
                        int skip_size_check = subpkt_j["SubPacket Size"].get<int>();
                        if (start_subpkt + skip_size_check <= start + length && skip_size_check > 0) {
                            offset = start_subpkt + skip_size_check;
                        } else { return offset - start;}
                    } else { return offset - start; }
                    continue;
                }
                offset += header_consumed;

                const char* subpkt_type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                           ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                           subpkt_id);
                subpkt_j["SubPacket ID"] = (subpkt_type_name ? subpkt_type_name : ("Unknown (" + std::to_string(subpkt_id) + ")"));

                if (subpkt_type_name == nullptr) {
                    LOGD("(MI)Unknown LTE MAC Uplink Transport Block Subpacket ID: 0x%x", subpkt_id);
                    int payload_to_skip = subpkt_size - header_consumed;
                    if (payload_to_skip > 0 && offset + payload_to_skip <= start + length) {
                        offset += payload_to_skip;
                    } else if (payload_to_skip < 0) {
                        LOGD("Error: payload_to_skip is negative for subpacket ID %d", subpkt_id);
                    }
                } else {
                    json samples_j = json::array();
                    int current_payload_offset_for_subpacket = offset;


                    const Fmt* sample_fmt_array = nullptr;
                    size_t sample_fmt_size = 0;
                    if (subpkt_payload_ver == 1) {
                        sample_fmt_array = LteMacULTransportBlock_SubpktV1_SampleFmt;
                        sample_fmt_size = ARRAY_SIZE(LteMacULTransportBlock_SubpktV1_SampleFmt, Fmt);
                    } else if (subpkt_payload_ver == 2) {
                        sample_fmt_array = LteMacULTransportBlock_SubpktV2_SampleFmt;
                        sample_fmt_size = ARRAY_SIZE(LteMacULTransportBlock_SubpktV2_SampleFmt, Fmt);
                    } else {
                        LOGD("(MI)Unknown LTE MAC UL TB Subpacket Payload Version: %d for SubPkt ID %d", subpkt_payload_ver, subpkt_id);
                    }

                    if (sample_fmt_array) {
                        for (int k = 0; k < subpkt_nsample; ++k) {
                            json sample_j;
                            int sample_base_offset = current_payload_offset_for_subpacket;
                            int sample_header_part_consumed = _decode_by_fmt(sample_fmt_array, sample_fmt_size,
                                                                             b, sample_base_offset, length - (sample_base_offset - start), sample_j);

                            if (sample_header_part_consumed == 0) {
                                LOGD("Error decoding sample %d for subpacket ID %d, version %d", k, subpkt_id, subpkt_payload_ver);
                                offset = start_subpkt + subpkt_size;
                                break_outer_loop = true;
                                break;
                            }
                            current_payload_offset_for_subpacket += sample_header_part_consumed;


                            if (sample_j.find("BSR event") != sample_j.end() && sample_j["BSR event"].is_number()) {
                                const char* bsr_event_name = search_name(BSREvent, ARRAY_SIZE(BSREvent, ValueName), sample_j["BSR event"]);
                                sample_j["BSR event"] = (bsr_event_name ? bsr_event_name : "(MI)Unknown");
                            }
                            if (sample_j.find("BSR trig") != sample_j.end() && sample_j["BSR trig"].is_number()) {
                                const char* bsr_trig_name = search_name(BSRTrig, ARRAY_SIZE(BSRTrig, ValueName), sample_j["BSR trig"]);
                                sample_j["BSR trig"] = (bsr_trig_name ? bsr_trig_name : "(MI)Unknown");
                            }

                            int rnti_type_val_for_hdr_lookup = -1;
                            if (sample_j.find("RNTI Type") != sample_j.end() && sample_j["RNTI Type"].is_number()) {
                                rnti_type_val_for_hdr_lookup = sample_j["RNTI Type"];
                                const char* rnti_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), rnti_type_val_for_hdr_lookup);
                                sample_j["RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");
                            }
                            if (sample_j.find("Sub-FN") != sample_j.end() && sample_j["Sub-FN"].is_number()) {
                                int temp_sfn = sample_j["Sub-FN"];
                                sample_j["Sub-FN"] = temp_sfn & 15;
                                sample_j["SFN"] = (temp_sfn >> 4);
                            }

                            int hdr_len_from_sample = 0;
                            if (sample_j.find("HDR LEN") != sample_j.end() && sample_j["HDR LEN"].is_number()) {
                                hdr_len_from_sample = sample_j["HDR LEN"];
                            }

                            int mac_hdrs_and_ce_total_consumed = 0;

                            if (rnti_type_val_for_hdr_lookup != 0 && rnti_type_val_for_hdr_lookup != -1) {
                                if (current_payload_offset_for_subpacket + hdr_len_from_sample <= start + length) {
                                    mac_hdrs_and_ce_total_consumed = hdr_len_from_sample;
                                } else {
                                    LOGD("Error: Not enough data to skip HDR LEN (%d bytes) for RNTI type %d", hdr_len_from_sample, rnti_type_val_for_hdr_lookup);
                                    mac_hdrs_and_ce_total_consumed = length - (current_payload_offset_for_subpacket - start);
                                    if (mac_hdrs_and_ce_total_consumed < 0) mac_hdrs_and_ce_total_consumed = 0;
                                }
                            } else if (rnti_type_val_for_hdr_lookup == 0) {
                                std::vector<json> mac_hdrs_tmp_list_vec;
                                json mac_hdrs_ce_list_j_array = json::array();
                                int mac_header_parser_offset = current_payload_offset_for_subpacket;

                                while (mac_hdrs_and_ce_total_consumed < hdr_len_from_sample) {
                                    json mac_hdr_j_item;
                                    int mac_hdr_item_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_Hdr,
                                                                               ARRAY_SIZE(LteMacULTransportBlock_Mac_Hdr, Fmt),
                                                                               b, mac_header_parser_offset, length - (mac_header_parser_offset - start), mac_hdr_j_item);

                                    if (mac_hdr_item_consumed == 0) break;
                                    mac_header_parser_offset += mac_hdr_item_consumed;
                                    mac_hdrs_and_ce_total_consumed += mac_hdr_item_consumed;

                                    uint32_t header_field = mac_hdr_j_item["Header Field"];
                                    uint32_t iF2 = (header_field >> 6) & 1;
                                    uint32_t iE = (header_field >> 5) & 1;
                                    uint32_t iLCID_val = header_field & 31;

                                    mac_hdr_j_item["LC ID"] = iLCID_val;
                                    const char* lcid_name = search_name(LteMacULTransportBlock_Mac_Hdr_LCId, ARRAY_SIZE(LteMacULTransportBlock_Mac_Hdr_LCId, ValueName), iLCID_val);
                                    mac_hdr_j_item["LC ID_str"] = (lcid_name ? lcid_name : "(MI)Unknown");
                                    mac_hdr_j_item.erase("Header Field");

                                    int iSDULen = -1;
                                    if (iLCID_val >= 0 && iLCID_val <= 10 && iE != 0) {
                                        int l_field_item_consumed = 0;
                                        if (iF2 == 0) {
                                            json mac_hdr_L1_j;
                                            l_field_item_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_Hdr_L1, ARRAY_SIZE(LteMacULTransportBlock_Mac_Hdr_L1, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_L1_j);
                                            mac_header_parser_offset += l_field_item_consumed; mac_hdrs_and_ce_total_consumed += l_field_item_consumed;
                                            uint32_t l1_field = mac_hdr_L1_j["L1 Field"];
                                            if ((l1_field >> 7) == 1) {
                                                json mac_hdr_L2_j;
                                                int l2_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_Hdr_L2, ARRAY_SIZE(LteMacULTransportBlock_Mac_Hdr_L2, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_L2_j);
                                                mac_header_parser_offset += l2_consumed; mac_hdrs_and_ce_total_consumed += l2_consumed;
                                                iSDULen = (l1_field & 0x7f) * 0x100 + mac_hdr_L2_j["L2 Field"].get<uint32_t>();
                                            } else { iSDULen = l1_field & 0x7f; }
                                        } else {
                                            json mac_hdr_L1_j, mac_hdr_L2_j;
                                            int l1_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_Hdr_L1, ARRAY_SIZE(LteMacULTransportBlock_Mac_Hdr_L1, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_L1_j);
                                            mac_header_parser_offset += l1_consumed; mac_hdrs_and_ce_total_consumed += l1_consumed;
                                            int l2_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_Hdr_L2, ARRAY_SIZE(LteMacULTransportBlock_Mac_Hdr_L2, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_L2_j);
                                            mac_header_parser_offset += l2_consumed; mac_hdrs_and_ce_total_consumed += l2_consumed;
                                            iSDULen = mac_hdr_L1_j["L1 Field"].get<uint32_t>() * 0x100 + mac_hdr_L2_j["L2 Field"].get<uint32_t>();
                                        }
                                        mac_hdr_j_item["Len"] = iSDULen;
                                    }
                                    mac_hdrs_tmp_list_vec.push_back(mac_hdr_j_item);
                                    if (iE == 0) break;
                                    if (mac_hdrs_and_ce_total_consumed >= hdr_len_from_sample) break;
                                }

                                for (json& mac_hdr_ce_j_item : mac_hdrs_tmp_list_vec) {
                                    uint32_t lcid_val_ce = mac_hdr_ce_j_item["LC ID"];
                                    int ce_sdu_len = -1;
                                    int ce_data_consumed = 0;

                                    if (lcid_val_ce == 30) {
                                        ce_sdu_len = 3;
                                        if (mac_hdrs_and_ce_total_consumed < hdr_len_from_sample) {
                                            ce_data_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_CE_L_BSR, ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_L_BSR, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_ce_j_item);
                                            if(ce_data_consumed > 0){
                                                uint32_t f1 = mac_hdr_ce_j_item["L-BSR Field 1"];
                                                uint32_t f2 = mac_hdr_ce_j_item["L-BSR Field 2"];
                                                uint32_t f3 = mac_hdr_ce_j_item["L-BSR Field 3"];
                                                uint32_t i0 = (f1 >> 2);
                                                uint32_t i1 = ((f1 << 4) & 0x30) + (f2 >> 4);
                                                uint32_t i2 = ((f2 << 2) & 0x3c) + (f3 >> 6);
                                                uint32_t i3 = f3 & 0x3f;
                                                mac_hdr_ce_j_item["BSR LCG 0"] = i0; mac_hdr_ce_j_item["BSR LCG 0 (bytes)"] = (i0 < ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue,int)) ? LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue[i0] : -1;
                                                mac_hdr_ce_j_item["BSR LCG 1"] = i1; mac_hdr_ce_j_item["BSR LCG 1 (bytes)"] = (i1 < ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue,int)) ? LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue[i1] : -1;
                                                mac_hdr_ce_j_item["BSR LCG 2"] = i2; mac_hdr_ce_j_item["BSR LCG 2 (bytes)"] = (i2 < ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue,int)) ? LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue[i2] : -1;
                                                mac_hdr_ce_j_item["BSR LCG 3"] = i3; mac_hdr_ce_j_item["BSR LCG 3 (bytes)"] = (i3 < ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue,int)) ? LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue[i3] : -1;
                                                mac_hdr_ce_j_item.erase("L-BSR Field 1"); mac_hdr_ce_j_item.erase("L-BSR Field 2"); mac_hdr_ce_j_item.erase("L-BSR Field 3");
                                            }
                                        }
                                    } else if (lcid_val_ce == 29 || lcid_val_ce == 28) {
                                        ce_sdu_len = 1;
                                        if (mac_hdrs_and_ce_total_consumed < hdr_len_from_sample) {
                                            ce_data_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_CE_S_T_BSR, ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_S_T_BSR, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_ce_j_item);
                                            if(ce_data_consumed > 0 && mac_hdr_ce_j_item.find("S/T-BSR Field") != mac_hdr_ce_j_item.end()){
                                                uint32_t st_field = mac_hdr_ce_j_item["S/T-BSR Field"];
                                                uint32_t iLCGId = (st_field >> 6);
                                                uint32_t iIndex = st_field & 0x3f;
                                                std::string lcg_field_name = "BSR LCG " + std::to_string(iLCGId);
                                                std::string lcg_bytes_field_name = lcg_field_name + " (bytes)";
                                                mac_hdr_ce_j_item[lcg_field_name] = iIndex;
                                                mac_hdr_ce_j_item[lcg_bytes_field_name] = (iIndex < ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue,int)) ? LteMacULTransportBlock_Mac_CE_BSR_BufferSizeValue[iIndex] : -1;
                                                mac_hdr_ce_j_item.erase("S/T-BSR Field");
                                            }
                                        }
                                    } else if (lcid_val_ce == 27) {
                                        ce_sdu_len = 2;
                                        if (mac_hdrs_and_ce_total_consumed < hdr_len_from_sample) {
                                            ce_data_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_CE_C_RNTI, ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_C_RNTI, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_ce_j_item);
                                        }
                                    } else if (lcid_val_ce == 26) {
                                        ce_sdu_len = 1;
                                        if (mac_hdrs_and_ce_total_consumed < hdr_len_from_sample) {
                                            ce_data_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_CE_PHR, ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_PHR, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_ce_j_item);
                                            if(ce_data_consumed > 0 && mac_hdr_ce_j_item.find("PHR Field") != mac_hdr_ce_j_item.end()){
                                                mac_hdr_ce_j_item["PHR Ind"] = mac_hdr_ce_j_item["PHR Field"].get<uint32_t>() & 0x3f;
                                                mac_hdr_ce_j_item.erase("PHR Field");
                                            }
                                        }
                                    } else if (lcid_val_ce == 20) {
                                        ce_sdu_len = 2;
                                        if (mac_hdrs_and_ce_total_consumed < hdr_len_from_sample) {
                                            ce_data_consumed = _decode_by_fmt(LteMacULTransportBlock_Mac_CE_RBRQ, ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_RBRQ, Fmt), b, mac_header_parser_offset, length-(mac_header_parser_offset-start), mac_hdr_ce_j_item);
                                            if(ce_data_consumed > 0){
                                                uint32_t f1 = mac_hdr_ce_j_item["RBRQ Field 1"];
                                                uint32_t f2 = mac_hdr_ce_j_item["RBRQ Field 2"];
                                                mac_hdr_ce_j_item["LCID (RBRQ)"] = (f1 >> 4);
                                                mac_hdr_ce_j_item["UL/DL"] = (f1 >> 3) & 1;
                                                const char* ul_dl_name = search_name(LteMacULTransportBlock_Mac_CE_RBRQ_ULorDL, ARRAY_SIZE(LteMacULTransportBlock_Mac_CE_RBRQ_ULorDL, ValueName), mac_hdr_ce_j_item["UL/DL"]);
                                                mac_hdr_ce_j_item["UL/DL_str"] = (ul_dl_name ? ul_dl_name : "(MI)Unknown");
                                                mac_hdr_ce_j_item["Bit Rate"] = ((f1 & 0x7) << 3) | (f2 >> 5);
                                                mac_hdr_ce_j_item["Bit Rate Multiplier"] = (f2 >> 4) & 1;
                                                mac_hdr_ce_j_item.erase("RBRQ Field 1"); mac_hdr_ce_j_item.erase("RBRQ Field 2");
                                            }
                                        }
                                    }

                                    if (ce_data_consumed > 0) {
                                        mac_header_parser_offset += ce_data_consumed;
                                        mac_hdrs_and_ce_total_consumed += ce_data_consumed;
                                    }
                                    if (lcid_val_ce > 10 && lcid_val_ce < 26 && ce_sdu_len != -1) {
                                        if (mac_hdr_ce_j_item.find("Len") == mac_hdr_ce_j_item.end()){
                                            mac_hdr_ce_j_item["Len"] = ce_sdu_len;
                                        }
                                    }
                                    mac_hdrs_ce_list_j_array.push_back(mac_hdr_ce_j_item);
                                    if (mac_hdrs_and_ce_total_consumed >= hdr_len_from_sample) break;
                                }
                                sample_j["Mac Hdr + CE"] = mac_hdrs_ce_list_j_array;

                                int remaining_hdr_to_skip = hdr_len_from_sample - mac_hdrs_and_ce_total_consumed;
                                if (remaining_hdr_to_skip > 0) {
                                    if (mac_header_parser_offset + remaining_hdr_to_skip <= start + length) {
                                        mac_hdrs_and_ce_total_consumed += remaining_hdr_to_skip;
                                    } else {
                                        LOGD("Error: Not enough data to skip %d remaining HDR bytes for C-RNTI CE", remaining_hdr_to_skip);
                                        mac_hdrs_and_ce_total_consumed += (length - (mac_header_parser_offset - start));
                                        if (mac_hdrs_and_ce_total_consumed < hdr_len_from_sample) mac_hdrs_and_ce_total_consumed = hdr_len_from_sample;
                                    }
                                } else if (remaining_hdr_to_skip < 0) {
                                    LOGD("Error: Consumed more MAC HDR+CE bytes (%d) than HDR LEN (%d)", mac_hdrs_and_ce_total_consumed, hdr_len_from_sample);
                                }
                            }
                            current_payload_offset_for_subpacket += mac_hdrs_and_ce_total_consumed;
                            samples_j.push_back(sample_j);

                            int nsample_byte_count_check = current_payload_offset_for_subpacket - offset;

                            int subpacket_payload_size = subpkt_size - header_consumed;
                            if ( (subpacket_payload_size > 0) &&
                                 (nsample_byte_count_check > subpacket_payload_size - 5) &&
                                 (k < subpkt_nsample -1)
                                    ) {
                                LOGD("UL MAC TB: Insufficient bits for remaining samples based on Python logic. Sample %d/%d. Consumed in payload: %d, SubPktPayloadSize: %d", k+1, subpkt_nsample, nsample_byte_count_check, subpacket_payload_size);
                                break_outer_loop = true;
                                break;
                            }

                        }
                        subpkt_j["Samples"] = samples_j;
                        offset = current_payload_offset_for_subpacket;
                    }
                }

                int total_consumed_by_subpkt = offset - start_subpkt;
                if (total_consumed_by_subpkt != subpkt_size) {
                   // LOGD("Subpacket ID %d Ver %d: Consumed %d, SubPktSize %d. Adjusting offset.", subpkt_id, subpkt_payload_ver, total_consumed_by_subpkt, subpkt_size);
                    offset = start_subpkt + subpkt_size;
                    if (offset > start + length) {
                        LOGD("Error: Adjusted offset for subpacket ID %d Ver %d exceeds buffer length. Stopping.", subpkt_id, subpkt_payload_ver);
                        return offset - start;
                    }
                }
                subpackets_j.push_back(subpkt_j);
                if (break_outer_loop) break;
            }
            j["Subpackets"] = subpackets_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE MAC Uplink Transport Block packet version: 0x%x", pkt_ver);
            return 0;
    }
}