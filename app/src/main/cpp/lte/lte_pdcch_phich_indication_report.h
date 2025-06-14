//
// Created by Vladimir Ponomarenko on 18.06.2025.
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


static int _decode_lte_phy_pdcch_phich_indication_report_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PHY_PDCCH_PHICH_Indication_Report: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    LOGD("Decoding LTE_PHY_PDCCH_PHICH_Indication_Report with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 5: {
            offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Fmt_v5,
                                     ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt_v5, Fmt),
                                     b, offset, length, j);

            if (j.find("Duplex Mode") != j.end() && j["Duplex Mode"].is_number()){
                j["Duplex Mode"] = j["Duplex Mode"].get<int>() & 3;
            }

            int num_record = 0;
            if (j.find("Number of Records") != j.end() && j["Number of Records"].is_number()){
                num_record = j["Number of Records"].get<int>();
            }

            json records_j = json::array();
            for (int rec_idx = 0; rec_idx < num_record; ++rec_idx) {
                json record_item_j;
                int p1_consumed = _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v5_p1,
                                                 ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_p1, Fmt),
                                                 b, offset, length - (offset-start), record_item_j);
                offset += p1_consumed;

                if (record_item_j.find("Num PDCCH Results") != record_item_j.end() && record_item_j["Num PDCCH Results"].is_number()) {
                    unsigned int non_decode_p1_1 = record_item_j["Num PDCCH Results"];
                    int iNumPdcch = non_decode_p1_1 & 7;
                    record_item_j["Num PDCCH Results"] = iNumPdcch;
                    record_item_j["PDCCH Timing SFN"] = (non_decode_p1_1 >> 3) & 1023;
                    record_item_j["PDCCH Timing Sub-FN"] = (non_decode_p1_1 >> 13) & 15;
                    int iPhichIncluded = (non_decode_p1_1 >> 17) & 1;
                    record_item_j["PHICH Included"] = iPhichIncluded;
                    const char* phich_incl_name = search_name(LtePdcchPhichIndicationReport_Record_v5_Included, ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Included, ValueName), iPhichIncluded);
                    record_item_j["PHICH Included"] = (phich_incl_name ? phich_incl_name : "(MI)Unknown");
                    int iPhich1Included = (non_decode_p1_1 >> 18) & 1;
                    record_item_j["PHICH 1 Included"] = iPhich1Included;
                    const char* phich1_incl_name = search_name(LtePdcchPhichIndicationReport_Record_v5_Included, ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Included, ValueName), iPhich1Included);
                    record_item_j["PHICH 1 Included"] = (phich1_incl_name ? phich1_incl_name : "(MI)Unknown");
                    int iPhichSFN = (non_decode_p1_1 >> 19) & 1023;

                    if (record_item_j.find("PHICH Timing Sub-FN") != record_item_j.end() && record_item_j["PHICH Timing Sub-FN"].is_number()) {
                        unsigned int non_decode_p1_2 = record_item_j["PHICH Timing Sub-FN"];
                        record_item_j["PHICH Timing Sub-FN"] = non_decode_p1_2 & 15;
                        int iPhichValue = (non_decode_p1_2 >> 4) & 1;
                        int iPhich1Value = (non_decode_p1_2 >> 5) & 1;

                        if (iPhichIncluded == 1) {
                            record_item_j["PHICH Timing SFN"] = iPhichSFN;

                            record_item_j["PHICH Value"] = iPhichValue;
                            const char* phich_val_name = search_name(LtePdcchPhichIndicationReport_Record_v5_Value, ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Value, ValueName), iPhichValue);
                            record_item_j["PHICH Value"] = (phich_val_name ? phich_val_name : "(MI)Unknown");
                        }
                        if (iPhich1Included == 1) {
                            record_item_j["PHICH 1 Value"] = iPhich1Value;
                            const char* phich1_val_name = search_name(LtePdcchPhichIndicationReport_Record_v5_Value, ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Value, ValueName), iPhich1Value);
                            record_item_j["PHICH 1 Value"] = (phich1_val_name ? phich1_val_name : "(MI)Unknown");
                        }
                    }

                    json pdcch_info_j = json::array();
                    for (int pdcch_idx = 0; pdcch_idx < iNumPdcch; ++pdcch_idx) {
                        json pdcch_item_j;
                        offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v5_p2,
                                                 ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_p2, Fmt),
                                                 b, offset, length - (offset-start), pdcch_item_j);
                        if (pdcch_item_j.find("Serv Cell Idx") != pdcch_item_j.end()) {
                            unsigned int p2_1 = pdcch_item_j["Serv Cell Idx"];
                            pdcch_item_j["Serv Cell Idx"] = p2_1 & 7;
                            pdcch_item_j["RNTI Type"] = (p2_1 >> 3) & 15;
                            const char* rnti_name_rec = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), pdcch_item_j["RNTI Type"]);
                            pdcch_item_j["RNTI Type"] = (rnti_name_rec ? rnti_name_rec : "(MI)Unknown");
                            pdcch_item_j["Payload Size"] = (p2_1 >> 7) & 127;
                            pdcch_item_j["Aggregation Level"] = (p2_1 >> 14) & 3;
                            const char* agg_lvl_name_rec = search_name(LtePdcchPhichIndicationReport_Record_v5_AggLv, ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_AggLv, ValueName), pdcch_item_j["Aggregation Level"]);
                            pdcch_item_j["Aggregation Level"] = (agg_lvl_name_rec ? agg_lvl_name_rec : "(MI)Unknown");
                        }
                        if (pdcch_item_j.find("Search Space") != pdcch_item_j.end()) {
                            unsigned int p2_2 = pdcch_item_j["Search Space"];
                            pdcch_item_j["Search Space"] = p2_2 & 1;
                            const char* ss_name_rec = search_name(LtePdcchPhichIndicationReport_Record_v5_SS, ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_SS, ValueName), pdcch_item_j["Search Space"]);
                            pdcch_item_j["Search Space"] = (ss_name_rec ? ss_name_rec : "(MI)Unknown");

                            pdcch_item_j["New DL Tx"] = (p2_2 >> 4) & 1;
                            const char* newtx_name_rec = search_name(LtePdcchPhichIndicationReport_Record_v5_NewDLTx, ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_NewDLTx, ValueName), pdcch_item_j["New DL Tx"]);
                            pdcch_item_j["New DL Tx"] = (newtx_name_rec ? newtx_name_rec : "(MI)Unknown");
                            pdcch_item_j["Num DL Trblks"] = (p2_2 >> 5) & 3;
                        }
                        pdcch_info_j.push_back(pdcch_item_j);
                    }
                    record_item_j["PDCCH Info"] = pdcch_info_j;


                    int num_pdcch_read = iNumPdcch;
                    int num_pdcch_to_skip = 8 - num_pdcch_read;
                    if (num_pdcch_to_skip > 0) {

                        int bytes_to_skip = num_pdcch_to_skip * 4;
                        if (offset + bytes_to_skip <= start + length) {
                            offset += bytes_to_skip;
                        } else {
                            LOGD("Not enough data to skip hidden PDCCH info in v5 record");
                        }
                    }
                }
                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            break;
        }
        case 25:
        case 33:
        case 42: {
            const Fmt* payload_fmt = nullptr; size_t payload_fmt_size = 0;
            const Fmt* record_p1_fmt = nullptr; size_t record_p1_fmt_size = 0;
            const Fmt* phich_fmt = nullptr; size_t phich_fmt_size = 0;
            const Fmt* pdcch_fmt = nullptr; size_t pdcch_fmt_size = 0;
            int num_phich_total_bytes = 0;
            int num_pdcch_total_bytes = 0;
            int phich_item_size = 0;
            int pdcch_item_size = 0;

            if (pkt_ver == 25) {
                payload_fmt = LtePdcchPhichIndicationReport_Fmt_v25; payload_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt_v25, Fmt);
                record_p1_fmt = LtePdcchPhichIndicationReport_Record_v25_p1; record_p1_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_p1, Fmt);
                phich_fmt = LtePdcchPhichIndicationReport_Record_v25_phich; phich_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_phich, Fmt);
                pdcch_fmt = LtePdcchPhichIndicationReport_Record_v25_pdcch; pdcch_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_pdcch, Fmt);
                num_phich_total_bytes = 12; phich_item_size = 4;
                num_pdcch_total_bytes = 64; pdcch_item_size = 8;
            } else if (pkt_ver == 33 || pkt_ver == 34) {
                payload_fmt = LtePdcchPhichIndicationReport_Fmt_v33; payload_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt_v33, Fmt);
                record_p1_fmt = LtePdcchPhichIndicationReport_Record_v33_p1; record_p1_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v33_p1, Fmt);
                phich_fmt = LtePdcchPhichIndicationReport_Record_v25_phich; phich_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_phich, Fmt);
                pdcch_fmt = LtePdcchPhichIndicationReport_Record_v33_pdcch; pdcch_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v33_pdcch, Fmt);
                if (pkt_ver == 33) {
                    num_phich_total_bytes = 20;
                } else {
                    num_phich_total_bytes = 20;
                }
                phich_item_size = 4;
                num_pdcch_total_bytes = 0;
                if (pkt_ver == 34) num_pdcch_total_bytes = 16 * 8;
                pdcch_item_size = 8;
            } else {
                payload_fmt = LtePdcchPhichIndicationReport_Fmt_v42; payload_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt_v42, Fmt);
                record_p1_fmt = LtePdcchPhichIndicationReport_Record_v42_p1; record_p1_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v42_p1, Fmt);
                phich_fmt = LtePdcchPhichIndicationReport_Record_v42_phich; phich_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v42_phich, Fmt);
                pdcch_fmt = LtePdcchPhichIndicationReport_Record_v42_pdcch; pdcch_fmt_size = ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v42_pdcch, Fmt);

                num_phich_total_bytes = 0; phich_item_size = 4;
                num_pdcch_total_bytes = 0; pdcch_item_size = 8;

            }

            offset += _decode_by_fmt(payload_fmt, payload_fmt_size, b, offset, length, j);

            if (j.find("Duplex Mode") != j.end() && j["Duplex Mode"].is_number()){
                int duplex_raw = j["Duplex Mode"];
                j["Duplex Mode"] = duplex_raw & 3;
                if (pkt_ver == 42) {
                    j["UL DL Config"] = (duplex_raw >> 2) & 0xf;
                }
            }
            int num_record = 0;
            if (j.find("Number of Records") != j.end() && j["Number of Records"].is_number()){
                num_record = j["Number of Records"].get<int>();
            }

            json records_j = json::array();
            for (int rec_idx = 0; rec_idx < num_record; ++rec_idx) {
                json record_item_j;
                int p1_consumed = _decode_by_fmt(record_p1_fmt, record_p1_fmt_size,
                                                 b, offset, length - (offset-start), record_item_j);
                offset += p1_consumed;

                unsigned int non_decode_p1_1 = record_item_j["Num PDCCH Results"];
                int iNumPdcch = non_decode_p1_1 & 7;
                record_item_j["Num PDCCH Results"] = iNumPdcch;
                int iNumPhich = (non_decode_p1_1 >> 3) & 7;
                record_item_j["Num PHICH Results"] = iNumPhich;
                record_item_j["PDCCH Timing SFN"] = (non_decode_p1_1 >> 6) & 1023;
                record_item_j["PDCCH Timing Sub-FN"] = (non_decode_p1_1 >> 16) & 15;

                json phich_list_j = json::array();
                int phich_bytes_read_total = 0;
                int phich_items_to_read = (pkt_ver == 34) ? 5 : iNumPhich;

                for (int phich_idx = 0; phich_idx < phich_items_to_read; ++phich_idx) {
                    json phich_item_j;
                    int phich_item_consumed = _decode_by_fmt(phich_fmt, phich_fmt_size,
                                                             b, offset + phich_bytes_read_total, length - (offset + phich_bytes_read_total - start), phich_item_j);
                    if (phich_item_consumed == 0) break;
                    phich_bytes_read_total += phich_item_consumed;

                    if (phich_item_j.find("Cell Index") != phich_item_j.end()){
                        unsigned int phich_raw = phich_item_j["Cell Index"];
                        phich_item_j["Cell Index"] = phich_raw & 7;
                        phich_item_j["PHICH Included"] = (phich_raw >> 3) & 1;
                        const char* phich_incl_name_rec = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), phich_item_j["PHICH Included"]);
                        phich_item_j["PHICH Included"] = (phich_incl_name_rec ? phich_incl_name_rec : "(MI)Unknown");
                        phich_item_j["PHICH 1 Included"] = (phich_raw >> 4) & 1;
                        const char* phich1_incl_name_rec = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), phich_item_j["PHICH 1 Included"]);
                        phich_item_j["PHICH 1 Included"] = (phich1_incl_name_rec ? phich1_incl_name_rec : "(MI)Unknown");
                        phich_item_j["PHICH Value"] = (phich_raw >> 5) & 1;
                        const char* phich_val_name_rec = search_name(ValueNameAckOrNack, ARRAY_SIZE(ValueNameAckOrNack, ValueName), phich_item_j["PHICH Value"]);
                        phich_item_j["PHICH Value"] = (phich_val_name_rec ? phich_val_name_rec : "(MI)Unknown");
                        phich_item_j["PHICH 1 Value"] = (phich_raw >> 6) & 1;
                        const char* phich1_val_name_rec = search_name(ValueNameAckOrNack, ARRAY_SIZE(ValueNameAckOrNack, ValueName), phich_item_j["PHICH 1 Value"]);
                        phich_item_j["PHICH 1 Value"] = (phich1_val_name_rec ? phich1_val_name_rec : "(MI)Unknown");
                    }
                    phich_list_j.push_back(phich_item_j);
                }
                record_item_j["PHICH"] = phich_list_j;
                offset += phich_bytes_read_total;

                if (num_phich_total_bytes > 0 && phich_bytes_read_total < num_phich_total_bytes) {
                    if (offset + (num_phich_total_bytes - phich_bytes_read_total) <= start + length) {
                        offset += (num_phich_total_bytes - phich_bytes_read_total);
                    }
                }


                json pdcch_info_j = json::array();
                int pdcch_bytes_read_total = 0;
                int pdcch_items_to_read = (pkt_ver == 34) ? 16 : iNumPdcch;

                for (int pdcch_idx = 0; pdcch_idx < pdcch_items_to_read; ++pdcch_idx) {
                    json pdcch_item_j;
                    int pdcch_item_consumed = _decode_by_fmt(pdcch_fmt, pdcch_fmt_size,
                                                             b, offset + pdcch_bytes_read_total, length - (offset + pdcch_bytes_read_total - start), pdcch_item_j);
                    if (pdcch_item_consumed == 0) break;
                    pdcch_bytes_read_total += pdcch_item_consumed;

                    if (pdcch_item_j.find("Serv Cell Idx") != pdcch_item_j.end()){
                        unsigned int p2_1 = pdcch_item_j["Serv Cell Idx"];
                        pdcch_item_j["Serv Cell Idx"] = p2_1 & 7;
                        pdcch_item_j["RNTI Type"] = (p2_1 >> 3) & 15;
                        const char* rnti_name_rec = search_name(RNTIType, ARRAY_SIZE(RNTIType, ValueName), pdcch_item_j["RNTI Type"]);
                        pdcch_item_j["RNTI Type"] = (rnti_name_rec ? rnti_name_rec : "(MI)Unknown");
                        pdcch_item_j["Payload Size"] = (p2_1 >> 7) & 127;
                        pdcch_item_j["Aggregation Level"] = (p2_1 >> 14) & 3;
                        const char* agg_lvl_name_rec = search_name(ValueNameAggregationLevel, ARRAY_SIZE(ValueNameAggregationLevel, ValueName), pdcch_item_j["Aggregation Level"]);
                        pdcch_item_j["Aggregation Level"] = (agg_lvl_name_rec ? agg_lvl_name_rec : "(MI)Unknown");
                    }
                    if (pdcch_item_j.find("Search Space") != pdcch_item_j.end()){
                        unsigned int p2_2 = pdcch_item_j["Search Space"];
                        pdcch_item_j["Search Space"] = p2_2 & 1;
                        const char* ss_name_rec = search_name(ValueNameSearchSpaceType, ARRAY_SIZE(ValueNameSearchSpaceType, ValueName), pdcch_item_j["Search Space"]);
                        pdcch_item_j["Search Space"] = (ss_name_rec ? ss_name_rec : "(MI)Unknown");
                        pdcch_item_j["New DL Tx"] = (p2_2 >> 4) & 1;
                        const char* newtx_name_rec = search_name(ValueNameTrueOrFalse, ARRAY_SIZE(ValueNameTrueOrFalse, ValueName), pdcch_item_j["New DL Tx"]);
                        pdcch_item_j["New DL Tx"] = (newtx_name_rec ? newtx_name_rec : "(MI)Unknown");
                        pdcch_item_j["Num DL Trblks"] = (p2_2 >> 5) & 3;
                    }
                    if (pkt_ver == 42 && pdcch_item_j.find("S0 Index") != pdcch_item_j.end()) {
                        unsigned int s_index_raw = pdcch_item_j["S0 Index"];
                        pdcch_item_j["S0 Index"] = (s_index_raw >> 0) & 0x1f;
                        pdcch_item_j["S1 Index"] = (s_index_raw >> 5) & 0x1f;
                        pdcch_item_j["S2 Index"] = (s_index_raw >> 10) & 0x1f;
                        pdcch_item_j["S3 Index"] = (s_index_raw >> 15) & 0x1f;
                        pdcch_item_j["Msleep"] = (s_index_raw >> 20) & 0x1;
                        pdcch_item_j["Usleep"] = (s_index_raw >> 21) & 0x1;
                        pdcch_item_j["uSleep Duration"] = (s_index_raw >> 22) & 0x1f;
                        pdcch_item_j["Fake Pdcch Sf"] = (s_index_raw >> 27) & 0x1;
                        pdcch_item_j["Is Ul Dropped"] = (s_index_raw >> 28) & 0x1;
                        pdcch_item_j["Interf Active"] = (s_index_raw >> 29) & 0x1;
                    }
                    pdcch_info_j.push_back(pdcch_item_j);
                }
                record_item_j["PDCCH Info"] = pdcch_info_j;
                offset += pdcch_bytes_read_total;

                if (num_pdcch_total_bytes > 0 && pdcch_bytes_read_total < num_pdcch_total_bytes) {
                    if (offset + (num_pdcch_total_bytes - pdcch_bytes_read_total) <= start + length) {
                        offset += (num_pdcch_total_bytes - pdcch_bytes_read_total);
                    }
                }

                if (pkt_ver == 42) {
                    json p3_j;
                    offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v42_p3,
                                             ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v42_p3, Fmt),
                                             b, offset, length - (offset-start), p3_j);
                    if (p3_j.find("Force Send PDCCH Ind") != p3_j.end()) {
                        record_item_j["Force Send PDCCH Ind"] = p3_j["Force Send PDCCH Ind"].get<unsigned int>() & 0x1;
                        record_item_j.update(p3_j);
                    }
                }

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            break;
        }


        case 101:
        case 121:
        case 123: {
            const Fmt* payload_fmt_v1xx = nullptr; size_t payload_fmt_size_v1xx = 0;
            const Fmt* hypothesis_fmt_v1xx = nullptr; size_t hypothesis_fmt_size_v1xx = 0;

            if(pkt_ver == 101) {payload_fmt_v1xx = LtePhyPdcchDecodingResult_Payload_v101; payload_fmt_size_v1xx = ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v101, Fmt); hypothesis_fmt_v1xx = LtePhyPdcchDecodingResult_Hypothesis_v101; hypothesis_fmt_size_v1xx = ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v101, Fmt);}
            else if (pkt_ver == 121) {payload_fmt_v1xx = LtePhyPdcchDecodingResult_Payload_v121; payload_fmt_size_v1xx = ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v121, Fmt); hypothesis_fmt_v1xx = LtePhyPdcchDecodingResult_Hypothesis_v121; hypothesis_fmt_size_v1xx = ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v121, Fmt);}
            else { /* pkt_ver == 123 */ payload_fmt_v1xx = LtePhyPdcchDecodingResult_Payload_v123; payload_fmt_size_v1xx = ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v123, Fmt); hypothesis_fmt_v1xx = LtePhyPdcchDecodingResult_Hypothesis_v123; hypothesis_fmt_size_v1xx = ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v123, Fmt);}

            offset += _decode_by_fmt(payload_fmt_v1xx, payload_fmt_size_v1xx, b, offset, length, j);

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


                j["__pad"] = ((pad_val_raw << 1) | ((temp_sfn_data >> 31) & 1)) & 1;
            }

            int temp_dms_data = j["Demback Mode Select"];
            j["Demback Mode Select"] = temp_dms_data & 15;
            j["Carrier Index"] = (temp_dms_data >> 4) & 15;
            const char* carrier_name_v1xx = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v1xx ? carrier_name_v1xx : "(MI)Unknown");

            int num_hypothesis_v1xx = 0;
            if(j.find("Number of Hypothesis") != j.end() && j["Number of Hypothesis"].is_number()){
                num_hypothesis_v1xx = j["Number of Hypothesis"].get<int>();
            }

            json hypothesis_list_v1xx_j = json::array();
            for (int k = 0; k < num_hypothesis_v1xx; ++k) {
                json hypothesis_item_j;
                offset += _decode_by_fmt(hypothesis_fmt_v1xx, hypothesis_fmt_size_v1xx,
                                         b, offset, length, hypothesis_item_j);

                unsigned int agg_lvl_raw = hypothesis_item_j["Aggregation Level"];
                hypothesis_item_j["Aggregation Level"] = agg_lvl_raw & 3;
                const char* agg_lvl_name_v1xx = search_name(ValueNameAggregationLevel, ARRAY_SIZE(ValueNameAggregationLevel, ValueName), hypothesis_item_j["Aggregation Level"]);
                hypothesis_item_j["Aggregation Level"] = (agg_lvl_name_v1xx ? agg_lvl_name_v1xx : "(MI)Unknown");
                hypothesis_item_j["Candidate"] = (agg_lvl_raw >> 2) & 7;
                hypothesis_item_j["Search Space Type"] = (agg_lvl_raw >> 5) & 1;
                const char* search_space_name_v1xx = search_name(ValueNameSearchSpaceType, ARRAY_SIZE(ValueNameSearchSpaceType, ValueName), hypothesis_item_j["Search Space Type"]);
                hypothesis_item_j["Search Space Type"] = (search_space_name_v1xx ? search_space_name_v1xx : "(MI)Unknown");
                hypothesis_item_j["DCI Format"] = (agg_lvl_raw >> 6) & 15;
                const char* dci_fmt_name_v1xx = search_name(ValueNameDCIFormat, ARRAY_SIZE(ValueNameDCIFormat, ValueName), hypothesis_item_j["DCI Format"]);
                hypothesis_item_j["DCI Format"] = (dci_fmt_name_v1xx ? dci_fmt_name_v1xx : "(MI)Unknown");
                hypothesis_item_j["Decode States"] = (agg_lvl_raw >> 10) & 15;
                const char* decode_stat_name_v1xx = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), hypothesis_item_j["Decode States"]);
                hypothesis_item_j["Decode States"] = (decode_stat_name_v1xx ? decode_stat_name_v1xx : "(MI)Unknown");
                hypothesis_item_j["Payload Size"] = (agg_lvl_raw >> 14) & 255;
                hypothesis_item_j["Tail Match"] = (agg_lvl_raw >> 22) & 1;
                const char* tail_match_name_v1xx = search_name(ValueNameMatchOrNot, ARRAY_SIZE(ValueNameMatchOrNot, ValueName), hypothesis_item_j["Tail Match"]);
                hypothesis_item_j["Tail Match"] = (tail_match_name_v1xx ? tail_match_name_v1xx : "(MI)Unknown");
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
                const char* prune_name_v1xx = search_name(ValueNamePruneStatus, ARRAY_SIZE(ValueNamePruneStatus, ValueName), hypothesis_item_j["Prune Status"]);
                hypothesis_item_j["Prune Status"] = (prune_name_v1xx ? prune_name_v1xx : "(MI)Unknown");
                hypothesis_item_j["Energy Metric"] = prune_stat_raw >> 11;

                if (hypothesis_item_j.find("Norm Energy Metric") != hypothesis_item_j.end() && hypothesis_item_j["Norm Energy Metric"].is_number()) {
                    hypothesis_item_j["Norm Energy Metric"] = hypothesis_item_j["Norm Energy Metric"].get<unsigned int>() / 65535.0;
                }
                if (hypothesis_item_j.find("Symbol Error Rate") != hypothesis_item_j.end() && hypothesis_item_j["Symbol Error Rate"].is_number()) {
                    hypothesis_item_j["Symbol Error Rate"] = hypothesis_item_j["Symbol Error Rate"].get<unsigned int>() / 2147483648.0;
                }
                hypothesis_list_v1xx_j.push_back(hypothesis_item_j);
            }
            j["Hypothesis"] = hypothesis_list_v1xx_j;
            break;
        }
        case 43: {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v43,
                                     ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v43, Fmt),
                                     b, offset, length, j);

            unsigned int temp_num_rec_v43 = j["Num Records"];
            int num_records_v43 = temp_num_rec_v43 & 15;
            j["Subframe Number"] = (temp_num_rec_v43 >> 4) & 15;
            j["System Frame Number"] = (temp_num_rec_v43 >> 8) & 1023;
            int bw_val_v43 = (temp_num_rec_v43 >> 18) & 15;
            j["Band Width (MHz)"] = (bw_val_v43 > 0) ? (bw_val_v43 - 1) * 5 : 0;
            j["Frame Structure"] = (temp_num_rec_v43 >> 22) & 3;
            const char* frame_struct_name_v43 = search_name(ValueNameFrameStructure, ARRAY_SIZE(ValueNameFrameStructure, ValueName), j["Frame Structure"]);
            j["Frame Structure"] = (frame_struct_name_v43 ? frame_struct_name_v43 : "(MI)Unknown");
            j["Num eNB Antennas"] = (temp_num_rec_v43 >> 24) & 7;
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
            int current_record_offset = offset;
            for (int rec_idx = 0; rec_idx < num_records_v43; ++rec_idx) {
                json record_item_j;
                int record_v43_consumed = _decode_by_fmt(LtePhyPdcchDecodingResult_Record_v43,
                                                         ARRAY_SIZE(LtePhyPdcchDecodingResult_Record_v43, Fmt),
                                                         b, current_record_offset, length - (current_record_offset - start), record_item_j);
                if(record_v43_consumed == 0) { LOGD("Error decoding v43 record %d", rec_idx); break; }
                current_record_offset += record_v43_consumed;

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
                int current_hypothesis_offset = current_record_offset;

                for (int k = 0; k < num_hypothesis_rec; ++k) {
                    json hypothesis_item_j;
                    int hyp_consumed = _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v43,
                                                      ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v43, Fmt),
                                                      b, current_hypothesis_offset, length - (current_hypothesis_offset-start), hypothesis_item_j);
                    if(hyp_consumed == 0) {LOGD("Error decoding v43 hypothesis %d for record %d", k, rec_idx); break;}
                    current_hypothesis_offset += hyp_consumed;

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
                offset = current_hypothesis_offset;

                int hypothesis_data_size_per_item = 28;
                int num_padded_hypothesis = 11;
                if (num_hypothesis_rec < num_padded_hypothesis) {
                    int bytes_to_skip_v43 = (num_padded_hypothesis - num_hypothesis_rec) * hypothesis_data_size_per_item;
                    if (offset + bytes_to_skip_v43 <= start + length) {
                        offset += bytes_to_skip_v43;
                    } else {
                        LOGD("Not enough data to skip hypothesis padding in v43 record");
                    }
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

            unsigned int temp_ci_v141 = j["Carrier Index"];
            j["Carrier Index"] = temp_ci_v141 & 0xf;
            const char* carrier_name_v141 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v141 ? carrier_name_v141 : "(MI)Unknown");
            int num_records_v141 = (temp_ci_v141 >> 4) & 0x1ff;
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
            j["Records"] = hypothesis_list_v141_j;
            break;
        }
        default:
            LOGD("(MI)Unknown LTE PHY PDCCH Decoding Result version: 0x%x", pkt_ver);
            return 0;
    }

    return offset - start;
}