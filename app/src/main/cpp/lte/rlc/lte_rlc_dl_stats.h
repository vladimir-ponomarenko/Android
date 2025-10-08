//
// Created by Vladimir Ponomarenko on 28.04.2025.
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


static int _decode_lte_rlc_dl_stats_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    int n_subpkt = 0;
    if (j.find("Num Subpkt") != j.end() && j["Num Subpkt"].is_number()) {
        n_subpkt = j["Num Subpkt"].get<int>();
    } else {
        LOGD("Error: Missing or invalid 'Num Subpkt' in RLC DL Stats header");
        return 0;
    }

    //LOGD("Decoding LTE_RLC_DL_Stats with packet version: %d, num_subpackets: %d", pkt_ver, n_subpkt);


    if (pkt_ver != 1) {
        LOGD("(MI)Unknown LTE RLC DL Stats version: 0x%x", pkt_ver);
        return 0;
    }

    json subpackets_j = json::array();
    for (int i = 0; i < n_subpkt; ++i) {
        json subpkt_j;
        int start_subpkt = offset;


        int header_consumed = _decode_by_fmt(LteRlcDlStats_SubpktHeader,
                                             ARRAY_SIZE(LteRlcDlStats_SubpktHeader, Fmt),
                                             b, offset, length, subpkt_j);


        int subpkt_size = 0;
        int subpkt_id = -1;
        int subpkt_ver = -1;
        bool header_ok = false;

        if (header_consumed > 0 &&
            subpkt_j.find("Subpacket Size") != subpkt_j.end() && subpkt_j["Subpacket Size"].is_number() &&
            subpkt_j.find("Subpacket ID") != subpkt_j.end() && subpkt_j["Subpacket ID"].is_number() &&
            subpkt_j.find("Subpacket Version") != subpkt_j.end() && subpkt_j["Subpacket Version"].is_number())
        {
            subpkt_size = subpkt_j["Subpacket Size"].get<int>();
            subpkt_id = subpkt_j["Subpacket ID"].get<int>();
            subpkt_ver = subpkt_j["Subpacket Version"].get<int>();

            if (subpkt_size >= header_consumed && start_subpkt + subpkt_size <= start + length) {
                header_ok = true;
            } else {
                LOGD("Invalid subpacket size %d (header consumed %d, total length %zu, start_subpkt %d)", subpkt_size, header_consumed, length, start_subpkt);
            }
        }

        if (!header_ok) {
            LOGD("Error decoding subpacket header or invalid size/data. Subpacket index %d", i);
            if (subpkt_size > 0 && start_subpkt + subpkt_size <= start + length) {
                offset = start_subpkt + subpkt_size;
                LOGD("Attempting skip based on reported size %d", subpkt_size);
            } else {
                LOGD("Cannot reliably skip subpacket, stopping.");
                return offset - start;
            }
            continue;
        }

        offset += header_consumed;
        int payload_offset_start = offset;
        int payload_consumed = 0;


        if (subpkt_id == 66) {
            int num_RB = 0;
            const Fmt* rb_fmt_array = nullptr;
            size_t rb_fmt_size = 0;

            if (subpkt_ver == 3) {
                payload_consumed = _decode_by_fmt(LteRlcDlStats_SubpktPayload_v3,
                                                  ARRAY_SIZE(LteRlcDlStats_SubpktPayload_v3, Fmt),
                                                  b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                if (subpkt_j.find("Num RBs") != subpkt_j.end() && subpkt_j["Num RBs"].is_number()) {
                    num_RB = subpkt_j["Num RBs"].get<int>();
                }
                rb_fmt_array = LteRlcDlStats_Subpkt_RB_Fmt_v3;
                rb_fmt_size = ARRAY_SIZE(LteRlcDlStats_Subpkt_RB_Fmt_v3, Fmt);
            } else if (subpkt_ver == 2) {
                payload_consumed = _decode_by_fmt(LteRlcDlStats_SubpktPayload_v2,
                                                  ARRAY_SIZE(LteRlcDlStats_SubpktPayload_v2, Fmt),
                                                  b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                if (subpkt_j.find("Num RBs") != subpkt_j.end() && subpkt_j["Num RBs"].is_number()) {
                    num_RB = subpkt_j["Num RBs"].get<int>();
                }
                rb_fmt_array = LteRlcDlStats_Subpkt_RB_Fmt_v2;
                rb_fmt_size = ARRAY_SIZE(LteRlcDlStats_Subpkt_RB_Fmt_v2, Fmt);
            } else {
                LOGD("(MI)Unknown LTE RLC DL Stats subpacket version: %d for ID 0x42", subpkt_ver);

                int remaining_payload = subpkt_size - header_consumed;
                if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                    payload_consumed = remaining_payload;
                } else {
                    LOGD("Cannot skip unknown subpacket payload v%d reliably.", subpkt_ver);
                }
            }


            if (payload_consumed > 0 && rb_fmt_array != nullptr) {
                int current_rb_offset = payload_offset_start + payload_consumed;
                json rbs_j = json::array();
                for (int j_rb = 0; j_rb < num_RB; ++j_rb) {
                    json rb_item_j;
                    int rb_consumed = _decode_by_fmt(rb_fmt_array, rb_fmt_size,
                                                     b, current_rb_offset, length - (current_rb_offset - start), rb_item_j);

                    if (rb_consumed == 0) {
                        LOGD("Error decoding RB %d for RLC DL Stats subpacket v%d", j_rb, subpkt_ver);
                        break;
                    }
                    payload_consumed += rb_consumed;
                    current_rb_offset += rb_consumed;


                    if (rb_item_j.find("Mode") != rb_item_j.end() && rb_item_j["Mode"].is_number()) {
                        const char* mode_name = search_name(LteRlcDlStats_Subpkt_RB_Mode, ARRAY_SIZE(LteRlcDlStats_Subpkt_RB_Mode, ValueName), rb_item_j["Mode"]);
                        rb_item_j["Mode"] = (mode_name ? mode_name : "(MI)Unknown");
                    }
                    rbs_j.push_back(rb_item_j);
                }
                subpkt_j["RBs"] = rbs_j;
            }

        } else {
            LOGD("(MI)Unknown LTE RLC DL Stats subpkt id: 0x%x", subpkt_id);

            int remaining_payload = subpkt_size - header_consumed;
            if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                payload_consumed = remaining_payload;
            } else {
                LOGD("Cannot skip unknown subpacket id 0x%x payload reliably.", subpkt_id);
            }
        }


        offset = payload_offset_start + payload_consumed;


        int total_consumed_in_subpacket = offset - start_subpkt;
        if (total_consumed_in_subpacket != subpkt_size) {
           // LOGD("Subpacket %d v%d: Consumed %d bytes, but expected size %d. Adjusting offset.", subpkt_id, subpkt_ver, total_consumed_in_subpacket, subpkt_size);
            offset = start_subpkt + subpkt_size;
            if (offset > start + length) {
                LOGD("Error: Adjusted offset exceeds buffer length. Stopping.");


                subpackets_j.push_back(subpkt_j);
                return offset - start;
            }
        }

        subpackets_j.push_back(subpkt_j);
    }

    j["Subpackets"] = subpackets_j;
    return offset - start;
}

