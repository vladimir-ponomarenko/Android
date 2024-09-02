//
// Created by Vladimir Ponomarenko on 21.05.2025.
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


static int _decode_lte_rlc_dl_config_log_packet_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_RLC_DL_Config_Log_Packet: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num SubPkt") != j.end() && j["Num SubPkt"].is_number()) {
        n_subpkt = j["Num SubPkt"].get<int>();
    } else {
        LOGD("LTE_RLC_DL_Config_Log_Packet: Missing or invalid 'Num SubPkt'");
        j["error"] = "Missing Num SubPkt in main header";
        return 0;
    }

   // LOGD("Decoding LTE_RLC_DL_Config_Log_Packet with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LteRlcDlConfigLogPacket_SubpktHeader,
                                                     ARRAY_SIZE(LteRlcDlConfigLogPacket_SubpktHeader, Fmt),
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
                    LOGD("LTE_RLC_DL_Config: Error decoding subpacket %d header or missing fields/invalid size.", i);
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

                if (subpkt_id == 64) {
                    if (subpkt_payload_ver == 1) {
                        int payload_consumed_v1 = _decode_by_fmt(LteRlcDlConfigLogPacket_SubpktPayload,
                                                                 ARRAY_SIZE(LteRlcDlConfigLogPacket_SubpktPayload, Fmt),
                                                                 b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset += payload_consumed_v1;

                        const char* reason_name = search_name(LteRlcDlConfigLogPacket_Subpkt_Reason, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_Reason, ValueName), subpkt_j["Reason"]);
                        subpkt_j["Reason"] = (reason_name ? reason_name : "(MI)Unknown");

                        int maxSizeRbs = 0;
                        if (subpkt_j.find("Max Size RBs") != subpkt_j.end() && subpkt_j["Max Size RBs"].is_number()) {
                            maxSizeRbs = subpkt_j["Max Size RBs"].get<int>();
                        }


                        int start_ReleasedRBStruct = offset;
                        int released_header_consumed = _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header,
                                                                      ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header, Fmt),
                                                                      b, offset, length - (offset-start), subpkt_j);
                        offset += released_header_consumed;
                        int num_ReleasedRB = 0;
                        if (subpkt_j.find("Number of Released RBs") != subpkt_j.end() && subpkt_j["Number of Released RBs"].is_number()) {
                            num_ReleasedRB = subpkt_j["Number of Released RBs"].get<int>();
                        }
                        json released_rbs_j = json::array();
                        for (int k = 0; k < num_ReleasedRB; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt, Fmt), b, offset, length - (offset-start), item_j);
                            released_rbs_j.push_back(item_j);
                        }
                        subpkt_j["Released RBs"] = released_rbs_j;
                        int expected_released_block_size = 1 + maxSizeRbs * 1;
                        int actual_released_consumed = offset - start_ReleasedRBStruct;
                        if (actual_released_consumed < expected_released_block_size) {
                            offset += (expected_released_block_size - actual_released_consumed);
                        }



                        int start_AddedModifiedRBStruct = offset;
                        int addedmod_header_consumed = _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                                                                      ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header, Fmt),
                                                                      b, offset, length - (offset-start), subpkt_j);
                        offset += addedmod_header_consumed;
                        int num_AddedModifiedRB = 0;
                        if (subpkt_j.find("Number of Added/Modified RBs") != subpkt_j.end() && subpkt_j["Number of Added/Modified RBs"].is_number()){
                            num_AddedModifiedRB = subpkt_j["Number of Added/Modified RBs"].get<int>();
                        }
                        json added_modified_rbs_j = json::array();
                        for (int k = 0; k < num_AddedModifiedRB; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt, Fmt), b, offset, length - (offset-start), item_j);
                            if (item_j.find("Action") != item_j.end()) {
                                const char* action_name = search_name(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action, ValueName), item_j["Action"]);
                                item_j["Action"] = (action_name ? action_name : "(MI)Unknown");
                            }
                            added_modified_rbs_j.push_back(item_j);
                        }
                        subpkt_j["Added/Modified RBs"] = added_modified_rbs_j;

                        int expected_addedmod_block_size = 1 + maxSizeRbs * 2;
                        int actual_addedmod_consumed = offset - start_AddedModifiedRBStruct;
                        if (actual_addedmod_consumed < expected_addedmod_block_size) {
                            offset += (expected_addedmod_block_size - actual_addedmod_consumed);
                        }



                        int active_header_consumed = _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header,
                                                                    ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header, Fmt),
                                                                    b, offset, length - (offset-start), subpkt_j);
                        offset += active_header_consumed;
                        int num_ActiveRB = 0;
                        if (subpkt_j.find("Number of Active RBs") != subpkt_j.end() && subpkt_j["Number of Active RBs"].is_number()){
                            num_ActiveRB = subpkt_j["Number of Active RBs"].get<int>();
                        }
                        json active_rbs_j = json::array();
                        for (int k = 0; k < num_ActiveRB; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt, Fmt), b, offset, length - (offset-start), item_j);
                            if (item_j.find("RB Mode") != item_j.end()) {
                                const char* rb_mode_name = search_name(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode, ValueName), item_j["RB Mode"]);
                                item_j["RB Mode"] = (rb_mode_name ? rb_mode_name : "(MI)Unknown");
                            }
                            if (item_j.find("RB Type") != item_j.end()) {
                                const char* rb_type_name = search_name(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType, ValueName), item_j["RB Type"]);
                                item_j["RB Type"] = (rb_type_name ? rb_type_name : "(MI)Unknown");
                            }
                            item_j["RLCDL CFG"] = "RLCDL CFG";
                            item_j["SN Length"] = 10;
                            active_rbs_j.push_back(item_j);
                        }
                        subpkt_j["Active RBs"] = active_rbs_j;

                    } else if (subpkt_id == 64 && subpkt_payload_ver == 2) {
                        int payload_consumed_v2 = _decode_by_fmt(LteRlcDlConfigLogPacket_SubpktPayload_v2,
                                                                 ARRAY_SIZE(LteRlcDlConfigLogPacket_SubpktPayload_v2, Fmt),
                                                                 b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset += payload_consumed_v2;

                        const char* reason_name = search_name(LteRlcDlConfigLogPacket_Subpkt_Reason, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_Reason, ValueName), subpkt_j["Reason"]);
                        subpkt_j["Reason"] = (reason_name ? reason_name : "(MI)Unknown");

                        int maxSizeRbs = 0;
                        if (subpkt_j.find("Max Size RBs") != subpkt_j.end() && subpkt_j["Max Size RBs"].is_number()) {
                            maxSizeRbs = subpkt_j["Max Size RBs"].get<int>();
                        }


                        int start_ReleasedRBStruct_v2 = offset;
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header_v2, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header_v2, Fmt), b, offset, length - (offset-start), subpkt_j);
                        int num_ReleasedRB_v2 = 0;
                        if (subpkt_j.find("Number of Released RBs") != subpkt_j.end() && subpkt_j["Number of Released RBs"].is_number()){
                            num_ReleasedRB_v2 = subpkt_j["Number of Released RBs"].get<int>();
                        }
                        json released_rbs_v2_j = json::array();
                        for (int k = 0; k < num_ReleasedRB_v2; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt_v2, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt_v2, Fmt), b, offset, length - (offset-start), item_j);
                            released_rbs_v2_j.push_back(item_j);
                        }
                        subpkt_j["Released RBs"] = released_rbs_v2_j;
                        int expected_released_v2_block_size = 1 + maxSizeRbs * 1;
                        int actual_released_v2_consumed = offset - start_ReleasedRBStruct_v2;
                        if (actual_released_v2_consumed < expected_released_v2_block_size) {
                            offset += (expected_released_v2_block_size - actual_released_v2_consumed);
                        }



                        int start_AddedModifiedRBStruct_v2 = offset;
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header_v2, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header_v2, Fmt), b, offset, length - (offset-start), subpkt_j);
                        int num_AddedModifiedRB_v2 = 0;
                        if(subpkt_j.find("Number of Added/Modified RBs") != subpkt_j.end() && subpkt_j["Number of Added/Modified RBs"].is_number()){
                            num_AddedModifiedRB_v2 = subpkt_j["Number of Added/Modified RBs"].get<int>();
                        }
                        json added_modified_rbs_v2_j = json::array();
                        for (int k = 0; k < num_AddedModifiedRB_v2; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt_v2, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt_v2, Fmt), b, offset, length - (offset-start), item_j);
                            if (item_j.find("Action") != item_j.end()){
                                const char* action_name = search_name(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action, ValueName), item_j["Action"]);
                                item_j["Action"] = (action_name ? action_name : "(MI)Unknown");
                            }
                            added_modified_rbs_v2_j.push_back(item_j);
                        }
                        subpkt_j["Added/Modified RBs"] = added_modified_rbs_v2_j;
                        int expected_addedmod_v2_block_size = 1 + maxSizeRbs * 2;
                        int actual_addedmod_v2_consumed = offset - start_AddedModifiedRBStruct_v2;
                        if (actual_addedmod_v2_consumed < expected_addedmod_v2_block_size) {
                            offset += (expected_addedmod_v2_block_size - actual_addedmod_v2_consumed);
                        }



                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header_v2, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header_v2, Fmt), b, offset, length - (offset-start), subpkt_j);
                        int num_ActiveRB_v2 = 0;
                        if(subpkt_j.find("Number of Active RBs") != subpkt_j.end() && subpkt_j["Number of Active RBs"].is_number()){
                            num_ActiveRB_v2 = subpkt_j["Number of Active RBs"].get<int>();
                        }
                        json active_rbs_v2_j = json::array();
                        for (int k = 0; k < num_ActiveRB_v2; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt_v2, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt_v2, Fmt), b, offset, length - (offset-start), item_j);
                            if (item_j.find("RB Mode") != item_j.end()) {
                                const char* rb_mode_name = search_name(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode, ValueName), item_j["RB Mode"]);
                                item_j["RB Mode"] = (rb_mode_name ? rb_mode_name : "(MI)Unknown");
                            }
                            if (item_j.find("RB Type") != item_j.end()){
                                const char* rb_type_name = search_name(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType, ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType, ValueName), item_j["RB Type"]);
                                item_j["RB Type"] = (rb_type_name ? rb_type_name : "(MI)Unknown");
                            }
                            item_j["RLCDL CFG"] = "RLCDL CFG";

                            active_rbs_v2_j.push_back(item_j);
                        }
                        subpkt_j["Active RBs"] = active_rbs_v2_j;

                    } else {
                        LOGD("(MI)Unknown LTE RLC DL Config Log Packet subpkt id and version: 0x%x - %d", subpkt_id, subpkt_payload_ver);

                        int remaining_payload_size = subpkt_size - header_consumed;
                        if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                            offset += remaining_payload_size;
                        }
                    }
                } else {
                    LOGD("(MI)Unknown LTE RLC DL Config Log Packet Subpacket ID: 0x%x", subpkt_id);
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
            LOGD("(MI)Unknown LTE RLC DL Config Log Packet version: 0x%x", pkt_ver);
            return 0;
    }
}