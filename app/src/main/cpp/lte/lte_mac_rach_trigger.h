//
// Created by Vladimir Ponomarenko on 13.04.2025.
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



const Fmt LteMacRachTriggerFmt[] = {
        {UINT, "Version", 1},
        {UINT, "Number of Subpackets", 1},
        {SKIP, NULL, 2},
};

const Fmt LteMacRachTrigger_SubpktHeader[] = {
        {UINT, "Subpacket ID", 1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size", 2},
};



const Fmt LteMacRachTrigger_RachConfigSubpktPayload[] = {
        {WCDMA_MEAS, "Preamble initial power (dB)", 1},
        {SKIP, NULL, 1},
        {UINT, "Power ramping step (dB)", 1},
        {UINT, "RA index1", 1},
        {UINT, "RA index2", 1},
        {UINT, "Preamble trans max", 1},
        {UINT, "Contention resolution timer (ms)", 2},
        {UINT, "Message size Group_A", 2},
        {UINT, "Power offset Group_B", 1},
        {UINT, "PMax (dBm)", 2},
        {UINT, "Delta preamble Msg3", 2},
        {UINT, "PRACH config", 1},
        {UINT, "CS zone length", 1},
        {UINT, "Root seq index", 2},
        {UINT, "PRACH Freq Offset", 1},
        {PLACEHOLDER, "Preamble Format", 0},
        {UINT, "High speed flag", 1},
        {UINT, "Max retx Msg3", 1},
        {UINT, "RA rsp win size", 1},
};


const Fmt LteMacRachTrigger_RachConfigSubpktPayload_v4[] = {
        {UINT, "Sub Id", 1},
        {UINT, "Num Active Cells", 1},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_v4_cell[] = {
        {UINT, "Cell Id", 1},
        {WCDMA_MEAS, "Preamble initial power (dB)", 1},
        {SKIP, NULL, 1},
        {UINT, "Power ramping step (dB)", 1},
        {UINT, "RA index1", 1},
        {UINT, "RA index2", 1},
        {UINT, "Preamble trans max", 1},
        {UINT, "Contention resolution timer (ms)", 2},
        {UINT, "Message size Group_A", 2},
        {UINT, "Power offset Group_B", 1},
        {UINT, "PMax (dBm)", 2},
        {UINT, "Delta preamble Msg3", 2},
        {UINT, "PRACH config", 1},
        {UINT, "CS zone length", 1},
        {UINT, "Root seq index", 2},
        {UINT, "PRACH Freq Offset", 1},
        {PLACEHOLDER, "Preamble Format", 0},
        {UINT, "High speed flag", 1},
        {UINT, "Max retx Msg3", 1},
        {UINT, "RA rsp win size", 1},
};


const Fmt LteMacRachTrigger_RachConfigSubpktPayload_v5[] = {
        {UINT, "Preamble initial power (dB)", 2},
        {UINT, "Power ramping step (dB)", 1},
        {UINT, "RA index1", 1},
        {UINT, "RA index2", 1},
        {UINT, "Preamble trans max", 1},
        {UINT, "Contention resolution timer (ms)", 2},
        {UINT, "Message size Group_A", 2},
        {UINT, "Power offset Group_B", 1},
        {UINT, "PMax (dBm)", 2},
        {UINT, "Delta preamble Msg3", 2},
        {UINT, "PRACH config", 1},
        {UINT, "CS zone length", 1},
        {UINT, "Root seq index", 2},
        {UINT, "PRACH Freq Offset", 1},
        {PLACEHOLDER, "Preamble Format", 0},
        {UINT, "High speed flag", 1},
        {UINT, "Max retx Msg3", 1},
        {UINT, "RA rsp win size", 1},
        {UINT, "PRACH Cfg R13 Present", 1},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_rsrp_prach_list_size_v5[] = {
        {UINT, "RSRP Thresh PRACH List Size", 1},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_rsrp_prach_list_v5[] = {
        {UINT, "RSRP Thresh PRACH List", 1},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_hidden_rsrp_prach_list_v5[] = {
        {UINT, "Hidden RSRP Thresh PRACH List", 1},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_prach_param_ce_list_size_v5[] = {
        {UINT, "PRACH Param CE List Size", 1},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_prach_list_v5[] = {
        {UINT,"First Preamble", 1},
        {UINT,"Last Preamble", 1},
        {UINT,"Max Preamble Tx Attempt Per CE", 1},
        {UINT,"Contention Resol Timer", 2},
        {UINT,"Prach Cfg Index", 1},
        {UINT,"RA RSP Win Size", 1},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_hidden_prach_list_v5[] = {
        {BYTE_STREAM, "Hidden PRACH Param Ce", 7},
};
const Fmt LteMacRachTrigger_RachConfigSubpktPayload_prach_last_part[] = {
        {UINT, "Initial CE Level", 2},
        {UINT, "Preamble Trans Max CE", 2},
};



const Fmt LteMacRachTrigger_RachReasonSubpktPayload[] = {
        {UINT, "Rach reason", 1},
        {PLACEHOLDER, "RACH Contention", 0},
        {UINT, "Maching ID", 8},
        {PLACEHOLDER, "Preamble", 0},
        {BYTE_STREAM, "Preamble RA mask", 1},
        {UINT, "Msg3 size", 1},
        {UINT, "Group chosen", 1},
        {UINT, "Radio condn (dB)", 1},
        {BYTE_STREAM_LITTLE_ENDIAN, "CRNTI", 2},
};

const Fmt LteMacRachTrigger_RachReasonSubpktPayload_v2[] = {
        {UINT, "Sub Id", 1},
        {UINT, "Cell Id", 1},
        {UINT, "Rach reason", 1},
        {BYTE_STREAM, "Maching ID", 8},
        {PLACEHOLDER, "RACH Contention", 0},
        {PLACEHOLDER, "Preamble", 0},
        {BYTE_STREAM, "Preamble RA mask", 1},
        {UINT, "Msg3 size", 1},
        {UINT, "Group chosen", 1},
        {UINT, "Radio condn (dB)", 1},
        {BYTE_STREAM_LITTLE_ENDIAN, "CRNTI", 2},
};

const Fmt LteMacRachTrigger_RachReasonSubpktPayload_v3[] = {
        {UINT, "Rach reason", 1},
        {PLACEHOLDER, "RACH Contention", 0},
        {UINT, "Maching ID", 6},
        {PLACEHOLDER, "Preamble", 0},
        {BYTE_STREAM, "Preamble RA mask", 1},
        {UINT, "Msg3 size", 1},
        {UINT, "CE Level", 1},
        {UINT, "Radio condn (dB)", 1},
        {BYTE_STREAM_LITTLE_ENDIAN, "CRNTI", 2},
        {SKIP, NULL, 1},
};


const ValueName LteMacRachTrigger_RachReasonSubpkt_RachReason[] = { {0, "CONNECTION_REQ"}, {1, "RLF"}, {2, "UL_DATA"}, {3, "DL_DATA"}, {4, "HO"}, };
const ValueName LteMacRachTrigger_RachReasonSubpkt_GroupChosen[] = { {0, "Group A(0)"}, {1, "Group B(1)"}, };
const ValueName ValueNameRachContention[] = { {0, "NonContention Based RACH procedure"}, {1, "Contention Based RACH procedure"}, };




static int _decode_lte_mac_rach_trigger_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    int n_subpkt = 0;
    if (j.find("Number of Subpackets") != j.end() && j["Number of Subpackets"].is_number()) {
        n_subpkt = j["Number of Subpackets"].get<int>();
    } else {
        LOGD("Error: Missing or invalid 'Number of Subpackets' in main header.");
        return 0;
    }


    if (pkt_ver != 1) {
        LOGD("(MI)Unsupported LTE MAC RACH Trigger Packet Version: 0x%x", pkt_ver);
        return 0;
    }

    json subpackets_j = json::array();
    for (int i = 0; i < n_subpkt; ++i) {
        json subpkt_j;
        int start_subpkt = offset;

        int header_consumed = _decode_by_fmt(LteMacRachTrigger_SubpktHeader,
                                             ARRAY_SIZE(LteMacRachTrigger_SubpktHeader, Fmt),
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
            }
        }

        if (!header_ok) {
            LOGD("Error decoding subpacket header or insufficient data/invalid size. Subpacket index %d", i);
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

        if (subpkt_id == 3) {
            if (subpkt_ver == 2) {
                payload_consumed = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload,
                                                  ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload, Fmt),
                                                  b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);





                if (subpkt_j.find("PMax (dBm)") != subpkt_j.end() && subpkt_j["PMax (dBm)"].is_number()) {
                    int16_t pmax_signed = static_cast<int16_t>(subpkt_j["PMax (dBm)"].get<unsigned int>());
                    subpkt_j["PMax (dBm)"] = static_cast<int>(pmax_signed);
                }

                if (subpkt_j.find("Delta preamble Msg3") != subpkt_j.end() && subpkt_j["Delta preamble Msg3"].is_number()) {
                    int16_t delta_signed = static_cast<int16_t>(subpkt_j["Delta preamble Msg3"].get<unsigned int>());
                    subpkt_j["Delta preamble Msg3"] = delta_signed -1;
                }

                subpkt_j["Preamble Format"] = 0;

            } else if (subpkt_ver == 4) {
                int header_v4_consumed = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_v4,
                                                        ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_v4, Fmt),
                                                        b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                payload_consumed += header_v4_consumed;
                int current_cell_offset = payload_offset_start + header_v4_consumed;

                int num_Cells = 0;
                if (subpkt_j.find("Num Active Cells") != subpkt_j.end() && subpkt_j["Num Active Cells"].is_number()) {
                    num_Cells = subpkt_j["Num Active Cells"].get<int>();
                }

                json cells_j = json::array();
                for (int cell_idx = 0; cell_idx < num_Cells; ++cell_idx) {
                    json cell_item_j;
                    int cell_consumed = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_v4_cell,
                                                       ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_v4_cell, Fmt),
                                                       b, current_cell_offset, length - (current_cell_offset - start), cell_item_j);

                    if (cell_consumed == 0) {
                        LOGD("Error decoding cell %d in RACH Config v4", cell_idx);
                        break;
                    }



                    if (cell_item_j.find("PMax (dBm)") != cell_item_j.end() && cell_item_j["PMax (dBm)"].is_number()) {
                        int16_t pmax_signed = static_cast<int16_t>(cell_item_j["PMax (dBm)"].get<unsigned int>());
                        cell_item_j["PMax (dBm)"] = static_cast<int>(pmax_signed);
                    }
                    if (cell_item_j.find("Delta preamble Msg3") != cell_item_j.end() && cell_item_j["Delta preamble Msg3"].is_number()) {
                        int16_t delta_signed = static_cast<int16_t>(cell_item_j["Delta preamble Msg3"].get<unsigned int>());
                        cell_item_j["Delta preamble Msg3"] = delta_signed -1;
                    }

                    cell_item_j["Preamble Format"] = 0;
                    cells_j.push_back(cell_item_j);
                    payload_consumed += cell_consumed;
                    current_cell_offset += cell_consumed;
                }
                subpkt_j["Cells"] = cells_j;
            } else if (subpkt_ver == 5) {
                int v5_header_consumed = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_v5,
                                                        ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_v5, Fmt),
                                                        b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                payload_consumed += v5_header_consumed;
                int current_r13_offset = payload_offset_start + v5_header_consumed;


                if (subpkt_j.find("Preamble initial power (dB)") != subpkt_j.end() && subpkt_j["Preamble initial power (dB)"].is_number()) {
                    uint16_t raw_power_index_16 = subpkt_j["Preamble initial power (dB)"];
                    subpkt_j["Preamble initial power (dB)"] = -120 + (2 * raw_power_index_16);
                }
                if (subpkt_j.find("PMax (dBm)") != subpkt_j.end() && subpkt_j["PMax (dBm)"].is_number()) {
                    int16_t pmax_signed = static_cast<int16_t>(subpkt_j["PMax (dBm)"].get<unsigned int>());
                    subpkt_j["PMax (dBm)"] = static_cast<int>(pmax_signed);
                }
                if (subpkt_j.find("Delta preamble Msg3") != subpkt_j.end() && subpkt_j["Delta preamble Msg3"].is_number()) {
                    int16_t delta_signed = static_cast<int16_t>(subpkt_j["Delta preamble Msg3"].get<unsigned int>());
                    subpkt_j["Delta preamble Msg3"] = delta_signed -1;
                }

                if (subpkt_j.find("Power offset Group_B") != subpkt_j.end() && subpkt_j["Power offset Group_B"] == 0) {
                    subpkt_j["Power offset Group_B"] = "- Infinity";
                }

                if (subpkt_j.find("PRACH config") != subpkt_j.end() && subpkt_j["PRACH config"].is_number()) {
                    int prach_cfg = subpkt_j["PRACH config"];
                    if (prach_cfg < 16) {
                        subpkt_j["Preamble Format"] = 0;
                    } else if (prach_cfg == 30) {
                        subpkt_j["Preamble Format"] = "N/A";
                    } else {
                        subpkt_j["Preamble Format"] = 1;
                    }
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
                    int consumed_size = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_rsrp_prach_list_size_v5,
                                                       ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_rsrp_prach_list_size_v5, Fmt),
                                                       b, current_r13_offset, length - (current_r13_offset - start), rsrp_size_j);
                    if (consumed_size == 0) goto skip_r13;
                    payload_consumed += consumed_size;
                    current_r13_offset += consumed_size;
                    int rsrp_list_size = rsrp_size_j["RSRP Thresh PRACH List Size"];
                    prach_cfg_r13_j["RSRP Thresh PRACH List Size"] = rsrp_list_size;

                    json rsrp_list_j = json::array();
                    for (int k = 0; k < rsrp_list_size; ++k) {
                        json rsrp_item_j;
                        int consumed_item = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_rsrp_prach_list_v5,
                                                           ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_rsrp_prach_list_v5, Fmt),
                                                           b, current_r13_offset, length - (current_r13_offset - start), rsrp_item_j);
                        if (consumed_item == 0) goto skip_r13_loops;
                        payload_consumed += consumed_item;
                        current_r13_offset += consumed_item;
                        if(rsrp_item_j.find("RSRP Thresh PRACH List") != rsrp_item_j.end() && rsrp_item_j["RSRP Thresh PRACH List"].is_number()) {
                            uint8_t rsrp_index = rsrp_item_j["RSRP Thresh PRACH List"];
                            rsrp_item_j["RSRP Thresh PRACH List (dBm)"] = rsrp_index - 140;
                            rsrp_item_j.erase("RSRP Thresh PRACH List");
                        }
                        rsrp_list_j.push_back(rsrp_item_j);
                    }
                    skip_r13_loops:;
                    prach_cfg_r13_j["RSRP Thresh PRACH List"] = rsrp_list_j;

                    int hidden_rsrp_count = 3 - rsrp_list_size;
                    if (hidden_rsrp_count > 0) {
                        int consumed_hidden = hidden_rsrp_count * 1;
                        if (current_r13_offset + consumed_hidden > start + length) goto skip_r13;
                        payload_consumed += consumed_hidden;
                        current_r13_offset += consumed_hidden;
                    }

                    json ce_size_j;
                    int consumed_ce_size = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_prach_param_ce_list_size_v5,
                                                          ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_prach_param_ce_list_size_v5, Fmt),
                                                          b, current_r13_offset, length - (current_r13_offset - start), ce_size_j);
                    if (consumed_ce_size == 0) goto skip_r13;
                    payload_consumed += consumed_ce_size;
                    current_r13_offset += consumed_ce_size;
                    int ce_list_size = ce_size_j["PRACH Param CE List Size"];
                    prach_cfg_r13_j["PRACH Param CE List Size"] = ce_list_size;

                    json ce_list_j = json::array();
                    for (int k = 0; k < ce_list_size; ++k) {
                        json ce_item_j;
                        int consumed_ce_item = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_prach_list_v5,
                                                              ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_prach_list_v5, Fmt),
                                                              b, current_r13_offset, length - (current_r13_offset - start), ce_item_j);
                        if (consumed_ce_item == 0) goto skip_r13_loops_ce;
                        payload_consumed += consumed_ce_item;
                        current_r13_offset += consumed_ce_item;
                        ce_list_j.push_back(ce_item_j);
                    }
                    skip_r13_loops_ce:;
                    prach_cfg_r13_j["PRACH Param Ce"] = ce_list_j;

                    int hidden_ce_count = 4 - ce_list_size;
                    if (hidden_ce_count > 0) {
                        int consumed_hidden_ce = hidden_ce_count * 7;
                        if (current_r13_offset + consumed_hidden_ce > start + length) goto skip_r13;
                        payload_consumed += consumed_hidden_ce;
                        current_r13_offset += consumed_hidden_ce;
                    }

                    json last_part_j;
                    int consumed_last_part = _decode_by_fmt(LteMacRachTrigger_RachConfigSubpktPayload_prach_last_part,
                                                            ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload_prach_last_part, Fmt),
                                                            b, current_r13_offset, length - (current_r13_offset - start), last_part_j);
                    if (consumed_last_part == 0) goto skip_r13;
                    payload_consumed += consumed_last_part;

                    if (last_part_j.find("Initial CE Level") != last_part_j.end() && last_part_j["Initial CE Level"] == 0xffff) {
                        last_part_j["Initial CE Level"] = "NA";
                    }
                    prach_cfg_r13_j.update(last_part_j);
                    subpkt_j["PRACH Cfg R13"] = prach_cfg_r13_j;
                }
                skip_r13:;

            } else {
                LOGD("(MI)Unknown RACH Config Subpacket Version: %d", subpkt_ver);

                int remaining_payload = subpkt_size - header_consumed;
                if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                    payload_consumed = remaining_payload;
                } else {
                    LOGD("Cannot skip unknown RACH Config subpacket payload reliably.");

                }
            }
        } else if (subpkt_id == 5) {
            if (subpkt_ver == 1) {
                payload_consumed = _decode_by_fmt(LteMacRachTrigger_RachReasonSubpktPayload,
                                                  ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpktPayload, Fmt),
                                                  b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);

                const char* reason_name = search_name(LteMacRachTrigger_RachReasonSubpkt_RachReason, ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpkt_RachReason, ValueName), subpkt_j["Rach reason"]);
                subpkt_j["Rach reason"] = (reason_name ? reason_name : "(MI)Unknown");
                const char* group_name = search_name(LteMacRachTrigger_RachReasonSubpkt_GroupChosen, ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpkt_GroupChosen, ValueName), subpkt_j["Group chosen"]);
                subpkt_j["Group chosen"] = (group_name ? group_name : "(MI)Unknown");

                if (subpkt_j.find("Maching ID") != subpkt_j.end() && subpkt_j["Maching ID"].is_number()) {
                    uint64_t maching_id_raw = subpkt_j["Maching ID"];
                    subpkt_j["Preamble"] = (maching_id_raw >> 56) & 0xFF;
                    std::stringstream ss;
                    ss << "0x" << std::hex << std::setfill('0') << std::setw(2) << ((maching_id_raw >> 40) & 0xFF);
                    ss << ",0x" << std::hex << std::setfill('0') << std::setw(2) << ((maching_id_raw >> 32) & 0xFF);
                    ss << ",0x" << std::hex << std::setfill('0') << std::setw(2) << ((maching_id_raw >> 24) & 0xFF);
                    ss << ",0x" << std::hex << std::setfill('0') << std::setw(2) << ((maching_id_raw >> 16) & 0xFF);
                    ss << ",0x" << std::hex << std::setfill('0') << std::setw(2) << ((maching_id_raw >> 8) & 0xFF);
                    ss << ",0x" << std::hex << std::setfill('0') << std::setw(2) << (maching_id_raw & 0xFF);
                    subpkt_j["Maching ID"] = ss.str();
                } else {
                    subpkt_j["Preamble"] = "(Error)";
                    subpkt_j["Maching ID"] = "(Error)";
                }


                subpkt_j["RACH Contention"] = "Contention Based RACH procedure";

                if (subpkt_j.find("Radio condn (dB)") != subpkt_j.end() && subpkt_j["Radio condn (dB)"].is_number()) {
                    int8_t radio_condn_signed = static_cast<int8_t>(subpkt_j["Radio condn (dB)"].get<unsigned int>());
                    subpkt_j["Radio condn (dB)"] = static_cast<int>(radio_condn_signed);
                }

            } else if (subpkt_ver == 2) {
                payload_consumed = _decode_by_fmt(LteMacRachTrigger_RachReasonSubpktPayload_v2,
                                                  ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpktPayload_v2, Fmt),
                                                  b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);

                const char* reason_name = search_name(LteMacRachTrigger_RachReasonSubpkt_RachReason, ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpkt_RachReason, ValueName), subpkt_j["Rach reason"]);
                subpkt_j["Rach reason"] = (reason_name ? reason_name : "(MI)Unknown");

                bool is_contention = true;
                if (subpkt_j.find("Maching ID") != subpkt_j.end() && subpkt_j["Maching ID"].is_string()) {
                    std::string mach_id_str = subpkt_j["Maching ID"];
                    if (mach_id_str == "0x0000000000000000") {
                        is_contention = false;
                    }
                }
                const char* contention_name = search_name(ValueNameRachContention, ARRAY_SIZE(ValueNameRachContention, ValueName), is_contention ? 1 : 0);
                subpkt_j["RACH Contention"] = (contention_name ? contention_name : "(MI)Unknown");


                const char* group_name = search_name(LteMacRachTrigger_RachReasonSubpkt_GroupChosen, ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpkt_GroupChosen, ValueName), subpkt_j["Group chosen"]);
                subpkt_j["Group chosen"] = (group_name ? group_name : "(MI)Unknown");

                if (subpkt_j.find("Radio condn (dB)") != subpkt_j.end() && subpkt_j["Radio condn (dB)"].is_number()) {
                    int8_t radio_condn_signed = static_cast<int8_t>(subpkt_j["Radio condn (dB)"].get<unsigned int>());
                    subpkt_j["Radio condn (dB)"] = static_cast<int>(radio_condn_signed);
                }

            } else if (subpkt_ver == 3) {
                payload_consumed = _decode_by_fmt(LteMacRachTrigger_RachReasonSubpktPayload_v3,
                                                  ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpktPayload_v3, Fmt),
                                                  b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                const char* reason_name = search_name(LteMacRachTrigger_RachReasonSubpkt_RachReason, ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpkt_RachReason, ValueName), subpkt_j["Rach reason"]);
                subpkt_j["Rach reason"] = (reason_name ? reason_name : "(MI)Unknown");

                if (subpkt_j.find("Radio condn (dB)") != subpkt_j.end() && subpkt_j["Radio condn (dB)"].is_number()) {
                    int8_t radio_condn_signed = static_cast<int8_t>(subpkt_j["Radio condn (dB)"].get<unsigned int>());
                    subpkt_j["Radio condn (dB)"] = static_cast<int>(radio_condn_signed);
                }

            } else {
                LOGD("(MI)Unknown RACH Reason Subpacket Version: %d", subpkt_ver);
                int remaining_payload = subpkt_size - header_consumed;
                if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                    payload_consumed = remaining_payload;
                } else {
                    LOGD("Cannot skip unknown RACH Reason subpacket payload reliably.");
                }
            }
        } else {
            LOGD("(MI)Unknown LTE MAC RACH Trigger subpkt id: 0x%x", subpkt_id);

            int remaining_payload = subpkt_size - header_consumed;
            if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                payload_consumed = remaining_payload;
            } else {
                LOGD("Cannot skip unknown subpacket payload reliably.");
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