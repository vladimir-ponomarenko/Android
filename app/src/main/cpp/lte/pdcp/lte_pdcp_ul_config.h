//
// Created by v.ponomarenko on 01.06.2025.
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


static int _decode_lte_pdcp_ul_config_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PDCP_UL_Config: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num Subpkt") != j.end() && j["Num Subpkt"].is_number()) {
        n_subpkt = j["Num Subpkt"].get<int>();
    } else {
        LOGD("LTE_PDCP_UL_Config: Missing or invalid 'Num Subpkt'");
        j["error"] = "Missing Num Subpkt in main header";
        return 0;
    }

    LOGD("Decoding LTE_PDCP_UL_Config with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LtePdcpUlConfig_SubpktHeader,
                                                     ARRAY_SIZE(LtePdcpUlConfig_SubpktHeader, Fmt),
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
                    LOGD("LTE_PDCP_UL_Config: Error decoding subpacket %d header or missing fields/invalid size.", i);
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

                if (subpkt_id == 193) {
                    if (subpkt_payload_ver == 2 || subpkt_payload_ver == 3 || subpkt_payload_ver == 24) {
                        offset += _decode_by_fmt(LtePdcpUlConfig_SubpktPayload,
                                                 ARRAY_SIZE(LtePdcpUlConfig_SubpktPayload, Fmt),
                                                 b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);

                        const char* reason_name = search_name(LtePdcpUlConfig_Subpkt_Reason, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_Reason, ValueName), subpkt_j["Reason"]);
                        subpkt_j["Reason"] = (reason_name ? reason_name : "(MI)Unknown");
                        const char* srb_ciph_name = search_name(LtePdcpUlConfig_Subpkt_CipherAlgo, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_CipherAlgo, ValueName), subpkt_j["SRB Cipher Algorithm"]);
                        subpkt_j["SRB Cipher Algorithm"] = (srb_ciph_name ? srb_ciph_name : "(MI)Unknown");
                        const char* drb_ciph_name = search_name(LtePdcpUlConfig_Subpkt_CipherAlgo, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_CipherAlgo, ValueName), subpkt_j["DRB Cipher Algorithm"]);
                        subpkt_j["DRB Cipher Algorithm"] = (drb_ciph_name ? drb_ciph_name : "(MI)Unknown");
                        const char* srb_integ_name = search_name(LtePdcpUlConfig_Subpkt_IntegAlgo, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_IntegAlgo, ValueName), subpkt_j["SRB Integrity Algorithm"]);
                        subpkt_j["SRB Integrity Algorithm"] = (srb_integ_name ? srb_integ_name : "(MI)Unknown");

                        int iArraySize = subpkt_j["Array size"];

                        int start_ReleasedRBStruct = offset;
                        offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_ReleaseRB_Header, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ReleaseRB_Header, Fmt), b, offset, length - (offset-start), subpkt_j);
                        int num_ReleasedRB = subpkt_j["Number of Released RBs"];
                        json released_rbs_j = json::array();
                        for (int k = 0; k < num_ReleasedRB; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_ReleaseRB_Fmt, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ReleaseRB_Fmt, Fmt), b, offset, length - (offset-start), item_j);
                            released_rbs_j.push_back(item_j);
                        }
                        subpkt_j["Released RBs"] = released_rbs_j;
                        int expected_released_block_size = 1 + iArraySize * 1;
                        int actual_released_consumed = offset - start_ReleasedRBStruct;
                        if (actual_released_consumed < expected_released_block_size && offset + (expected_released_block_size - actual_released_consumed) <= start + length) {
                            offset += (expected_released_block_size - actual_released_consumed);
                        }

                        int start_AddedModifiedRBStruct = offset;
                        offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Header, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Header, Fmt), b, offset, length - (offset-start), subpkt_j);
                        int num_AddedModifiedRB = subpkt_j["Number of Added/Modified RBs"];
                        json added_modified_rbs_j = json::array();
                        for (int k = 0; k < num_AddedModifiedRB; ++k) {
                            json item_j;
                            offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Fmt, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Fmt, Fmt), b, offset, length - (offset-start), item_j);
                            const char* action_name = search_name(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Action, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Action, ValueName), item_j["Action"]);
                            item_j["Action"] = (action_name ? action_name : "(MI)Unknown");
                            added_modified_rbs_j.push_back(item_j);
                        }
                        subpkt_j["Added/Modified RBs"] = added_modified_rbs_j;
                        int expected_addedmod_block_size = 1 + iArraySize * 2;
                        int actual_addedmod_consumed = offset - start_AddedModifiedRBStruct;
                        if (actual_addedmod_consumed < expected_addedmod_block_size && offset + (expected_addedmod_block_size - actual_addedmod_consumed) <= start + length) {
                            offset += (expected_addedmod_block_size - actual_addedmod_consumed);
                        }

                        offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_ActiveRB_Header, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_Header, Fmt), b, offset, length - (offset-start), subpkt_j);
                        int num_ActiveRB = subpkt_j["Number of active RBs"];
                        json active_rbs_j = json::array();
                        for (int k = 0; k < num_ActiveRB; ++k) {
                            json item_j;
                            const Fmt* active_rb_fmt_array = nullptr;
                            size_t active_rb_fmt_size = 0;

                            if (subpkt_payload_ver == 2) {
                                active_rb_fmt_array = LtePdcpUlConfig_Subpkt_ActiveRB_Fmt;
                                active_rb_fmt_size = ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_Fmt, Fmt);
                            } else if (subpkt_payload_ver == 3) {
                                active_rb_fmt_array = LtePdcpUlConfig_Subpkt_ActiveRB_Fmt_v3;
                                active_rb_fmt_size = ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_Fmt_v3, Fmt);
                            } else if (subpkt_payload_ver == 24) {
                                active_rb_fmt_array = LtePdcpUlConfig_Subpkt_ActiveRB_Fmt_v24;
                                active_rb_fmt_size = ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_Fmt_v24, Fmt);
                            } else {
                                LOGD("LTE_PDCP_UL_Config: Unsupported Active RB Fmt for subpkt_ver %d", subpkt_payload_ver);
                                break;
                            }

                            offset += _decode_by_fmt(active_rb_fmt_array, active_rb_fmt_size, b, offset, length - (offset-start), item_j);

                            const char* rb_mode_name_act = search_name(LtePdcpUlConfig_Subpkt_ActiveRB_RBmode, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_RBmode, ValueName), item_j["RB mode"]);
                            item_j["RB mode"] = (rb_mode_name_act ? rb_mode_name_act : "(MI)Unknown");
                            const char* rb_type_name_act = search_name(LtePdcpUlConfig_Subpkt_ActiveRB_RBtype, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_RBtype, ValueName), item_j["RB type"]);
                            item_j["RB type"] = (rb_type_name_act ? rb_type_name_act : "(MI)Unknown");

                            if (subpkt_payload_ver == 2) {
                                const char* rohc_name = search_name(LtePdcpUlConfig_Subpkt_ActiveRB_RoHCEnabled, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_RoHCEnabled, ValueName), item_j["RoHC Enabled"]);
                                item_j["RoHC Enabled"] = (rohc_name ? rohc_name : "true");
                            } else if (subpkt_payload_ver == 24) {
                                if (item_j.find("RoHC Enabled") != item_j.end()){
                                    const char* rohc_name = search_name(LtePdcpUlConfig_Subpkt_ActiveRB_RoHCEnabled, ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_RoHCEnabled, ValueName), item_j["RoHC Enabled"]);
                                    item_j["RoHC Enabled"] = (rohc_name ? rohc_name : "true");
                                }
                            }

                            active_rbs_j.push_back(item_j);
                        }
                        subpkt_j["Active RBs"] = active_rbs_j;

                    } else {
                        LOGD("(MI)Unknown LTE PDCP UL Config subpkt version for ID 193: %d", subpkt_payload_ver);
                        int remaining_payload_size = subpkt_size - header_consumed;
                        if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                            offset += remaining_payload_size;
                        }
                    }
                } else {
                    LOGD("(MI)Unknown LTE PDCP UL Config Subpacket ID: 0x%x", subpkt_id);
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
            LOGD("(MI)Unknown LTE PDCP UL Config Log Packet version: 0x%x", pkt_ver);
            return 0;
    }
}


