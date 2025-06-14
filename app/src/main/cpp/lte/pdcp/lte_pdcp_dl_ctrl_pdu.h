//
// Created by Vladimir Ponomarenko on 18.06.2025.
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

static int _decode_lte_pdcp_dl_ctrl_pdu_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PDCP_DL_Ctrl_PDU: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num Subpkt") != j.end() && j["Num Subpkt"].is_number()) {
        n_subpkt = j["Num Subpkt"].get<int>();
    } else {
        LOGD("LTE_PDCP_DL_Ctrl_PDU: Missing or invalid 'Num Subpkt'");
        j["error"] = "Missing Num Subpkt in main header";
        return 0;
    }

    LOGD("Decoding LTE_PDCP_DL_Ctrl_PDU with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LtePdcpDlCtrlPdu_SubpktHeader,
                                                     ARRAY_SIZE(LtePdcpDlCtrlPdu_SubpktHeader, Fmt),
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
                    LOGD("LTE_PDCP_DL_Ctrl_PDU: Error decoding subpacket %d header or missing fields/invalid size.", i);
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


                if (subpkt_id == 194) {
                    if (subpkt_payload_ver == 1) {
                        payload_consumed_total = _decode_by_fmt(LtePdcpDlCtrlPdu_SubpktPayload,
                                                                ARRAY_SIZE(LtePdcpDlCtrlPdu_SubpktPayload, Fmt),
                                                                b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset = payload_offset_start + payload_consumed_total;

                        if (subpkt_j.find("Mode") != subpkt_j.end()) {
                            const char* mode_name = search_name(LtePdcpDlCtrlPdu_Subpkt_Mode, ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_Mode, ValueName), subpkt_j["Mode"]);
                            subpkt_j["Mode"] = (mode_name ? mode_name : "(MI)Unknown");
                        }

                        int pdu_header_consumed = _decode_by_fmt(LtePdcpDlCtrlPdu_Subpkt_PDU_Header,
                                                                 ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_PDU_Header, Fmt),
                                                                 b, offset, length - (offset - start), subpkt_j);
                        offset += pdu_header_consumed;

                        int num_PDU = 0;
                        if (subpkt_j.find("Num PDUs") != subpkt_j.end() && subpkt_j["Num PDUs"].is_number()) {
                            num_PDU = subpkt_j["Num PDUs"].get<int>();
                        }

                        json pdu_list_j = json::array();
                        for (int pdu_idx = 0; pdu_idx < num_PDU; ++pdu_idx) {
                            json pdu_item_j;
                            offset += _decode_by_fmt(LtePdcpDlCtrlPdu_Subpkt_PDU_Fmt,
                                                     ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_PDU_Fmt, Fmt),
                                                     b, offset, length - (offset - start), pdu_item_j);

                            if (pdu_item_j.find("System Frame Number") != pdu_item_j.end() && pdu_item_j["System Frame Number"].is_number()) {
                                int iNonDecodeFN = pdu_item_j["System Frame Number"];
                                const unsigned int SFN_RSHIFT_PDU = 4, SFN_MASK_PDU = (1 << 12) - 1;
                                const unsigned int SUBFRAME_RSHIFT_PDU = 0, SUBFRAME_MASK_PDU = (1 << 4) - 1;
                                pdu_item_j["System Frame Number"] = (iNonDecodeFN >> SFN_RSHIFT_PDU) & SFN_MASK_PDU;
                                pdu_item_j["Subframe Number"] = (iNonDecodeFN >> SUBFRAME_RSHIFT_PDU) & SUBFRAME_MASK_PDU;
                            }
                            pdu_item_j["type"] = "STATUS REPORT";
                            pdu_list_j.push_back(pdu_item_j);
                        }
                        subpkt_j["PDCP DL Ctrl PDU"] = pdu_list_j;

                    } else if (subpkt_payload_ver == 24) {
                        payload_consumed_total = _decode_by_fmt(LtePdcpDlCtrlPdu_SubpktPayload_v24,
                                                                ARRAY_SIZE(LtePdcpDlCtrlPdu_SubpktPayload_v24, Fmt),
                                                                b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset = payload_offset_start + payload_consumed_total;

                        if (subpkt_j.find("Mode") != subpkt_j.end()) {
                            const char* mode_name = search_name(LtePdcpDlCtrlPdu_Subpkt_Mode, ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_Mode, ValueName), subpkt_j["Mode"]);
                            subpkt_j["Mode"] = (mode_name ? mode_name : "(MI)Unknown");
                        }

                        int pdu_header_consumed = _decode_by_fmt(LtePdcpDlCtrlPdu_Subpkt_PDU_Header_v24,
                                                                 ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_PDU_Header_v24, Fmt),
                                                                 b, offset, length - (offset - start), subpkt_j);
                        offset += pdu_header_consumed;

                        int num_PDU = 0;
                        if (subpkt_j.find("Num PDUs") != subpkt_j.end() && subpkt_j["Num PDUs"].is_number()) {
                            num_PDU = subpkt_j["Num PDUs"].get<int>();
                        }

                        json pdu_list_j = json::array();
                        for (int pdu_idx = 0; pdu_idx < num_PDU; ++pdu_idx) {
                            json pdu_item_j;
                            offset += _decode_by_fmt(LtePdcpDlCtrlPdu_Subpkt_PDU_Fmt_v24,
                                                     ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_PDU_Fmt_v24, Fmt),
                                                     b, offset, length - (offset - start), pdu_item_j);

                            if (pdu_item_j.find("sys fn") != pdu_item_j.end() && pdu_item_j["sys fn"].is_number()) {
                                int iNonDecodeFN = pdu_item_j["sys fn"];
                                const unsigned int SFN_RSHIFT_PDU = 4, SFN_MASK_PDU = (1 << 12) - 1;
                                const unsigned int SUBFRAME_RSHIFT_PDU = 0, SUBFRAME_MASK_PDU = (1 << 4) - 1;
                                pdu_item_j["sys fn"] = (iNonDecodeFN >> SFN_RSHIFT_PDU) & SFN_MASK_PDU;
                                pdu_item_j["sub fn"] = (iNonDecodeFN >> SUBFRAME_RSHIFT_PDU) & SUBFRAME_MASK_PDU;
                            }

                            if (pdu_item_j.find("PDU Type") != pdu_item_j.end() && pdu_item_j["PDU Type"].is_number()) {
                                int pdu_type_val = pdu_item_j["PDU Type"];
                                pdu_item_j["PDU Type"] = pdu_type_val & 0x3;
                                const char* pdu_type_name = search_name(LtePdcpDlCtrlPdu_PDU_Type, ARRAY_SIZE(LtePdcpDlCtrlPdu_PDU_Type, ValueName), pdu_item_j["PDU Type"]);
                                pdu_item_j["PDU Type"] = (pdu_type_name ? pdu_type_name : "(MI)Unknown");
                            }

                            if (pdu_item_j.find("fms") != pdu_item_j.end() && pdu_item_j["fms"].is_number()) {
                                int temp_fms = pdu_item_j["fms"];
                                pdu_item_j["DC"] = temp_fms & 0x1;
                                pdu_item_j["type"] = (temp_fms >> 1) & 0x7;
                                const char* type_name = search_name(LtePdcpDlCtrlPdu_Type, ARRAY_SIZE(LtePdcpDlCtrlPdu_Type, ValueName), pdu_item_j["type"]);
                                pdu_item_j["type"] = (type_name ? type_name : "(MI)Unknown");
                                pdu_item_j["fms"] = ((temp_fms >> 8) & 0xff) | ((temp_fms & 0xf0) << 4);
                                uint16_t fms_raw = temp_fms;
                                pdu_item_j["DC"] = (fms_raw >> 15) & 0x1;
                                pdu_item_j["type"] = (fms_raw >> 12) & 0x7;
                                pdu_item_j["fms"] = fms_raw & 0xFFF;
                                const char* type_name_fms = search_name(LtePdcpDlCtrlPdu_Type, ARRAY_SIZE(LtePdcpDlCtrlPdu_Type, ValueName), pdu_item_j["type"]);
                                pdu_item_j["type"] = (type_name_fms ? type_name_fms : "(MI)Unknown");

                            }
                            pdu_list_j.push_back(pdu_item_j);
                        }
                        subpkt_j["PDCP DL Ctrl PDU"] = pdu_list_j;

                    } else {
                        LOGD("(MI)Unknown LTE PDCP DL Ctrl PDU subpkt version for ID 194: %d", subpkt_payload_ver);
                        int remaining_payload_size = subpkt_size - header_consumed;
                        if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                            offset += remaining_payload_size;
                        }
                    }
                } else {
                    LOGD("(MI)Unknown LTE PDCP DL Ctrl PDU Subpacket ID: 0x%x", subpkt_id);
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
            LOGD("(MI)Unknown LTE PDCP DL Ctrl PDU packet version: 0x%x", pkt_ver);
            return 0;
    }
}