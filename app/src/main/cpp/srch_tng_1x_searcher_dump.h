//
// Created by Vladimir Ponomarenko on 23.03.2025.
//

#pragma once

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include "log_packet.h"
#include "log_packet_utils.h"
#include "utils.h"
#include "consts.h"
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "gsm_rr_cell_reselection_meas.h"
#include "lte/phy/lte_phy_serving_cell_meas_res.h"
#include "lte/rrc/lte_rrc_mib_message_log_packet.h"
#include "wcdma_signaling_messages.h"

using json = nlohmann::json;
using namespace std;

const Fmt SrchTng1xsd_Fmt [] = {
        {UINT, "Version", 1},
        {UINT, "Number of Subpackets", 1},
        {SKIP, NULL, 2},
};

const Fmt SrchTng1xsd_Subpkt_header_v1 [] = {
        {UINT, "Subpacket ID", 1},
        {UINT, "Subpacket Version", 1},
        {UINT, "Subpacket Size", 2},
};

const Fmt SrchTng1xsd_Subpkt_payload_2v2 [] = {
        {UINT, "Tx Power", 2},  // x / 10.0
        {UINT, "Tx Gain Adj", 1},
        {UINT, "Tx Power Limit", 1},    // (x - 157) / 3
        {BYTE_STREAM, "Dev Mode Bit Mask", 1},
        {UINT, "Num Chains", 1},
        {UINT, "Diversity Enabled", 1},
        {SKIP, NULL, 1},
};

const Fmt SrchTng1xsd_RxChain_v2 [] = {
        {UINT, "Client", 1},
        {UINT, "Band Class", 1},
        {UINT, "Channel", 2},
        {UINT, "Rx AGC", 1},    //  (x - 446) / 3
        {BYTE_STREAM, "Dev Cap Mask", 1},
        {UINT, "RF Dev Num", 1},
        {SKIP, NULL, 1},
};

const Fmt SrchTng1xsd_Subpkt_payload_7v1 [] = {
        {UINT, "Technology", 1},
        {UINT, "Queue", 1},
        {UINT, "Priority", 1},
        {SKIP, NULL, 1},
        {UINT, "Frequency Offset", 2},  // x * 4.6875
        {UINT, "Num Tasks", 1},
        {UINT, "Num Peaks", 1},
};

const Fmt SrchTng1xsd_Task_v1 [] = {
        {UINT, "QOF", 2},   // 2 bits
        {PLACEHOLDER, "HDR Burst Len", 0},  // 2 bits
        {PLACEHOLDER, "Div Antenna", 0},    // 2 bits
        {PLACEHOLDER, "Pilot PN", 0},   // 10 bits
        {UINT, "Coherent Length", 2},   // 13 bits
        {PLACEHOLDER, "Coh Trunc", 0},  // 3 bits
        {UINT, "Non Coh Len", 1},
        {UINT, "Pilot Phase", 1},
        {UINT, "Walsh Index", 2},
        {UINT, "Window Start", 2},
        {UINT, "Window Size", 2},
        {UINT, "Pilot Set", 4},
};

const Fmt SrchTng1xsd_Peak_v1 [] = {
        {UINT, "Peak Position", 4},
        {UINT, "Peak Energy", 2},
        {SKIP, NULL, 2},
};

const ValueName ValueNameBandClassCDMA[] = {
        {0,  "BC0"},
        {1,  "BC1"},
        {2,  "BC2"},
        {3,  "BC3"},
        {4,  "BC4"},
        {5,  "BC5"},
        {6,  "BC6"},
        {7,  "BC7"},
        {8,  "BC8"},
        {9,  "BC9"},
        {10, "BC10"},
        {11, "BC11"},
        {12, "BC12"},
        {13, "BC13"},
        {14, "BC14"},
        {15, "BC15"},
        {16, "BC16"},
        {17, "BC17"},
        {18, "BC18"},
        {19, "BC19"}
};

const ValueName ValueNameTechnology[] = {
        {0, "CDMA"},
        {1, "HDR"},
        {2, "GSM"},
        {3, "WCDMA"},
        {4, "LTE"},
        {5, "TDSCDMA"},
        {6, "Other"}
};

const ValueName ValueNameQueue[] = {
        {0, "Low"},
        {1, "Medium"},
        {2, "High"}
};
const ValueName ValueNamePilotSet[] = {
        {0, "Inactive"},
        {1, "Active"},
        {2, "Candidate"},
        {4, "Neighbor"},
        {8, "Remaining"},
};

static int _decode_srch_tng_1xsd_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    //LOGD("Decoding Srch_TNG_1x_Searcher_Dump with version: 0x%x", pkt_ver);

    switch (pkt_ver) {
        case 1: {
            int num_subpkts = j["Number of Subpackets"];
            json all_subpkts_j = json::array();

            for (int i = 0; i < num_subpkts; i++) {
                json subpkt_j;
                int start_subpkt = offset;

                // Decode subpacket header
                offset += _decode_by_fmt(SrchTng1xsd_Subpkt_header_v1,
                                         ARRAY_SIZE(SrchTng1xsd_Subpkt_header_v1, Fmt),
                                         b, offset, length, subpkt_j);

                int subpkt_id = subpkt_j["Subpacket ID"];
                int subpkt_ver = subpkt_j["Subpacket Version"];
                int subpkt_size = subpkt_j["Subpacket Size"];


                if (subpkt_id == 2 && subpkt_ver == 2) {
                    // RF subpkt v2
                    offset += _decode_by_fmt(SrchTng1xsd_Subpkt_payload_2v2,
                                             ARRAY_SIZE(SrchTng1xsd_Subpkt_payload_2v2, Fmt),
                                             b, offset, length, subpkt_j);

                    subpkt_j["Tx Power"] = (double)subpkt_j["Tx Power"].get<int>() / 10.0;
                    subpkt_j["Tx Power Limit"] = (double)(subpkt_j["Tx Power Limit"].get<int>() - 157) / 3.0;

                    const char* diversity_enabled_name = search_name(
                            ValueNameEnableOrDisable,
                            ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                            subpkt_j["Diversity Enabled"]
                    );
                    subpkt_j["Diversity Enabled"] = (diversity_enabled_name != NULL) ? diversity_enabled_name : "(MI)Unknown";


                    int num_chains = subpkt_j["Num Chains"];
                    json chains_j = json::array();
                    for (int k = 0; k < num_chains; k++) {
                        json chain_item_j;
                        offset += _decode_by_fmt(SrchTng1xsd_RxChain_v2,
                                                 ARRAY_SIZE(SrchTng1xsd_RxChain_v2, Fmt),
                                                 b, offset, length, chain_item_j);

                        chain_item_j["Rx AGC"] = (double)(chain_item_j["Rx AGC"].get<int>() - 446) / 3.0;

                        const char* band_class_name = search_name(ValueNameBandClassCDMA,
                                                                  ARRAY_SIZE(ValueNameBandClassCDMA, ValueName),
                                                                  chain_item_j["Band Class"]);
                        chain_item_j["Band Class"] = (band_class_name != NULL) ? band_class_name : "(MI)Unknown";

                        chains_j.push_back(chain_item_j);
                    }
                    subpkt_j["Clients"] = chains_j;

                }
                else if (subpkt_id == 7 && subpkt_ver == 1) {
                    // Searcher 4 subpkt v1
                    offset += _decode_by_fmt(SrchTng1xsd_Subpkt_payload_7v1,
                                             ARRAY_SIZE(SrchTng1xsd_Subpkt_payload_7v1, Fmt),
                                             b, offset, length, subpkt_j);

                    const char* technology_name = search_name(ValueNameTechnology,
                                                              ARRAY_SIZE(ValueNameTechnology, ValueName),
                                                              subpkt_j["Technology"]);
                    subpkt_j["Technology"] = (technology_name != NULL) ? technology_name : "(MI)Unknown";

                    const char* queue_name = search_name(ValueNameQueue,
                                                         ARRAY_SIZE(ValueNameQueue, ValueName),
                                                         subpkt_j["Queue"]);
                    subpkt_j["Queue"] = (queue_name != NULL) ? queue_name : "(MI)Unknown";

                    subpkt_j["Frequency Offset"] = subpkt_j["Frequency Offset"].get<int>() * 4.6875;

                    int num_tasks = subpkt_j["Num Tasks"];
                    int num_peaks = subpkt_j["Num Peaks"];

                    json tasks_j = json::array();
                    for (int iter = 0; iter < num_tasks; iter++) {
                        json task_item_j;
                        offset += _decode_by_fmt(SrchTng1xsd_Task_v1,
                                                 ARRAY_SIZE(SrchTng1xsd_Task_v1, Fmt),
                                                 b, offset, length, task_item_j);

                        int temp = task_item_j["QOF"];
                        int iQOF = temp & 3;
                        int iHBL = (temp >> 2) & 3;
                        int iDA = (temp >> 4) & 3;
                        int iPPN = (temp >> 6) & 1023;
                        temp = task_item_j["Coherent Length"];
                        int iCL = temp & 8191;
                        int iCT = (temp >> 13) & 7;

                        task_item_j["QOF"] = iQOF;
                        task_item_j["HDR Burst Len"] = iHBL;
                        task_item_j["Div Antenna"] = iDA;
                        task_item_j["Pilot PN"] = iPPN;
                        task_item_j["Coherent Length"] = iCL;
                        task_item_j["Coh Trunc"] = iCT;

                        unsigned int utemp = task_item_j["Pilot Set"];
                        if (utemp & 1)
                            utemp = 1;
                        else if (utemp & 2)
                            utemp = 2;
                        else if (utemp & 4)
                            utemp = 4;
                        else if (utemp & 8)
                            utemp = 8;
                        else if (!(utemp & 15) | 0)
                            utemp = 0;

                        task_item_j["Pilot Set"] = utemp;
                        const char* pilot_set_name = search_name(ValueNamePilotSet,
                                                                 ARRAY_SIZE(ValueNamePilotSet, ValueName),
                                                                 task_item_j["Pilot Set"]);
                        task_item_j["Pilot Set"] = (pilot_set_name != NULL) ? pilot_set_name : "(MI)Unknown";

                        json peaks_j = json::array();
                        for (int k = 0; k < num_peaks; k++) {
                            json peak_item_j;
                            offset += _decode_by_fmt(SrchTng1xsd_Peak_v1,
                                                     ARRAY_SIZE(SrchTng1xsd_Peak_v1, Fmt),
                                                     b, offset, length, peak_item_j);
                            peaks_j.push_back(peak_item_j);
                        }
                        task_item_j["Peaks"] = peaks_j;
                        tasks_j.push_back(task_item_j);
                    }
                    subpkt_j["Tasks"] = tasks_j;

                }
                else {
                    LOGD("(MI)Unknown Srch TNG 1x Searcher Dump subpkt id and version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                }

                all_subpkts_j.push_back(subpkt_j);
                offset += subpkt_size - (offset - start_subpkt);
            }
            j["Subpackets"] = all_subpkts_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown Srch TNG 1x Searcher Dump version: 0x%x\n", pkt_ver);
            return 0;
    }
}

