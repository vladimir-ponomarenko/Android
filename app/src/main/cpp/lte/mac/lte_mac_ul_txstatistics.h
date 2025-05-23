//
// Created by Vladimir Ponomarenko on 19.05.2025.
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


static int _decode_lte_mac_ul_txstatistics_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_MAC_UL_Tx_Statistics: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num SubPkt") != j.end() && j["Num SubPkt"].is_number()) {
        n_subpkt = j["Num SubPkt"].get<int>();
    } else {
        LOGD("LTE_MAC_UL_Tx_Statistics: Missing or invalid 'Num SubPkt'");
        j["error"] = "Missing Num SubPkt in main header";
        return 0;
    }

    LOGD("Decoding LTE_MAC_UL_Tx_Statistics with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LteMacULTxStatistics_SubpktHeaderFmt,
                                                     ARRAY_SIZE(LteMacULTxStatistics_SubpktHeaderFmt, Fmt),
                                                     b, offset, length, subpkt_j);

                int subpkt_id = -1, subpkt_payload_ver = -1, subpkt_size = 0;
                bool header_ok = false;

                if (header_consumed > 0 &&
                    subpkt_j.find("SubPacket ID") != subpkt_j.end() && subpkt_j["SubPacket ID"].is_number() &&
                    subpkt_j.find("Version") != subpkt_j.end() && subpkt_j["Version"].is_number() &&
                    subpkt_j.find("SubPacket Size") != subpkt_j.end() && subpkt_j["SubPacket Size"].is_number()) {
                    subpkt_id = subpkt_j["SubPacket ID"].get<int>();
                    subpkt_payload_ver = subpkt_j["Version"].get<int>();
                    subpkt_size = subpkt_j["SubPacket Size"].get<int>();
                    if (subpkt_size >= header_consumed && start_subpkt + subpkt_size <= start + length) {
                        header_ok = true;
                    }
                }

                if (!header_ok) {
                    LOGD("LTE_MAC_UL_Tx_Statistics: Error decoding subpacket %d header or missing fields/invalid size.", i);
                    if (subpkt_j.find("SubPacket Size") != subpkt_j.end() && subpkt_j["SubPacket Size"].is_number()) {
                        int skip_size_check = subpkt_j["SubPacket Size"].get<int>();
                        if (start_subpkt + skip_size_check <= start + length && skip_size_check > 0) {
                            offset = start_subpkt + skip_size_check;
                        } else { return offset - start; }
                    } else { return offset - start; }
                    continue;
                }
                offset += header_consumed;

                const char* subpkt_type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                           ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                           subpkt_id);

                subpkt_j["SubPacket ID"] = (subpkt_type_name ? subpkt_type_name : ("Unknown (" + std::to_string(subpkt_id) + ")"));

                int payload_offset_start = offset;
                int payload_consumed = 0;
                bool subpacket_decoded_successfully = false;

                if (subpkt_type_name != nullptr) {

                    json sample_data_j;

                    switch (subpkt_payload_ver) {
                        case 1: {
                            payload_consumed = _decode_by_fmt(LteMacULTxStatistics_ULTxStatsSubPacketFmt,
                                                              ARRAY_SIZE(LteMacULTxStatistics_ULTxStatsSubPacketFmt, Fmt),
                                                              b, payload_offset_start, length - (payload_offset_start - start), sample_data_j);
                            if (payload_consumed > 0) {
                                subpkt_j["Sample"] = sample_data_j;
                                subpacket_decoded_successfully = true;
                            }
                            break;
                        }
                        case 2: {
                            payload_consumed = _decode_by_fmt(LteMacULTxStatistics_ULTxStatsSubPacketFmtV2,
                                                              ARRAY_SIZE(LteMacULTxStatistics_ULTxStatsSubPacketFmtV2, Fmt),
                                                              b, payload_offset_start, length - (payload_offset_start - start), sample_data_j);
                            if (payload_consumed > 0) {
                                subpkt_j["Sample"] = sample_data_j;
                                subpacket_decoded_successfully = true;
                            }
                            break;
                        }
                        default:
                            LOGD("(MI)Unknown LTE MAC Uplink Tx Statistics Subpacket payload version: ID 0x%x - Ver %d", subpkt_id, subpkt_payload_ver);
                            payload_consumed = subpkt_size - header_consumed;
                            if (payload_consumed < 0) payload_consumed = 0;
                            break;
                    }
                } else {
                    LOGD("(MI)Skipping payload for Unknown LTE MAC Uplink Tx Statistics Subpacket ID: 0x%x", subpkt_id);
                    payload_consumed = subpkt_size - header_consumed;
                    if (payload_consumed < 0) payload_consumed = 0;
                }

                offset = payload_offset_start + payload_consumed;


                int total_consumed_by_subpkt = offset - start_subpkt;
                if (total_consumed_by_subpkt != subpkt_size) {
                    LOGD("Subpacket %s (ID %d, Ver %d): Consumed %d, SubPktSize %d. Adjusting offset.",
                         (subpkt_type_name ? subpkt_type_name : "Unknown"), subpkt_id, subpkt_payload_ver,
                         total_consumed_by_subpkt, subpkt_size);
                    offset = start_subpkt + subpkt_size;
                    if (offset > start + length) {
                        LOGD("Error: Adjusted offset for subpacket ID %d Ver %d exceeds buffer length. Stopping.", subpkt_id, subpkt_payload_ver);
                        return offset - start;
                    }
                }

                if (subpacket_decoded_successfully || subpkt_type_name == nullptr) {
                    subpackets_j.push_back(subpkt_j);
                }
            }
            j["Subpackets"] = subpackets_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE MAC Uplink Tx Statistics packet version: 0x%x", pkt_ver);
            return 0;
    }
}
