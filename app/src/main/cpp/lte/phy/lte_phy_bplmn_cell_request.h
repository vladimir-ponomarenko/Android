//
// Created by Vladimir Ponomarenko on 10.04.2025.
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



const Fmt LtePhyBplmnCellRequest_Fmt[] = {
        {UINT, "Version", 1},
};


const Fmt LtePhyBplmnCellRequest_Payload_v4[] = {
        {UINT, "Standards Version", 1},
        {SKIP, NULL, 2},
        {UINT, "Frequency", 4},
        {UINT, "Cell ID", 2},
        {PLACEHOLDER, "Barred Status", 0},
        {SKIP, NULL, 2},
        {UINT, "Q Rx Lev Min", 4},
        {PLACEHOLDER, "Q Rx Lev Min Offset", 0},
        {PLACEHOLDER, "P Max", 0},
};



const Fmt LtePhyBplmnCellRequest_Rel9Info[] = {
        {PLACEHOLDER, "Rel 9 Info Q Qual Min Data", 0},
        {PLACEHOLDER, "Rel 9 Info Q Qual Min Offset", 0},
};

const ValueName LtePhyBplmnCellRequest_StandardsVersion[] = {
        {1, "Release 9"},

};

const ValueName LtePhyBplmnCellRequest_BarredStatus[] = {
        {0, "Not Barred"},
        {1, "Cell Barred"},

};




static int _decode_lte_phy_bplmn_cell_request_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

   // LOGD("Decoding LTE_PHY_BPLMN_Cell_Request with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 4: {
            offset += _decode_by_fmt(LtePhyBplmnCellRequest_Payload_v4,
                                     ARRAY_SIZE(LtePhyBplmnCellRequest_Payload_v4, Fmt),
                                     b, offset, length, j);

            int iStandardsVersion = j["Standards Version"];
            const char* std_ver_name = search_name(LtePhyBplmnCellRequest_StandardsVersion, ARRAY_SIZE(LtePhyBplmnCellRequest_StandardsVersion, ValueName), iStandardsVersion);
            j["Standards Version"] = (std_ver_name ? std_ver_name : "(MI)Unknown");

            int non_decode_p1 = j["Cell ID"];
            j["Cell ID"] = non_decode_p1 & 1023;
            j["Barred Status"] = (non_decode_p1 >> 10) & 15;
            const char* barred_status_name = search_name(LtePhyBplmnCellRequest_BarredStatus, ARRAY_SIZE(LtePhyBplmnCellRequest_BarredStatus, ValueName), j["Barred Status"]);
            j["Barred Status"] = (barred_status_name ? barred_status_name : "(MI)Unknown");

            unsigned int non_decode_p2 = j["Q Rx Lev Min"];
            int iQRxLevMin = non_decode_p2 & 255;
            int iQRxLevMinOffset = (non_decode_p2 >> 8) & 15;
            int iPMax = (non_decode_p2 >> 12) & 255;


            iQRxLevMin -= 256;

            j["Q Rx Lev Min"] = iQRxLevMin;
            j["Q Rx Lev Min Offset"] = iQRxLevMinOffset;
            j["P Max"] = iPMax;

            if (iStandardsVersion == 1) {

                j["Rel 9 Info Q Qual Min Data"] = (non_decode_p2 >> 20) & 63;
                j["Rel 9 Info Q Qual Min Offset"] = (non_decode_p2 >> 26) & 63;



            }

            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY BPLMN Cell Request version: 0x%x", pkt_ver);
            return 0;
    }
}
