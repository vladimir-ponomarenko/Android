//
// Created by Vladimir Ponomarenko on 18.05.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_mac_rach_attempt_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_MAC_Rach_Attempt: Missing or invalid 'Version'");
        return 0;
    }
    if (j.find("Number of Subpackets") != j.end() && j["Number of Subpackets"].is_number()) {
        n_subpkt = j["Number of Subpackets"].get<int>();
    } else {
        LOGD("LTE_MAC_Rach_Attempt: Missing or invalid 'Number of Subpackets'");
        return 0;
    }

   // LOGD("Decoding LTE_MAC_Rach_Attempt with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LteMacRachAttempt_SubpktHeader,
                                                     ARRAY_SIZE(LteMacRachAttempt_SubpktHeader, Fmt),
                                                     b, offset, length, subpkt_j);

                if (header_consumed == 0 ||
                    subpkt_j.find("Subpacket ID") == subpkt_j.end() || !subpkt_j["Subpacket ID"].is_number() ||
                    subpkt_j.find("Subpacket Version") == subpkt_j.end() || !subpkt_j["Subpacket Version"].is_number() ||
                    subpkt_j.find("Subpacket Size") == subpkt_j.end() || !subpkt_j["Subpacket Size"].is_number()) {
                    LOGD("LTE_MAC_Rach_Attempt: Error decoding subpacket %d header or missing fields.", i);

                    if (subpkt_j.find("Subpacket Size") != subpkt_j.end() && subpkt_j["Subpacket Size"].is_number()) {
                        int skip_size = subpkt_j["Subpacket Size"].get<int>();
                        if (start_subpkt + skip_size <= start + length && skip_size > 0) {
                            offset = start_subpkt + skip_size;
                            LOGD("Skipped subpacket based on reported size: %d", skip_size);
                        } else {
                            LOGD("Cannot reliably skip subpacket, stopping.");
                            return offset-start;
                        }
                    } else {
                        LOGD("Cannot determine subpacket size, stopping.");
                        return offset - start;
                    }
                    continue;
                }
                offset += header_consumed;

                int subpkt_id = subpkt_j["Subpacket ID"];
                int subpkt_ver = subpkt_j["Subpacket Version"];
                int subpkt_size = subpkt_j["Subpacket Size"];
                int payload_offset_start = offset;
                int payload_consumed = 0;


                if (subpkt_id == 6) {
                    if (subpkt_ver == 2) {
                        payload_consumed = _decode_by_fmt(LteMacRachAttempt_SubpktPayload,
                                                          ARRAY_SIZE(LteMacRachAttempt_SubpktPayload, Fmt),
                                                          b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    } else if (subpkt_ver == 3) {
                        payload_consumed = _decode_by_fmt(LteMacRachAttempt_SubpktPayload_v3,
                                                          ARRAY_SIZE(LteMacRachAttempt_SubpktPayload_v3, Fmt),
                                                          b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    } else if (subpkt_ver == 4) {
                        payload_consumed = _decode_by_fmt(LteMacRachAttempt_SubpktPayload_v4,
                                                          ARRAY_SIZE(LteMacRachAttempt_SubpktPayload_v4, Fmt),
                                                          b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                    } else {
                        LOGD("(MI)Unknown LTE MAC RACH Attempt Subpkt version for ID 6: %d", subpkt_ver);
                        int remaining_payload = subpkt_size - header_consumed;
                        if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                            payload_consumed = remaining_payload;
                        }
                    }

                    if (payload_consumed > 0) {
                        offset += payload_consumed;

                        const char* rach_res_name = search_name(LteMacRachAttempt_Subpkt_RachResult, ARRAY_SIZE(LteMacRachAttempt_Subpkt_RachResult, ValueName), subpkt_j["Rach result"]);
                        subpkt_j["Rach result"] = (rach_res_name ? rach_res_name : "(MI)Unknown");

                        const char* contention_name = search_name(LteMacRachAttempt_Subpkt_ContentionProcedure, ARRAY_SIZE(LteMacRachAttempt_Subpkt_ContentionProcedure, ValueName), subpkt_j["Contention procedure"]);
                        subpkt_j["Contention procedure"] = (contention_name ? contention_name : "(MI)Unknown");

                        if (subpkt_j.find("Rach msg bmasks") != subpkt_j.end() && subpkt_j["Rach msg bmasks"].is_number()) {
                            int iRachMsgBMasks = subpkt_j["Rach msg bmasks"];
                            int current_msg_offset = offset;

                            if ((iRachMsgBMasks) & (1 << 2)) {
                                json msg1_j;
                                int msg1_consumed = 0;
                                if (subpkt_ver == 3) {
                                    msg1_consumed = _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg1_v3, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg1_v3, Fmt), b, current_msg_offset, length - (current_msg_offset-start), msg1_j);
                                } else if (subpkt_ver == 4) {
                                    msg1_consumed = _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg1_v4, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg1_v4, Fmt), b, current_msg_offset, length - (current_msg_offset-start), msg1_j);
                                    if (msg1_j.find("Preamble index mask") != msg1_j.end() && msg1_j["Preamble index mask"] == 0xff) {
                                        const char* mask_name = search_name(LteMacRachAttempt_Subpkt_Preamble_index_mask, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Preamble_index_mask, ValueName), 0xff);
                                        msg1_j["Preamble index mask"] = (mask_name ? mask_name : "0xff");
                                    }
                                    if (msg1_j.find("CE Level") != msg1_j.end()){
                                        const char* ce_name = search_name(LteMacRachAttempt_Subpkt_CELEVEL, ARRAY_SIZE(LteMacRachAttempt_Subpkt_CELEVEL, ValueName), msg1_j["CE Level"]);
                                        msg1_j["CE Level"] = (ce_name ? ce_name : "(MI)Unknown");
                                    }
                                } else {
                                    msg1_consumed = _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg1, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg1, Fmt), b, current_msg_offset, length - (current_msg_offset-start), msg1_j);
                                }
                                if(msg1_consumed > 0) subpkt_j["Msg1"] = msg1_j;
                                offset += msg1_consumed;
                                current_msg_offset += msg1_consumed;
                            }

                            if ((iRachMsgBMasks) & (1 << 1)) {
                                json msg2_j;
                                int msg2_consumed = _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg2, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg2, Fmt), b, current_msg_offset, length - (current_msg_offset-start), msg2_j);
                                if(msg2_consumed > 0){
                                    const char* msg2_res_name = search_name(LteMacRachAttempt_Subpkt_Msg2_Result, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg2_Result, ValueName), msg2_j["Result"]);
                                    msg2_j["Result"] = (msg2_res_name ? msg2_res_name : "False");
                                    subpkt_j["Msg2"] = msg2_j;
                                }
                                offset += msg2_consumed;
                                current_msg_offset += msg2_consumed;
                            }

                            if ((iRachMsgBMasks) & 1) {
                                json msg3_j;
                                int msg3_header_consumed = 0;
                                if(subpkt_ver == 4){
                                    msg3_header_consumed = _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg3_v4, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg3_v4, Fmt), b, current_msg_offset, length - (current_msg_offset-start), msg3_j);
                                    if (msg3_j.find("Grant Raw") != msg3_j.end() && msg3_j["Grant Raw"].is_number()){
                                        unsigned int iGrantRaw = msg3_j["Grant Raw"];
                                        std::stringstream ss_grant;
                                        ss_grant << "0x"
                                                 << std::hex << std::setfill('0') << std::setw(2) << ((iGrantRaw >> 8) & 0x0F)
                                                 << std::hex << std::setfill('0') << std::setw(2) << (iGrantRaw & 0xFF)
                                                 << std::hex << std::setfill('0') << std::setw(2) << ((iGrantRaw >> 16) & 0xFF);
                                        msg3_j["Grant Raw"] = ss_grant.str();
                                    }
                                } else {
                                    msg3_header_consumed = _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg3, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg3, Fmt), b, current_msg_offset, length - (current_msg_offset-start), msg3_j);
                                }


                                if(msg3_header_consumed > 0) {
                                    offset += msg3_header_consumed;
                                    current_msg_offset += msg3_header_consumed;
                                    int iGrantBytes = 0;
                                    if(msg3_j.find("Grant") != msg3_j.end() && msg3_j["Grant"].is_number()){
                                        iGrantBytes = msg3_j["Grant"];
                                    }

                                    json mac_pdus_j = json::array();

                                    for (int k = 0; k < iGrantBytes; ++k) {
                                        json mac_pdu_item_j;
                                        int pdu_consumed = _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg3_MACPDU, ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg3_MACPDU, Fmt), b, current_msg_offset, length - (current_msg_offset-start), mac_pdu_item_j);
                                        if (pdu_consumed == 0) {
                                            LOGD("Error decoding MAC PDU item %d for Msg3", k);
                                            break;
                                        }
                                        mac_pdus_j.push_back(mac_pdu_item_j);
                                        offset += pdu_consumed;
                                        current_msg_offset += pdu_consumed;
                                    }
                                    msg3_j["MAC PDUs"] = mac_pdus_j;
                                    subpkt_j["Msg3"] = msg3_j;
                                }
                            }
                        }
                    }
                } else {
                    LOGD("(MI)Unknown LTE MAC RACH Attempt Subpkt id: 0x%x", subpkt_id);
                    int remaining_payload = subpkt_size - header_consumed;
                    if (remaining_payload > 0 && payload_offset_start + remaining_payload <= start + length) {
                        payload_consumed = remaining_payload;
                    }
                    offset = payload_offset_start + payload_consumed;
                }


                int total_consumed_in_subpacket = offset - start_subpkt;
                if (total_consumed_in_subpacket != subpkt_size) {
                    LOGD("Subpacket %d v%d: Consumed %d bytes, but expected size %d. Adjusting offset.", subpkt_id, subpkt_ver, total_consumed_in_subpacket, subpkt_size);
                    offset = start_subpkt + subpkt_size;
                    if (offset > start + length) {
                        LOGD("Error: Adjusted offset for subpacket %d v%d exceeds buffer length. Stopping.", subpkt_id, subpkt_ver);
                        return offset - start;
                    }
                }
                subpackets_j.push_back(subpkt_j);
            }
            j["Subpackets"] = subpackets_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE MAC RACH Attempt Packet Version: 0x%x", pkt_ver);
            return 0;
    }
}