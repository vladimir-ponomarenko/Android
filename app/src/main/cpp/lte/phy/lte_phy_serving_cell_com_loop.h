//
// Created by Vladimir Ponomarenko on 06.04.2025.
//

#ifndef LOGIN_LTE_PHY_SERVING_CELL_COM_LOOP_H
#define LOGIN_LTE_PHY_SERVING_CELL_COM_LOOP_H

#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;



const Fmt LtePhyServingCellComLoop_Fmt[] = {
        {UINT, "Version", 1},
};

const Fmt LtePhyServingCellComLoop_Payload_v41[] = {
        {UINT, "Carrier Index", 2},
        {PLACEHOLDER, "Cell ID", 0},
        {UINT, "Number of Records", 1},
};

const Fmt LtePhyServingCellComLoop_Payload_v22[] = {
        {UINT, "Carrier Index", 2},
        {PLACEHOLDER, "Number of Records", 0},
        {SKIP, NULL, 1},
};

const ValueName LtePhyServingCellComLoop_CarrierIndex[] = {
        {0, "PCC"},
        {1, "SCC"},

};

const Fmt LtePhyServingCellComLoop_Record_v41[] = {
        {UINT, "System Frame Number", 2},
        {PLACEHOLDER, "Subframe Number", 0},
        {PLACEHOLDER, "Enabled", 0},
        {UINT, "COM IIR Alpha", 2},
        {UINT, "COM Adjustment[0]", 4},
        {UINT, "COM Adjustment[1]", 4},
        {UINT, "COM Unwrap Base[0]", 4},
        {UINT, "COM Unwrap Base[1]", 4},
        {BYTE_STREAM, "COM Acc Stat[0]", 8},
        {BYTE_STREAM, "COM Acc Stat[1]", 8},
        {SKIP, NULL, 16},
};

const ValueName LtePhyServingCellComLoop_Record_Enabled[] = {
        {0, "Disabled"},
        {1, "Enabled"},
};




static int _decode_lte_phy_serving_cell_com_loop_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

   // LOGD("Decoding LTE_PHY_Serving_Cell_COM_Loop with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 22: {
            offset += _decode_by_fmt(LtePhyServingCellComLoop_Payload_v22,
                                     ARRAY_SIZE(LtePhyServingCellComLoop_Payload_v22, Fmt),
                                     b, offset, length, j);

            int non_decode_p1 = j["Carrier Index"];
            int iCarrierIndex = non_decode_p1 & 15;
            int num_records = (non_decode_p1 >> 4) & 31;

            j["Carrier Index"] = iCarrierIndex;
            const char* carrier_name = search_name(LtePhyServingCellComLoop_CarrierIndex, ARRAY_SIZE(LtePhyServingCellComLoop_CarrierIndex, ValueName), iCarrierIndex);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            j["Number of Records"] = num_records;

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;

                offset += _decode_by_fmt(LtePhyServingCellComLoop_Record_v41,
                                         ARRAY_SIZE(LtePhyServingCellComLoop_Record_v41, Fmt),
                                         b, offset, length, record_item_j);

                int non_decode_p3_1 = record_item_j["System Frame Number"];
                int isysfn = non_decode_p3_1 & 1023;
                int isubfn = (non_decode_p3_1 >> 10) & 15;
                int ienabled = (non_decode_p3_1 >> 14) & 1;

                record_item_j["System Frame Number"] = isysfn;
                record_item_j["Subframe Number"] = isubfn;
                record_item_j["Enabled"] = ienabled;
                const char* enabled_name = search_name(LtePhyServingCellComLoop_Record_Enabled, ARRAY_SIZE(LtePhyServingCellComLoop_Record_Enabled, ValueName), ienabled);
                record_item_j["Enabled"] = (enabled_name ? enabled_name : "(MI)Unknown");


                unsigned int i_cia = record_item_j["COM IIR Alpha"];
                record_item_j["COM IIR Alpha"] = static_cast<double>(i_cia) / 32765.0;


                unsigned int i_ca0 = record_item_j["COM Adjustment[0]"];
                record_item_j["COM Adjustment[0]"] = static_cast<double>(i_ca0) / 64.0;

                unsigned int i_ca1 = record_item_j["COM Adjustment[1]"];
                record_item_j["COM Adjustment[1]"] = static_cast<double>(i_ca1) / 64.0;

                unsigned int i_cnb0 = record_item_j["COM Unwrap Base[0]"];
                record_item_j["COM Unwrap Base[0]"] = static_cast<double>(i_cnb0) / 64.0;

                unsigned int i_cnb1 = record_item_j["COM Unwrap Base[1]"];
                record_item_j["COM Unwrap Base[1]"] = static_cast<double>(i_cnb1) / 64.0;



                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 41: {
            offset += _decode_by_fmt(LtePhyServingCellComLoop_Payload_v41,
                                     ARRAY_SIZE(LtePhyServingCellComLoop_Payload_v41, Fmt),
                                     b, offset, length, j);

            int non_decode_p1 = j["Carrier Index"];
            int iCarrierIndex = non_decode_p1 & 15;
            int iCellId = (non_decode_p1 >> 4) & 511;

            j["Carrier Index"] = iCarrierIndex;
            const char* carrier_name = search_name(LtePhyServingCellComLoop_CarrierIndex, ARRAY_SIZE(LtePhyServingCellComLoop_CarrierIndex, ValueName), iCarrierIndex);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            j["Cell ID"] = iCellId;

            int non_decode_p2 = j["Number of Records"];
            int num_records = (non_decode_p2 >> 1) & 31;
            j["Number of Records"] = num_records;


            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyServingCellComLoop_Record_v41,
                                         ARRAY_SIZE(LtePhyServingCellComLoop_Record_v41, Fmt),
                                         b, offset, length, record_item_j);

                int non_decode_p3_1 = record_item_j["System Frame Number"];
                int isysfn = non_decode_p3_1 & 1023;
                int isubfn = (non_decode_p3_1 >> 10) & 15;
                int ienabled = (non_decode_p3_1 >> 14) & 1;

                record_item_j["System Frame Number"] = isysfn;
                record_item_j["Subframe Number"] = isubfn;
                record_item_j["Enabled"] = ienabled;
                const char* enabled_name = search_name(LtePhyServingCellComLoop_Record_Enabled, ARRAY_SIZE(LtePhyServingCellComLoop_Record_Enabled, ValueName), ienabled);
                record_item_j["Enabled"] = (enabled_name ? enabled_name : "(MI)Unknown");


                unsigned int i_cia = record_item_j["COM IIR Alpha"];
                record_item_j["COM IIR Alpha"] = static_cast<double>(i_cia) / 32765.0;

                unsigned int i_ca0 = record_item_j["COM Adjustment[0]"];
                record_item_j["COM Adjustment[0]"] = static_cast<double>(i_ca0) / 64.0;

                unsigned int i_ca1 = record_item_j["COM Adjustment[1]"];
                record_item_j["COM Adjustment[1]"] = static_cast<double>(i_ca1) / 64.0;

                unsigned int i_cnb0 = record_item_j["COM Unwrap Base[0]"];
                record_item_j["COM Unwrap Base[0]"] = static_cast<double>(i_cnb0) / 64.0;

                unsigned int i_cnb1 = record_item_j["COM Unwrap Base[1]"];
                record_item_j["COM Unwrap Base[1]"] = static_cast<double>(i_cnb1) / 64.0;

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY Serving Cell COM Loop version: 0x%x", pkt_ver);
            return 0;
    }
}


#endif //LOGIN_LTE_PHY_SERVING_CELL_COM_LOOP_H
