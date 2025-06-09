//
// Created by v.ponomarenko on 09.06.2025.
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


static int _decode_lte_pdcp_dl_cipher_data_pdu_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PDCP_DL_Cipher_Data_PDU: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num Subpkts") != j.end() && j["Num Subpkts"].is_number()) {
        n_subpkt = j["Num Subpkts"].get<int>();
    } else {
        LOGD("LTE_PDCP_DL_Cipher_Data_PDU: Missing or invalid 'Num Subpkts'");
        j["error"] = "Missing Num Subpkts in main header";
        return 0;
    }

//    LOGD("Decoding LTE_PDCP_DL_Cipher_Data_PDU with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LtePdcpDlCipherDataPdu_SubpktHeader_v1,
                                                     ARRAY_SIZE(LtePdcpDlCipherDataPdu_SubpktHeader_v1, Fmt),
                                                     b, offset, length, subpkt_j);

                int subpkt_id = -1, subpkt_payload_ver = -1, subpkt_size = 0;
                bool header_ok = false;

                if (header_consumed > 0 &&
                    subpkt_j.find("Subpacket ID") != subpkt_j.end() && subpkt_j["Subpacket ID"].is_number() &&
                    subpkt_j.find("Subpacket Version") != subpkt_j.end() && subpkt_j["Subpacket Version"].is_number() &&
                    subpkt_j.find("Subpacket Size") != subpkt_j.end() && subpkt_j["Subpacket Size"].is_number()) {
                    subpkt_id = subpkt_j["Subpacket ID"].get<int>();
                    subpkt_payload_ver = subpkt_j["Subpacket Version"].get<int>();
                    subpkt_size = subpkt_j["Subpacket Size"].get<int>();
                    if (subpkt_size >= header_consumed && start_subpkt + subpkt_size <= start + length) {
                        header_ok = true;
                    }
                }

                if (!header_ok) {
                    LOGD("LTE_PDCP_DL_Cipher_Data_PDU: Error decoding subpacket %d header or missing fields/invalid size.", i);
                    if (subpkt_j.find("Subpacket Size") != subpkt_j.end() && subpkt_j["Subpacket Size"].is_number()) {
                        int skip_size_check = subpkt_j["Subpacket Size"].get<int>();
                        if (start_subpkt + skip_size_check <= start + length && skip_size_check > 0) {
                            offset = start_subpkt + skip_size_check;
                        } else { return offset - start; }
                    } else { return offset - start; }
                    continue;
                }
                offset += header_consumed;
                int payload_offset_start = offset;
                int payload_consumed_total = 0;

                if (subpkt_id == 195) {
                    const Fmt* subpkt_payload_fmt_array = nullptr;
                    size_t subpkt_payload_fmt_size = 0;
                    const Fmt* pdu_data_fmt_array = nullptr;
                    size_t pdu_data_fmt_size = 0;

                    if (subpkt_payload_ver == 1) {
                        subpkt_payload_fmt_array = LtePdcpDlCipherDataPdu_SubpktPayload_v1;
                        subpkt_payload_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_SubpktPayload_v1, Fmt);
                        pdu_data_fmt_array = LtePdcpDlCipherDataPdu_Data_v1;
                        pdu_data_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_Data_v1, Fmt);
                    } else if (subpkt_payload_ver == 3) {
                        subpkt_payload_fmt_array = LtePdcpDlCipherDataPdu_SubpktPayload_v3;
                        subpkt_payload_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_SubpktPayload_v3, Fmt);
                        pdu_data_fmt_array = LtePdcpDlCipherDataPdu_Data_v3;
                        pdu_data_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_Data_v3, Fmt);
                    } else if (subpkt_payload_ver == 24) {
                        subpkt_payload_fmt_array = LtePdcpDlCipherDataPdu_SubpktPayload_v24;
                        subpkt_payload_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_SubpktPayload_v24, Fmt);
                        pdu_data_fmt_array = LtePdcpDlCipherDataPdu_Data_v24;
                        pdu_data_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_Data_v24, Fmt);
                    } else if (subpkt_payload_ver == 40) {
                        subpkt_payload_fmt_array = LtePdcpDlCipherDataPdu_SubpktPayload_v40;
                        subpkt_payload_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_SubpktPayload_v40, Fmt);
                        pdu_data_fmt_array = LtePdcpDlCipherDataPdu_Data_v40;
                        pdu_data_fmt_size = ARRAY_SIZE(LtePdcpDlCipherDataPdu_Data_v40, Fmt);
                    } else {
                        LOGD("(MI)Unknown LTE PDCP DL Cipher Data PDU subpkt version for ID 195: %d", subpkt_payload_ver);
                    }

                    if (subpkt_payload_fmt_array && pdu_data_fmt_array) {
                        payload_consumed_total = _decode_by_fmt(subpkt_payload_fmt_array, subpkt_payload_fmt_size,
                                                                b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset = payload_offset_start + payload_consumed_total;

                        const char* srb_algo_name = search_name(ValueNameCipherAlgo, ARRAY_SIZE(ValueNameCipherAlgo, ValueName), subpkt_j["SRB Cipher Algorithm"]);
                        subpkt_j["SRB Cipher Algorithm"] = (srb_algo_name ? srb_algo_name : "(MI)Unknown");
                        const char* drb_algo_name = search_name(ValueNameCipherAlgo, ARRAY_SIZE(ValueNameCipherAlgo, ValueName), subpkt_j["DRB Cipher Algorithm"]);
                        subpkt_j["DRB Cipher Algorithm"] = (drb_algo_name ? drb_algo_name : "(MI)Unknown");

                        int iNumPDUs = 0;
                        if (subpkt_j.find("Num PDUs") != subpkt_j.end() && subpkt_j["Num PDUs"].is_number()) {
                            iNumPDUs = subpkt_j["Num PDUs"].get<int>();
                        }

                        json pdu_list_j = json::array();
                        int current_pdu_data_offset = offset;

                        for (int pdu_idx = 0; pdu_idx < iNumPDUs; ++pdu_idx) {
                            json pdu_item_j;
                            int pdu_data_consumed = _decode_by_fmt(pdu_data_fmt_array, pdu_data_fmt_size,
                                                                   b, current_pdu_data_offset, length - (current_pdu_data_offset - start), pdu_item_j);

                            if (pdu_data_consumed == 0) {
                                LOGD("Error decoding PDU data item %d for subpacket ID 195, version %d", pdu_idx, subpkt_payload_ver);
                                break;
                            }
                            current_pdu_data_offset += pdu_data_consumed;

                            int temp_cfg_idx = pdu_item_j["Cfg Idx"];
                            pdu_item_j["Cfg Idx"] = temp_cfg_idx & 63;
                            pdu_item_j["Mode"] = (temp_cfg_idx >> 6) & 1;
                            const char* mode_name = search_name(ValueNamePdcpCipherDataPduMode, ARRAY_SIZE(ValueNamePdcpCipherDataPduMode, ValueName), pdu_item_j["Mode"]);
                            pdu_item_j["Mode"] = (mode_name ? mode_name : "(MI)Unknown");
                            pdu_item_j["SN Length"] = (temp_cfg_idx >> 7) & 3;
                            const char* sn_len_name = search_name(ValueNamePdcpSNLength, ARRAY_SIZE(ValueNamePdcpSNLength, ValueName), pdu_item_j["SN Length"]);
                            pdu_item_j["SN Length"] = (sn_len_name ? sn_len_name : "(MI)Unknown");
                            pdu_item_j["Bearer ID"] = (temp_cfg_idx >> 9) & 31;
                            pdu_item_j["Valid PDU"] = (temp_cfg_idx >> 14) & 1;
                            const char* valid_pdu_name = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), pdu_item_j["Valid PDU"]);
                            pdu_item_j["Valid PDU"] = (valid_pdu_name ? valid_pdu_name : "(MI)Unknown");

                            int temp_sub_fn = pdu_item_j["Sub FN"];
                            pdu_item_j["Sub FN"] = temp_sub_fn & 15;
                            pdu_item_j["Sys FN"] = (temp_sub_fn >> 4) & 1023;

                            if (subpkt_payload_ver == 40) {
                                pdu_item_j["Reserved FN"] = (temp_sub_fn >> 14) & 3;
                            } else {
                                if (pdu_item_j.find("SN") != pdu_item_j.end() && pdu_item_j["SN"].is_number()){
                                    pdu_item_j["SN"] = pdu_item_j["SN"].get<int>() & 4095;
                                }
                            }
                            pdu_list_j.push_back(pdu_item_j);

                            int iLoggedBytes = 0;
                            if(pdu_item_j.find("Logged Bytes") != pdu_item_j.end() && pdu_item_j["Logged Bytes"].is_number()){
                                iLoggedBytes = pdu_item_j["Logged Bytes"].get<int>();
                            }
                            if (iLoggedBytes > 0 && current_pdu_data_offset + iLoggedBytes <= start + length) {
                                current_pdu_data_offset += iLoggedBytes;
                            } else if (iLoggedBytes < 0) {
                                LOGD("PDCP DL Cipher PDU: Logged Bytes is negative (%d)", iLoggedBytes);
                            } else if (current_pdu_data_offset + iLoggedBytes > start + length) {
                                LOGD("PDCP DL Cipher PDU: Not enough data to skip Logged Bytes (%d). Remaining: %zu", iLoggedBytes, (start + length) - current_pdu_data_offset);
                                current_pdu_data_offset = start + length;
                                break;
                            }
                        }
                        subpkt_j["PDCPDL CIPH DATA"] = pdu_list_j;
                        offset = current_pdu_data_offset;
                    } else {
                        int remaining_payload_size = subpkt_size - header_consumed;
                        if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                            offset += remaining_payload_size;
                        }
                    }
                } else {
                    LOGD("(MI)Unknown LTE PDCP DL Cipher Data PDU Subpacket ID: 0x%x", subpkt_id);
                    int remaining_payload_size = subpkt_size - header_consumed;
                    if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                        offset += remaining_payload_size;
                    }
                }

                int total_consumed_by_subpkt = offset - start_subpkt;
                if (total_consumed_by_subpkt != subpkt_size) {
                    LOGD("Subpacket ID %d Ver %d: Consumed %d, SubPktSize %d. Adjusting offset.", subpkt_id, subpkt_payload_ver, total_consumed_by_subpkt, subpkt_size);
                    offset = start_subpkt + subpkt_size;
                    if (offset > start + length) {
                        LOGD("Error: Adjusted offset for subpacket ID %d Ver %d exceeds buffer length. Stopping.", subpkt_id, subpkt_payload_ver);
                        return offset - start;
                    }
                }
                subpackets_j.push_back(subpkt_j);
            }
            j["Subpackets"] = subpackets_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PDCP DL Cipher Data PDU Packet version: %d", pkt_ver);
            return 0;
    }
}