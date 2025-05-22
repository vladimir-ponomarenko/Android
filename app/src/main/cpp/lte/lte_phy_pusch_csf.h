//
// Created by Vladimir Ponomarenko on 06.04.2025.
//

#ifndef LOGIN_LTE_PHY_PUSCH_CSF_H
#define LOGIN_LTE_PHY_PUSCH_CSF_H

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;



const Fmt LtePhyPuschCsf_Fmt[] = {
        {UINT, "Version", 1},
};


const Fmt LtePhyPuschCsf_Payload_v22[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
};


const Fmt LtePhyPuschCsf_Payload_v23[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v42[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Rank Index", 0},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v101[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v102[] = {
        {UINT, "Start System Sub-frame Number", 2},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {UINT, "PUSCH Reporting Mode", 1},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {UINT, "Number of Subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 27},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num CSIrs Ports", 0},
};



const Fmt LtePhyPuschCsf_Payload_v103[] = {
        {UINT, "Start System Sub-frame Number", 4},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {PLACEHOLDER, "PUSCH Reporting Mode", 0},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {PLACEHOLDER, "Wideband PMI1", 0},
        {PLACEHOLDER, "Number of subbands", 0},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {UINT, "WideBand CQI CW1", 2},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Size M", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {UINT, "Single MB PMI", 1},
        {PLACEHOLDER, "CSF Tx Mode", 0},
        {SKIP, NULL, 14},
        {UINT, "Forced Max RI", 1},
        {SKIP, NULL, 5},
        {UINT, "Alt Cqi Table Data", 1},
        {SKIP, NULL, 7},
        {UINT, "Carrier Index", 1},
        {PLACEHOLDER, "Num Csirs Ports", 0},
};


const Fmt LtePhyPuschCsf_Payload_v142[] = {
        {UINT, "Start System Sub-frame Number", 4},
        {PLACEHOLDER, "Start System Frame Number", 0},
        {PLACEHOLDER, "Carrier Index", 0},
        {PLACEHOLDER, "Scell Index", 0},
        {PLACEHOLDER, "PUSCH Reporting Mode", 0},
        {PLACEHOLDER, "Csi Meas Set Index", 0},
        {PLACEHOLDER, "Rank Index", 0},
        {PLACEHOLDER, "Wideband PMI1", 0},
        {UINT, "Number of subbands", 4},
        {PLACEHOLDER, "WideBand CQI CW0", 0},
        {PLACEHOLDER, "WideBand CQI CW1", 0},
        {PLACEHOLDER, "SubBand Size (k)", 0},
        {PLACEHOLDER, "Size M", 0},
        {PLACEHOLDER, "Single WB PMI", 0},
        {PLACEHOLDER, "Single MB PMI", 0},
        {UINT, "CSF Tx Mode", 1},
        {SKIP, NULL, 25},
        {UINT, "Num Csirs Ports", 1},
        {UINT, "CRI", 4},
        {PLACEHOLDER, "UL Frame Number", 0},
        {PLACEHOLDER, "UL Subframe Number", 0},
        {PLACEHOLDER, "UL Payload Length", 0},
};


const ValueName ValueNamePuschReportingMode[] = {
        {0, "MODE_APERIODIC_RM12"},
        {1, "MODE_APERIODIC_RM20"},
        {2, "MODE_APERIODIC_RM22"},
        {3, "MODE_APERIODIC_RM30"},
        {4, "MODE_APERIODIC_RM31"},
        {5, "MODE_PERIODIC_RM10"},
        {6, "MODE_PERIODIC_RM11"},
        {7, "MODE_PERIODIC_RM20"},
        {8, "MODE_PERIODIC_RM21"}
};



const ValueName ValueNameCSFTxMode[] = {
        {0, "TX_MODE_1"},
        {1, "TX_MODE_2"},
        {2, "TX_MODE_3"},
        {3, "TX_MODE_4"},
        {4, "TX_MODE_5"},
        {5, "TX_MODE_6"},
        {6, "TX_MODE_7"},
        {7, "TX_MODE_8"},
        {8, "TX_MODE_9"},
        {9, "TX_MODE_10"}
};

const ValueName ValueNameCsiMeasSetIndex[] = {
        {0, "Set 1"},
        {1, "Set 2"}
};




static int _decode_lte_phy_pusch_csf_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    //LOGD("Decoding LTE_PHY_PUSCH_CSF with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 142: {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v142,
                                     ARRAY_SIZE(LtePhyPuschCsf_Payload_v142, Fmt),
                                     b, offset, length, j);

            unsigned int utemp0 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = (utemp0 >> 0) & 0xf;
            j["Start System Frame Number"] = (utemp0 >> 4) & 0x3ff;
            j["Carrier Index"] = (utemp0 >> 14) & 0xf;
            j["Scell Index"] = (utemp0 >> 18) & 0x1f;

            j["PUSCH Reporting Mode"] = (utemp0 >> 24) & 0x7;
            j["Csi Meas Set Index"] = (utemp0 >> 27) & 0x1;
            j["Rank Index"] = (utemp0 >> 28) & 0x3;
            j["Wideband PMI1"] = (utemp0 >> 30) & 0x3;

            unsigned int utemp1 = j["Number of subbands"];
            j["Number of subbands"] = (utemp1 >> 2) & 0x1f;
            j["WideBand CQI CW0"] = (utemp1 >> 7) & 0xf;
            j["WideBand CQI CW1"] = (utemp1 >> 11) & 0xf;
            j["SubBand Size (k)"] = (utemp1 >> 15) & 0xf;
            j["Size M"] = (utemp1 >> 19) & 0x7;

            j["Single WB PMI"] = (utemp1 >> 24) & 0xf;
            j["Single MB PMI"] = (utemp1 >> 28) & 0xf;

            unsigned int utemp2 = j["CSF Tx Mode"];
            j["CSF Tx Mode"] = (utemp2 >> 0) & 0xf;

            unsigned int utemp3 = j["Num Csirs Ports"];
            j["Num Csirs Ports"] = (utemp3 >> 0) & 0xf;


            unsigned int utemp4 = j["CRI"];
            j["CRI"] = (utemp4 >> 0) & 0x7;

            j["UL Frame Number"] = (utemp4 >> 4) & 0x3ff;
            j["UL Subframe Number"] = (utemp4 >> 14) & 0xf;
            j["UL Payload Length"] = (utemp4 >> 18) & 0x7f;


            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");
            const char* mode_name = search_name(ValueNamePuschReportingMode, ARRAY_SIZE(ValueNamePuschReportingMode, ValueName), j["PUSCH Reporting Mode"]);
            j["PUSCH Reporting Mode"] = (mode_name ? mode_name : "(MI)Unknown");
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name ? rank_name : "(MI)Unknown");
            const char* txmode_name = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (txmode_name ? txmode_name : "(MI)Unknown");
            const char* csi_set_name = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name ? csi_set_name : "(MI)Unknown");


            return offset - start;
        }
        case 103: {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v103,
                                     ARRAY_SIZE(LtePhyPuschCsf_Payload_v103, Fmt),
                                     b, offset, length, j);

            unsigned int utemp0 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = (utemp0 >> 0) & 0xf;
            j["Start System Frame Number"] = (utemp0 >> 4) & 0x3ff;
            j["PUSCH Reporting Mode"] = (utemp0 >> 14) & 0x7;
            j["Csi Meas Set Index"] = (utemp0 >> 17) & 0x1;
            j["Rank Index"] = (utemp0 >> 18) & 0x3;
            j["Wideband PMI1"] = (utemp0 >> 20) & 0xf;
            j["Number of subbands"] = (utemp0 >> 24) & 0x1f;
            int wb_cqi0_part1 = (utemp0 >> 29) & 0x7;

            unsigned int utemp1 = j["WideBand CQI CW1"];
            int wb_cqi0_part2 = (utemp1 >> 0) & 0x1;
            j["WideBand CQI CW0"] = wb_cqi0_part1 | (wb_cqi0_part2 << 3);
            j["WideBand CQI CW1"] = (utemp1 >> 1) & 0xf;
            j["SubBand Size (k)"] = (utemp1 >> 5) & 0xf;
            j["Size M"] = (utemp1 >> 9) & 0x7;
            j["Single WB PMI"] = (utemp1 >> 12) & 0xf;

            unsigned int utemp2 = j["Single MB PMI"];
            j["Single MB PMI"] = (utemp2 >> 0) & 0xf;
            j["CSF Tx Mode"] = (utemp2 >> 4) & 0xf;



            unsigned int utemp3 = j["Alt Cqi Table Data"];
            j["Alt Cqi Table Data"] = (utemp3 >> 1) & 0x1;

            unsigned int utemp4 = j["Carrier Index"];
            j["Carrier Index"] = (utemp4 >> 0) & 0xf;
            j["Num Csirs Ports"] = (utemp4 >> 4) & 0xf;

            const char* mode_name = search_name(ValueNamePuschReportingMode, ARRAY_SIZE(ValueNamePuschReportingMode, ValueName), j["PUSCH Reporting Mode"]);
            j["PUSCH Reporting Mode"] = (mode_name ? mode_name : "(MI)Unknown");
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name ? rank_name : "(MI)Unknown");
            const char* txmode_name = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (txmode_name ? txmode_name : "(MI)Unknown");
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");
            const char* csi_set_name = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name ? csi_set_name : "(MI)Unknown");

            return offset - start;
        }
        case 102: {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v102,
                                     ARRAY_SIZE(LtePhyPuschCsf_Payload_v102, Fmt),
                                     b, offset, length, j);

            int temp0 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp0 & 15;
            j["Start System Frame Number"] = (temp0 >> 4) & 1023;
            int before_temp = (temp0 >> 14) & 3;

            int temp1 = j["PUSCH Reporting Mode"];
            j["PUSCH Reporting Mode"] = ((temp1 & 1) << 2) | before_temp;

            j["Rank Index"] = (temp1 >> 2) & 1;
            j["Csi Meas Set Index"] = (temp1 >> 1) & 1;

            unsigned int utemp = j["Number of Subbands"];
            j["Number of Subbands"] = utemp & 31;
            j["WideBand CQI CW0"] = (utemp >> 5) & 15;
            j["WideBand CQI CW1"] = (utemp >> 9) & 15;
            j["SubBand Size (k)"] = (utemp >> 13) & 127;
            j["Single WB PMI"] = (utemp >> 20) & 15;
            j["Single MB PMI"] = (utemp >> 24) & 15;
            j["CSF Tx Mode"] = (utemp >> 28) & 15;

            int temp2 = j["Carrier Index"];
            j["Carrier Index"] = temp2 & 15;
            j["Num CSIrs Ports"] = (temp2 >> 4) & 15;

            const char* mode_name = search_name(ValueNamePuschReportingMode, ARRAY_SIZE(ValueNamePuschReportingMode, ValueName), j["PUSCH Reporting Mode"]);
            j["PUSCH Reporting Mode"] = (mode_name ? mode_name : "(MI)Unknown");
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name ? rank_name : "(MI)Unknown");
            const char* txmode_name = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (txmode_name ? txmode_name : "(MI)Unknown");
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");
            const char* csi_set_name = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name ? csi_set_name : "(MI)Unknown");


            return offset - start;
        }
        case 101: {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v101,
                                     ARRAY_SIZE(LtePhyPuschCsf_Payload_v101, Fmt),
                                     b, offset, length, j);

            int temp0 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp0 & 15;
            j["Start System Frame Number"] = (temp0 >> 4) & 1023;
            int ileft = (temp0 >> 14) & 3;

            int temp1 = j["PUSCH Reporting Mode"];
            j["PUSCH Reporting Mode"] = ((temp1 & 1) << 2) | ileft;
            j["Csi Meas Set Index"] = (temp1 >> 1) & 1;
            j["Rank Index"] = (temp1 >> 2) & 63;

            unsigned int utemp = j["Number of Subbands"];
            j["Number of Subbands"] = utemp & 31;
            j["WideBand CQI CW0"] = (utemp >> 5) & 15;
            j["WideBand CQI CW1"] = (utemp >> 9) & 15;
            j["SubBand Size (k)"] = (utemp >> 13) & 127;
            j["Single WB PMI"] = (utemp >> 20) & 15;
            j["Single MB PMI"] = (utemp >> 24) & 15;
            j["CSF Tx Mode"] = (utemp >> 28) & 15;

            int temp2 = j["Carrier Index"];
            j["Carrier Index"] = temp2 & 15;
            j["Num CSIrs Ports"] = (temp2 >> 4) & 15;

            const char* mode_name = search_name(ValueNamePuschReportingMode, ARRAY_SIZE(ValueNamePuschReportingMode, ValueName), j["PUSCH Reporting Mode"]);
            j["PUSCH Reporting Mode"] = (mode_name ? mode_name : "(MI)Unknown");
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name ? rank_name : "(MI)Unknown");
            const char* csi_set_name = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name ? csi_set_name : "(MI)Unknown");
            const char* txmode_name = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (txmode_name ? txmode_name : "(MI)Unknown");
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            return offset - start;
        }
        case 42: {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v42,
                                     ARRAY_SIZE(LtePhyPuschCsf_Payload_v42, Fmt),
                                     b, offset, length, j);

            int temp0 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp0 & 15;
            j["Start System Frame Number"] = (temp0 >> 4) & 1023;
            int ileft = (temp0 >> 14) & 3;

            int temp1 = j["PUSCH Reporting Mode"];
            j["PUSCH Reporting Mode"] = ((temp1 & 1) << 2) | ileft;
            j["Rank Index"] = (temp1 >> 1) & 1;
            j["Csi Meas Set Index"] = (temp1 >> 2) & 1;

            unsigned int utemp = j["Number of Subbands"];
            j["Number of Subbands"] = utemp & 31;
            j["WideBand CQI CW0"] = (utemp >> 5) & 15;
            j["WideBand CQI CW1"] = (utemp >> 9) & 15;
            j["SubBand Size (k)"] = (utemp >> 13) & 127;
            j["Single WB PMI"] = (utemp >> 20) & 15;
            j["Single MB PMI"] = (utemp >> 24) & 15;
            j["CSF Tx Mode"] = (utemp >> 28) & 15;

            int temp2 = j["Carrier Index"];
            j["Carrier Index"] = temp2 & 15;
            j["Num CSIrs Ports"] = (temp2 >> 4) & 15;

            const char* mode_name = search_name(ValueNamePuschReportingMode, ARRAY_SIZE(ValueNamePuschReportingMode, ValueName), j["PUSCH Reporting Mode"]);
            j["PUSCH Reporting Mode"] = (mode_name ? mode_name : "(MI)Unknown");
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name ? rank_name : "(MI)Unknown");
            const char* csi_set_name = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name ? csi_set_name : "(MI)Unknown");
            const char* txmode_name = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (txmode_name ? txmode_name : "(MI)Unknown");
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            return offset - start;
        }
        case 23: {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v23,
                                     ARRAY_SIZE(LtePhyPuschCsf_Payload_v23, Fmt),
                                     b, offset, length, j);

            int temp0 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp0 & 15;
            j["Start System Frame Number"] = (temp0 >> 4) & 1023;
            int ileft = (temp0 >> 14) & 3;

            int temp1 = j["PUSCH Reporting Mode"];
            j["PUSCH Reporting Mode"] = ((temp1 & 1) << 2) | ileft;
            j["Rank Index"] = (temp1 >> 1) & 1;

            unsigned int utemp = j["Number of Subbands"];
            j["Number of Subbands"] = utemp & 31;
            j["WideBand CQI CW0"] = (utemp >> 5) & 15;
            j["WideBand CQI CW1"] = (utemp >> 9) & 15;
            j["SubBand Size (k)"] = (utemp >> 13) & 127;
            j["Single WB PMI"] = (utemp >> 20) & 15;
            j["Single MB PMI"] = (utemp >> 24) & 15;
            j["CSF Tx Mode"] = (utemp >> 28) & 15;

            int temp2 = j["Carrier Index"];
            j["Carrier Index"] = temp2 & 15;
            j["Num CSIrs Ports"] = (temp2 >> 4) & 15;

            const char* mode_name = search_name(ValueNamePuschReportingMode, ARRAY_SIZE(ValueNamePuschReportingMode, ValueName), j["PUSCH Reporting Mode"]);
            j["PUSCH Reporting Mode"] = (mode_name ? mode_name : "(MI)Unknown");
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name ? rank_name : "(MI)Unknown");
            const char* txmode_name = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (txmode_name ? txmode_name : "(MI)Unknown");
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            return offset - start;
        }
        case 22: {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v22,
                                     ARRAY_SIZE(LtePhyPuschCsf_Payload_v22, Fmt),
                                     b, offset, length, j);

            int temp0 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp0 & 15;
            j["Start System Frame Number"] = (temp0 >> 4) & 1023;

            int temp1 = j["PUSCH Reporting Mode"];
            j["PUSCH Reporting Mode"] = (temp1 >> 1) & 7;
            j["Rank Index"] = (temp1 >> 4) & 1;

            unsigned int utemp = j["Number of Subbands"];
            j["Number of Subbands"] = utemp & 31;
            j["WideBand CQI CW0"] = (utemp >> 5) & 15;
            j["WideBand CQI CW1"] = (utemp >> 9) & 15;
            j["SubBand Size (k)"] = (utemp >> 13) & 127;
            j["Single WB PMI"] = (utemp >> 20) & 15;
            j["Single MB PMI"] = (utemp >> 24) & 15;
            j["CSF Tx Mode"] = (utemp >> 28) & 15;

            int temp2 = j["Carrier Index"];
            j["Carrier Index"] = temp2 & 15;

            const char* mode_name = search_name(ValueNamePuschReportingMode, ARRAY_SIZE(ValueNamePuschReportingMode, ValueName), j["PUSCH Reporting Mode"]);
            j["PUSCH Reporting Mode"] = (mode_name ? mode_name : "(MI)Unknown");
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name ? rank_name : "(MI)Unknown");
            const char* txmode_name = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (txmode_name ? txmode_name : "(MI)Unknown");
            const char* carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE PHY PUSCH CSF version: 0x%x", pkt_ver);
            return 0;
    }
}

#endif //LOGIN_LTE_PHY_PUSCH_CSF_H
