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



const Fmt LtePhyCdrxEventsInfo_Fmt[] = {
        {UINT, "Version", 1},
};


const Fmt LtePhyCdrxEventsInfo_Payload_v1[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};

const Fmt LtePhyCdrxEventsInfo_Record_v1[] = {
        {UINT, "SFN", 4},
        {PLACEHOLDER, "Sub-FN", 0},
        {PLACEHOLDER, "CDRX Event", 0},
};


const Fmt LtePhyCdrxEventsInfo_Payload_v2[] = {
        {UINT, "Num Records", 1},
        {SKIP, NULL, 2},
};

const Fmt LtePhyCdrxEventsInfo_Record_v2[] = {
        {UINT, "SFN", 4},
        {PLACEHOLDER, "Sub-FN", 0},
        {PLACEHOLDER, "CDRX Event", 0},
        {UINT, "Internal Field Mask", 4},
};



const ValueName ValueNameCDRXEvent[] = {
        {0, "WAKEUP"},
        {1, "ON_DURATION_EXPIRY"},
        {2, "INACTIVITY_EXPIRY"},
        {3, "RETX_TIMER_EXPIRY"},
        {4, "MISSING_CYCLE_EXPIRY"},
        {5, "DRX_CTRL_RCVD"},
        {6, "DL_HARQ_END"},
        {7, "UL_GRANT_RCVD"},
        {8, "ACTIVE_TIME_START"},
        {9, "ACTIVE_TIME_END"},
        {10, "UL_RETX_TIMER_EXPIRY"},
        {11, "SLEEP"},
        {12, "ON_DURATION_STARTED"},
        {13, "DRX_SHORT_CYCLE_STARTED"},
        {14, "DRX_SHORT_CYCLE_ENDED"},
        {15, "PSM_ENTER"},
        {16, "WAKEUP_MISSED_CYCLE_EXPIRY"},
        {17, "CATM1_UL_RETX_TIMER_EXPIRY"},
};




static int _decode_lte_phy_cdrx_events_info_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

   // LOGD("Decoding LTE_PHY_CDRX_Events_Info with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 1: {
            offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Payload_v1,
                                     ARRAY_SIZE(LtePhyCdrxEventsInfo_Payload_v1, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Record_v1,
                                         ARRAY_SIZE(LtePhyCdrxEventsInfo_Record_v1, Fmt),
                                         b, offset, length, record_item_j);

                unsigned int utemp = record_item_j["SFN"];
                record_item_j["SFN"] = utemp & 1023;
                record_item_j["Sub-FN"] = (utemp >> 10) & 15;
                record_item_j["CDRX Event"] = (utemp >> 14) & 63;

                const char* event_name = search_name(ValueNameCDRXEvent, ARRAY_SIZE(ValueNameCDRXEvent, ValueName), record_item_j["CDRX Event"]);
                record_item_j["CDRX Event"] = (event_name ? event_name : "(MI)Unknown");

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        case 2: {
            offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Payload_v2,
                                     ARRAY_SIZE(LtePhyCdrxEventsInfo_Payload_v2, Fmt),
                                     b, offset, length, j);
            int num_records = j["Num Records"];

            json records_j = json::array();
            for (int i = 0; i < num_records; ++i) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Record_v2,
                                         ARRAY_SIZE(LtePhyCdrxEventsInfo_Record_v2, Fmt),
                                         b, offset, length, record_item_j);

                unsigned int utemp = record_item_j["SFN"];
                record_item_j["SFN"] = utemp & 1023;
                record_item_j["Sub-FN"] = (utemp >> 10) & 15;
                record_item_j["CDRX Event"] = (utemp >> 14) & 63;

                const char* event_name = search_name(ValueNameCDRXEvent, ARRAY_SIZE(ValueNameCDRXEvent, ValueName), record_item_j["CDRX Event"]);
                record_item_j["CDRX Event"] = (event_name ? event_name : "(MI)Unknown");

                unsigned int internal_mask = record_item_j["Internal Field Mask"];
                string strInternalFieldMask = "|";
                int count = 0;
                int check = 0;

                if (((internal_mask >> 0) & 1)) { strInternalFieldMask += "CYCLE_START|"; count++; }

                if (((internal_mask >> 2) & 1)) { strInternalFieldMask += "ON_DURATION_TIMER|"; count++; }
                if (((internal_mask >> 3) & 1)) { strInternalFieldMask += "INACTIVITY_TIMER|"; count++; }
                if (((internal_mask >> 4) & 1)) { strInternalFieldMask += "DRX_RETX_TIMER|"; count++; }
                if (((internal_mask >> 5) & 1)) { strInternalFieldMask += "MISSING_CYCLE_TIMER|"; count++; }
                if (((internal_mask >> 6) & 1)) { strInternalFieldMask += "PENDING_SR|"; count++; }
                if (((internal_mask >> 7) & 1)) { strInternalFieldMask += "PENDING_UL_RETX|"; count++; }
                if (((internal_mask >> 8) & 1)) { strInternalFieldMask += "RACH|"; count++; }

                if (((internal_mask >> 11) & 1)) { strInternalFieldMask += "T310|"; count++; }

                if (((internal_mask >> 15) & 1)) { strInternalFieldMask += "WAKEUP_MISSED_CYCLE_TIMER|"; count++; }
                if (((internal_mask >> 16) & 1)) { strInternalFieldMask += "CATM1_UL_RETX_TIMER|"; count++; }



                for (int bit = 0; bit < 32; bit++) {
                    if (((internal_mask >> bit) & 1)) {
                        check++;
                    }
                }

                if (check == 0 && count == 0) {
                    strInternalFieldMask += "NO_EVENTS|";
                } else if (check != count) {
                    strInternalFieldMask += "(MI)Unknown|";
                }

                record_item_j["Internal Field Mask"] = strInternalFieldMask;

                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY CDRX Events Info version: 0x%x", pkt_ver);
            return 0;
    }
}

