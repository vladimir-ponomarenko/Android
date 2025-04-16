//
// Created by Vladimir Ponomarenko on 10.04.2025.
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



const Fmt LtePhyBplmnCellConfirm_Fmt[] = {
        {UINT, "Version", 1},
};


const Fmt LtePhyBplmnCellConfirm_Payload_v4[] = {
        {UINT, "Standards Version", 1},
        {SKIP, NULL, 2},
        {UINT, "Frequency", 4},
        {UINT, "RSRP", 2},
        {SKIP, NULL, 2},
        {UINT, "BW", 1},
        {UINT, "Cell ID", 2},
        {SKIP, NULL, 1},
        {UINT, "SRX Lev Calculated", 4},
        {PLACEHOLDER, "SRX Lev", 0},
};



const Fmt LtePhyBplmnCellConfirm_Rel9Info[] = {

        {UINT, "Rel 9 Info S Qual Calculated", 4},
        {PLACEHOLDER, "Rel 9 Info S Qual", 0},
};

const ValueName LtePhyBplmnCellConfirm_StandardsVersion[] = {
        {1, "Release 9"},
};

const ValueName LtePhyBplmnCellConfirm_SRXLevCalculated[] = {
        {0, "false"},
        {1, "true"},
};

const ValueName LtePhyBplmnCellConfirm_Rel9InfoSQualCalculated[] = {
        {0, "No"},
        {1, "Yes"},
};




static int _decode_lte_phy_bplmn_cell_confirm_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    LOGD("Decoding LTE_PHY_BPLMN_Cell_Confirm with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 4: {

            int consumed_v4 = _decode_by_fmt(LtePhyBplmnCellConfirm_Payload_v4,
                                             ARRAY_SIZE(LtePhyBplmnCellConfirm_Payload_v4, Fmt),
                                             b, offset, length, j);
            offset += consumed_v4;

            int iStandardsVersion = j["Standards Version"];
            const char* std_ver_name = search_name(LtePhyBplmnCellConfirm_StandardsVersion, ARRAY_SIZE(LtePhyBplmnCellConfirm_StandardsVersion, ValueName), iStandardsVersion);
            j["Standards Version"] = (std_ver_name ? std_ver_name : "(MI)Unknown");



            int16_t iRSRP = static_cast<int16_t>(j["RSRP"].get<unsigned int>());







            j["RSRP"] = static_cast<int>(iRSRP);


            j["Cell ID"] = j["Cell ID"].get<unsigned int>() & 1023;


            unsigned int non_decode_p1 = j["SRX Lev Calculated"];
            j["SRX Lev Calculated"] = non_decode_p1 & 1;
            j["SRX Lev"] = (non_decode_p1 >> 1) & 65535;
            const char* srx_calc_name = search_name(LtePhyBplmnCellConfirm_SRXLevCalculated, ARRAY_SIZE(LtePhyBplmnCellConfirm_SRXLevCalculated, ValueName), j["SRX Lev Calculated"]);
            j["SRX Lev Calculated"] = (srx_calc_name ? srx_calc_name : "(MI)Unknown");


            if (iStandardsVersion == 1) {

                json rel9_info_j;
                int consumed_rel9 = _decode_by_fmt(LtePhyBplmnCellConfirm_Rel9Info,
                                                   ARRAY_SIZE(LtePhyBplmnCellConfirm_Rel9Info, Fmt),
                                                   b, offset, length, rel9_info_j);
                offset += consumed_rel9;

                unsigned int non_decode_p2 = rel9_info_j["Rel 9 Info S Qual Calculated"];
                j["Rel 9 Info S Qual Calculated"] = non_decode_p2 & 1;
                uint16_t s_qual_raw = (non_decode_p2 >> 1) & 65535;

                int16_t s_qual_signed = static_cast<int16_t>(s_qual_raw);

                j["Rel 9 Info S Qual"] = static_cast<int>(s_qual_signed);
                const char* sq_calc_name = search_name(LtePhyBplmnCellConfirm_Rel9InfoSQualCalculated, ARRAY_SIZE(LtePhyBplmnCellConfirm_Rel9InfoSQualCalculated, ValueName), j["Rel 9 Info S Qual Calculated"]);
                j["Rel 9 Info S Qual Calculated"] = (sq_calc_name ? sq_calc_name : "(MI)Unknown");
            }

            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY BPLMN Cell Confirm version: 0x%x", pkt_ver);
            return 0;
    }
}