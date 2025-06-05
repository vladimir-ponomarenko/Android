//
// Created by v.ponomarenko on 02.06.2025.
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


static int _decode_lte_pdcp_dl_stats_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PDCP_DL_Stats: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num Subpkt") != j.end() && j["Num Subpkt"].is_number()) {
        n_subpkt = j["Num Subpkt"].get<int>();
    } else {
        LOGD("LTE_PDCP_DL_Stats: Missing or invalid 'Num Subpkt'");
        j["error"] = "Missing Num Subpkt in main header";
        return 0;
    }

//    LOGD("Decoding LTE_PDCP_DL_Stats with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LtePdcpDlStats_SubpktHeader,
                                                     ARRAY_SIZE(LtePdcpDlStats_SubpktHeader, Fmt),
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
                    LOGD("LTE_PDCP_DL_Stats: Error decoding subpacket %d header or missing fields/invalid size.", i);
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

                if (subpkt_id == 196) {
                    if (subpkt_payload_ver == 2) {
                        payload_consumed_total = _decode_by_fmt(LtePdcpDlStats_SubpktPayload,
                                                                ARRAY_SIZE(LtePdcpDlStats_SubpktPayload, Fmt),
                                                                b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset = payload_offset_start + payload_consumed_total;

                        int num_RB = 0;
                        if (subpkt_j.find("Num RBs") != subpkt_j.end() && subpkt_j["Num RBs"].is_number()) {
                            num_RB = subpkt_j["Num RBs"].get<int>();
                        }
                        json rb_list_j = json::array();
                        for (int rb_idx = 0; rb_idx < num_RB; ++rb_idx) {
                            json rb_item_j;
                            offset += _decode_by_fmt(LtePdcpDlStats_Subpkt_RB_Fmt,
                                                     ARRAY_SIZE(LtePdcpDlStats_Subpkt_RB_Fmt, Fmt),
                                                     b, offset, length - (offset - start), rb_item_j);
                            if (rb_item_j.find("Mode") != rb_item_j.end()) {
                                const char* mode_name = search_name(LtePdcpDlStats_Subpkt_RB_Mode, ARRAY_SIZE(LtePdcpDlStats_Subpkt_RB_Mode, ValueName), rb_item_j["Mode"]);
                                rb_item_j["Mode"] = (mode_name ? mode_name : "(MI)Unknown");
                            }
                            rb_list_j.push_back(rb_item_j);
                        }
                        subpkt_j["RBs"] = rb_list_j;

                    } else if (subpkt_payload_ver == 41) {
                        payload_consumed_total = _decode_by_fmt(LtePdcpDlStats_SubpktPayload_v41,
                                                                ARRAY_SIZE(LtePdcpDlStats_SubpktPayload_v41, Fmt),
                                                                b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset = payload_offset_start + payload_consumed_total;

                        int num_RB = 0;
                        if (subpkt_j.find("Num RBs") != subpkt_j.end() && subpkt_j["Num RBs"].is_number()) {
                            num_RB = subpkt_j["Num RBs"].get<int>();
                        }
                        json rb_list_j = json::array();
                        for (int rb_idx = 0; rb_idx < num_RB; ++rb_idx) {
                            json rb_item_j;
                            offset += _decode_by_fmt(LtePdcpDlStats_Subpkt_RB_Fmt_v41,
                                                     ARRAY_SIZE(LtePdcpDlStats_Subpkt_RB_Fmt_v41, Fmt),
                                                     b, offset, length - (offset - start), rb_item_j);
                            if (rb_item_j.find("Mode") != rb_item_j.end()) {
                                const char* mode_name = search_name(LtePdcpDlStats_Subpkt_RB_Mode, ARRAY_SIZE(LtePdcpDlStats_Subpkt_RB_Mode, ValueName), rb_item_j["Mode"]);
                                rb_item_j["Mode"] = (mode_name ? mode_name : "(MI)Unknown");
                            }
                            rb_list_j.push_back(rb_item_j);
                        }
                        subpkt_j["RBs"] = rb_list_j;

                    } else {
                        LOGD("(MI)Unknown LTE PDCP DL Stats subpkt version for ID 196: %d", subpkt_payload_ver);
                        int remaining_payload_size = subpkt_size - header_consumed;
                        if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                            offset += remaining_payload_size;
                        }
                    }
                } else {
                    LOGD("(MI)Unknown LTE PDCP DL Stats Subpacket ID: 0x%x", subpkt_id);
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
            LOGD("(MI)Unknown LTE PDCP DL Stats packet version: 0x%x", pkt_ver);
            return 0;
    }
}