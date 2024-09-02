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

static int _decode_lte_mac_ul_bufferstatusinternal_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_MAC_UL_Buffer_Status_Internal: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Num SubPkt") != j.end() && j["Num SubPkt"].is_number()) {
        n_subpkt = j["Num SubPkt"].get<int>();
    } else {
        LOGD("LTE_MAC_UL_Buffer_Status_Internal: Missing or invalid 'Num SubPkt'");
        j["error"] = "Missing Num SubPkt in main header";
        return 0;
    }

   // LOGD("Decoding LTE_MAC_UL_Buffer_Status_Internal with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;
                bool break_outer_loop_flag = false;

                int header_consumed = _decode_by_fmt(LteMacULBufferStatusInternal_SubpktHeaderFmt,
                                                     ARRAY_SIZE(LteMacULBufferStatusInternal_SubpktHeaderFmt, Fmt),
                                                     b, offset, length, subpkt_j);

                int subpkt_id = -1, subpkt_payload_ver = -1, subpkt_size = 0, subpkt_nsample = 0;
                bool header_ok = false;

                if (header_consumed > 0 &&
                    subpkt_j.find("SubPacket ID") != subpkt_j.end() && subpkt_j["SubPacket ID"].is_number() &&
                    subpkt_j.find("Version") != subpkt_j.end() && subpkt_j["Version"].is_number() &&
                    subpkt_j.find("SubPacket Size") != subpkt_j.end() && subpkt_j["SubPacket Size"].is_number() &&
                    subpkt_j.find("Num Samples") != subpkt_j.end() && subpkt_j["Num Samples"].is_number()) {
                    subpkt_id = subpkt_j["SubPacket ID"].get<int>();
                    subpkt_payload_ver = subpkt_j["Version"].get<int>();
                    subpkt_size = subpkt_j["SubPacket Size"].get<int>();
                    subpkt_nsample = subpkt_j["Num Samples"].get<int>();
                    if (subpkt_size >= header_consumed && start_subpkt + subpkt_size <= start + length) {
                        header_ok = true;
                    }
                }

                if (!header_ok) {
                    LOGD("LTE_MAC_UL_Buffer_Status_Internal: Error decoding subpacket %d header or missing fields/invalid size.", i);
                    if (subpkt_j.find("SubPacket Size") != subpkt_j.end() && subpkt_j["SubPacket Size"].is_number()){
                        int skip_size_check = subpkt_j["SubPacket Size"].get<int>();
                        if(start_subpkt + skip_size_check <= start + length && skip_size_check > 0) {
                            offset = start_subpkt + skip_size_check;
                        } else { return offset - start;}
                    } else { return offset - start; }
                    continue;
                }
                offset += header_consumed;

                const char* subpkt_type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                           ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                           subpkt_id);
                subpkt_j["SubPacket ID"] = (subpkt_type_name ? subpkt_type_name : ("Unknown (" + std::to_string(subpkt_id) + ")"));

                int payload_offset_start = offset;
                bool subpacket_processed_successfully = false;

                if (subpkt_type_name != nullptr) {
                    json samples_j = json::array();
                    int current_sample_data_offset = payload_offset_start;

                    for (int k = 0; k < subpkt_nsample; ++k) {
                        json sample_j;
                        int sample_consumed = 0;
                        const Fmt* sample_fmt_lcid = nullptr;
                        size_t sample_fmt_lcid_size = 0;

                        const Fmt* current_sample_fmt_array = nullptr;
                        size_t current_sample_fmt_size = 0;

                        if (subpkt_payload_ver == 3) {
                            current_sample_fmt_array = LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt;
                            current_sample_fmt_size = ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt, Fmt);
                            sample_fmt_lcid = LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt;
                            sample_fmt_lcid_size = ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt, Fmt);
                        } else if (subpkt_payload_ver == 24) {
                            current_sample_fmt_array = LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v24;
                            current_sample_fmt_size = ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v24, Fmt);
                            sample_fmt_lcid = LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v24;
                            sample_fmt_lcid_size = ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v24, Fmt);
                        } else if (subpkt_payload_ver == 5) {
                            current_sample_fmt_array = LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v5;
                            current_sample_fmt_size = ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v5, Fmt);
                            sample_fmt_lcid = LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v5;
                            sample_fmt_lcid_size = ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v5, Fmt);
                        } else {
                            LOGD("(MI)Unknown LTE MAC UL Buffer Status Subpacket payload version: ID 0x%x - Ver %d", subpkt_id, subpkt_payload_ver);
                            break_outer_loop_flag = true;
                            break;
                        }

                        sample_consumed = _decode_by_fmt(current_sample_fmt_array, current_sample_fmt_size,
                                                         b, current_sample_data_offset, length - (current_sample_data_offset - start), sample_j);

                        if (sample_consumed == 0) {
                            LOGD("Error decoding sample %d for subpacket ID %d, payload ver %d", k, subpkt_id, subpkt_payload_ver);
                            offset = start_subpkt + subpkt_size;
                            break_outer_loop_flag = true;
                            break;
                        }
                        current_sample_data_offset += sample_consumed;

                        int num_active_lcid = 0;
                        if (sample_j.find("Number of active LCID") != sample_j.end() && sample_j["Number of active LCID"].is_number()) {
                            num_active_lcid = sample_j["Number of active LCID"];
                        }

                        if (sample_j.find("Sub FN") != sample_j.end() && sample_j["Sub FN"].is_number()) {
                            unsigned int temp_combined_sfn = sample_j["Sub FN"].get<unsigned int>();

                            //LOGD("Raw SFN/SubFN field for sample %d: 0x%04X (%u)", k, temp_combined_sfn, temp_combined_sfn);

                            sample_j["Sub FN"] = temp_combined_sfn & 15;
                            sample_j["Sys FN"] = (temp_combined_sfn >> 4) & 1023;
                        }


                        json lcids_j = json::array();
                        bool break_lcid_loop = false;
                        for (int l = 0; l < num_active_lcid; ++l) {
                            json lcid_item_j;
                            int lcid_consumed = _decode_by_fmt(sample_fmt_lcid, sample_fmt_lcid_size,
                                                               b, current_sample_data_offset, length - (current_sample_data_offset - start), lcid_item_j);
                            if (lcid_consumed == 0) {
                                LOGD("Error decoding LCID item %d for sample %d", l, k);
                                break_lcid_loop = true;
                                break;
                            }
                            current_sample_data_offset += lcid_consumed;

                            if (subpkt_payload_ver == 24 || subpkt_payload_ver == 5) {
                                unsigned int new_uncompressed = lcid_item_j.value("New Uncompressed Bytes", 0u);
                                unsigned int new_compressed = lcid_item_j.value("New Compressed Bytes", 0u);
                                unsigned int retx_bytes = lcid_item_j.value("Retx bytes", 0u);
                                int ctrl_bytes = lcid_item_j.value("Ctrl bytes", 0);
                                lcid_item_j["Total Bytes"] = new_uncompressed + new_compressed + retx_bytes + ctrl_bytes;
                            }
                            lcids_j.push_back(lcid_item_j);
                        }
                        if(break_lcid_loop){
                            offset = start_subpkt + subpkt_size;
                            break_outer_loop_flag = true;
                            break;
                        }
                        sample_j["LCIDs"] = lcids_j;
                        samples_j.push_back(sample_j);
                        subpacket_processed_successfully = true;
                    }
                    if (break_outer_loop_flag) break;

                    subpkt_j["Samples"] = samples_j;
                    offset = current_sample_data_offset;
                } else {
                    LOGD("(MI)Skipping payload for Unknown LTE MAC Uplink Buffer Status Subpacket ID: 0x%x", subpkt_id);
                    int payload_to_skip = subpkt_size - header_consumed;
                    if (payload_to_skip > 0 && offset + payload_to_skip <= start + length) {
                        offset += payload_to_skip;
                    } else if (payload_to_skip < 0) { LOGD("Error: payload_to_skip is negative."); }
                }

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
                subpackets_j.push_back(subpkt_j);
                if (break_outer_loop_flag) break;
            }
            j["Subpackets"] = subpackets_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE MAC Uplink Buffer Status Internal packet version: 0x%x", pkt_ver);
            return 0;
    }
}