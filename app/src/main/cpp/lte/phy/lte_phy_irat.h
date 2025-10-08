//
// Created by Vladimir Ponomarenko on 24.05.2025.
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


static int _decode_lte_phy_irat_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PHY_IRAT_Measurement_Request: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Subpacket count") != j.end() && j["Subpacket count"].is_number()) {
        n_subpkt = j["Subpacket count"].get<int>();
    } else {
        LOGD("LTE_PHY_IRAT_Measurement_Request: Missing or invalid 'Subpacket count'");
        j["error"] = "Missing Subpacket count in main header";
        return 0;
    }

   // LOGD("Decoding LTE_PHY_IRAT_Measurement_Request with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_array_j = json::array();

            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LtePhyIratSubPktFmt,
                                                     ARRAY_SIZE(LtePhyIratSubPktFmt, Fmt),
                                                     b, offset, length, subpkt_j);

                int subpkt_id_val = -1;
                // int subpkt_payload_ver_val = -1;
                int subpkt_size_val = 0;
                bool subpkt_header_ok = false;

                if (header_consumed > 0 &&
                    subpkt_j.find("Subpacket ID") != subpkt_j.end() && subpkt_j["Subpacket ID"].is_number() &&
                    subpkt_j.find("Version") != subpkt_j.end() && subpkt_j["Version"].is_number() &&
                    subpkt_j.find("Subpacket size") != subpkt_j.end() && subpkt_j["Subpacket size"].is_number()) {
                    subpkt_id_val = subpkt_j["Subpacket ID"].get<int>();
                    subpkt_size_val = subpkt_j["Subpacket size"].get<int>();
                    if (subpkt_size_val >= header_consumed && start_subpkt + subpkt_size_val <= start + length) {
                        subpkt_header_ok = true;
                    }
                }

                if (!subpkt_header_ok) {
                    LOGD("LTE_PHY_IRAT: Error decoding subpacket %d header or invalid size. Header consumed: %d", i, header_consumed);
                    if (subpkt_j.find("Subpacket size") != subpkt_j.end() && subpkt_j["Subpacket size"].is_number()) {
                        int skip_size_check = subpkt_j["Subpacket size"].get<int>();
                        if (start_subpkt + skip_size_check <= start + length && skip_size_check >= header_consumed && skip_size_check > 0) {
                            LOGD("Attempting to skip %d bytes for malformed subpacket header.", skip_size_check);
                            offset = start_subpkt + skip_size_check;
                        } else {
                            LOGD("Cannot reliably skip subpacket with malformed header, stopping. Skip size: %d, Header consumed: %d, Remaining: %zu", skip_size_check, header_consumed, length - (offset - start));
                            return offset - start;
                        }
                    } else {
                        LOGD("Cannot determine subpacket size for malformed header, stopping.");
                        return offset - start;
                    }
                    continue;
                }
                int payload_for_subpkt_offset = offset;

                json current_subpacket_content_j;

                switch (subpkt_id_val) {
                    case LtePhyIratType_WCDMA: {
                        current_subpacket_content_j["Type"] = "WCDMA";
                        int wcdma_payload_consumed = 0;
                        int wcdma_header_consumed = _decode_by_fmt(LtePhyIratWCDMAFmt,
                                                                   ARRAY_SIZE(LtePhyIratWCDMAFmt, Fmt),
                                                                   b, payload_for_subpkt_offset, length - (payload_for_subpkt_offset - start), current_subpacket_content_j);
                        wcdma_payload_consumed += wcdma_header_consumed;
                        int current_wcdma_offset = payload_for_subpkt_offset + wcdma_header_consumed;

                        int n_freq = 0;
                        if (current_subpacket_content_j.find("Number of frequencies") != current_subpacket_content_j.end() && current_subpacket_content_j["Number of frequencies"].is_number()){
                            n_freq = current_subpacket_content_j["Number of frequencies"].get<int>();
                        }
                        json frequencies_j = json::array();
                        for (int freq_idx = 0; freq_idx < n_freq; ++freq_idx) {
                            json freq_item_j;
                            int freq_meta_consumed = _decode_by_fmt(LtePhyIratWCDMACellMetaFmt,
                                                                    ARRAY_SIZE(LtePhyIratWCDMACellMetaFmt, Fmt),
                                                                    b, current_wcdma_offset, length - (current_wcdma_offset - start), freq_item_j);
                            wcdma_payload_consumed += freq_meta_consumed;
                            current_wcdma_offset += freq_meta_consumed;

                            int n_cell = 0;
                            if (freq_item_j.find("Number of cells") != freq_item_j.end() && freq_item_j["Number of cells"].is_number()) {
                                n_cell = freq_item_j["Number of cells"].get<int>();
                            }
                            json cells_j = json::array();
                            for (int cell_idx = 0; cell_idx < n_cell; ++cell_idx) {
                                json cell_item_j;
                                int cell_data_consumed = _decode_by_fmt(LtePhyIratWCDMACellFmt,
                                                                        ARRAY_SIZE(LtePhyIratWCDMACellFmt, Fmt),
                                                                        b, current_wcdma_offset, length - (current_wcdma_offset - start), cell_item_j);
                                wcdma_payload_consumed += cell_data_consumed;
                                current_wcdma_offset += cell_data_consumed;
                                cells_j.push_back(cell_item_j);
                            }
                            freq_item_j["Cells"] = cells_j;
                            frequencies_j.push_back(freq_item_j);
                        }
                        current_subpacket_content_j["Frequencies"] = frequencies_j;
                        offset = start_subpkt + header_consumed + wcdma_payload_consumed;
                        break;
                    }
                    default: {
                        int payload_to_skip = subpkt_size_val - header_consumed;
                        if (payload_to_skip > 0) {
                            if (payload_for_subpkt_offset + payload_to_skip <= start + length) {
                                LOGD("(MI)Unknown LTE PHY IRAT Subpacket ID: 0x%x. Skipping %d payload bytes.", subpkt_id_val, payload_to_skip);
                                offset = payload_for_subpkt_offset + payload_to_skip;
                            } else {
                                LOGD("Error: Cannot skip %d payload bytes for unknown subpacket ID %d, not enough data. Skipping what's left.", payload_to_skip, subpkt_id_val);
                                offset = start + length;
                            }
                        } else if (payload_to_skip < 0) {
                            LOGD("Error: payload_to_skip is negative (%d) for unknown subpacket ID %d. Size: %d, Header: %d", payload_to_skip, subpkt_id_val, subpkt_size_val, header_consumed);
                            offset = payload_for_subpkt_offset;
                        } else {

                            offset = payload_for_subpkt_offset;
                        }

                        current_subpacket_content_j["Type"] = "UnknownOrSkipped";
                        current_subpacket_content_j["ID_Hex"] = subpkt_id_val;
                        current_subpacket_content_j["SkippedPayloadBytes"] = payload_to_skip > 0 ? payload_to_skip : 0;
                        break;
                    }
                }

                subpkt_j["Content"] = current_subpacket_content_j;
                subpackets_array_j.push_back(subpkt_j);

                int current_subpkt_end_offset = start_subpkt + subpkt_size_val;
                if (offset != current_subpkt_end_offset) {
                    if (current_subpkt_end_offset <= start + length && current_subpkt_end_offset > offset) {
                       // LOGD("Subpacket ID %d: Actual consumed %d, SubPktSize %d. Adjusting offset to %d.", subpkt_id_val, (offset - start_subpkt), subpkt_size_val, current_subpkt_end_offset);
                        offset = current_subpkt_end_offset;
                    } else if (current_subpkt_end_offset < offset) {
                        LOGD("Subpacket ID %d: Consumed %d, EXCEEDED SubPktSize %d. Offset already at %d.", subpkt_id_val, (offset - start_subpkt), subpkt_size_val, offset);
                    } else {
                        LOGD("Error: Subpacket ID %d reported size %d exceeds buffer length. Current offset %d.", subpkt_id_val, subpkt_size_val, offset);
                    }
                }
            }
            j["Subpackets"] = subpackets_array_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY IRAT Measurement Request packet version: 0x%x", pkt_ver);
            return 0;
    }
}