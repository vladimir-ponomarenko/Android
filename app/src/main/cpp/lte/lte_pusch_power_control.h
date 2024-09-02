//
// Created by Vladimir Ponomarenko on 28.04.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;

static int _decode_lte_pusch_power_control_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    //LOGD("Decoding LTE_PUSCH_Power_Control with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 25: {
            offset += _decode_by_fmt(LtePuschPowerControl_Fmt_v25,
                                     ARRAY_SIZE(LtePuschPowerControl_Fmt_v25, Fmt),
                                     b, offset, length, j);

            int num_records = 0;
            if (j.find("Number of Records") != j.end() && j["Number of Records"].is_number()) {
                num_records = j["Number of Records"].get<int>();
            } else {
                LOGD("Error: Missing or invalid 'Number of Records' in PUSCH Power Control v25 header");
                return offset - start;
            }


            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                int record_consumed = _decode_by_fmt(LtePuschPowerControl_Record_Fmt_v25,
                                                     ARRAY_SIZE(LtePuschPowerControl_Record_Fmt_v25, Fmt),
                                                     b, offset, length - (offset - start), record_item_j);

                if (record_consumed == 0) {
                    LOGD("Error decoding record %d for PUSCH Power Control v25", i);
                    break;
                }
                offset += record_consumed;


                unsigned int non_decode_sfn = record_item_j["SFN"];
                record_item_j["Cell Index"] = non_decode_sfn & 0x7;
                record_item_j["SFN"] = (non_decode_sfn >> 3) & 0x3ff;
                record_item_j["Sub-FN"] = (non_decode_sfn >> 13) & 15;
                int8_t iPower = (non_decode_sfn >> 17) & 0xff;
                record_item_j["PUSCH Tx Power (dBm)"] = static_cast<int>(iPower);
                record_item_j["DCI Format"] = (non_decode_sfn >> 25) & 15;
                record_item_j["Tx Type"] = (non_decode_sfn >> 29) & 3;
                record_item_j["TPC Frozen"] = (non_decode_sfn >> 31) & 1;

                const char* dci_name = search_name(LtePuschPowerControl_Record_v5_DCI_Format, ARRAY_SIZE(LtePuschPowerControl_Record_v5_DCI_Format, ValueName), record_item_j["DCI Format"]);
                record_item_j["DCI Format"] = (dci_name ? dci_name : "(MI)Unknown");
                const char* tx_type_name = search_name(LtePuschPowerControl_Record_v5_TxType, ARRAY_SIZE(LtePuschPowerControl_Record_v5_TxType, ValueName), record_item_j["Tx Type"]);
                record_item_j["Tx Type"] = (tx_type_name ? tx_type_name : "(MI)Unknown");


                unsigned int non_decode_nrb = record_item_j["Num RBs"];
                record_item_j["Num RBs"] = non_decode_nrb & 255;
                record_item_j["Transport Block Size"] = (non_decode_nrb >> 8) & 16383;
                record_item_j["DL Path Loss"] = (non_decode_nrb >> 22) & 255;

                unsigned int non_decode_fi = record_item_j["F(i)"];
                int iFi = non_decode_fi & 1023;
                if (iFi >= 512) { iFi -= 1024; }
                record_item_j["F(i)"] = iFi;
                record_item_j["TPC"] = (non_decode_fi >> 10) & 31;
                int8_t iActualPower = (non_decode_fi >> 15) & 0xff;
                record_item_j["PUSCH Actual Tx Power"] = static_cast<int>(iActualPower);
                int8_t iMaxPower = (non_decode_fi >> 23) & 0xff;
                record_item_j["Max Power"] = static_cast<int>(iMaxPower);



                int tpc_val = record_item_j["TPC"];
                if (tpc_val == 15 || tpc_val == 31) {
                    const char* tpc_name = search_name(LtePuschPowerControl_Record_v5_TPC, ARRAY_SIZE(LtePuschPowerControl_Record_v5_TPC, ValueName), tpc_val);
                    record_item_j["TPC"] = (tpc_name ? tpc_name : std::to_string(tpc_val));
                }

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 5: {
            offset += _decode_by_fmt(LtePuschPowerControl_Fmt_v5,
                                     ARRAY_SIZE(LtePuschPowerControl_Fmt_v5, Fmt),
                                     b, offset, length, j);

            int num_records = 0;
            if (j.find("Number of Records") != j.end() && j["Number of Records"].is_number()) {
                num_records = j["Number of Records"].get<int>();
            } else {
                LOGD("Error: Missing or invalid 'Number of Records' in PUSCH Power Control v5 header");
                return offset - start;
            }


            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                int record_consumed = _decode_by_fmt(LtePuschPowerControl_Record_Fmt_v5,
                                                     ARRAY_SIZE(LtePuschPowerControl_Record_Fmt_v5, Fmt),
                                                     b, offset, length - (offset - start), record_item_j);

                if (record_consumed == 0) {
                    LOGD("Error decoding record %d for PUSCH Power Control v5", i);
                    break;
                }
                offset += record_consumed;

                unsigned int non_decode_sfn = record_item_j["SFN"];
                record_item_j["SFN"] = non_decode_sfn & 1023;
                record_item_j["Sub-FN"] = (non_decode_sfn >> 10) & 15;
                int8_t iPower = (non_decode_sfn >> 14) & 0xff;
                record_item_j["PUSCH Tx Power (dBm)"] = static_cast<int>(iPower);
                record_item_j["DCI Format"] = (non_decode_sfn >> 22) & 15;
                record_item_j["Tx Type"] = (non_decode_sfn >> 26) & 3;

                const char* dci_name = search_name(LtePuschPowerControl_Record_v5_DCI_Format, ARRAY_SIZE(LtePuschPowerControl_Record_v5_DCI_Format, ValueName), record_item_j["DCI Format"]);
                record_item_j["DCI Format"] = (dci_name ? dci_name : "(MI)Unknown");
                const char* tx_type_name = search_name(LtePuschPowerControl_Record_v5_TxType, ARRAY_SIZE(LtePuschPowerControl_Record_v5_TxType, ValueName), record_item_j["Tx Type"]);
                record_item_j["Tx Type"] = (tx_type_name ? tx_type_name : "(MI)Unknown");

                unsigned int non_decode_nrb = record_item_j["Num RBs"];
                record_item_j["Num RBs"] = non_decode_nrb & 255;
                record_item_j["Transport Block Size"] = (non_decode_nrb >> 8) & 16383;
                record_item_j["DL Path Loss"] = (non_decode_nrb >> 22) & 255;

                unsigned int non_decode_fi = record_item_j["F(i)"];
                int iFi = non_decode_fi & 1023;
                if (iFi >= 512) { iFi -= 1024; }
                record_item_j["F(i)"] = iFi;
                record_item_j["TPC"] = (non_decode_fi >> 10) & 31;
                int8_t iActualPower = (non_decode_fi >> 15) & 0xff;
                record_item_j["PUSCH Actual Tx Power"] = static_cast<int>(iActualPower);

                int tpc_val = record_item_j["TPC"];
                if (tpc_val == 15 || tpc_val == 31) {
                    const char* tpc_name = search_name(LtePuschPowerControl_Record_v5_TPC, ARRAY_SIZE(LtePuschPowerControl_Record_v5_TPC, ValueName), tpc_val);
                    record_item_j["TPC"] = (tpc_name ? tpc_name : std::to_string(tpc_val));
                }

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 4: {
            offset += _decode_by_fmt(LtePuschPowerControl_Fmt_v4,
                                     ARRAY_SIZE(LtePuschPowerControl_Fmt_v4, Fmt),
                                     b, offset, length, j);

            int num_records = 0;
            if (j.find("Number of Records") != j.end() && j["Number of Records"].is_number()) {
                num_records = j["Number of Records"].get<int>();
            } else {
                LOGD("Error: Missing or invalid 'Number of Records' in PUSCH Power Control v4 header");
                return offset - start;
            }

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                int record_consumed = _decode_by_fmt(LtePuschPowerControl_Record_Fmt_v4,
                                                     ARRAY_SIZE(LtePuschPowerControl_Record_Fmt_v4, Fmt),
                                                     b, offset, length - (offset - start), record_item_j);

                if (record_consumed == 0) {
                    LOGD("Error decoding record %d for PUSCH Power Control v4", i);
                    break;
                }
                offset += record_consumed;

                unsigned int non_decode_sfn = record_item_j["SFN"];
                record_item_j["SFN"] = non_decode_sfn & 1023;
                record_item_j["Sub-FN"] = (non_decode_sfn >> 10) & 15;
                int8_t iPower = (non_decode_sfn >> 14) & 0xff;
                record_item_j["PUSCH Tx Power (dBm)"] = static_cast<int>(iPower);
                record_item_j["DCI Format"] = (non_decode_sfn >> 22) & 15;
                record_item_j["Tx Type"] = (non_decode_sfn >> 26) & 3;

                const char* dci_name = search_name(LtePuschPowerControl_Record_v5_DCI_Format, ARRAY_SIZE(LtePuschPowerControl_Record_v5_DCI_Format, ValueName), record_item_j["DCI Format"]);
                record_item_j["DCI Format"] = (dci_name ? dci_name : "(MI)Unknown");
                const char* tx_type_name = search_name(LtePuschPowerControl_Record_v5_TxType, ARRAY_SIZE(LtePuschPowerControl_Record_v5_TxType, ValueName), record_item_j["Tx Type"]);
                record_item_j["Tx Type"] = (tx_type_name ? tx_type_name : "(MI)Unknown");

                unsigned int non_decode_tbs = record_item_j["Transport Block Size"];
                record_item_j["Transport Block Size"] = non_decode_tbs & 16383;
                record_item_j["DL Path Loss"] = (non_decode_tbs >> 14) & 255;
                int iFi = (non_decode_tbs >> 22) & 1023;
                if (iFi >= 512) { iFi -= 1024; }
                record_item_j["F(i)"] = iFi;

                unsigned int non_decode_tpc = record_item_j["TPC"];
                record_item_j["TPC"] = non_decode_tpc & 31;
                int8_t iActualPower = (non_decode_tpc >> 5) & 0xff;
                record_item_j["PUSCH Actual Tx Power"] = static_cast<int>(iActualPower);

                int tpc_val = record_item_j["TPC"];
                if (tpc_val == 15 || tpc_val == 31) {
                    const char* tpc_name = search_name(LtePuschPowerControl_Record_v5_TPC, ARRAY_SIZE(LtePuschPowerControl_Record_v5_TPC, ValueName), tpc_val);
                    record_item_j["TPC"] = (tpc_name ? tpc_name : std::to_string(tpc_val));
                }

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PUSCH Power Control version: 0x%x", pkt_ver);
            return 0;
    }
}