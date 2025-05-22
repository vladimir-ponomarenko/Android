//
// Created by Vladimir Ponomarenko on 06.04.2025.
//

#ifndef LOGIN_LTE_PHY_RLM_REPORT_H
#define LOGIN_LTE_PHY_RLM_REPORT_H

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;



const Fmt LtePhyRlmReport_Fmt[] = {
        {UINT, "Version", 1},
};

const Fmt LtePhyRlmReport_Payload_v1[] = {
        {SKIP, NULL, 2},
        {UINT, "Number of Records", 1},
};

const Fmt LtePhyRlmReport_Record_v1[] = {
        {UINT, "System Frame Number", 2},
        {PLACEHOLDER, "Sub-frame Number", 0},
        {UINT, "Out of Sync BLER (%)", 2},
        {UINT, "In Sync BLER (%)", 2},
        {UINT, "Out of Sync Count", 2},
        {PLACEHOLDER, "In Sync Count", 0},
        {UINT, "T310 Timer Status", 1},
        {SKIP, NULL, 3},
};




static int _decode_lte_phy_rlm_report_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    LOGD("Decoding LTE_PHY_RLM_Report with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 1: {
            offset += _decode_by_fmt(LtePhyRlmReport_Payload_v1,
                                     ARRAY_SIZE(LtePhyRlmReport_Payload_v1, Fmt),
                                     b, offset, length, j);
            int num_records = j["Number of Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyRlmReport_Record_v1,
                                         ARRAY_SIZE(LtePhyRlmReport_Record_v1, Fmt),
                                         b, offset, length, record_item_j);

                int temp_sfn = record_item_j["System Frame Number"];
                record_item_j["System Frame Number"] = temp_sfn & 1023;
                record_item_j["Sub-frame Number"] = (temp_sfn >> 10) & 15;

                int i_oos_bler = record_item_j["Out of Sync BLER (%)"];
                record_item_j["Out of Sync BLER (%)"] = static_cast<double>(i_oos_bler) * 100.0 / 32768.0;

                int i_is_bler = record_item_j["In Sync BLER (%)"];
                record_item_j["In Sync BLER (%)"] = static_cast<double>(i_is_bler) * 100.0 / 32768.0;

                int temp_count = record_item_j["Out of Sync Count"];
                record_item_j["Out of Sync Count"] = temp_count & 31;
                record_item_j["In Sync Count"] = (temp_count >> 5) & 31;



                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY RLM Report version: 0x%x", pkt_ver);
            return 0;
    }
}

#endif //LOGIN_LTE_PHY_RLM_REPORT_H
