//
// Created by Vladimir Ponomarenko on 02.04.2025.
//

#ifndef LOGIN_LTE_PHY_SYSTEM_SCAN_RESULTS_H
#define LOGIN_LTE_PHY_SYSTEM_SCAN_RESULTS_H

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;

const Fmt LtePhySystemScanResults_Fmt [] = {
        {UINT, "Version", 1},
};

// --- Version 2 ---
const Fmt LtePhySystemScanResults_Payload_v2 [] = {
        {SKIP, NULL, 1},
        {UINT, "Num Candidates", 2}
};

const Fmt LtePhySystemScanResults_Candidate_v2 [] = {
        {UINT, "EARFCN", 4},
        {UINT, "Band", 2},
        {UINT, "Bandwidth (MHz)", 2},
        {UINT, "Energy (dBm/100KHz)", 2},
        {SKIP, NULL, 2},
};

// --- Version 32 ---
const Fmt LtePhySystemScanResults_Payload_v32 [] = {
        {UINT, "Use Init Search", 1},
        {UINT, "Num Candidates", 2}
};

const Fmt LtePhySystemScanResults_Candidate_v32 [] = {
        {UINT, "EARFCN", 4},
        {UINT, "Band", 2},
        {UINT, "Bandwidth (MHz)", 2},
        {UINT, "Energy (dBm/100KHz)", 2},
        {SKIP, NULL, 2},
};

const ValueName ValueNameUseInitSearch[] = {
        {0, "False"},
        {1, "True"}
};


static int _decode_lte_phy_system_scan_results_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

   // LOGD("Decoding LTE_PHY_System_Scan_Results with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 2: {
            offset += _decode_by_fmt(LtePhySystemScanResults_Payload_v2,
                                     ARRAY_SIZE(LtePhySystemScanResults_Payload_v2, Fmt),
                                     b, offset, length, j);
            int num_candidates = j["Num Candidates"];

            json candidates_j = json::array();
            for (int i = 0; i < num_candidates; ++i) {
                json candidate_item_j;
                offset += _decode_by_fmt(LtePhySystemScanResults_Candidate_v2,
                                         ARRAY_SIZE(LtePhySystemScanResults_Candidate_v2, Fmt),
                                         b, offset, length, candidate_item_j);


                int iBandwidth = candidate_item_j["Bandwidth (MHz)"];
                iBandwidth = (iBandwidth - 1) * 5;
                candidate_item_j["Bandwidth (MHz)"] = iBandwidth;

                unsigned int rawEnergy = candidate_item_j["Energy (dBm/100KHz)"];
                int iEnergy = static_cast<int>(rawEnergy) - 65536;
                candidate_item_j["Energy (dBm/100KHz)"] = iEnergy;

                candidates_j.push_back(candidate_item_j);
            }
            j["Candidates"] = candidates_j;
            return offset - start;
        }
        case 32: {
            offset += _decode_by_fmt(LtePhySystemScanResults_Payload_v32,
                                     ARRAY_SIZE(LtePhySystemScanResults_Payload_v32, Fmt),
                                     b, offset, length, j);

            int temp = j["Use Init Search"];
            int iUseInitSearch = temp & 0x1;
            j["Use Init Search"] = iUseInitSearch;
            // const char* use_init_name = search_name(ValueNameUseInitSearch, ARRAY_SIZE(ValueNameUseInitSearch, ValueName), iUseInitSearch);
            // j["Use Init Search"] = (use_init_name ? use_init_name : "(MI)Unknown");

            int num_candidates = j["Num Candidates"];

            json candidates_j = json::array();
            for (int i = 0; i < num_candidates; ++i) {
                json candidate_item_j;
                offset += _decode_by_fmt(LtePhySystemScanResults_Candidate_v32,
                                         ARRAY_SIZE(LtePhySystemScanResults_Candidate_v32, Fmt),
                                         b, offset, length, candidate_item_j);

                int iBandwidth = candidate_item_j["Bandwidth (MHz)"];
                iBandwidth = (iBandwidth - 1) * 5;
                candidate_item_j["Bandwidth (MHz)"] = iBandwidth;

                unsigned int rawEnergy = candidate_item_j["Energy (dBm/100KHz)"];
                int iEnergy = static_cast<int>(rawEnergy) - 65536;
                candidate_item_j["Energy (dBm/100KHz)"] = iEnergy;

                candidates_j.push_back(candidate_item_j);
            }
            j["Candidates"] = candidates_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY System Scan Result version: 0x%x", pkt_ver);
            // j["unknown_payload_hex"] = bytes_to_hex_string(b + start, length - start);
            return 0;
    }
}

#endif //LOGIN_LTE_PHY_SYSTEM_SCAN_RESULTS_H
