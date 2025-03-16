//
// Created by Vladimir Ponomarenko on 24.05.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_phy_pucch_csf_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PHY_PUCCH_CSF: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    LOGD("Decoding LTE_PHY_PUCCH_CSF with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 22: {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v22,
                                     ARRAY_SIZE(LtePhyPucchCsf_Payload_v22, Fmt),
                                     b, offset, length, j);

            int temp_sfn = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp_sfn & 15;
            j["Start System Frame Number"] = (temp_sfn >> 4) & 1023;

            int temp_report = j["PUCCH Reporting Mode"];
            j["PUCCH Reporting Mode"] = (temp_report >> 1) & 0x3;
            const char* mode_name_v22 = search_name(ValueNamePucchReportingMode_v22, ARRAY_SIZE(ValueNamePucchReportingMode_v22, ValueName), j["PUCCH Reporting Mode"]);
            j["PUCCH Reporting Mode"] = (mode_name_v22 ? mode_name_v22 : "(MI)Unknown");

            j["PUCCH Report Type"] = (temp_report >> 3) & 0x3;
            const char* type_name_v22 = search_name(ValueNamePucchReportType_v22, ARRAY_SIZE(ValueNamePucchReportType_v22, ValueName), j["PUCCH Report Type"]);
            j["PUCCH Report Type"] = (type_name_v22 ? type_name_v22 : "(MI)Unknown");
            j["Size BWP"] = (temp_report >> 5) & 7;


            unsigned int utemp_sb = j["Number of Subbands"];
            j["Number of Subbands"] = utemp_sb & 15;
            j["BWP Index"] = (utemp_sb >> 4) & 7;
            j["Rank Index"] = (utemp_sb >> 7) & 1;
            const char* rank_name_v22 = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name_v22 ? rank_name_v22 : "(MI)Unknown");
            j["SubBand Label"] = (utemp_sb >> 8) & 3;
            j["CQI CW0"] = (utemp_sb >> 10) & 15;
            j["CQI CW1"] = (utemp_sb >> 14) & 15;
            j["Wideband PMI"] = (utemp_sb >> 18) & 15;
            j["Carrier Index"] = (utemp_sb >> 22) & 15;
            const char* carrier_name_v22 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v22 ? carrier_name_v22 : "(MI)Unknown");
            j["CSF Tx Mode"] = (utemp_sb >> 26) & 15;
            const char* csf_tx_name_v22 = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (csf_tx_name_v22 ? csf_tx_name_v22 : "(MI)Unknown");
            break;
        }
        case 24: {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v24,
                                     ARRAY_SIZE(LtePhyPucchCsf_Payload_v24, Fmt),
                                     b, offset, length, j);

            int temp_sfn = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp_sfn & 15;
            j["Start System Frame Number"] = (temp_sfn >> 4) & 1023;
            int ileft_v24 = (temp_sfn >> 14) & 3;

            int temp_report = j["PUCCH Reporting Mode"];
            j["PUCCH Reporting Mode"] = ((temp_report & 1) << 2) | ileft_v24;
            const char* mode_name_v24 = search_name(ValueNamePucchReportingMode, ARRAY_SIZE(ValueNamePucchReportingMode, ValueName), j["PUCCH Reporting Mode"]);
            j["PUCCH Reporting Mode"] = (mode_name_v24 ? mode_name_v24 : "(MI)Unknown");
            j["PUCCH Report Type"] = (temp_report >> 1) & 15;
            const char* type_name_v24 = search_name(ValueNamePucchReportType, ARRAY_SIZE(ValueNamePucchReportType, ValueName), j["PUCCH Report Type"]);
            j["PUCCH Report Type"] = (type_name_v24 ? type_name_v24 : "(MI)Unknown");
            j["Size BWP"] = (temp_report >> 5) & 7;

            unsigned int utemp_sb = j["Number of Subbands"];
            j["Number of Subbands"] = utemp_sb & 15;
            j["BWP Index"] = (utemp_sb >> 4) & 7;
            j["Rank Index"] = (utemp_sb >> 7) & 1;
            const char* rank_name_v24 = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name_v24 ? rank_name_v24 : "(MI)Unknown");
            j["SubBand Label"] = (utemp_sb >> 8) & 3;
            j["CQI CW0"] = (utemp_sb >> 10) & 15;
            j["CQI CW1"] = (utemp_sb >> 14) & 15;
            j["Wideband PMI"] = (utemp_sb >> 18) & 15;
            j["Carrier Index"] = (utemp_sb >> 22) & 15;
            const char* carrier_name_v24 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v24 ? carrier_name_v24 : "(MI)Unknown");
            j["CSF Tx Mode"] = (utemp_sb >> 26) & 15;
            const char* csf_tx_name_v24 = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (csf_tx_name_v24 ? csf_tx_name_v24 : "(MI)Unknown");

            unsigned int utemp_csirs = j["Num Csirs Ports"];
            j["Num Csirs Ports"] = utemp_csirs & 0xf;
            j["Wideband PMI1"] = (utemp_csirs >> 4) & 0xf;

            unsigned int utemp_pti = j["Pti"];
            j["Pti"] = utemp_pti & 0x1;
            break;
        }
        case 43: {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v43,
                                     ARRAY_SIZE(LtePhyPucchCsf_Payload_v43, Fmt),
                                     b, offset, length, j);

            int temp_sfn = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp_sfn & 15;
            j["Start System Frame Number"] = (temp_sfn >> 4) & 1023;
            int ileft_v43 = (temp_sfn >> 14) & 3;

            int temp_report = j["PUCCH Reporting Mode"];
            j["PUCCH Reporting Mode"] = ((temp_report & 1) << 2) | ileft_v43;
            const char* mode_name_v43 = search_name(ValueNamePucchReportingMode, ARRAY_SIZE(ValueNamePucchReportingMode, ValueName), j["PUCCH Reporting Mode"]);
            j["PUCCH Reporting Mode"] = (mode_name_v43 ? mode_name_v43 : "(MI)Unknown");
            j["PUCCH Report Type"] = (temp_report >> 1) & 15;
            const char* type_name_v43 = search_name(ValueNamePucchReportType, ARRAY_SIZE(ValueNamePucchReportType, ValueName), j["PUCCH Report Type"]);
            j["PUCCH Report Type"] = (type_name_v43 ? type_name_v43 : "(MI)Unknown");

            unsigned int utemp_sb = j["Number of Subbands"];
            j["Number of Subbands"] = utemp_sb & 15;
            j["Rank Index"] = (utemp_sb >> 7) & 1;
            const char* rank_name_v43 = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name_v43 ? rank_name_v43 : "(MI)Unknown");
            j["CQI CW0"] = (utemp_sb >> 10) & 15;
            j["CQI CW1"] = (utemp_sb >> 14) & 15;
            j["Wideband PMI"] = (utemp_sb >> 18) & 15;
            j["Carrier Index"] = (utemp_sb >> 22) & 15;
            const char* carrier_name_v43 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v43 ? carrier_name_v43 : "(MI)Unknown");
            j["CSF Tx Mode"] = (utemp_sb >> 26) & 15;
            const char* csf_tx_name_v43 = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (csf_tx_name_v43 ? csf_tx_name_v43 : "(MI)Unknown");
            break;
        }
        case 101:
        case 102: {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v102,
                                     ARRAY_SIZE(LtePhyPucchCsf_Payload_v102, Fmt),
                                     b, offset, length, j);

            int temp_sfn = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp_sfn & 15;
            j["Start System Frame Number"] = (temp_sfn >> 4) & 1023;
            int ileft_v10x = (temp_sfn >> 14) & 3;

            int temp_report = j["PUCCH Reporting Mode"];
            j["PUCCH Reporting Mode"] = ((temp_report & 1) << 2) | ileft_v10x;
            const char* mode_name_v10x = search_name(ValueNamePucchReportingMode, ARRAY_SIZE(ValueNamePucchReportingMode, ValueName), j["PUCCH Reporting Mode"]);
            j["PUCCH Reporting Mode"] = (mode_name_v10x ? mode_name_v10x : "(MI)Unknown");
            j["PUCCH Report Type"] = (temp_report >> 1) & 15;
            const char* type_name_v10x = search_name(ValueNamePucchReportType, ARRAY_SIZE(ValueNamePucchReportType, ValueName), j["PUCCH Report Type"]);
            j["PUCCH Report Type"] = (type_name_v10x ? type_name_v10x : "(MI)Unknown");

            unsigned int utemp_sb = j["Number of Subbands"];
            j["Number of Subbands"] = utemp_sb & 15;
            j["Alt Cqi Table Data"] = (utemp_sb >> 7) & 1;
            j["CQI CW0"] = (utemp_sb >> 10) & 15;
            j["CQI CW1"] = (utemp_sb >> 14) & 15;
            j["Wideband PMI"] = (utemp_sb >> 18) & 15;
            j["Carrier Index"] = (utemp_sb >> 22) & 15;
            const char* carrier_name_v10x = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v10x ? carrier_name_v10x : "(MI)Unknown");
            j["CSF Tx Mode"] = (utemp_sb >> 26) & 15;
            const char* csf_tx_name_v10x = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (csf_tx_name_v10x ? csf_tx_name_v10x : "(MI)Unknown");

            int temp_csirs = j["Num Csirs Ports"];
            j["Num Csirs Ports"] = temp_csirs & 15;

            int temp_csi_meas = j["Csi Meas Set Index"];
            j["Csi Meas Set Index"] = (temp_csi_meas >> 1) & 0x1;
            const char* csi_set_name_v10x = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name_v10x ? csi_set_name_v10x : "(MI)Unknown");
            j["Rank Index"] = (temp_csi_meas >> 2) & 0x3;
            const char* rank_name_v10x = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name_v10x ? rank_name_v10x : "(MI)Unknown");
            break;
        }
        case 103: {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v103,
                                     ARRAY_SIZE(LtePhyPucchCsf_Payload_v103, Fmt),
                                     b, offset, length, j);

            int temp_sfn = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp_sfn & 15;
            j["Start System Frame Number"] = (temp_sfn >> 4) & 1023;
            int ileft_v103 = (temp_sfn >> 14) & 3;

            int temp_report = j["PUCCH Reporting Mode"];
            j["PUCCH Reporting Mode"] = ((temp_report & 1) << 2) | ileft_v103;
            const char* mode_name_v103 = search_name(ValueNamePucchReportingMode, ARRAY_SIZE(ValueNamePucchReportingMode, ValueName), j["PUCCH Reporting Mode"]);
            j["PUCCH Reporting Mode"] = (mode_name_v103 ? mode_name_v103 : "(MI)Unknown");
            j["PUCCH Report Type"] = (temp_report >> 1) & 15;
            const char* type_name_v103 = search_name(ValueNamePucchReportType, ARRAY_SIZE(ValueNamePucchReportType, ValueName), j["PUCCH Report Type"]);
            j["PUCCH Report Type"] = (type_name_v103 ? type_name_v103 : "(MI)Unknown");

            unsigned int utemp_sb = j["Number of Subbands"];
            j["Number of Subbands"] = utemp_sb & 15;
            j["Alt Cqi Table Data"] = (utemp_sb >> 7) & 1;
            j["CQI CW0"] = (utemp_sb >> 10) & 15;
            j["CQI CW1"] = (utemp_sb >> 14) & 15;
            j["Wideband PMI"] = (utemp_sb >> 18) & 15;
            j["Carrier Index"] = (utemp_sb >> 22) & 15;
            const char* carrier_name_v103 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v103 ? carrier_name_v103 : "(MI)Unknown");
            j["CSF Tx Mode"] = (utemp_sb >> 26) & 15;
            const char* csf_tx_name_v103 = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (csf_tx_name_v103 ? csf_tx_name_v103 : "(MI)Unknown");

            int temp_csirs = j["Num Csirs Ports"];
            j["Num Csirs Ports"] = temp_csirs & 15;

            int temp_csi_meas = j["Csi Meas Set Index"];
            j["Csi Meas Set Index"] = (temp_csi_meas >> 1) & 0x1;
            const char* csi_set_name_v103 = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name_v103 ? csi_set_name_v103 : "(MI)Unknown");
            j["Rank Index"] = (temp_csi_meas >> 2) & 0x3;
            const char* rank_name_v103 = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name_v103 ? rank_name_v103 : "(MI)Unknown");

            int iForcedMaxRI_part1 = (temp_csi_meas >> 4) & 0xf;
            int iForcedMaxRI_part2 = j["Forced Max RI"];
            j["Forced Max RI"] = iForcedMaxRI_part1 | ((iForcedMaxRI_part2 & 0xf) << 4);

            break;
        }
        case 142: {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v142,
                                     ARRAY_SIZE(LtePhyPucchCsf_Payload_v142, Fmt),
                                     b, offset, length, j);

            unsigned int temp0_v142 = j["Start System Sub-frame Number"];
            j["Start System Sub-frame Number"] = temp0_v142 & 15;
            j["Start System Frame Number"] = (temp0_v142 >> 4) & 1023;
            j["Carrier Index"] = (temp0_v142 >> 14) & 15;
            const char* carrier_name_v142 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v142 ? carrier_name_v142 : "(MI)Unknown");
            j["Scell Index"] = (temp0_v142 >> 18) & 31;
            j["PUCCH Reporting Mode"] = (temp0_v142 >> 24) & 3;
            const char* mode_name_v142 = search_name(ValueNamePucchReportingMode_v22, ARRAY_SIZE(ValueNamePucchReportingMode_v22, ValueName), j["PUCCH Reporting Mode"]);
            j["PUCCH Reporting Mode"] = (mode_name_v142 ? mode_name_v142 : "(MI)Unknown");
            j["PUCCH Report Type"] = (temp0_v142 >> 26) & 15;
            const char* type_name_v142 = search_name(ValueNamePucchReportType, ARRAY_SIZE(ValueNamePucchReportType, ValueName), j["PUCCH Report Type"]);
            j["PUCCH Report Type"] = (type_name_v142 ? type_name_v142 : "(MI)Unknown");
            int size_bwp_part1 = (temp0_v142 >> 30) & 3;


            unsigned int temp1_v142 = j["Number of Subbands"];
            j["Size BWP"] = size_bwp_part1 | ((temp1_v142 & 1) << 2);
            j["Number of Subbands"] = (temp1_v142 >> 1) & 15;
            j["BWP Index"] = (temp1_v142 >> 5) & 7;


            unsigned int temp2_v142 = j["Alt Cqi Table Data"];
            j["Alt Cqi Table Data"] = temp2_v142 & 1;
            j["SubBand Label"] = (temp2_v142 >> 1) & 3;
            j["CQI CW0"] = (temp2_v142 >> 3) & 15;
            j["CQI CW1"] = (temp2_v142 >> 7) & 15;
            j["Wideband PMI"] = (temp2_v142 >> 11) & 15;

            unsigned int temp3_v142 = j["CSF Tx Mode"];
            j["CSF Tx Mode"] = temp3_v142 & 15;
            const char* csf_tx_name_v142 = search_name(ValueNameCSFTxMode, ARRAY_SIZE(ValueNameCSFTxMode, ValueName), j["CSF Tx Mode"]);
            j["CSF Tx Mode"] = (csf_tx_name_v142 ? csf_tx_name_v142 : "(MI)Unknown");
            j["PUCCH Reporting Submode"] = (temp3_v142 >> 4) & 3;
            j["Num Csirs Ports"] = (temp3_v142 >> 6) & 15;
            j["Wideband PMI1"] = (temp3_v142 >> 10) & 15;
            j["Pti"] = (temp3_v142 >> 14) & 1;
            j["Csi Meas Set Index"] = (temp3_v142 >> 15) & 1;
            const char* csi_set_name_v142 = search_name(ValueNameCsiMeasSetIndex, ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName), j["Csi Meas Set Index"]);
            j["Csi Meas Set Index"] = (csi_set_name_v142 ? csi_set_name_v142 : "(MI)Unknown");

            unsigned int temp4_v142 = j["Rank Index"];
            j["Rank Index"] = temp4_v142 & 3;
            const char* rank_name_v142 = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), j["Rank Index"]);
            j["Rank Index"] = (rank_name_v142 ? rank_name_v142 : "(MI)Unknown");
            j["CRI"] = (temp4_v142 >> 2) & 7;
            j["UL Frame Number"] = (temp4_v142 >> 5) & 0x3ff;
            j["UL Channel Type"] = (temp4_v142 >> 15) & 1;

            unsigned int temp5_v142 = j["UL Subframe Number"];
            j["UL Subframe Number"] = temp5_v142 & 0xf;
            j["UL Payload Length"] = (temp5_v142 >> 4) & 0x1f;

            break;
        }
        default:
            LOGD("(MI)Unknown LTE PHY PUCCH CSF version: 0x%x", pkt_ver);
            return 0;
    }

    return offset - start;
}