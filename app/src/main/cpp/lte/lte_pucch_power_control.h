//
// Created by Vladimir Ponomarenko on 09.04.2025.
//

#pragma  once

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


static int _decode_lte_pucch_power_control_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

   // LOGD("Decoding LTE_PUCCH_Power_Control with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 24: {
            offset += _decode_by_fmt(LtePucchPowerControl_Fmt_v24,
                                     ARRAY_SIZE(LtePucchPowerControl_Fmt_v24, Fmt),
                                     b, offset, length, j);
            int num_records = j["Number of Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;

                int record_offset_start = offset;
                int consumed_in_record = _decode_by_fmt(LtePucchPowerControl_Record_Fmt_v24,
                                                        ARRAY_SIZE(LtePucchPowerControl_Record_Fmt_v24, Fmt),
                                                        b, offset, length, record_item_j);
                offset += consumed_in_record;

                unsigned int non_decode_sfn = record_item_j["SFN"];
                record_item_j["SFN"] = non_decode_sfn & 1023;
                record_item_j["Sub-FN"] = (non_decode_sfn >> 10) & 15;

                int8_t iPower_raw = (non_decode_sfn >> 14) & 0xFF;
                record_item_j["PUCCH Tx Power (dBm)"] = static_cast<int>(iPower_raw);
                record_item_j["DCI Format"] = (non_decode_sfn >> 22) & 15;
                record_item_j["PUCCH Format"] = (non_decode_sfn >> 26) & 7;
                record_item_j["N_HARQ"] = (non_decode_sfn >> 29) & 1;

                const char* dci_name = search_name(LtePucchPowerControl_Record_v24_DCI_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v24_DCI_Format, ValueName), record_item_j["DCI Format"]);
                record_item_j["DCI Format"] = (dci_name ? dci_name : "(MI)Unknown");
                const char* pucch_fmt_name = search_name(LtePucchPowerControl_Record_v24_PUCCH_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v24_PUCCH_Format, ValueName), record_item_j["PUCCH Format"]);
                record_item_j["PUCCH Format"] = (pucch_fmt_name ? pucch_fmt_name : "(MI)Unknown");

                unsigned int non_decode_tpc = record_item_j["TPC Command"];
                int iTPC = non_decode_tpc & 63;
                record_item_j["TPC Command"] = iTPC;
                record_item_j["N_CQI"] = (non_decode_tpc >> 6) & 31;
                record_item_j["DL Path Loss"] = (non_decode_tpc >> 11) & 255;

                if (iTPC == 31 || iTPC == 63) {
                    const char* tpc_name = search_name(LtePucchPowerControl_Record_v24_TPC, ARRAY_SIZE(LtePucchPowerControl_Record_v24_TPC, ValueName), iTPC);
                    record_item_j["TPC Command"] = (tpc_name ? tpc_name : std::to_string(iTPC));
                }

                int16_t iGi = static_cast<int16_t>(record_item_j["g(i)"].get<unsigned int>());
                record_item_j["g(i)"] = iGi;



                int actual_power_byte_offset = 0;
                for(int k=0; k < ARRAY_SIZE(LtePucchPowerControl_Record_Fmt_v24, Fmt); ++k) {
                    if (strcmp(LtePucchPowerControl_Record_Fmt_v24[k].field_name, "PUCCH Actual Tx Power") == 0) {
                        break;
                    }
                    if (LtePucchPowerControl_Record_Fmt_v24[k].type != PLACEHOLDER) {
                        actual_power_byte_offset += LtePucchPowerControl_Record_Fmt_v24[k].len;
                    }
                }

                const char *p_actual_power = b + record_offset_start + actual_power_byte_offset;
                int8_t iActualPower_raw = *((unsigned char*)p_actual_power);
                record_item_j["PUCCH Actual Tx Power"] = static_cast<int>(iActualPower_raw);


                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 4: {
            offset += _decode_by_fmt(LtePucchPowerControl_Fmt_v4,
                                     ARRAY_SIZE(LtePucchPowerControl_Fmt_v4, Fmt),
                                     b, offset, length, j);
            int num_records = j["Number of Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                int record_offset_start = offset;
                int consumed_in_record = _decode_by_fmt(LtePucchPowerControl_Record_Fmt_v4,
                                                        ARRAY_SIZE(LtePucchPowerControl_Record_Fmt_v4, Fmt),
                                                        b, offset, length, record_item_j);
                offset += consumed_in_record;

                unsigned int non_decode_sfn = record_item_j["SFN"];
                record_item_j["SFN"] = non_decode_sfn & 1023;
                record_item_j["Sub-FN"] = (non_decode_sfn >> 10) & 15;

                int8_t iPower_raw = (non_decode_sfn >> 14) & 0xFF;
                record_item_j["PUCCH Tx Power (dBm)"] = static_cast<int>(iPower_raw);
                record_item_j["DCI Format"] = (non_decode_sfn >> 22) & 15;
                record_item_j["PUCCH Format"] = (non_decode_sfn >> 26) & 7;
                record_item_j["N_HARQ"] = (non_decode_sfn >> 29) & 1;

                const char* dci_name = search_name(LtePucchPowerControl_Record_v4_DCI_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v4_DCI_Format, ValueName), record_item_j["DCI Format"]);
                record_item_j["DCI Format"] = (dci_name ? dci_name : "(MI)Unknown");
                const char* pucch_fmt_name = search_name(LtePucchPowerControl_Record_v4_PUCCH_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format, ValueName), record_item_j["PUCCH Format"]);
                record_item_j["PUCCH Format"] = (pucch_fmt_name ? pucch_fmt_name : "(MI)Unknown");

                unsigned int non_decode_tpc = record_item_j["TPC Command"];
                int iTPC = non_decode_tpc & 63;
                record_item_j["TPC Command"] = iTPC;
                record_item_j["N_CQI"] = (non_decode_tpc >> 6) & 31;
                record_item_j["DL Path Loss"] = (non_decode_tpc >> 11) & 127;

                if (iTPC == 31 || iTPC == 63) {
                    const char* tpc_name = search_name(LtePucchPowerControl_Record_v4_TPC, ARRAY_SIZE(LtePucchPowerControl_Record_v4_TPC, ValueName), iTPC);
                    record_item_j["TPC Command"] = (tpc_name ? tpc_name : std::to_string(iTPC));
                }

                int16_t iGi = static_cast<int16_t>(record_item_j["g(i)"].get<unsigned int>());
                record_item_j["g(i)"] = iGi;


                int actual_power_byte_offset = 0;
                for(int k=0; k < ARRAY_SIZE(LtePucchPowerControl_Record_Fmt_v4, Fmt); ++k) {
                    if (strcmp(LtePucchPowerControl_Record_Fmt_v4[k].field_name, "PUCCH Actual Tx Power") == 0) {
                        break;
                    }
                    if (LtePucchPowerControl_Record_Fmt_v4[k].type != PLACEHOLDER) {
                        actual_power_byte_offset += LtePucchPowerControl_Record_Fmt_v4[k].len;
                    }
                }
                const char *p_actual_power = b + record_offset_start + actual_power_byte_offset;
                int8_t iActualPower_raw = *((unsigned char*)p_actual_power);
                record_item_j["PUCCH Actual Tx Power"] = static_cast<int>(iActualPower_raw);

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PUCCH Power Control version: 0x%x", pkt_ver);
            return 0;
    }
}

