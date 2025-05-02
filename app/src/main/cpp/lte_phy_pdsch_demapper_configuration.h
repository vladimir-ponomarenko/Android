//
// Created by Vladimir Ponomarenko on 23.04.2025.
//

#pragma once

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


static int _decode_lte_phy_pdsch_demapper_config(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = j["Version"];

    LOGD("Decoding LTE_PHY_PDSCH_Demapper_Configuration with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 23: {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v23,
                                     ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v23, Fmt),
                                     b, offset, length, j);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = j["System Frame Number"];
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = j["Serving Cell ID"];
            serv_cell += (tmp & 0x1) << 8;

            j["Serving Cell ID"] = serv_cell;
            j["System Frame Number"] = sfn;
            j["Subframe Number"] = subframe;


            tmp = j["Number of Tx Antennas(M)"];
            int iRNTIType = tmp & 15;
            int iSpatialRank = (tmp >> 14) & 3;
            int m_n_combined = (tmp >> 8) & 0x0f;
            int M = m_n_combined & 0x3;
            M = (M != 3 ? (1 << M) : -1);
            int N = (m_n_combined >> 2) & 0x1;
            N = (1 << N);

            j["PDSCH RNTI Type"] = iRNTIType;
            const char* rnti_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), iRNTIType);
            j["PDSCH RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

            j["Number of Tx Antennas(M)"] = M;
            j["Number of Rx Antennas(N)"] = N;

            j["Spatial Rank"] = iSpatialRank;
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), iSpatialRank);
            j["Spatial Rank"] = (rank_name ? rank_name : "(MI)Unknown");


            tmp = j["Frequency Selective PMI"];
            int iFrequencySelectivePMI = (tmp >> 1) & 3;
            int iPMIIndex = (tmp >> 4) & 15;
            tmp = j["Transmission Scheme"];
            int iTransmissionScheme = tmp & 15;

            j["Frequency Selective PMI"] = iFrequencySelectivePMI;
            const char* freq_pmi_name = search_name(ValueNameFrequencySelectivePMI, ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName), iFrequencySelectivePMI);
            j["Frequency Selective PMI"] = (freq_pmi_name ? freq_pmi_name : "(MI)Unknown");

            j["PMI Index"] = iPMIIndex;
            j["Transmission Scheme"] = iTransmissionScheme;
            const char* tx_scheme_name = search_name(ValueNameTransmissionScheme, ARRAY_SIZE(ValueNameTransmissionScheme, ValueName), iTransmissionScheme);
            j["Transmission Scheme"] = (tx_scheme_name ? tx_scheme_name : "(MI)Unknown");


            tmp = j["MCS 0"];
            int mod_stream0 = (tmp >> 1) & 0x3;

            float ratio = static_cast<float>((tmp >> 3) & 4095) / 256.0;

            tmp = j["MCS 1"];
            int mod_stream1 = (tmp >> 1) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            j["MCS 0"] = mod_stream0;
            const char* mod0_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream0);
            j["MCS 0"] = (mod0_name ? mod0_name : "(MI)Unknown");

            j["Traffic to Pilot Ratio"] = ratio;

            j["MCS 1"] = mod_stream1;
            const char* mod1_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream1);
            j["MCS 1"] = (mod1_name ? mod1_name : "(MI)Unknown");

            j["Carrier Index"] = carrier_index;
            const char* carrier_name = search_name(LtePhyPdschDemapperConfig_v23_Carrier_Index, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName), carrier_index);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");


            if (j.find("TBS 0") != j.end()) { j["Transport Block Size Stream 0"] = j["TBS 0"]; j.erase("TBS 0"); }
            if (j.find("TBS 1") != j.end()) { j["Transport Block Size Stream 1"] = j["TBS 1"]; j.erase("TBS 1"); }

            break;
        }
        case 28: {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v28,
                                     ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v28, Fmt),
                                     b, offset, length, j);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = j["System Frame Number"];
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = j["Serving Cell ID"];
            serv_cell += (tmp & 0x1) << 8;

            j["Serving Cell ID"] = serv_cell;
            j["System Frame Number"] = sfn;
            j["Subframe Number"] = subframe;

            tmp = j["Number of Tx Antennas(M)"];
            int iRNTIType = tmp & 15;
            int iSpatialRank = (tmp >> 14) & 3;
            int m_n_combined = (tmp >> 8) & 0x0f;
            int M = m_n_combined & 0x3;
            M = (M != 3 ? (1 << M) : -1);
            int N = (m_n_combined >> 2) & 0x1;
            N = (1 << N);

            j["PDSCH RNTI Type"] = iRNTIType;
            const char* rnti_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), iRNTIType);
            j["PDSCH RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

            j["Number of Tx Antennas(M)"] = M;
            j["Number of Rx Antennas(N)"] = N;

            j["Spatial Rank"] = iSpatialRank;
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), iSpatialRank);
            j["Spatial Rank"] = (rank_name ? rank_name : "(MI)Unknown");

            tmp = j["Frequency Selective PMI"];
            int iFrequencySelectivePMI = (tmp >> 1) & 3;
            int iPMIIndex = (tmp >> 4) & 15;

            int tmp_tx_scheme = j["Transmission Scheme"];
            int iTransmissionScheme = tmp_tx_scheme & 15;


            int tmp_rep_idx = j["Repetition Index Data"];

            int iRepetitionIndexData = ((tmp_rep_idx & 0x3ff) << 2) | ((tmp_tx_scheme >> 6) & 0x3);
            j["Repetition Index Data"] = iRepetitionIndexData;


            j["Frequency Selective PMI"] = iFrequencySelectivePMI;
            const char* freq_pmi_name = search_name(ValueNameFrequencySelectivePMI, ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName), iFrequencySelectivePMI);
            j["Frequency Selective PMI"] = (freq_pmi_name ? freq_pmi_name : "(MI)Unknown");

            j["PMI Index"] = iPMIIndex;
            j["Transmission Scheme"] = iTransmissionScheme;
            const char* tx_scheme_name = search_name(ValueNameTransmissionScheme, ARRAY_SIZE(ValueNameTransmissionScheme, ValueName), iTransmissionScheme);
            j["Transmission Scheme"] = (tx_scheme_name ? tx_scheme_name : "(MI)Unknown");

            tmp = j["MCS 0"];
            int mod_stream0 = (tmp >> 1) & 0x3;
            float ratio = static_cast<float>((tmp >> 3) & 4095) / 256.0;

            tmp = j["MCS 1"];
            int mod_stream1 = (tmp >> 1) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            j["MCS 0"] = mod_stream0;
            const char* mod0_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream0);
            j["MCS 0"] = (mod0_name ? mod0_name : "(MI)Unknown");

            j["Traffic to Pilot Ratio"] = ratio;

            j["MCS 1"] = mod_stream1;
            const char* mod1_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream1);
            j["MCS 1"] = (mod1_name ? mod1_name : "(MI)Unknown");

            j["Carrier Index"] = carrier_index;
            const char* carrier_name = search_name(LtePhyPdschDemapperConfig_v23_Carrier_Index, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName), carrier_index);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");


            tmp = j["Repetition Total"];
            int iRepetitionTotal = (tmp >> 8) & 0xffff;
            int iNBIndex = (tmp >> 24) & 0x3f;
            int iSIB1BRCollision = (tmp >> 30) & 0x1;
            int iSIBxCollision = (tmp >> 31) & 0x1;

            j["Repetition Total"] = iRepetitionTotal;
            j["NB Index"] = iNBIndex;
            j["SIB1-BR Collision"] = iSIB1BRCollision;
            j["SIBx Collision"] = iSIBxCollision;

            if (j.find("TBS 0") != j.end()) { j["Transport Block Size Stream 0"] = j["TBS 0"]; j.erase("TBS 0"); }
            if (j.find("TBS 1") != j.end()) { j["Transport Block Size Stream 1"] = j["TBS 1"]; j.erase("TBS 1"); }

            break;
        }
        case 103: {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v103,
                                     ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v103, Fmt),
                                     b, offset, length, j);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = j["System Frame Number"];
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = j["Serving Cell ID"];
            serv_cell += (tmp & 0x1) << 8;

            j["Serving Cell ID"] = serv_cell;
            j["System Frame Number"] = sfn;
            j["Subframe Number"] = subframe;

            tmp = j["Number of Tx Antennas(M)"];
            int iRNTIType = tmp & 15;
            int iSpatialRank = (tmp >> 14) & 3;
            int m_n_combined = (tmp >> 8) & 0x0f;
            int M = m_n_combined & 0x3;
            M = (M != 3 ? (1 << M) : -1);
            int N = (m_n_combined >> 2) & 0x1;
            N = (1 << N);

            j["PDSCH RNTI Type"] = iRNTIType;
            const char* rnti_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), iRNTIType);
            j["PDSCH RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

            j["Number of Tx Antennas(M)"] = M;
            j["Number of Rx Antennas(N)"] = N;

            j["Spatial Rank"] = iSpatialRank;
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), iSpatialRank);
            j["Spatial Rank"] = (rank_name ? rank_name : "(MI)Unknown");

            tmp = j["Frequency Selective PMI"];
            int iFrequencySelectivePMI = (tmp >> 1) & 3;
            int iPMIIndex = (tmp >> 4) & 15;
            tmp = j["Transmission Scheme"];
            int iTransmissionScheme = tmp & 15;

            j["Frequency Selective PMI"] = iFrequencySelectivePMI;
            const char* freq_pmi_name = search_name(ValueNameFrequencySelectivePMI, ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName), iFrequencySelectivePMI);
            j["Frequency Selective PMI"] = (freq_pmi_name ? freq_pmi_name : "(MI)Unknown");

            j["PMI Index"] = iPMIIndex;
            j["Transmission Scheme"] = iTransmissionScheme;
            const char* tx_scheme_name = search_name(ValueNameTransmissionScheme, ARRAY_SIZE(ValueNameTransmissionScheme, ValueName), iTransmissionScheme);
            j["Transmission Scheme"] = (tx_scheme_name ? tx_scheme_name : "(MI)Unknown");

            tmp = j["MCS 0"];
            int mod_stream0 = (tmp >> 2) & 0x3;
            float ratio = static_cast<float>((tmp >> 4) & 0x1fff) / 256.0;

            tmp = j["MCS 1"];
            int mod_stream1 = (tmp >> 2) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            j["MCS 0"] = mod_stream0;
            const char* mod0_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream0);
            j["MCS 0"] = (mod0_name ? mod0_name : "(MI)Unknown");

            j["Traffic to Pilot Ratio"] = ratio;

            j["MCS 1"] = mod_stream1;
            const char* mod1_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream1);
            j["MCS 1"] = (mod1_name ? mod1_name : "(MI)Unknown");

            j["Carrier Index"] = carrier_index;
            const char* carrier_name = search_name(LtePhyPdschDemapperConfig_v23_Carrier_Index, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName), carrier_index);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            if (j.find("TBS 0") != j.end()) { j["Transport Block Size Stream 0"] = j["TBS 0"]; j.erase("TBS 0"); }
            if (j.find("TBS 1") != j.end()) { j["Transport Block Size Stream 1"] = j["TBS 1"]; j.erase("TBS 1"); }

            break;
        }
        case 104: {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v104,
                                     ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v104, Fmt),
                                     b, offset, length, j);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = j["System Frame Number"];
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = j["Serving Cell ID"];
            serv_cell += (tmp & 0x1) << 8;

            j["Serving Cell ID"] = serv_cell;
            j["System Frame Number"] = sfn;
            j["Subframe Number"] = subframe;

            tmp = j["Number of Tx Antennas(M)"];
            int iRNTIType = tmp & 15;
            int iSpatialRank = (tmp >> 14) & 3;
            int m_n_combined = (tmp >> 8) & 0x0f;
            int M = m_n_combined & 0x3;
            M = (M != 3 ? (1 << M) : -1);
            int N = (m_n_combined >> 2) & 0x1;
            N = (1 << N);

            j["PDSCH RNTI Type"] = iRNTIType;
            const char* rnti_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), iRNTIType);
            j["PDSCH RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

            j["Number of Tx Antennas(M)"] = M;
            j["Number of Rx Antennas(N)"] = N;

            j["Spatial Rank"] = iSpatialRank;
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), iSpatialRank);
            j["Spatial Rank"] = (rank_name ? rank_name : "(MI)Unknown");

            tmp = j["Frequency Selective PMI"];
            int iFrequencySelectivePMI = (tmp >> 1) & 3;
            int iPMIIndex = (tmp >> 4) & 15;
            tmp = j["Transmission Scheme"];
            int iTransmissionScheme = tmp & 15;

            j["Frequency Selective PMI"] = iFrequencySelectivePMI;
            const char* freq_pmi_name = search_name(ValueNameFrequencySelectivePMI, ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName), iFrequencySelectivePMI);
            j["Frequency Selective PMI"] = (freq_pmi_name ? freq_pmi_name : "(MI)Unknown");

            j["PMI Index"] = iPMIIndex;
            j["Transmission Scheme"] = iTransmissionScheme;
            const char* tx_scheme_name = search_name(ValueNameTransmissionScheme, ARRAY_SIZE(ValueNameTransmissionScheme, ValueName), iTransmissionScheme);
            j["Transmission Scheme"] = (tx_scheme_name ? tx_scheme_name : "(MI)Unknown");

            tmp = j["MCS 0"];
            int mod_stream0 = (tmp >> 2) & 0x3;
            float ratio = static_cast<float>((tmp >> 4) & 0x1fff) / 256.0;

            tmp = j["MCS 1"];
            int mod_stream1 = (tmp >> 2) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            j["MCS 0"] = mod_stream0;
            const char* mod0_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream0);
            j["MCS 0"] = (mod0_name ? mod0_name : "(MI)Unknown");

            j["Traffic to Pilot Ratio"] = ratio;

            j["MCS 1"] = mod_stream1;
            const char* mod1_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream1);
            j["MCS 1"] = (mod1_name ? mod1_name : "(MI)Unknown");

            j["Carrier Index"] = carrier_index;
            const char* carrier_name = search_name(LtePhyPdschDemapperConfig_v23_Carrier_Index, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName), carrier_index);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            if (j.find("TBS 0") != j.end()) { j["Transport Block Size Stream 0"] = j["TBS 0"]; j.erase("TBS 0"); }
            if (j.find("TBS 1") != j.end()) { j["Transport Block Size Stream 1"] = j["TBS 1"]; j.erase("TBS 1"); }

            break;
        }
        case 122: {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v122,
                                     ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v122, Fmt),
                                     b, offset, length, j);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = j["System Frame Number"];
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = j["Serving Cell ID"];
            serv_cell += (tmp & 0x1) << 8;

            j["Serving Cell ID"] = serv_cell;
            j["System Frame Number"] = sfn;
            j["Subframe Number"] = subframe;

            tmp = j["Number of Tx Antennas(M)"];
            int iRNTIType = tmp & 15;
            int M = (tmp >> 6) & 0x3;
            int N = (tmp >> 8) & 0x3;
            int iSpatialRank = (tmp >> 12) & 3;
            int iFrequencySelectivePMI = (tmp >> 15) & 1;

            j["PDSCH RNTI Type"] = iRNTIType;
            const char* rnti_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), iRNTIType);
            j["PDSCH RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

            j["Number of Tx Antennas(M)"] = M;
            const char* tx_ant_name = search_name(LtePhyPdschDemapperConfig_v122_antenna, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_antenna, ValueName), M);
            j["Number of Tx Antennas(M)"] = (tx_ant_name ? tx_ant_name : "(MI)Unknown");

            j["Number of Rx Antennas(N)"] = N;
            const char* rx_ant_name = search_name(LtePhyPdschDemapperConfig_v122_antenna, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_antenna, ValueName), N);
            j["Number of Rx Antennas(N)"] = (rx_ant_name ? rx_ant_name : "(MI)Unknown");

            j["Spatial Rank"] = iSpatialRank;
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), iSpatialRank);
            j["Spatial Rank"] = (rank_name ? rank_name : "(MI)Unknown");

            j["Frequency Selective PMI"] = iFrequencySelectivePMI;
            const char* freq_pmi_name = search_name(ValueNameFrequencySelectivePMI, ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName), iFrequencySelectivePMI);
            j["Frequency Selective PMI"] = (freq_pmi_name ? freq_pmi_name : "(MI)Unknown");

            tmp = j["PMI Index"];
            int iPMIIndex = (tmp >> 2) & 15;
            int iTransmissionScheme = (tmp >> 6) & 15;
            int iBMODFMSymIndex = (tmp >> 12) & 15;

            j["PMI Index"] = iPMIIndex;
            j["Transmission Scheme"] = iTransmissionScheme;
            const char* tx_scheme_name = search_name(ValueNameTransmissionScheme, ARRAY_SIZE(ValueNameTransmissionScheme, ValueName), iTransmissionScheme);
            j["Transmission Scheme"] = (tx_scheme_name ? tx_scheme_name : "(MI)Unknown");

            j["BMOD FD Sym Index"] = iBMODFMSymIndex;

            tmp = j["MVC"];
            int iMVC = (tmp >> 2) & 1;
            int iMVCClock = (tmp >> 3) & 15;
            int iMVCRequestUp = (tmp >> 7) & 1;

            j["MVC"] = iMVC;
            j["MVC Clock"] = iMVCClock;
            const char* mvc_clk_name = search_name(LtePhyPdschDemapperConfig_v122_MVC_Clock, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_MVC_Clock, ValueName), iMVCClock);
            j["MVC Clock"] = (mvc_clk_name ? mvc_clk_name : "(MI)Unknown");

            j["MVC Request Up"] = iMVCRequestUp;

            tmp = j["MCS 0"];
            int mod_stream0 = (tmp >> 2) & 0x3;
            float ratio = static_cast<float>((tmp >> 4) & 0x1fff) / 256.0;

            tmp = j["MCS 1"];
            int mod_stream1 = (tmp >> 2) & 0x3;
            int ipb = (tmp >> 4) & 0x3;
            int carrier_index = (tmp >> 6) & 0xf;
            int iCSIRSExist = (tmp >> 10) & 0x1;
            int iZPCSIRSExist = (tmp >> 11) & 0x1;
            int iCSIRSSymbolSkipped = (tmp >> 12) & 0x1;

            j["MCS 0"] = mod_stream0;
            const char* mod0_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream0);
            j["MCS 0"] = (mod0_name ? mod0_name : "(MI)Unknown");

            j["Traffic to Pilot Ratio"] = ratio;

            j["MCS 1"] = mod_stream1;
            const char* mod1_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream1);
            j["MCS 1"] = (mod1_name ? mod1_name : "(MI)Unknown");

            j["PB"] = ipb;
            j["Carrier Index"] = carrier_index;
            const char* carrier_name = search_name(LtePhyPdschDemapperConfig_v23_Carrier_Index, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName), carrier_index);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            j["CSI-RS Exist"] = iCSIRSExist;
            j["ZP CSI-RS Exist"] = iZPCSIRSExist;
            j["CSI-RS Symbol Skipped"] = iCSIRSSymbolSkipped;

            tmp = j["Op Mode"];
            int iOPMode = tmp & 0xf;
            int iStrongICellID = (tmp >> 4) & 0x1ff;
            int iJointDemodSkipReason = (tmp >> 13) & 0x1f;

            j["Op Mode"] = iOPMode;
            const char* opmode_name = search_name(LtePhyPdschDemapperConfig_v122_OPMode, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_OPMode, ValueName), iOPMode);
            j["Op Mode"] = (opmode_name ? opmode_name : "(MI)Unknown");

            j["Strong ICell ID"] = iStrongICellID;
            j["Joint Demod Skip Reason"] = iJointDemodSkipReason;
            const char* skip_reason_name = search_name(LtePhyPdschDemapperConfig_Joint_Demod_Skip_Reason, ARRAY_SIZE(LtePhyPdschDemapperConfig_Joint_Demod_Skip_Reason, ValueName), iJointDemodSkipReason);
            j["Joint Demod Skip Reason"] = (skip_reason_name ? skip_reason_name : "(MI)Unknown");

            if (j.find("TBS 0") != j.end()) { j["Transport Block Size Stream 0"] = j["TBS 0"]; j.erase("TBS 0"); }
            if (j.find("TBS 1") != j.end()) { j["Transport Block Size Stream 1"] = j["TBS 1"]; j.erase("TBS 1"); }

            break;
        }
        case 123: {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v123,
                                     ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v123, Fmt),
                                     b, offset, length, j);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = j["System Frame Number"];
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = j["Serving Cell ID"];
            serv_cell += (tmp & 0x1) << 8;

            j["Serving Cell ID"] = serv_cell;
            j["System Frame Number"] = sfn;
            j["Subframe Number"] = subframe;

            tmp = j["Number of Tx Antennas(M)"];
            int iRNTIType = tmp & 15;
            int M = (tmp >> 6) & 0x3;
            int N = (tmp >> 8) & 0x3;
            int iSpatialRank = (tmp >> 12) & 3;
            int iFrequencySelectivePMI = (tmp >> 15) & 1;

            j["PDSCH RNTI Type"] = iRNTIType;
            const char* rnti_name = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), iRNTIType);
            j["PDSCH RNTI Type"] = (rnti_name ? rnti_name : "(MI)Unknown");

            j["Number of Tx Antennas(M)"] = M;
            const char* tx_ant_name = search_name(LtePhyPdschDemapperConfig_v122_antenna, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_antenna, ValueName), M);
            j["Number of Tx Antennas(M)"] = (tx_ant_name ? tx_ant_name : "(MI)Unknown");

            j["Number of Rx Antennas(N)"] = N;
            const char* rx_ant_name = search_name(LtePhyPdschDemapperConfig_v122_antenna, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_antenna, ValueName), N);
            j["Number of Rx Antennas(N)"] = (rx_ant_name ? rx_ant_name : "(MI)Unknown");

            j["Spatial Rank"] = iSpatialRank;
            const char* rank_name = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), iSpatialRank);
            j["Spatial Rank"] = (rank_name ? rank_name : "(MI)Unknown");

            j["Frequency Selective PMI"] = iFrequencySelectivePMI;
            const char* freq_pmi_name = search_name(ValueNameFrequencySelectivePMI, ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName), iFrequencySelectivePMI);
            j["Frequency Selective PMI"] = (freq_pmi_name ? freq_pmi_name : "(MI)Unknown");

            tmp = j["PMI Index"];
            int iPMIIndex = (tmp >> 2) & 15;
            int iTransmissionScheme = (tmp >> 6) & 15;
            int iBMODFMSymIndex = (tmp >> 12) & 15;

            j["PMI Index"] = iPMIIndex;
            j["Transmission Scheme"] = iTransmissionScheme;
            const char* tx_scheme_name = search_name(ValueNameTransmissionScheme, ARRAY_SIZE(ValueNameTransmissionScheme, ValueName), iTransmissionScheme);
            j["Transmission Scheme"] = (tx_scheme_name ? tx_scheme_name : "(MI)Unknown");

            j["BMOD FD Sym Index"] = iBMODFMSymIndex;

            tmp = j["MCS 0"];
            int mod_stream0 = (tmp >> 2) & 0x3;
            float ratio = static_cast<float>((tmp >> 4) & 0x1fff) / 256.0;

            tmp = j["MCS 1"];
            int mod_stream1 = (tmp >> 2) & 0x3;
            int ipb = (tmp >> 4) & 0x3;
            int carrier_index = (tmp >> 6) & 0xf;
            int iCSIRSExist = (tmp >> 10) & 0x1;
            int iZPCSIRSExist = (tmp >> 11) & 0x1;
            int iCSIRSSymbolSkipped = (tmp >> 12) & 0x1;

            j["MCS 0"] = mod_stream0;
            const char* mod0_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream0);
            j["MCS 0"] = (mod0_name ? mod0_name : "(MI)Unknown");

            j["Traffic to Pilot Ratio"] = ratio;

            j["MCS 1"] = mod_stream1;
            const char* mod1_name = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), mod_stream1);
            j["MCS 1"] = (mod1_name ? mod1_name : "(MI)Unknown");

            j["PB"] = ipb;
            j["Carrier Index"] = carrier_index;
            const char* carrier_name = search_name(LtePhyPdschDemapperConfig_v23_Carrier_Index, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName), carrier_index);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");

            j["CSI-RS Exist"] = iCSIRSExist;
            j["ZP CSI-RS Exist"] = iZPCSIRSExist;
            j["CSI-RS Symbol Skipped"] = iCSIRSSymbolSkipped;

            tmp = j["Op Mode"];
            int iOPMode = tmp & 0xf;
            int iStrongICellID = (tmp >> 4) & 0x1ff;
            int iJointDemodSkipReason = (tmp >> 13) & 0x1f;

            j["Op Mode"] = iOPMode;
            const char* opmode_name = search_name(LtePhyPdschDemapperConfig_v122_OPMode, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_OPMode, ValueName), iOPMode);
            j["Op Mode"] = (opmode_name ? opmode_name : "(MI)Unknown");

            j["Strong ICell ID"] = iStrongICellID;
            j["Joint Demod Skip Reason"] = iJointDemodSkipReason;
            const char* skip_reason_name = search_name(LtePhyPdschDemapperConfig_Joint_Demod_Skip_Reason, ARRAY_SIZE(LtePhyPdschDemapperConfig_Joint_Demod_Skip_Reason, ValueName), iJointDemodSkipReason);
            j["Joint Demod Skip Reason"] = (skip_reason_name ? skip_reason_name : "(MI)Unknown");

            if (j.find("TBS 0") != j.end()) { j["Transport Block Size Stream 0"] = j["TBS 0"]; j.erase("TBS 0"); }
            if (j.find("TBS 1") != j.end()) { j["Transport Block Size Stream 1"] = j["TBS 1"]; j.erase("TBS 1"); }

            break;
        }
        case 144: {
            int header_consumed = _decode_by_fmt(LtePhyPdschDemapperConfigFmt_header_144,
                                                 ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_header_144, Fmt),
                                                 b, offset, length, j);
            offset += header_consumed;

            int tmp_header = j["Carrier Index"];
            int iCarrierIndex = tmp_header & 0xf;
            int iNumOfRecords = (tmp_header >> 4) & 0xff;

            j["Carrier Index"] = iCarrierIndex;
            const char* carrier_name = search_name(LtePhyPdschDemapperConfig_v23_Carrier_Index, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName), iCarrierIndex);
            j["Carrier Index"] = (carrier_name ? carrier_name : "(MI)Unknown");
            j["Num of Records"] = iNumOfRecords;

            json records_j = json::array();
            for (int i = 0; i < iNumOfRecords; ++i) {
                json record_item_j;
                int record_consumed = _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v144,
                                                     ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v144, Fmt),
                                                     b, offset, length, record_item_j);
                if (record_consumed == 0) {
                    LOGD("Error decoding record %d for PDSCH Demapper v144", i);
                    break;
                }
                offset += record_consumed;

                unsigned int utemp0 = record_item_j["Serving Cell ID"];
                record_item_j["Serving Cell ID"] = (utemp0 >> 0) & 0x1ff;
                record_item_j["Sub-frame Number"] = (utemp0 >> 9) & 0xf;
                record_item_j["System Frame Number"] = (utemp0 >> 13) & 0x3ff;
                record_item_j["PDSCH RNTI Type"] = (utemp0 >> 23) & 0xf;
                record_item_j["Number of Tx Antennas (M)"] = (utemp0 >> 27) & 0x3;
                record_item_j["Number of Rx Antennas (N)"] = (utemp0 >> 29) & 0x3;



                unsigned int utemp1 = record_item_j["Spatial Rank"];
                record_item_j["Spatial Rank"] = (utemp1 >> 2) & 0x3;
                record_item_j["Frequency Selective PMI"] = (utemp1 >> 5) & 0x1;
                record_item_j["MU Receiver Mode"] = (utemp1 >> 6) & 0x3;

                unsigned int utemp2 = record_item_j["PMI Index"];
                record_item_j["PMI Index"] = (utemp2 >> 0) & 0xf;
                record_item_j["Transmission Scheme"] = (utemp2 >> 4) & 0xf;



                unsigned int utemp_uers = record_item_j["UERS Port Enabled"];
                record_item_j["UERS Port Enabled"] = (utemp_uers >> 0) & 0x7;
                record_item_j["BMOD FD Sym Index"] = (utemp_uers >> 3) & 0xf;
                record_item_j["Transport Block Size Stream 0"] = (utemp_uers >> 7) & 0x3ffff;
                record_item_j["Modulation Stream 0"] = (utemp_uers >> 25) & 0x3;
                record_item_j["PB"] = (utemp_uers >> 27) & 0x3;
                record_item_j["RhoB/RhoA"] = (utemp_uers >> 29) & 0x3;
                record_item_j["CSI-RS Exist"] = (utemp_uers >> 31) & 0x1;

                unsigned int utemp_zp = record_item_j["ZP CSI-RS Exist"];
                record_item_j["ZP CSI-RS Exist"] = (utemp_zp >> 0) & 0x1;
                record_item_j["CSI-RS Symbol Skipped"] = (utemp_zp >> 1) & 0x1;
                record_item_j["Traffic to Pilot Ratio Data"] = (utemp_zp >> 2) & 0xfff;
                record_item_j["Transport Block Size Stream 1"] = (utemp_zp >> 14) & 0x3ffff;

                unsigned int utemp_mod1 = record_item_j["Modulation Stream 1"];
                record_item_j["Modulation Stream 1"] = (utemp_mod1 >> 0) & 0x3;
                record_item_j["SCH0 Memory Map Mode"] = (utemp_mod1 >> 2) & 0x3;
                record_item_j["SCH1 Memory Map Mode"] = (utemp_mod1 >> 4) & 0x3;
                record_item_j["Strong ICell ID"] = (utemp_mod1 >> 6) & 0x1ff;
                record_item_j["Qice Enable Mode"] = (utemp_mod1 >> 15) & 0x3;
                record_item_j["Qice Skip Reason"] = (utemp_mod1 >> 17) & 0x7;
                record_item_j["Csf Dual Rnn Sel"] = (utemp_mod1 >> 20) & 0x1;
                record_item_j["Plq Num Enabled Rd Groups"] = (utemp_mod1 >> 21) & 0x1f;
                record_item_j["Plg Num Loaded Rb Groups"] = (utemp_mod1 >> 26) & 0x1f;
                record_item_j["Qed Mode"] = (utemp_mod1 >> 31) & 0x7;



                const char* rnti_name_rec = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), record_item_j["PDSCH RNTI Type"]);
                record_item_j["PDSCH RNTI Type"] = (rnti_name_rec ? rnti_name_rec : "(MI)Unknown");
                const char* tx_ant_name_rec = search_name(LtePhyPdschDemapperConfig_v122_antenna, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_antenna, ValueName), record_item_j["Number of Tx Antennas (M)"]);
                record_item_j["Number of Tx Antennas (M)"] = (tx_ant_name_rec ? tx_ant_name_rec : "(MI)Unknown");
                const char* rx_ant_name_rec = search_name(LtePhyPdschDemapperConfig_v122_antenna, ARRAY_SIZE(LtePhyPdschDemapperConfig_v122_antenna, ValueName), record_item_j["Number of Rx Antennas (N)"]);
                record_item_j["Number of Rx Antennas (N)"] = (rx_ant_name_rec ? rx_ant_name_rec : "(MI)Unknown");
                const char* rank_name_rec = search_name(ValueNameRankIndex, ARRAY_SIZE(ValueNameRankIndex, ValueName), record_item_j["Spatial Rank"]);
                record_item_j["Spatial Rank"] = (rank_name_rec ? rank_name_rec : "(MI)Unknown");
                const char* freq_pmi_name_rec = search_name(ValueNameFrequencySelectivePMI, ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName), record_item_j["Frequency Selective PMI"]);
                record_item_j["Frequency Selective PMI"] = (freq_pmi_name_rec ? freq_pmi_name_rec : "(MI)Unknown");
                const char* tx_scheme_name_rec = search_name(ValueNameTransmissionScheme, ARRAY_SIZE(ValueNameTransmissionScheme, ValueName), record_item_j["Transmission Scheme"]);
                record_item_j["Transmission Scheme"] = (tx_scheme_name_rec ? tx_scheme_name_rec : "(MI)Unknown");
                const char* mod0_name_rec = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), record_item_j["Modulation Stream 0"]);
                record_item_j["Modulation Stream 0"] = (mod0_name_rec ? mod0_name_rec : "(MI)Unknown");
                const char* mod1_name_rec = search_name(LtePhyPdschDemapperConfig_v23_Modulation, ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName), record_item_j["Modulation Stream 1"]);
                record_item_j["Modulation Stream 1"] = (mod1_name_rec ? mod1_name_rec : "(MI)Unknown");


                if(record_item_j.find("RB Allocation Slot 0[0]") != record_item_j.end()) record_item_j.erase("RB Allocation Slot 0[0]");
                if(record_item_j.find("RB Allocation Slot 0[1]") != record_item_j.end()) record_item_j.erase("RB Allocation Slot 0[1]");
                if(record_item_j.find("RB Allocation Slot 1[0]") != record_item_j.end()) record_item_j.erase("RB Allocation Slot 1[0]");
                if(record_item_j.find("RB Allocation Slot 1[1]") != record_item_j.end()) record_item_j.erase("RB Allocation Slot 1[1]");


                records_j.push_back(record_item_j);
            }
            j["Records"] = records_j;
            break;
        }
        default:
            LOGD("(MI)Unknown LTE PHY PDSCH Demapper Configuration version: 0x%x", pkt_ver);
            return 0;
    }

    return offset - start;
}

