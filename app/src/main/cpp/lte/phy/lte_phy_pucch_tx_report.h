//
// Created by Vladimir Ponomarenko on 24.05.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_phy_pucch_tx_report_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PHY_PUCCH_Tx_Report: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    LOGD("Decoding LTE_PHY_PUCCH_Tx_Report with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 21: {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v21,
                                     ARRAY_SIZE(LtePhyPucchTxReport_Payload_v21, Fmt),
                                     b, offset, length, j);

            int temp_serv_cell = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell & 511;
            int num_record_v21 = (temp_serv_cell >> 9) & 31;
            j["Number of Records"] = num_record_v21;

            json records_v21_j = json::array();
            for (int k = 0; k < num_record_v21; ++k) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v21,
                                         ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v21, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("CQI Payload") != record_item_j.end() && record_item_j["CQI Payload"].is_number()) {
                    record_item_j["CQI Payload"] = std::bitset<16>(record_item_j["CQI Payload"].get<int>()).to_string();
                }

                unsigned int u_temp_fmt = record_item_j["Format"];
                record_item_j["Format"] = u_temp_fmt & 7;
                const char* fmt_name_v21 = search_name(LtePucchPowerControl_Record_v4_PUCCH_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format, ValueName), record_item_j["Format"]);
                record_item_j["Format"] = (fmt_name_v21 ? fmt_name_v21 : "(MI)Unknown");
                record_item_j["Start RB Slot 0"] = (u_temp_fmt >> 3) & 127;
                record_item_j["Start RB Slot 1"] = (u_temp_fmt >> 10) & 127;
                record_item_j["SRS Shorting for 2nd Slot"] = (u_temp_fmt >> 17) & 1;
                const char* srs_short_name_v21 = search_name(ValueNameSRSShortingfor2ndSlot, ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot, ValueName), record_item_j["SRS Shorting for 2nd Slot"]);
                record_item_j["SRS Shorting for 2nd Slot"] = (srs_short_name_v21 ? srs_short_name_v21 : "(MI)Unknown");
                record_item_j["UE SRS"] = (u_temp_fmt >> 18) & 1;
                const char* ue_srs_name_v21 = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), record_item_j["UE SRS"]);
                record_item_j["UE SRS"] = (ue_srs_name_v21 ? ue_srs_name_v21 : "(MI)Unknown");
                record_item_j["DMRS Seq Slot 0"] = (u_temp_fmt >> 19) & 31;
                record_item_j["DMRS Seq Slot 1"] = (u_temp_fmt >> 24) & 31;
                int ack_payload_val_v21 = (u_temp_fmt >> 29) & 3;
                record_item_j["ACK Payload"] = std::bitset<2>(ack_payload_val_v21).to_string();


                json css_list_j = json::array();
                for (int css_idx = 0; css_idx < 14; ++css_idx) {
                    json css_item_j;
                    offset += _decode_by_fmt(LtePhyPucchTxReport_Record_CyclicShift_v21, ARRAY_SIZE(LtePhyPucchTxReport_Record_CyclicShift_v21, Fmt), b, offset, length, css_item_j);
                    css_list_j.push_back(css_item_j);
                }
                record_item_j["Cyclic Shift Seq"] = css_list_j;

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v21,
                                         ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v21, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("PUCCH Tx Power (dBm)") != record_item_j.end() && record_item_j["PUCCH Tx Power (dBm)"].is_number()) {
                    record_item_j["PUCCH Tx Power (dBm)"] = record_item_j["PUCCH Tx Power (dBm)"].get<int>() & 63;
                }
                if (record_item_j.find("Tx Resampler") != record_item_j.end() && record_item_j["Tx Resampler"].is_number()) {
                    record_item_j["Tx Resampler"] = record_item_j["Tx Resampler"].get<unsigned int>() / 4294967295.0;
                }
                records_v21_j.push_back(record_item_j);
            }
            j["Records"] = records_v21_j;
            break;
        }
        case 22: {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v21,
                                     ARRAY_SIZE(LtePhyPucchTxReport_Payload_v21, Fmt),
                                     b, offset, length, j);

            int temp_serv_cell = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell & 511;
            int num_record_v22 = (temp_serv_cell >> 9) & 31;
            j["Number of Records"] = num_record_v22;

            json records_v22_j = json::array();
            for (int k = 0; k < num_record_v22; ++k) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v21,
                                         ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v21, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("CQI Payload") != record_item_j.end() && record_item_j["CQI Payload"].is_number()) {
                    record_item_j["CQI Payload"] = std::bitset<16>(record_item_j["CQI Payload"].get<int>()).to_string();
                }

                unsigned int u_temp_fmt = record_item_j["Format"];
                record_item_j["Format"] = u_temp_fmt & 7;
                const char* fmt_name_v22 = search_name(LtePucchPowerControl_Record_v4_PUCCH_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format, ValueName), record_item_j["Format"]);
                record_item_j["Format"] = (fmt_name_v22 ? fmt_name_v22 : "(MI)Unknown");
                record_item_j["Start RB Slot 0"] = (u_temp_fmt >> 3) & 127;
                record_item_j["Start RB Slot 1"] = (u_temp_fmt >> 10) & 127;
                record_item_j["SRS Shorting for 2nd Slot"] = (u_temp_fmt >> 17) & 1;
                const char* srs_short_name_v22 = search_name(ValueNameSRSShortingfor2ndSlot, ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot, ValueName), record_item_j["SRS Shorting for 2nd Slot"]);
                record_item_j["SRS Shorting for 2nd Slot"] = (srs_short_name_v22 ? srs_short_name_v22 : "(MI)Unknown");
                record_item_j["UE SRS"] = (u_temp_fmt >> 18) & 1;
                const char* ue_srs_name_v22 = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), record_item_j["UE SRS"]);
                record_item_j["UE SRS"] = (ue_srs_name_v22 ? ue_srs_name_v22 : "(MI)Unknown");
                record_item_j["DMRS Seq Slot 0"] = (u_temp_fmt >> 19) & 31;
                record_item_j["DMRS Seq Slot 1"] = (u_temp_fmt >> 24) & 31;
                int ack_payload_val_v22 = (u_temp_fmt >> 29) & 3;
                record_item_j["ACK Payload"] = std::bitset<2>(ack_payload_val_v22).to_string();

                json css_list_j_v22 = json::array();
                for (int css_idx = 0; css_idx < 14; ++css_idx) {
                    json css_item_j;
                    offset += _decode_by_fmt(LtePhyPucchTxReport_Record_CyclicShift_v21, ARRAY_SIZE(LtePhyPucchTxReport_Record_CyclicShift_v21, Fmt), b, offset, length, css_item_j);
                    css_list_j_v22.push_back(css_item_j);
                }
                record_item_j["Cyclic Shift Seq"] = css_list_j_v22;

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v22,
                                         ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v22, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("PUCCH Tx Power (dBm)") != record_item_j.end() && record_item_j["PUCCH Tx Power (dBm)"].is_number()) {
                    record_item_j["PUCCH Tx Power (dBm)"] = record_item_j["PUCCH Tx Power (dBm)"].get<int>() & 63;
                }
                if (record_item_j.find("Tx Resampler") != record_item_j.end() && record_item_j["Tx Resampler"].is_number()) {
                    record_item_j["Tx Resampler"] = record_item_j["Tx Resampler"].get<unsigned int>() / 4294967295.0;
                }
                if (record_item_j.find("Num Repetition") != record_item_j.end() && record_item_j["Num Repetition"].is_number()) {
                    unsigned int num_rep_raw = record_item_j["Num Repetition"];
                    record_item_j["Num Repetition"] = num_rep_raw & 0xfff;
                    record_item_j["RB NB Start Index"] = (num_rep_raw >> 12) & 0xff;
                }
                records_v22_j.push_back(record_item_j);
            }
            j["Records"] = records_v22_j;
            break;
        }
        case 43: {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v43,
                                     ARRAY_SIZE(LtePhyPucchTxReport_Payload_v43, Fmt),
                                     b, offset, length, j);

            int temp_serv_cell = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell & 511;
            int num_record_v43 = (temp_serv_cell >> 9) & 31;
            j["Number of Records"] = num_record_v43;

            json records_v43_j = json::array();
            for (int k = 0; k < num_record_v43; ++k) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v43,
                                         ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v43, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("CQI Payload") != record_item_j.end() && record_item_j["CQI Payload"].is_number()) {
                    record_item_j["CQI Payload"] = std::bitset<16>(record_item_j["CQI Payload"].get<int>()).to_string();
                }

                unsigned int u_temp_fmt = record_item_j["Format"];
                record_item_j["Format"] = u_temp_fmt & 7;
                const char* fmt_name_v43 = search_name(LtePucchPowerControl_Record_v4_PUCCH_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format, ValueName), record_item_j["Format"]);
                record_item_j["Format"] = (fmt_name_v43 ? fmt_name_v43 : "(MI)Unknown");
                record_item_j["Start RB Slot 0"] = (u_temp_fmt >> 3) & 127;
                record_item_j["Start RB Slot 1"] = (u_temp_fmt >> 10) & 127;
                record_item_j["SRS Shorting for 2nd Slot"] = (u_temp_fmt >> 17) & 1;
                const char* srs_short_name_v43 = search_name(ValueNameSRSShortingfor2ndSlot, ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot, ValueName), record_item_j["SRS Shorting for 2nd Slot"]);
                record_item_j["SRS Shorting for 2nd Slot"] = (srs_short_name_v43 ? srs_short_name_v43 : "(MI)Unknown");
                record_item_j["UE SRS"] = (u_temp_fmt >> 18) & 1;
                const char* ue_srs_name_v43 = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), record_item_j["UE SRS"]);
                record_item_j["UE SRS"] = (ue_srs_name_v43 ? ue_srs_name_v43 : "(MI)Unknown");
                record_item_j["DMRS Seq Slot 0"] = (u_temp_fmt >> 19) & 31;
                record_item_j["DMRS Seq Slot 1"] = (u_temp_fmt >> 24) & 31;

                json css_list_j_v43 = json::array();
                for (int css_idx = 0; css_idx < 14; ++css_idx) {
                    json css_item_j;
                    offset += _decode_by_fmt(LtePhyPucchTxReport_Record_CyclicShift_v43, ARRAY_SIZE(LtePhyPucchTxReport_Record_CyclicShift_v43, Fmt), b, offset, length, css_item_j);
                    css_list_j_v43.push_back(css_item_j);
                }
                record_item_j["Cyclic Shift Seq"] = css_list_j_v43;

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v43,
                                         ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v43, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("PUCCH Tx Power (dBm)") != record_item_j.end() && record_item_j["PUCCH Tx Power (dBm)"].is_number()) {
                    record_item_j["PUCCH Tx Power (dBm)"] = record_item_j["PUCCH Tx Power (dBm)"].get<int>() & 63;
                }

                unsigned int u_temp_ack = record_item_j["ACK Payload"];
                record_item_j["ACK Payload"] = std::bitset<20>(u_temp_ack & 1048575).to_string();
                record_item_j["ACK Payload Length"] = (u_temp_ack >> 20) & 31;
                record_item_j["SR Bit FMT3"] = (u_temp_ack >> 25) & 1;
                record_item_j["Num DL Carriers"] = (u_temp_ack >> 26) & 15;

                int temp_ack_idx = record_item_j["Ack Nak Index"];
                std::stringstream ss_ack_idx;
                ss_ack_idx << "0x" << std::hex << std::setfill('0') << std::setw(3) << (temp_ack_idx & 4095);
                record_item_j["Ack Nak Index"] = ss_ack_idx.str();
                record_item_j["Ack Nak Late"] = (temp_ack_idx >> 12) & 1;
                record_item_j["Csf Late"] = (temp_ack_idx >> 13) & 1;
                record_item_j["Dropped Pucch"] = (temp_ack_idx >> 14) & 1;

                if (record_item_j.find("Tx Resampler") != record_item_j.end() && record_item_j["Tx Resampler"].is_number()) {
                    record_item_j["Tx Resampler"] = record_item_j["Tx Resampler"].get<unsigned int>() / 4294967295.0;
                }
                records_v43_j.push_back(record_item_j);
            }
            j["Records"] = records_v43_j;
            break;
        }
        case 101:
        case 121:
        {
            const Fmt* p1_fmt = (pkt_ver == 121) ? LtePhyPucchTxReport_Record_p1_v121 : LtePhyPucchTxReport_Record_p1_v101;
            size_t p1_fmt_size = (pkt_ver == 121) ? ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v121, Fmt) : ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v101, Fmt);
            const Fmt* css_fmt = (pkt_ver == 121) ? LtePhyPucchTxReport_Record_CyclicShift_v121 : LtePhyPucchTxReport_Record_CyclicShift_v101;
            size_t css_fmt_size = (pkt_ver == 121) ? ARRAY_SIZE(LtePhyPucchTxReport_Record_CyclicShift_v121, Fmt) : ARRAY_SIZE(LtePhyPucchTxReport_Record_CyclicShift_v101, Fmt);
            const Fmt* p2_fmt = (pkt_ver == 121) ? LtePhyPucchTxReport_Record_p2_v121 : LtePhyPucchTxReport_Record_p2_v101;
            size_t p2_fmt_size = (pkt_ver == 121) ? ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v121, Fmt) : ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v101, Fmt);
            const Fmt* payload_fmt_header = (pkt_ver == 121) ? LtePhyPucchTxReport_Payload_v121 : LtePhyPucchTxReport_Payload_v101;
            size_t payload_fmt_header_size = (pkt_ver == 121) ? ARRAY_SIZE(LtePhyPucchTxReport_Payload_v121, Fmt) : ARRAY_SIZE(LtePhyPucchTxReport_Payload_v101, Fmt);


            offset += _decode_by_fmt(payload_fmt_header, payload_fmt_header_size, b, offset, length, j);

            int temp_serv_cell = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell & 511;
            int num_record_v10x = (temp_serv_cell >> 9) & 31;
            j["Number of Records"] = num_record_v10x;

            json records_v10x_j = json::array();
            for (int k = 0; k < num_record_v10x; ++k) {
                json record_item_j;
                offset += _decode_by_fmt(p1_fmt, p1_fmt_size, b, offset, length, record_item_j);

                if (record_item_j.find("CQI Payload") != record_item_j.end() && record_item_j["CQI Payload"].is_number()) {
                    record_item_j["CQI Payload"] = std::bitset<16>(record_item_j["CQI Payload"].get<int>()).to_string();
                }

                unsigned int u_temp_ci_fmt = record_item_j["Carrier Index"];
                record_item_j["Carrier Index"] = u_temp_ci_fmt & 3;
                const char* carrier_name_v10x = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), record_item_j["Carrier Index"]);
                record_item_j["Carrier Index"] = (carrier_name_v10x ? carrier_name_v10x : "(MI)Unknown");
                record_item_j["Format"] = (u_temp_ci_fmt >> 2) & 7;
                const char* fmt_name_v10x = search_name(LtePucchPowerControl_Record_v4_PUCCH_Format, ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format, ValueName), record_item_j["Format"]);
                record_item_j["Format"] = (fmt_name_v10x ? fmt_name_v10x : "(MI)Unknown");
                record_item_j["Start RB Slot 0"] = (u_temp_ci_fmt >> 5) & 127;
                record_item_j["Start RB Slot 1"] = (u_temp_ci_fmt >> 12) & 127;
                record_item_j["SRS Shorting for 2nd Slot"] = (u_temp_ci_fmt >> 19) & 1;
                const char* srs_short_name_v10x = search_name(ValueNameSRSShortingfor2ndSlot, ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot, ValueName), record_item_j["SRS Shorting for 2nd Slot"]);
                record_item_j["SRS Shorting for 2nd Slot"] = (srs_short_name_v10x ? srs_short_name_v10x : "(MI)Unknown");
                record_item_j["UE SRS"] = (u_temp_ci_fmt >> 20) & 1;
                const char* ue_srs_name_v10x = search_name(ValueNameOnOrOff, ARRAY_SIZE(ValueNameOnOrOff, ValueName), record_item_j["UE SRS"]);
                record_item_j["UE SRS"] = (ue_srs_name_v10x ? ue_srs_name_v10x : "(MI)Unknown");
                record_item_j["DMRS Seq Slot 0"] = (u_temp_ci_fmt >> 21) & 31;
                record_item_j["DMRS Seq Slot 1"] = (u_temp_ci_fmt >> 26) & 31;

                json css_list_j_v10x = json::array();
                for (int css_idx = 0; css_idx < 14; ++css_idx) {
                    json css_item_j;
                    offset += _decode_by_fmt(css_fmt, css_fmt_size, b, offset, length, css_item_j);
                    css_list_j_v10x.push_back(css_item_j);
                }
                record_item_j["Cyclic Shift Seq"] = css_list_j_v10x;

                offset += _decode_by_fmt(p2_fmt, p2_fmt_size, b, offset, length, record_item_j);

                if (record_item_j.find("PUCCH Tx Power (dBm)") != record_item_j.end() && record_item_j["PUCCH Tx Power (dBm)"].is_number()) {
                    record_item_j["PUCCH Tx Power (dBm)"] = record_item_j["PUCCH Tx Power (dBm)"].get<int>() & 63;
                }

                unsigned int u_temp_ack_v10x = record_item_j["ACK Payload"];
                record_item_j["ACK Payload"] = std::bitset<20>(u_temp_ack_v10x & 1048575).to_string();
                record_item_j["ACK Payload Length"] = (u_temp_ack_v10x >> 20) & 31;
                record_item_j["SR Bit FMT3"] = (u_temp_ack_v10x >> 25) & 1;
                record_item_j["Num DL Carriers"] = (u_temp_ack_v10x >> 26) & 15;
                if(pkt_ver == 121){
                    record_item_j["DL Carrier Index"] = (u_temp_ack_v10x >> 30) & 1;
                    const char* dl_carrier_name = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), record_item_j["DL Carrier Index"]);
                    record_item_j["DL Carrier Index"] = (dl_carrier_name ? dl_carrier_name : "(MI)Unknown");
                }


                int temp_ack_idx_v10x = record_item_j["Ack Nak Index"];
                std::stringstream ss_ack_idx_v10x;
                ss_ack_idx_v10x << "0x" << std::hex << std::setfill('0') << std::setw(3) << (temp_ack_idx_v10x & 4095);
                record_item_j["Ack Nak Index"] = ss_ack_idx_v10x.str();
                record_item_j["Ack Nak Late"] = (temp_ack_idx_v10x >> 12) & 1;
                record_item_j["Csf Late"] = (temp_ack_idx_v10x >> 13) & 1;
                record_item_j["Dropped Pucch"] = (temp_ack_idx_v10x >> 14) & 1;

                if (record_item_j.find("Tx Resampler") != record_item_j.end() && record_item_j["Tx Resampler"].is_number()) {
                    record_item_j["Tx Resampler"] = record_item_j["Tx Resampler"].get<unsigned int>() / 4294967295.0;
                }
                records_v10x_j.push_back(record_item_j);
            }
            j["Records"] = records_v10x_j;
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
            int size_bwp_part1_v142 = (temp0_v142 >> 30) & 3;

            unsigned int temp1_v142 = j["Number of Subbands"];
            j["Size BWP"] = size_bwp_part1_v142 | ((temp1_v142 & 1) << 2);
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
            LOGD("(MI)Unknown LTE PHY PUCCH CSF version: %d", pkt_ver);
            return 0;
    }

    return offset - start;
}
