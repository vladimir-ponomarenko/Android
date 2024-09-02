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
#include "log_packet.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_phy_pdcch_decoding_result_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PHY_PDCCH_Decoding_Result: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
   // LOGD("Decoding LTE_PHY_PDCCH_Decoding_Result with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 21: {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v21,
                                     ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v21, Fmt),
                                     b, offset, length, j);

            int non_decode_p1 = j["Subframe Number"];
            j["Subframe Number"] = non_decode_p1 & 15;
            j["System Frame Number"] = (non_decode_p1 >> 4) & 1023;

            int non_decode_p2 = j["Demback Mode Select"];
            j["Demback Mode Select"] = non_decode_p2 & 15;
            j["Carrier Index"] = (non_decode_p2 >> 4) & 15;
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            int num_hypothesis = 0;
            if(j.find("Number of Hypothesis") != j.end() && j["Number of Hypothesis"].is_number()) {
                num_hypothesis = j["Number of Hypothesis"].get<int>();
            }

            json hypothesis_list_j = json::array();
            for (int k = 0; k < num_hypothesis; ++k) {
                json hypothesis_item_j;
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v21,
                                         ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v21, Fmt),
                                         b, offset, length, hypothesis_item_j);

                unsigned int non_decode_p3 = hypothesis_item_j["Aggregation Level"];
                hypothesis_item_j["Aggregation Level"] = non_decode_p3 & 3;
                const char* agg_lvl_name = search_name(ValueNameAggregationLevel, ARRAY_SIZE(ValueNameAggregationLevel, ValueName), hypothesis_item_j["Aggregation Level"]);
                hypothesis_item_j["Aggregation Level"] = (agg_lvl_name ? agg_lvl_name : "(MI)Unknown");

                hypothesis_item_j["Candidate"] = (non_decode_p3 >> 2) & 7;
                hypothesis_item_j["Search Space Type"] = (non_decode_p3 >> 5) & 1;
                const char* search_space_name = search_name(ValueNameSearchSpaceType, ARRAY_SIZE(ValueNameSearchSpaceType, ValueName), hypothesis_item_j["Search Space Type"]);
                hypothesis_item_j["Search Space Type"] = (search_space_name ? search_space_name : "(MI)Unknown");

                hypothesis_item_j["DCI Format"] = (non_decode_p3 >> 6) & 15;
                const char* dci_fmt_name = search_name(ValueNameDCIFormat, ARRAY_SIZE(ValueNameDCIFormat, ValueName), hypothesis_item_j["DCI Format"]);
                hypothesis_item_j["DCI Format"] = (dci_fmt_name ? dci_fmt_name : "(MI)Unknown");

                hypothesis_item_j["Decode Status"] = (non_decode_p3 >> 10) & 15;
                const char* decode_stat_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), hypothesis_item_j["Decode Status"]);
                hypothesis_item_j["Decode Status"] = (decode_stat_name ? decode_stat_name : "(MI)Unknown");

                hypothesis_item_j["Start CCE"] = (non_decode_p3 >> 14) & 127;
                hypothesis_item_j["Payload Size"] = (non_decode_p3 >> 21) & 255;
                hypothesis_item_j["Tail Match"] = (non_decode_p3 >> 29) & 1;
                const char* tail_match_name = search_name(ValueNameMatchOrNot, ARRAY_SIZE(ValueNameMatchOrNot, ValueName), hypothesis_item_j["Tail Match"]);
                hypothesis_item_j["Tail Match"] = (tail_match_name ? tail_match_name : "(MI)Unknown");

                const char* prune_name = search_name(ValueNamePruneStatus, ARRAY_SIZE(ValueNamePruneStatus, ValueName), hypothesis_item_j["Prune Status"]);
                hypothesis_item_j["Prune Status"] = (prune_name ? prune_name : "(MI)Unknown");

                if (hypothesis_item_j.find("Norm Energy Metric") != hypothesis_item_j.end() && hypothesis_item_j["Norm Energy Metric"].is_number()) {
                    hypothesis_item_j["Norm Energy Metric"] = hypothesis_item_j["Norm Energy Metric"].get<unsigned int>() / 65535.0;
                }
                if (hypothesis_item_j.find("Symbol Error Rate") != hypothesis_item_j.end() && hypothesis_item_j["Symbol Error Rate"].is_number()) {
                    hypothesis_item_j["Symbol Error Rate"] = hypothesis_item_j["Symbol Error Rate"].get<unsigned int>() / 2147483648.0;
                }
                hypothesis_list_j.push_back(hypothesis_item_j);
            }
            j["Hypothesis"] = hypothesis_list_j;
            break;
        }
        case 24: {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v21,
                                     ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v21, Fmt),
                                     b, offset, length, j);

            int non_decode_p1 = j["Subframe Number"];
            j["Subframe Number"] = non_decode_p1 & 15;
            j["System Frame Number"] = (non_decode_p1 >> 4) & 1023;

            int non_decode_p2 = j["Demback Mode Select"];
            j["Demback Mode Select"] = non_decode_p2 & 15;
            j["Carrier Index"] = (non_decode_p2 >> 4) & 15;
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            int num_hypothesis = 0;
            if(j.find("Number of Hypothesis") != j.end() && j["Number of Hypothesis"].is_number()) {
                num_hypothesis = j["Number of Hypothesis"].get<int>();
            }

            json hypothesis_list_j = json::array();
            for (int k = 0; k < num_hypothesis; ++k) {
                json hypothesis_item_j;
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v24,
                                         ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v24, Fmt),
                                         b, offset, length, hypothesis_item_j);

                unsigned int non_decode_p3 = hypothesis_item_j["Aggregation Level"];
                hypothesis_item_j["Aggregation Level"] = non_decode_p3 & 7;
                const char* agg_lvl_name = search_name(ValueNameAggregationLevel, ARRAY_SIZE(ValueNameAggregationLevel, ValueName), hypothesis_item_j["Aggregation Level"]);
                hypothesis_item_j["Aggregation Level"] = (agg_lvl_name ? agg_lvl_name : "(MI)Unknown");

                hypothesis_item_j["Candidate"] = (non_decode_p3 >> 3) & 7;
                hypothesis_item_j["Search Space Type"] = (non_decode_p3 >> 6) & 3;
                const char* search_space_name = search_name(ValueNameSearchSpaceType, ARRAY_SIZE(ValueNameSearchSpaceType, ValueName), hypothesis_item_j["Search Space Type"]);
                hypothesis_item_j["Search Space Type"] = (search_space_name ? search_space_name : "(MI)Unknown");

                hypothesis_item_j["DCI Format"] = (non_decode_p3 >> 8) & 15;
                const char* dci_fmt_name = search_name(ValueNameDCIFormat, ARRAY_SIZE(ValueNameDCIFormat, ValueName), hypothesis_item_j["DCI Format"]);
                hypothesis_item_j["DCI Format"] = (dci_fmt_name ? dci_fmt_name : "(MI)Unknown");

                hypothesis_item_j["Decode Status"] = (non_decode_p3 >> 12) & 15;
                const char* decode_stat_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), hypothesis_item_j["Decode Status"]);
                hypothesis_item_j["Decode Status"] = (decode_stat_name ? decode_stat_name : "(MI)Unknown");

                hypothesis_item_j["Start CCE"] = (non_decode_p3 >> 16) & 127;
                hypothesis_item_j["Payload Size"] = (non_decode_p3 >> 23) & 255;
                hypothesis_item_j["Tail Match"] = (non_decode_p3 >> 31) & 1;
                const char* tail_match_name = search_name(ValueNameMatchOrNot, ARRAY_SIZE(ValueNameMatchOrNot, ValueName), hypothesis_item_j["Tail Match"]);
                hypothesis_item_j["Tail Match"] = (tail_match_name ? tail_match_name : "(MI)Unknown");

                const char* prune_name = search_name(ValueNamePruneStatus, ARRAY_SIZE(ValueNamePruneStatus, ValueName), hypothesis_item_j["Prune Status"]);
                hypothesis_item_j["Prune Status"] = (prune_name ? prune_name : "(MI)Unknown");

                unsigned int rmax_val = hypothesis_item_j["Rmax"];
                hypothesis_item_j["Rmax"] = rmax_val & 0xf;
                hypothesis_item_j["Hypothesis Repetition"] = (rmax_val >> 4) & 0xf;

                unsigned int decoded_rep_val = hypothesis_item_j["Decoded Repetition"];
                hypothesis_item_j["Decoded Repetition"] = decoded_rep_val & 0xf;
                hypothesis_item_j["Mpdcch Group"] = (decoded_rep_val >> 4) & 0xf;


                if (hypothesis_item_j.find("Norm Energy Metric") != hypothesis_item_j.end() && hypothesis_item_j["Norm Energy Metric"].is_number()) {
                    hypothesis_item_j["Norm Energy Metric"] = hypothesis_item_j["Norm Energy Metric"].get<unsigned int>() / 65535.0;
                }
                if (hypothesis_item_j.find("Symbol Error Rate") != hypothesis_item_j.end() && hypothesis_item_j["Symbol Error Rate"].is_number()) {
                    hypothesis_item_j["Symbol Error Rate"] = hypothesis_item_j["Symbol Error Rate"].get<unsigned int>() / 2147483648.0;
                }
                hypothesis_list_j.push_back(hypothesis_item_j);
            }
            j["Hypothesis"] = hypothesis_list_j;
            break;
        }
        case 42:
        case 101:
        case 121:
        case 123: {
            const Fmt* payload_fmt = nullptr;
            const Fmt* hypothesis_fmt = nullptr;
            size_t payload_fmt_size = 0;
            size_t hypothesis_fmt_size = 0;

            if (pkt_ver == 42) { payload_fmt = LtePhyPdcchDecodingResult_Payload_v42; payload_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v42, Fmt); hypothesis_fmt = LtePhyPdcchDecodingResult_Hypothesis_v42; hypothesis_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v42, Fmt); }
            else if (pkt_ver == 101) { payload_fmt = LtePhyPdcchDecodingResult_Payload_v101; payload_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v101, Fmt); hypothesis_fmt = LtePhyPdcchDecodingResult_Hypothesis_v101; hypothesis_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v101, Fmt); }
            else if (pkt_ver == 121) { payload_fmt = LtePhyPdcchDecodingResult_Payload_v121; payload_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v121, Fmt); hypothesis_fmt = LtePhyPdcchDecodingResult_Hypothesis_v121; hypothesis_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v121, Fmt); }
            else { /* pkt_ver == 123 */ payload_fmt = LtePhyPdcchDecodingResult_Payload_v123; payload_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v123, Fmt); hypothesis_fmt = LtePhyPdcchDecodingResult_Hypothesis_v123; hypothesis_fmt_size = ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v123, Fmt); }


            offset += _decode_by_fmt(payload_fmt, payload_fmt_size, b, offset, length, j);

            unsigned int temp_sfn_data = j["Subframe Number"];
            j["Subframe Number"] = temp_sfn_data & 15;
            j["System Frame Number"] = (temp_sfn_data >> 4) & 1023;
            int bw_val = (temp_sfn_data >> 14) & 7;
            j["Band Width (MHz)"] = (bw_val > 0) ? (bw_val - 1) * 5 : 0;
            j["CIF Configured"] = (temp_sfn_data >> 17) & 1;
            const char* cif_name = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), j["CIF Configured"]);
            j["CIF Configured"] = (cif_name ? cif_name : "(MI)Unknown");
            j["Two bits CSI Configured"] = (temp_sfn_data >> 18) & 1;
            const char* tbcc_name = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), j["Two bits CSI Configured"]);
            j["Two bits CSI Configured"] = (tbcc_name ? tbcc_name : "(MI)Unknown");
            j["Aperiodic SRS Configured"] = (temp_sfn_data >> 19) & 1;
            const char* asc_name = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), j["Aperiodic SRS Configured"]);
            j["Aperiodic SRS Configured"] = (asc_name ? asc_name : "(MI)Unknown");
            j["Frame Structure"] = (temp_sfn_data >> 20) & 3;
            const char* frame_struct_name = search_name(ValueNameFrameStructure, ARRAY_SIZE(ValueNameFrameStructure, ValueName), j["Frame Structure"]);
            j["Frame Structure"] = (frame_struct_name ? frame_struct_name : "(MI)Unknown");
            j["Num eNB Antennas"] = (temp_sfn_data >> 22) & 3;
            const char* num_ant_name = search_name(ValueNameNumNBAntennas, ARRAY_SIZE(ValueNameNumNBAntennas, ValueName), j["Num eNB Antennas"]);
            j["Num eNB Antennas"] = (num_ant_name ? num_ant_name : "(MI)Unknown");
            j["DL CP"] = (temp_sfn_data >> 24) & 3;
            const char* dlcp_name = search_name(ValueNameNormalOrNot, ARRAY_SIZE(ValueNameNormalOrNot, ValueName), j["DL CP"]);
            j["DL CP"] = (dlcp_name ? dlcp_name : "(MI)Unknown");
            j["SSC"] = (temp_sfn_data >> 26) & 3;
            j["CA FDD TDD"] = (temp_sfn_data >> 28) & 3;

            if (pkt_ver == 121 || pkt_ver == 123) {
                unsigned int pad_val_raw = j["__pad"];
                j["__pad"] = ((pad_val_raw << 1) + ((temp_sfn_data >> 31) & 1)) & 1;
            }


            int temp_dms_data = j["Demback Mode Select"];
            j["Demback Mode Select"] = temp_dms_data & 15;
            j["Carrier Index"] = (temp_dms_data >> 4) & 15;
            const char* carrier_name_v42 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v42 ? carrier_name_v42 : "(MI)Unknown");

            int num_hypothesis_v42 = 0;
            if(j.find("Number of Hypothesis") != j.end() && j["Number of Hypothesis"].is_number()){
                num_hypothesis_v42 = j["Number of Hypothesis"].get<int>();
            }

            json hypothesis_list_v42_j = json::array();
            for (int k = 0; k < num_hypothesis_v42; ++k) {
                json hypothesis_item_j;
                offset += _decode_by_fmt(hypothesis_fmt, hypothesis_fmt_size,
                                         b, offset, length, hypothesis_item_j);

                unsigned int agg_lvl_raw = hypothesis_item_j["Aggregation Level"];
                hypothesis_item_j["Aggregation Level"] = agg_lvl_raw & 3;
                const char* agg_lvl_name_v42 = search_name(ValueNameAggregationLevel, ARRAY_SIZE(ValueNameAggregationLevel, ValueName), hypothesis_item_j["Aggregation Level"]);
                hypothesis_item_j["Aggregation Level"] = (agg_lvl_name_v42 ? agg_lvl_name_v42 : "(MI)Unknown");
                hypothesis_item_j["Candidate"] = (agg_lvl_raw >> 2) & 7;
                hypothesis_item_j["Search Space Type"] = (agg_lvl_raw >> 5) & 1;
                const char* search_space_name_v42 = search_name(ValueNameSearchSpaceType, ARRAY_SIZE(ValueNameSearchSpaceType, ValueName), hypothesis_item_j["Search Space Type"]);
                hypothesis_item_j["Search Space Type"] = (search_space_name_v42 ? search_space_name_v42 : "(MI)Unknown");
                hypothesis_item_j["DCI Format"] = (agg_lvl_raw >> 6) & 15;
                const char* dci_fmt_name_v42 = search_name(ValueNameDCIFormat, ARRAY_SIZE(ValueNameDCIFormat, ValueName), hypothesis_item_j["DCI Format"]);
                hypothesis_item_j["DCI Format"] = (dci_fmt_name_v42 ? dci_fmt_name_v42 : "(MI)Unknown");
                hypothesis_item_j["Decode States"] = (agg_lvl_raw >> 10) & 15;
                const char* decode_stat_name_v42 = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), hypothesis_item_j["Decode States"]);
                hypothesis_item_j["Decode States"] = (decode_stat_name_v42 ? decode_stat_name_v42 : "(MI)Unknown");
                hypothesis_item_j["Payload Size"] = (agg_lvl_raw >> 14) & 255;
                hypothesis_item_j["Tail Match"] = (agg_lvl_raw >> 22) & 1;
                const char* tail_match_name_v42 = search_name(ValueNameMatchOrNot, ARRAY_SIZE(ValueNameMatchOrNot, ValueName), hypothesis_item_j["Tail Match"]);
                hypothesis_item_j["Tail Match"] = (tail_match_name_v42 ? tail_match_name_v42 : "(MI)Unknown");
                hypothesis_item_j["Non Zero Symbol Mismatch Count"] = (agg_lvl_raw >> 23) & 511;

                unsigned int start_cce_raw = hypothesis_item_j["Start CCE"];
                if(pkt_ver == 121 || pkt_ver == 123){
                    hypothesis_item_j["Alt TBS Enabled"] = start_cce_raw & 1;
                }
                hypothesis_item_j["Start CCE"] = (start_cce_raw >> 1) & 127;
                hypothesis_item_j["Non Zero Llr Count"] = (start_cce_raw >> 8) & 511;
                hypothesis_item_j["Normal"] = (start_cce_raw >> 17) & 32767;


                unsigned int prune_stat_raw = hypothesis_item_j["Prune Status"];
                hypothesis_item_j["Prune Status"] = prune_stat_raw & 2047;
                const char* prune_name_v42 = search_name(ValueNamePruneStatus, ARRAY_SIZE(ValueNamePruneStatus, ValueName), hypothesis_item_j["Prune Status"]);
                hypothesis_item_j["Prune Status"] = (prune_name_v42 ? prune_name_v42 : "(MI)Unknown");
                hypothesis_item_j["Energy Metric"] = prune_stat_raw >> 11;


                if (hypothesis_item_j.find("Norm Energy Metric") != hypothesis_item_j.end() && hypothesis_item_j["Norm Energy Metric"].is_number()) {
                    hypothesis_item_j["Norm Energy Metric"] = hypothesis_item_j["Norm Energy Metric"].get<unsigned int>() / 65535.0;
                }
                if (hypothesis_item_j.find("Symbol Error Rate") != hypothesis_item_j.end() && hypothesis_item_j["Symbol Error Rate"].is_number()) {
                    hypothesis_item_j["Symbol Error Rate"] = hypothesis_item_j["Symbol Error Rate"].get<unsigned int>() / 2147483648.0;
                }
                hypothesis_list_v42_j.push_back(hypothesis_item_j);
            }
            j["Hypothesis"] = hypothesis_list_v42_j;
            break;
        }
        case 43: {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v43,
                                     ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v43, Fmt),
                                     b, offset, length, j);

            unsigned int temp_num_rec = j["Num Records"];
            int num_records_v43 = temp_num_rec & 15;
            j["Subframe Number"] = (temp_num_rec >> 4) & 15;
            j["System Frame Number"] = (temp_num_rec >> 8) & 1023;
            int bw_val_v43 = (temp_num_rec >> 18) & 15;
            j["Band Width (MHz)"] = (bw_val_v43 > 0) ? (bw_val_v43 - 1) * 5 : 0;
            j["Frame Structure"] = (temp_num_rec >> 22) & 3;
            const char* frame_struct_name_v43 = search_name(ValueNameFrameStructure, ARRAY_SIZE(ValueNameFrameStructure, ValueName), j["Frame Structure"]);
            j["Frame Structure"] = (frame_struct_name_v43 ? frame_struct_name_v43 : "(MI)Unknown");
            j["Num eNB Antennas"] = (temp_num_rec >> 24) & 7;
            const char* num_ant_name_v43 = search_name(ValueNameNumNBAntennas, ARRAY_SIZE(ValueNameNumNBAntennas, ValueName), j["Num eNB Antennas"]);
            j["Num eNB Antennas"] = (num_ant_name_v43 ? num_ant_name_v43 : "(MI)Unknown");
            j.erase("Num Records");
            j["Number of Records"] = num_records_v43;

            int temp_dms_v43 = j["Demback Mode Select"];
            j["Demback Mode Select"] = temp_dms_v43 & 15;
            j["Carrier Index"] = (temp_dms_v43 >> 4) & 15;
            const char* carrier_name_v43 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v43 ? carrier_name_v43 : "(MI)Unknown");

            json sf_list_j = json::array();
            for (int rec_idx = 0; rec_idx < num_records_v43; ++rec_idx) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Record_v43,
                                         ARRAY_SIZE(LtePhyPdcchDecodingResult_Record_v43, Fmt),
                                         b, offset, length, record_item_j);

                unsigned int cif_combined = record_item_j["CIF Configured"];
                record_item_j["CIF Configured"] = cif_combined & 1;
                const char* cif_name_rec = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), record_item_j["CIF Configured"]);
                record_item_j["CIF Configured"] = (cif_name_rec ? cif_name_rec : "(MI)Unknown");
                record_item_j["Two bits CSI Configured"] = (cif_combined >> 1) & 1;
                const char* tbcc_name_rec = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), record_item_j["Two bits CSI Configured"]);
                record_item_j["Two bits CSI Configured"] = (tbcc_name_rec ? tbcc_name_rec : "(MI)Unknown");
                record_item_j["Aperiodic SRS Configured"] = (cif_combined >> 2) & 1;
                const char* asc_name_rec = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), record_item_j["Aperiodic SRS Configured"]);
                record_item_j["Aperiodic SRS Configured"] = (asc_name_rec ? asc_name_rec : "(MI)Unknown");
                int num_hypothesis_rec = (cif_combined >> 3) & 255;
                record_item_j["Number of Hypothesis"] = num_hypothesis_rec;


                json hypothesis_list_rec_j = json::array();
                for (int k = 0; k < num_hypothesis_rec; ++k) {
                    json hypothesis_item_j;
                    offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v43,
                                             ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v43, Fmt),
                                             b, offset, length, hypothesis_item_j);
                    unsigned int agg_lvl_raw_rec = hypothesis_item_j["Aggregation Level"];
                    hypothesis_item_j["Aggregation Level"] = agg_lvl_raw_rec & 3;
                    const char* agg_lvl_name_rec = search_name(ValueNameAggregationLevel, ARRAY_SIZE(ValueNameAggregationLevel, ValueName), hypothesis_item_j["Aggregation Level"]);
                    hypothesis_item_j["Aggregation Level"] = (agg_lvl_name_rec ? agg_lvl_name_rec : "(MI)Unknown");
                    hypothesis_item_j["Candidate"] = (agg_lvl_raw_rec >> 2) & 7;
                    hypothesis_item_j["Search Space Type"] = (agg_lvl_raw_rec >> 5) & 1;
                    const char* search_space_name_rec = search_name(ValueNameSearchSpaceType, ARRAY_SIZE(ValueNameSearchSpaceType, ValueName), hypothesis_item_j["Search Space Type"]);
                    hypothesis_item_j["Search Space Type"] = (search_space_name_rec ? search_space_name_rec : "(MI)Unknown");
                    hypothesis_item_j["DCI Format"] = (agg_lvl_raw_rec >> 6) & 15;
                    const char* dci_fmt_name_rec = search_name(ValueNameDCIFormat, ARRAY_SIZE(ValueNameDCIFormat, ValueName), hypothesis_item_j["DCI Format"]);
                    hypothesis_item_j["DCI Format"] = (dci_fmt_name_rec ? dci_fmt_name_rec : "(MI)Unknown");
                    hypothesis_item_j["Decode States"] = (agg_lvl_raw_rec >> 10) & 15;
                    const char* decode_stat_name_rec = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), hypothesis_item_j["Decode States"]);
                    hypothesis_item_j["Decode States"] = (decode_stat_name_rec ? decode_stat_name_rec : "(MI)Unknown");
                    hypothesis_item_j["Payload Size"] = (agg_lvl_raw_rec >> 14) & 255;
                    hypothesis_item_j["Tail Match"] = (agg_lvl_raw_rec >> 22) & 1;
                    const char* tail_match_name_rec = search_name(ValueNameMatchOrNot, ARRAY_SIZE(ValueNameMatchOrNot, ValueName), hypothesis_item_j["Tail Match"]);
                    hypothesis_item_j["Tail Match"] = (tail_match_name_rec ? tail_match_name_rec : "(MI)Unknown");
                    hypothesis_item_j["Non Zero Symbol Mismatch Count"] = (agg_lvl_raw_rec >> 23) & 511;

                    unsigned int start_cce_raw_rec = hypothesis_item_j["Start CCE"];
                    hypothesis_item_j["Start CCE"] = (start_cce_raw_rec >> 1) & 127;
                    hypothesis_item_j["Non Zero Llr Count"] = (start_cce_raw_rec >> 8) & 511;
                    hypothesis_item_j["Normal"] = (start_cce_raw_rec >> 17) & 32767;

                    unsigned int prune_stat_raw_rec = hypothesis_item_j["Prune Status"];
                    hypothesis_item_j["Prune Status"] = prune_stat_raw_rec & 2047;
                    const char* prune_name_rec = search_name(ValueNamePruneStatus, ARRAY_SIZE(ValueNamePruneStatus, ValueName), hypothesis_item_j["Prune Status"]);
                    hypothesis_item_j["Prune Status"] = (prune_name_rec ? prune_name_rec : "(MI)Unknown");
                    hypothesis_item_j["Energy Metric"] = prune_stat_raw_rec >> 11;

                    if (hypothesis_item_j.find("Norm Energy Metric") != hypothesis_item_j.end() && hypothesis_item_j["Norm Energy Metric"].is_number()) {
                        hypothesis_item_j["Norm Energy Metric"] = hypothesis_item_j["Norm Energy Metric"].get<unsigned int>() / 65535.0;
                    }
                    if (hypothesis_item_j.find("Symbol Error Rate") != hypothesis_item_j.end() && hypothesis_item_j["Symbol Error Rate"].is_number()) {
                        hypothesis_item_j["Symbol Error Rate"] = hypothesis_item_j["Symbol Error Rate"].get<unsigned int>() / 2147483648.0;
                    }
                    hypothesis_list_rec_j.push_back(hypothesis_item_j);
                }
                record_item_j["Hypothesis"] = hypothesis_list_rec_j;
                int hypothesis_data_size_per_item = 28;
                int num_padded_hypothesis = 11;
                if (num_hypothesis_rec < num_padded_hypothesis) {
                    offset += (num_padded_hypothesis - num_hypothesis_rec) * hypothesis_data_size_per_item;
                }
                sf_list_j.push_back(record_item_j);
            }
            j["SF"] = sf_list_j;
            break;
        }
        case 141: {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v141,
                                     ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v141, Fmt),
                                     b, offset, length, j);

            unsigned int temp_ci = j["Carrier Index"];
            j["Carrier Index"] = temp_ci & 0xf;
            const char* carrier_name_v141 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v141 ? carrier_name_v141 : "(MI)Unknown");
            int num_records_v141 = (temp_ci >> 4) & 0x1ff;
            j["Number of Records"] = num_records_v141;

            json hypothesis_list_v141_j = json::array();
            for (int k = 0; k < num_records_v141; ++k) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v141,
                                         ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v141, Fmt),
                                         b, offset, length, record_item_j);

                unsigned int temp_sfn_v141 = record_item_j["Subframe Number"];
                record_item_j["Subframe Number"] = temp_sfn_v141 & 15;
                record_item_j["System Frame Number"] = (temp_sfn_v141 >> 4) & 1023;
                int bw_val_v141 = (temp_sfn_v141 >> 14) & 7;
                record_item_j["Band Width (MHz)"] = (bw_val_v141 > 0) ? (bw_val_v141 -1) * 5 : 0;
                record_item_j["CIF Configured"] = (temp_sfn_v141 >> 17) & 1;
                const char* cif_name_v141 = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), record_item_j["CIF Configured"]);
                record_item_j["CIF Configured"] = (cif_name_v141 ? cif_name_v141 : "(MI)Unknown");
                record_item_j["Two bits CSI Configured"] = (temp_sfn_v141 >> 18) & 1;
                const char* tbcc_name_v141 = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), record_item_j["Two bits CSI Configured"]);
                record_item_j["Two bits CSI Configured"] = (tbcc_name_v141 ? tbcc_name_v141 : "(MI)Unknown");
                record_item_j["Aperiodic SRS Configured"] = (temp_sfn_v141 >> 19) & 1;
                const char* asc_name_v141 = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), record_item_j["Aperiodic SRS Configured"]);
                record_item_j["Aperiodic SRS Configured"] = (asc_name_v141 ? asc_name_v141 : "(MI)Unknown");
                record_item_j["Frame Structure"] = (temp_sfn_v141 >> 20) & 7;
                const char* frame_struct_name_v141 = search_name(ValueNameFrameStructure, ARRAY_SIZE(ValueNameFrameStructure, ValueName), record_item_j["Frame Structure"]);
                record_item_j["Frame Structure"] = (frame_struct_name_v141 ? frame_struct_name_v141 : "(MI)Unknown");
                record_item_j["Num eNB Antennas"] = (temp_sfn_v141 >> 23) & 1;
                const char* num_ant_name_v141 = search_name(ValueNameNumNBAntennas_PDCCH_V141, ARRAY_SIZE(ValueNameNumNBAntennas_PDCCH_V141, ValueName), record_item_j["Num eNB Antennas"]);
                record_item_j["Num eNB Antennas"] = (num_ant_name_v141 ? num_ant_name_v141 : "(MI)Unknown");
                record_item_j["DL CP"] = (temp_sfn_v141 >> 24) & 1;
                const char* dlcp_name_v141 = search_name(ValueNameNormalOrNot, ARRAY_SIZE(ValueNameNormalOrNot, ValueName), record_item_j["DL CP"]);
                record_item_j["DL CP"] = (dlcp_name_v141 ? dlcp_name_v141 : "(MI)Unknown");
                record_item_j["SSC"] = (temp_sfn_v141 >> 25) & 0xf;
                record_item_j["CA FDD TDD"] = (temp_sfn_v141 >> 29) & 1;

                unsigned int agg_lvl_raw_v141 = record_item_j["Aggregation Level"];
                record_item_j["Aggregation Level"] = agg_lvl_raw_v141 & 3;
                const char* agg_lvl_name_v141 = search_name(ValueNameAggregationLevel, ARRAY_SIZE(ValueNameAggregationLevel, ValueName), record_item_j["Aggregation Level"]);
                record_item_j["Aggregation Level"] = (agg_lvl_name_v141 ? agg_lvl_name_v141 : "(MI)Unknown");
                record_item_j["Candidate"] = (agg_lvl_raw_v141 >> 2) & 7;
                record_item_j["Search Space Type"] = (agg_lvl_raw_v141 >> 5) & 1;
                const char* search_space_name_v141 = search_name(ValueNameSearchSpaceType, ARRAY_SIZE(ValueNameSearchSpaceType, ValueName), record_item_j["Search Space Type"]);
                record_item_j["Search Space Type"] = (search_space_name_v141 ? search_space_name_v141 : "(MI)Unknown");
                record_item_j["DCI Format"] = (agg_lvl_raw_v141 >> 6) & 15;
                const char* dci_fmt_name_v141 = search_name(ValueNameDCIFormat, ARRAY_SIZE(ValueNameDCIFormat, ValueName), record_item_j["DCI Format"]);
                record_item_j["DCI Format"] = (dci_fmt_name_v141 ? dci_fmt_name_v141 : "(MI)Unknown");
                record_item_j["Decode States"] = (agg_lvl_raw_v141 >> 10) & 15;
                const char* decode_stat_name_v141 = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), record_item_j["Decode States"]);
                record_item_j["Decode States"] = (decode_stat_name_v141 ? decode_stat_name_v141 : "(MI)Unknown");
                record_item_j["Payload Size"] = (agg_lvl_raw_v141 >> 14) & 255;
                record_item_j["Tail Match"] = (agg_lvl_raw_v141 >> 22) & 1;
                const char* tail_match_name_v141 = search_name(ValueNameMatchOrNot, ARRAY_SIZE(ValueNameMatchOrNot, ValueName), record_item_j["Tail Match"]);
                record_item_j["Tail Match"] = (tail_match_name_v141 ? tail_match_name_v141 : "(MI)Unknown");
                record_item_j["Non Zero Symbol Mismatch Count"] = (agg_lvl_raw_v141 >> 23) & 511;

                unsigned int start_cce_raw_v141 = record_item_j["Start CCE"];
                record_item_j["Alt TBS Enabled"] = start_cce_raw_v141 & 1;
                record_item_j["Start CCE"] = (start_cce_raw_v141 >> 1) & 127;
                record_item_j["Non Zero Llr Count"] = (start_cce_raw_v141 >> 8) & 511;
                record_item_j["Normal"] = (start_cce_raw_v141 >> 17) & 32767;

                unsigned int prune_stat_raw_v141 = record_item_j["Prune Status"];
                record_item_j["Prune Status"] = prune_stat_raw_v141 & 0xff;
                const char* prune_name_v141 = search_name(ValueNamePruneStatus, ARRAY_SIZE(ValueNamePruneStatus, ValueName), record_item_j["Prune Status"]);
                record_item_j["Prune Status"] = (prune_name_v141 ? prune_name_v141 : "(MI)Unknown");
                record_item_j["Energy Metric"] = (prune_stat_raw_v141 >> 8) & 0x1fffff;

                if (record_item_j.find("Norm Energy Metric") != record_item_j.end() && record_item_j["Norm Energy Metric"].is_number()) {
                    record_item_j["Norm Energy Metric"] = record_item_j["Norm Energy Metric"].get<unsigned int>() / 65535.0;
                }
                if (record_item_j.find("Symbol Error Rate") != record_item_j.end() && record_item_j["Symbol Error Rate"].is_number()) {
                    record_item_j["Symbol Error Rate"] = record_item_j["Symbol Error Rate"].get<unsigned int>() / 2147483648.0;
                }
                hypothesis_list_v141_j.push_back(record_item_j);
            }
            j["Hypothesis"] = hypothesis_list_v141_j;
            break;
        }
        default:
            LOGD("(MI)Unknown LTE PHY PDCCH Decoding Result version: 0x%x", pkt_ver);
            return 0;
    }

    return offset - start;
}