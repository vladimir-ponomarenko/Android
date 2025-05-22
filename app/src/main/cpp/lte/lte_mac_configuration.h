//
// Created by Vladimir Ponomarenko on 23.04.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_mac_configuration_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;

    int pkt_ver = j["Version"];
    int n_subpkt = j["Num SubPkt"];


    if (pkt_ver != 1) {
        LOGD("(MI)Unsupported LTE MAC Configuration packet version: 0x%x", pkt_ver);
        return 0;
    }

    json subpackets_j = json::array();
    for (int i = 0; i < n_subpkt; ++i) {
        json subpkt_j;
        int start_subpkt = offset;


        int header_consumed = _decode_by_fmt(LteMacConfiguration_SubpktHeader,
                                             ARRAY_SIZE(LteMacConfiguration_SubpktHeader, Fmt),
                                             b, offset, length - (offset - start), subpkt_j);


        int subpkt_size = 0;
        int subpkt_id = -1;
        int subpkt_ver = -1;
        bool header_ok = false;

        if (header_consumed > 0 &&
            subpkt_j.find("SubPacket Size") != subpkt_j.end() && subpkt_j["SubPacket Size"].is_number() &&
            subpkt_j.find("SubPacket ID") != subpkt_j.end() && subpkt_j["SubPacket ID"].is_number() &&
            subpkt_j.find("Version") != subpkt_j.end() && subpkt_j["Version"].is_number())
        {
            subpkt_size = subpkt_j["SubPacket Size"].get<int>();
            subpkt_id = subpkt_j["SubPacket ID"].get<int>();
            subpkt_ver = subpkt_j["Version"].get<int>();
            if (subpkt_size >= header_consumed && start_subpkt + subpkt_size <= start + length) {
                header_ok = true;
            }
        }

        if (!header_ok) {
            LOGD("Error decoding MAC Config subpacket header or insufficient data/invalid size. Subpacket index %d", i);
            if (subpkt_size > 0 && start_subpkt + subpkt_size <= start + length) {
                offset = start_subpkt + subpkt_size;
                LOGD("Attempting skip based on reported size %d", subpkt_size);
            } else {
                LOGD("Cannot reliably skip MAC Config subpacket, stopping.");
                return offset - start;
            }
            continue;
        }

        offset += header_consumed;
        int payload_offset_start = offset;
        int payload_consumed = 0;


        const char* subpkt_type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                   ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                   subpkt_id);
        subpkt_j["SubPacket Type"] = (subpkt_type_name ? subpkt_type_name : "(MI)Unknown");
        // Удаляем числовой ID после маппинга
        // if(subpkt_j.find("SubPacket ID") != subpkt_j.end()) subpkt_j.erase("SubPacket ID");



        bool success = false;
        switch (subpkt_id) {
            case 0:
                if (subpkt_ver == 1) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_ConfigType,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_ConfigType, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    if (subpkt_j.find("Config reason") != subpkt_j.end()) {
                        const char* reason_name = search_name(LteMacConfigurationConfigType_ConfigReason, ARRAY_SIZE(LteMacConfigurationConfigType_ConfigReason, ValueName), subpkt_j["Config reason"]);

                        subpkt_j["Config reason"] = (reason_name ? reason_name : "NORMAL");
                    }
                    success = (payload_consumed > 0);
                } else if (subpkt_ver == 2) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_ConfigType_v2,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_ConfigType_v2, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    if (subpkt_j.find("Config reason") != subpkt_j.end()) {
                        const char* reason_name = search_name(LteMacConfigurationConfigType_ConfigReason, ARRAY_SIZE(LteMacConfigurationConfigType_ConfigReason, ValueName), subpkt_j["Config reason"]);
                        subpkt_j["Config reason"] = (reason_name ? reason_name : "NORMAL");
                    }
                    success = (payload_consumed > 0);
                }
                break;
            case 1:
                if (subpkt_ver == 1) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_DLConfig,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_DLConfig, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    if (subpkt_j.find("TA Timer") != subpkt_j.end() && subpkt_j["TA Timer"] == 0xffff) {
                        const char* timer_name = search_name(LteMacConfigurationConfigType_DLConfig_TA_Timer, ARRAY_SIZE(LteMacConfigurationConfigType_DLConfig_TA_Timer, ValueName), 0xffff);
                        subpkt_j["TA Timer"] = (timer_name ? timer_name : "(MI)Unknown");
                    }
                    success = (payload_consumed > 0);
                } else if (subpkt_ver == 2) {
                    int header_v2_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_DLConfig_v2,
                                                            ARRAY_SIZE(LteMacConfigurationSubpkt_DLConfig_v2, Fmt),
                                                            b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    payload_consumed += header_v2_consumed;
                    int current_stag_offset = payload_offset_start + header_v2_consumed;

                    int num_stag = 0;
                    if (subpkt_j.find("Num Active Stag") != subpkt_j.end() && subpkt_j["Num Active Stag"].is_number()) {
                        num_stag = subpkt_j["Num Active Stag"].get<int>();
                    }

                    json scell_tag_info_j = json::array();
                    for (int k = 0; k < num_stag; ++k) {
                        json stag_item_j;
                        int stag_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_DLConfig_Scell_Tag_Info_v2,
                                                           ARRAY_SIZE(LteMacConfigurationSubpkt_DLConfig_Scell_Tag_Info_v2, Fmt),
                                                           b, current_stag_offset, length - (current_stag_offset - start), stag_item_j);
                        if (stag_consumed == 0) break;
                        payload_consumed += stag_consumed;
                        current_stag_offset += stag_consumed;

                        if (stag_item_j.find("TA Timer") != stag_item_j.end() && stag_item_j["TA Timer"] == 0xffff) {
                            const char* timer_name = search_name(LteMacConfigurationConfigType_DLConfig_TA_Timer, ARRAY_SIZE(LteMacConfigurationConfigType_DLConfig_TA_Timer, ValueName), 0xffff);
                            stag_item_j["TA Timer"] = (timer_name ? timer_name : "(MI)Unknown");
                        }
                        scell_tag_info_j.push_back(stag_item_j);
                    }
                    subpkt_j["Scell Tag Info"] = scell_tag_info_j;
                    success = (header_v2_consumed > 0);
                }
                break;
            case 2:
                if (subpkt_ver == 1) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_ULConfig,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_ULConfig, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    if (subpkt_j.find("BSR timer") != subpkt_j.end() && subpkt_j["BSR timer"] == 0xffff) {
                        const char* timer_name = search_name(LteMacConfigurationConfigType_ULConfig_BSR_Timer, ARRAY_SIZE(LteMacConfigurationConfigType_ULConfig_BSR_Timer, ValueName), 0xffff);
                        subpkt_j["BSR timer"] = (timer_name ? timer_name : "(MI)Unknown");
                    }

                    success = (payload_consumed > 0);
                } else if (subpkt_ver == 2) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_ULConfig_v2,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_ULConfig_v2, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);

                    success = (payload_consumed > 0);
                }
                break;
            case 3:
                if (subpkt_ver == 1) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_RACHConfig,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_RACHConfig, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);

                    success = (payload_consumed > 0);
                } else if (subpkt_ver == 2) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_RACHConfig_v2,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_RACHConfig_v2, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);

                    success = (payload_consumed > 0);
                } else if (subpkt_ver == 5) {
                    int rach_v5_header_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_RACHConfig_v5,
                                                                 ARRAY_SIZE(LteMacConfigurationSubpkt_RACHConfig_v5, Fmt),
                                                                 b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    payload_consumed += rach_v5_header_consumed;
                    int current_r13_offset = payload_offset_start + rach_v5_header_consumed;


                    if (subpkt_j.find("Power offset Group_B") != subpkt_j.end() && subpkt_j["Power offset Group_B"] == 0) {
                        const char* offset_name = search_name(LteMacConfigurationSubpkt_RACHConfig_Power_offset_Group_B, ARRAY_SIZE(LteMacConfigurationSubpkt_RACHConfig_Power_offset_Group_B, ValueName), 0);
                        subpkt_j["Power offset Group_B"] = (offset_name ? offset_name : "-Infinity");
                    }
                    if (subpkt_j.find("PRACH config") != subpkt_j.end() && subpkt_j["PRACH config"].is_number()) {
                        int prach_cfg = subpkt_j["PRACH config"];
                        if (prach_cfg < 16) subpkt_j["Preamble Format"] = 0;
                        else if (prach_cfg == 30) subpkt_j["Preamble Format"] = "N/A";
                        else subpkt_j["Preamble Format"] = 1;
                    } else {
                        subpkt_j["Preamble Format"] = "(Error)";
                    }


                    int prach_cfg_r13_present = 0;
                    if (subpkt_j.find("PRACH Cfg R13 Present") != subpkt_j.end() && subpkt_j["PRACH Cfg R13 Present"].is_number()) {
                        prach_cfg_r13_present = subpkt_j["PRACH Cfg R13 Present"].get<int>();
                    }
                    json prach_cfg_r13_j;
                    if (prach_cfg_r13_present == 1) {
                        json rsrp_size_j;
                        int consumed_size = _decode_by_fmt(LteMacConfiguration_RachConfigSubpktPayload_rsrp_prach_list_size_v5, ARRAY_SIZE(LteMacConfiguration_RachConfigSubpktPayload_rsrp_prach_list_size_v5, Fmt), b, current_r13_offset, length-(current_r13_offset-start), rsrp_size_j);
                        if(consumed_size==0) goto skip_rach_v5_r13;
                        payload_consumed += consumed_size; current_r13_offset += consumed_size;
                        int rsrp_list_size = rsrp_size_j["RSRP Thresh PRACH List Size"];
                        prach_cfg_r13_j["RSRP Thresh PRACH List Size"] = rsrp_list_size;

                        json rsrp_list_j = json::array();
                        for (int k = 0; k < rsrp_list_size; ++k) {
                            json rsrp_item_j;
                            int consumed_item = _decode_by_fmt(LteMacConfiguration_RachConfigSubpktPayload_rsrp_prach_list_v5, ARRAY_SIZE(LteMacConfiguration_RachConfigSubpktPayload_rsrp_prach_list_v5, Fmt), b, current_r13_offset, length-(current_r13_offset-start), rsrp_item_j);
                            if(consumed_item==0) goto skip_rach_v5_r13_loops;
                            payload_consumed += consumed_item; current_r13_offset += consumed_item;
                            if(rsrp_item_j.find("RSRP Thresh PRACH List") != rsrp_item_j.end() && rsrp_item_j["RSRP Thresh PRACH List"].is_number()) {
                                uint8_t rsrp_index = rsrp_item_j["RSRP Thresh PRACH List"];
                                rsrp_item_j["RSRP Thresh PRACH List (dBm)"] = rsrp_index - 140;
                                rsrp_item_j.erase("RSRP Thresh PRACH List");
                            }
                            rsrp_list_j.push_back(rsrp_item_j);
                        }
                        skip_rach_v5_r13_loops:;
                        prach_cfg_r13_j["RSRP Thresh PRACH List"] = rsrp_list_j;

                        int hidden_rsrp_count = 3 - rsrp_list_size;
                        if (hidden_rsrp_count > 0) { int consumed_hidden = hidden_rsrp_count * 1; if(current_r13_offset+consumed_hidden > start+length) goto skip_rach_v5_r13; payload_consumed += consumed_hidden; current_r13_offset += consumed_hidden; }

                        json ce_size_j;
                        int consumed_ce_size = _decode_by_fmt(LteMacConfiguration_RachConfigSubpktPayload_prach_param_ce_list_size_v5, ARRAY_SIZE(LteMacConfiguration_RachConfigSubpktPayload_prach_param_ce_list_size_v5, Fmt), b, current_r13_offset, length-(current_r13_offset-start), ce_size_j);
                        if(consumed_ce_size==0) goto skip_rach_v5_r13;
                        payload_consumed += consumed_ce_size; current_r13_offset += consumed_ce_size;
                        int ce_list_size = ce_size_j["PRACH Param CE List Size"];
                        prach_cfg_r13_j["PRACH Param CE List Size"] = ce_list_size;

                        json ce_list_j = json::array();
                        for (int k = 0; k < ce_list_size; ++k) {
                            json ce_item_j;
                            int consumed_ce_item = _decode_by_fmt(LteMacConfiguration_RachConfigSubpktPayload_prach_list_v5, ARRAY_SIZE(LteMacConfiguration_RachConfigSubpktPayload_prach_list_v5, Fmt), b, current_r13_offset, length-(current_r13_offset-start), ce_item_j);
                            if(consumed_ce_item==0) goto skip_rach_v5_r13_loops_ce;
                            payload_consumed += consumed_ce_item; current_r13_offset += consumed_ce_item;
                            ce_list_j.push_back(ce_item_j);
                        }
                        skip_rach_v5_r13_loops_ce:;
                        prach_cfg_r13_j["PRACH Param Ce"] = ce_list_j;

                        int hidden_ce_count = 4 - ce_list_size;
                        if (hidden_ce_count > 0) { int consumed_hidden_ce = hidden_ce_count * 7; if(current_r13_offset+consumed_hidden_ce > start+length) goto skip_rach_v5_r13; payload_consumed += consumed_hidden_ce; current_r13_offset += consumed_hidden_ce; }

                        json last_part_j;
                        int consumed_last_part = _decode_by_fmt(LteMacConfiguration_RachConfigSubpktPayload_prach_last_part, ARRAY_SIZE(LteMacConfiguration_RachConfigSubpktPayload_prach_last_part, Fmt), b, current_r13_offset, length-(current_r13_offset-start), last_part_j);
                        if(consumed_last_part==0) goto skip_rach_v5_r13;
                        payload_consumed += consumed_last_part;

                        if (last_part_j.find("Initial CE Level") != last_part_j.end() && last_part_j["Initial CE Level"] == 0xffff) {
                            const char* ce_level_name = search_name(LteMacConfiguration_RachConfigSubpktPayload_prach_initial_ce_level, ARRAY_SIZE(LteMacConfiguration_RachConfigSubpktPayload_prach_initial_ce_level, ValueName), 0xffff);
                            last_part_j["Initial CE Level"] = (ce_level_name ? ce_level_name : "NA");
                        }
                        prach_cfg_r13_j.update(last_part_j);
                        subpkt_j["PRACH Cfg R13"] = prach_cfg_r13_j;
                    }
                    skip_rach_v5_r13:;
                    success = (rach_v5_header_consumed > 0);
                }
                break;
            case 4:
                if (subpkt_ver == 1 || subpkt_ver == 2) {
                    const Fmt* lc_fmt = (subpkt_ver == 1) ? LteMacConfigurationSubpkt_LCConfig : LteMacConfigurationSubpkt_LCConfig_v2;
                    size_t lc_fmt_size = (subpkt_ver == 1) ? ARRAY_SIZE(LteMacConfigurationSubpkt_LCConfig, Fmt) : ARRAY_SIZE(LteMacConfigurationSubpkt_LCConfig_v2, Fmt);

                    int lc_header_consumed = _decode_by_fmt(lc_fmt, lc_fmt_size, b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    payload_consumed += lc_header_consumed;
                    int current_lc_offset = payload_offset_start + lc_header_consumed;

                    int num_LC = 0;
                    if (subpkt_j.find("Number of added/modified LC") != subpkt_j.end() && subpkt_j["Number of added/modified LC"].is_number()) {
                        num_LC = subpkt_j["Number of added/modified LC"].get<int>();
                    }
                    int lc_list_start_offset = current_lc_offset;

                    json lc_list_j = json::array();
                    for (int k = 0; k < num_LC; ++k) {
                        json lc_item_j;
                        int lc_consumed = _decode_by_fmt(LteMacConfiguration_LCConfig_LC,
                                                         ARRAY_SIZE(LteMacConfiguration_LCConfig_LC, Fmt),
                                                         b, current_lc_offset, length - (current_lc_offset - start), lc_item_j);
                        if (lc_consumed == 0) break;
                        payload_consumed += lc_consumed;
                        current_lc_offset += lc_consumed;
                        lc_list_j.push_back(lc_item_j);
                    }
                    subpkt_j["added/modified LC List"] = lc_list_j;


                    int lc_list_actual_consumed = current_lc_offset - lc_list_start_offset;
                    int padding_size = 290 - lc_list_actual_consumed;
                    if (padding_size > 0 && current_lc_offset + padding_size <= start + length) {
                        payload_consumed += padding_size;
                    } else if (padding_size < 0) {
                        LOGD("Warning: LC list consumed more than 290 bytes in MAC Config LC subpkt v%d", subpkt_ver);
                    }
                    success = (lc_header_consumed > 0);
                }
                break;
            case 13:
                if (subpkt_ver == 1 || subpkt_ver == 2) {
                    const Fmt* embms_fmt = (subpkt_ver == 1) ? LteMacConfigurationSubpkt_eMBMSConfig : LteMacConfigurationSubpkt_eMBMSConfig_v2;
                    size_t embms_fmt_size = (subpkt_ver == 1) ? ARRAY_SIZE(LteMacConfigurationSubpkt_eMBMSConfig, Fmt) : ARRAY_SIZE(LteMacConfigurationSubpkt_eMBMSConfig_v2, Fmt);
                    payload_consumed = _decode_by_fmt(embms_fmt, embms_fmt_size, b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    success = (payload_consumed > 0);
                }
                break;
            case 14:
                if (subpkt_ver == 1) {
                    int all_rach_header_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_All_Rach_Config,
                                                                  ARRAY_SIZE(LteMacConfigurationSubpkt_All_Rach_Config, Fmt),
                                                                  b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    payload_consumed += all_rach_header_consumed;
                    int current_cell_offset = payload_offset_start + all_rach_header_consumed;

                    json cells_j = json::array();

                    for (int k = 0; k < 8; ++k) {
                        json cell_item_j;
                        int cell_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_All_Rach_Config_Cell_Info,
                                                           ARRAY_SIZE(LteMacConfigurationSubpkt_All_Rach_Config_Cell_Info, Fmt),
                                                           b, current_cell_offset, length - (current_cell_offset - start), cell_item_j);
                        if (cell_consumed == 0) break;


                        if (cell_item_j.find("Preamble initial power (dB)") != cell_item_j.end()) {
                            uint16_t raw_power_idx_16 = cell_item_j["Preamble initial power (dB)"];
                            cell_item_j["Preamble initial power (dB)"] = -120 + (2 * raw_power_idx_16);
                        }
                        if (cell_item_j.find("PMax (dBm)") != cell_item_j.end() && cell_item_j["PMax (dBm)"].is_number()) {
                            int16_t pmax_signed = static_cast<int16_t>(cell_item_j["PMax (dBm)"].get<unsigned int>());
                            cell_item_j["PMax (dBm)"] = static_cast<int>(pmax_signed);
                        }
                        if (cell_item_j.find("Delta preamble Msg3") != cell_item_j.end() && cell_item_j["Delta preamble Msg3"].is_number()) {
                            int16_t delta_signed = static_cast<int16_t>(cell_item_j["Delta preamble Msg3"].get<unsigned int>());
                            cell_item_j["Delta preamble Msg3"] = delta_signed -1;
                        }

                        cells_j.push_back(cell_item_j);
                        payload_consumed += cell_consumed;
                        current_cell_offset += cell_consumed;
                    }
                    subpkt_j["Cells"] = cells_j;
                    success = (all_rach_header_consumed > 0);
                }
                break;
            case 18:
                if (subpkt_ver == 1) {
                    payload_consumed = _decode_by_fmt(LteMacConfigurationSubpkt_ELS,
                                                      ARRAY_SIZE(LteMacConfigurationSubpkt_ELS, Fmt),
                                                      b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    success = (payload_consumed > 0);
                }
                break;
            default:
                LOGD("(MI)Unknown LTE MAC Configuration Subpacket ID: 0x%x", subpkt_id);

                int remaining_payload = subpkt_size - header_consumed;
                if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                    payload_consumed = remaining_payload;
                } else {
                    LOGD("Cannot skip unknown MAC Config subpacket payload reliably.");
                }
                break;
        }

        if (!success) {
            LOGD("(MI)Failed to decode or unknown version for LTE MAC Config Subpacket ID: 0x%x, Version: %d", subpkt_id, subpkt_ver);


            int remaining_payload = subpkt_size - header_consumed - payload_consumed;
            if (remaining_payload > 0 && payload_offset_start + payload_consumed + remaining_payload <= start + length) {
                payload_consumed += remaining_payload;
            }
        }


        offset = payload_offset_start + payload_consumed;


        int total_consumed_in_subpacket = offset - start_subpkt;
        if (total_consumed_in_subpacket != subpkt_size) {
            LOGD("Subpacket %d v%d: Consumed %d bytes, but expected size %d. Adjusting offset.", subpkt_id, subpkt_ver, total_consumed_in_subpacket, subpkt_size);
            offset = start_subpkt + subpkt_size;
            if (offset > start + length) {
                LOGD("Error: Adjusted offset exceeds buffer length. Stopping.");
                return offset - start;
            }
        }

        subpackets_j.push_back(subpkt_j);
    }

    j["Subpackets"] = subpackets_j;
    return offset - start;
}
