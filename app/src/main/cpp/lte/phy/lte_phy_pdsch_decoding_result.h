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
#include "log_packet_utils.h"
#include "log_packet.h"

using json = nlohmann::json;
using namespace std;


static int _decode_lte_phy_pdsch_decoding_result_payload(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_PHY_PDSCH_Decoding_Result: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
   // LOGD("Decoding LTE_PHY_PDSCH_Decoding_Result with version: %d", pkt_ver);

    switch (pkt_ver) {
        case 24: {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v24,
                                     ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v24, Fmt),
                                     b, offset, length, j);

            unsigned int temp_serv_cell_v24 = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell_v24 & 511;
            j["Starting Subframe Number"] = (temp_serv_cell_v24 >> 9) & 15;
            j["Starting System Frame Number"] = (temp_serv_cell_v24 >> 13) & 1023;
            j["UE Category"] = (temp_serv_cell_v24 >> 24) & 15;
            j["Num DL HARQ"] = (temp_serv_cell_v24 >> 28) & 15;

            int temp_tm_mode_v24 = j["TM Mode"];
            j["TM Mode"] = (temp_tm_mode_v24 >> 4) & 15;

            int temp_carrier_v24 = j["Carrier Index"];
            j["Carrier Index"] = temp_carrier_v24 & 7;
            const char* carrier_name_v24 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v24 ? carrier_name_v24 : "(MI)Unknown");
            j["Number of Records"] = (temp_carrier_v24 >> 3) & 31;

            int num_records_v24 = j["Number of Records"];
            json records_v24_j = json::array();
            for (int rec_idx = 0; rec_idx < num_records_v24; ++rec_idx) {
                json record_item_j;
                int record_offset_start = offset;
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v24,
                                         ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v24, Fmt),
                                         b, offset, length, record_item_j);

                int starting_subframe_num_v24 = j["Starting Subframe Number"];
                record_item_j["Subframe Number"] = (record_item_j["Subframe Offset"].get<int>() + starting_subframe_num_v24) % 10;

                int harq_id_raw_v24 = record_item_j["HARQ ID"];
                record_item_j["HARQ ID"] = harq_id_raw_v24 & 15;
                record_item_j["RNTI Type"] = (harq_id_raw_v24 >> 4) & 15;
                const char* rnti_type_name_v24 = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), record_item_j["RNTI Type"]);
                record_item_j["RNTI Type"] = (rnti_type_name_v24 ? rnti_type_name_v24 : "(MI)Unknown");

                int si_msg_num_raw_v24 = record_item_j["System Information Msg Number"];
                record_item_j["System Information Msg Number"] = si_msg_num_raw_v24 & 15;
                record_item_j["System Information Mask"] = (si_msg_num_raw_v24 >> 4) & 4095;

                int harq_log_stat_raw_v24 = record_item_j["HARQ Log Status"];
                record_item_j["HARQ Log Status"] = (harq_log_stat_raw_v24 >> 3) & 3;
                const char* harq_log_name_v24 = search_name(ValueNameHARQLogStatus, ARRAY_SIZE(ValueNameHARQLogStatus, ValueName), record_item_j["HARQ Log Status"]);
                record_item_j["HARQ Log Status"] = (harq_log_name_v24 ? harq_log_name_v24 : "(MI)Unknown");
                record_item_j["Codeword Swap"] = (harq_log_stat_raw_v24 >> 5) & 1;
                int num_stream_v24 = (harq_log_stat_raw_v24 >> 6) & 3;
                record_item_j["Number of Streams"] = num_stream_v24;

                json streams_v24_j = json::array();
                int stream_data_offset = offset - ( record_offset_start + ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v24, Fmt) * 0);
                int current_stream_offset = offset;

                for (int stream_idx = 0; stream_idx < num_stream_v24; ++stream_idx) {
                    json stream_item_j;
                    int stream_base_offset = current_stream_offset;
                    current_stream_offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v24,
                                                            ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v24, Fmt),
                                                            b, stream_base_offset, length - (stream_base_offset - start), stream_item_j);

                    unsigned int crc_raw_v24 = stream_item_j["Transport Block CRC"];
                    stream_item_j["Transport Block CRC"] = crc_raw_v24 & 1;
                    const char* crc_name_v24 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), stream_item_j["Transport Block CRC"]);
                    stream_item_j["Transport Block CRC"] = (crc_name_v24 ? crc_name_v24 : "(MI)Unknown");
                    stream_item_j["NDI"] = (crc_raw_v24 >> 1) & 1;
                    stream_item_j["Code Block Size Plus"] = (crc_raw_v24 >> 2) & 8191;
                    stream_item_j["Num Code Block Plus"] = (crc_raw_v24 >> 15) & 15;
                    stream_item_j["Max TDEC Iter"] = (crc_raw_v24 >> 19) & 15;
                    stream_item_j["Retransmission Number"] = (crc_raw_v24 >> 23) & 7;
                    const char* retx_num_name_v24 = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), stream_item_j["Retransmission Number"]);
                    stream_item_j["Retransmission Number"] = (retx_num_name_v24 ? retx_num_name_v24 : "(MI)Unknown");
                    stream_item_j["RVID"] = (crc_raw_v24 >> 26) & 3;
                    stream_item_j["Companding Stats"] = (crc_raw_v24 >> 28) & 3;
                    const char* comp_stats_name_v24 = search_name(ValueNameCompandingStats, ARRAY_SIZE(ValueNameCompandingStats, ValueName), stream_item_j["Companding Stats"]);
                    stream_item_j["Companding Stats"] = (comp_stats_name_v24 ? comp_stats_name_v24 : "(MI)Unknown");
                    stream_item_j["HARQ Combining"] = (crc_raw_v24 >> 30) & 1;
                    const char* harq_comb_name_v24 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), stream_item_j["HARQ Combining"]);
                    stream_item_j["HARQ Combining"] = (harq_comb_name_v24 ? harq_comb_name_v24 : "(MI)Unknown");
                    stream_item_j["Decob TB CRC"] = (crc_raw_v24 >> 31) & 1;

                    unsigned int num_re_raw_v24 = stream_item_j["Num RE"];
                    stream_item_j["Num RE"] = (num_re_raw_v24 >> 10) & 65535;
                    stream_item_j["Codeword Index"] = (num_re_raw_v24 >> 27) & 15;

                    int num_energy_metric_v24 = stream_item_j["Num Code Block Plus"];
                    json energy_metrics_v24_j = json::array();
                    int current_energy_offset = current_stream_offset;

                    for (int em_idx = 0; em_idx < num_energy_metric_v24; ++em_idx) {
                        json energy_item_j;
                        int em_base_offset = current_energy_offset;
                        current_energy_offset += _decode_by_fmt(LtePhyPdschDecodingResult_EnergyMetric_v24,
                                                                ARRAY_SIZE(LtePhyPdschDecodingResult_EnergyMetric_v24, Fmt),
                                                                b, em_base_offset, length - (em_base_offset-start), energy_item_j);
                        unsigned int em_raw_v24 = energy_item_j["Energy Metric"];
                        energy_item_j["Energy Metric"] = em_raw_v24 & 2097151;
                        energy_item_j["Iteration Number"] = (em_raw_v24 >> 21) & 15;
                        energy_item_j["Code Block CRC Pass"] = (em_raw_v24 >> 25) & 1;
                        const char* cb_crc_name_v24 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), energy_item_j["Code Block CRC Pass"]);
                        energy_item_j["Code Block CRC Pass"] = (cb_crc_name_v24 ? cb_crc_name_v24 : "(MI)Unknown");
                        energy_item_j["Early Termination"] = (em_raw_v24 >> 26) & 1;
                        const char* et_name_v24 = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), energy_item_j["Early Termination"]);
                        energy_item_j["Early Termination"] = (et_name_v24 ? et_name_v24 : "(MI)Unknown");
                        energy_item_j["HARQ Combine Enable"] = (em_raw_v24 >> 27) & 1;
                        const char* harq_en_name_v24 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), energy_item_j["HARQ Combine Enable"]);
                        energy_item_j["HARQ Combine Enable"] = (harq_en_name_v24 ? harq_en_name_v24 : "(MI)Unknown");
                        energy_item_j["Deint Decode Bypass"] = (em_raw_v24 >> 28) & 1;
                        energy_metrics_v24_j.push_back(energy_item_j);
                    }
                    stream_item_j["Energy Metrics"] = energy_metrics_v24_j;
                    int num_padded_em_slots_v24 = 13;
                    if (num_energy_metric_v24 < num_padded_em_slots_v24) {
                        current_energy_offset += (num_padded_em_slots_v24 - num_energy_metric_v24) * 4;
                    }
                    current_stream_offset = current_energy_offset;
                    streams_v24_j.push_back(stream_item_j);
                }
                record_item_j["Streams"] = streams_v24_j;
                offset = current_stream_offset;
                records_v24_j.push_back(record_item_j);
            }
            j["Records"] = records_v24_j;
            break;
        }
        case 27: {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v24,
                                     ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v24, Fmt),
                                     b, offset, length, j);

            unsigned int temp_serv_cell_v27 = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell_v27 & 511;
            j["Starting Subframe Number"] = (temp_serv_cell_v27 >> 9) & 15;
            j["Starting System Frame Number"] = (temp_serv_cell_v27 >> 13) & 1023;
            j["UE Category"] = (temp_serv_cell_v27 >> 24) & 15;
            j["Num DL HARQ"] = (temp_serv_cell_v27 >> 28) & 15;
            int temp_tm_mode_v27 = j["TM Mode"];
            j["TM Mode"] = (temp_tm_mode_v27 >> 4) & 15;
            int temp_carrier_v27 = j["Carrier Index"];
            j["Carrier Index"] = temp_carrier_v27 & 7;
            const char* carrier_name_v27 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v27 ? carrier_name_v27 : "(MI)Unknown");
            j["Number of Records"] = (temp_carrier_v27 >> 3) & 31;

            int num_records_v27 = j["Number of Records"];
            json records_v27_j = json::array();
            for (int rec_idx = 0; rec_idx < num_records_v27; ++rec_idx) {
                json record_item_j;
                int record_offset_start_v27 = offset;
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v24,
                                         ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v24, Fmt),
                                         b, offset, length, record_item_j);

                int starting_subframe_num_v27 = j["Starting Subframe Number"];
                record_item_j["Subframe Number"] = (record_item_j["Subframe Offset"].get<int>() + starting_subframe_num_v27) % 10;
                int harq_id_raw_v27 = record_item_j["HARQ ID"];
                record_item_j["HARQ ID"] = harq_id_raw_v27 & 15;
                record_item_j["RNTI Type"] = (harq_id_raw_v27 >> 4) & 15;
                const char* rnti_type_name_v27 = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), record_item_j["RNTI Type"]);
                record_item_j["RNTI Type"] = (rnti_type_name_v27 ? rnti_type_name_v27 : "(MI)Unknown");
                int si_msg_num_raw_v27 = record_item_j["System Information Msg Number"];
                record_item_j["System Information Msg Number"] = si_msg_num_raw_v27 & 15;
                record_item_j["System Information Mask"] = (si_msg_num_raw_v27 >> 4) & 4095;
                int harq_log_stat_raw_v27 = record_item_j["HARQ Log Status"];
                record_item_j["HARQ Log Status"] = (harq_log_stat_raw_v27 >> 3) & 3;
                const char* harq_log_name_v27 = search_name(ValueNameHARQLogStatus, ARRAY_SIZE(ValueNameHARQLogStatus, ValueName), record_item_j["HARQ Log Status"]);
                record_item_j["HARQ Log Status"] = (harq_log_name_v27 ? harq_log_name_v27 : "(MI)Unknown");
                record_item_j["Codeword Swap"] = (harq_log_stat_raw_v27 >> 5) & 1;
                int num_stream_v27 = (harq_log_stat_raw_v27 >> 6) & 3;
                record_item_j["Number of Streams"] = num_stream_v27;

                json streams_v27_j = json::array();
                int current_stream_offset_v27 = offset;
                for (int stream_idx = 0; stream_idx < num_stream_v27; ++stream_idx) {
                    json stream_item_j;
                    int stream_base_offset_v27 = current_stream_offset_v27;
                    current_stream_offset_v27 += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v27,
                                                                ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v27, Fmt),
                                                                b, stream_base_offset_v27, length - (stream_base_offset_v27 - start), stream_item_j);

                    unsigned int crc_raw_v27 = stream_item_j["Transport Block CRC"];
                    stream_item_j["Transport Block CRC"] = crc_raw_v27 & 1;
                    const char* crc_name_v27 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), stream_item_j["Transport Block CRC"]);
                    stream_item_j["Transport Block CRC"] = (crc_name_v27 ? crc_name_v27 : "(MI)Unknown");
                    stream_item_j["NDI"] = (crc_raw_v27 >> 1) & 1;
                    stream_item_j["Code Block Size Plus"] = (crc_raw_v27 >> 2) & 8191;
                    stream_item_j["Num Code Block Plus"] = (crc_raw_v27 >> 15) & 15;
                    stream_item_j["Max TDEC Iter"] = (crc_raw_v27 >> 19) & 15;
                    stream_item_j["RVID"] = (crc_raw_v27 >> 26) & 3;
                    stream_item_j["Companding Stats"] = (crc_raw_v27 >> 28) & 3;
                    const char* comp_stats_name_v27 = search_name(ValueNameCompandingStats, ARRAY_SIZE(ValueNameCompandingStats, ValueName), stream_item_j["Companding Stats"]);
                    stream_item_j["Companding Stats"] = (comp_stats_name_v27 ? comp_stats_name_v27 : "(MI)Unknown");
                    stream_item_j["HARQ Combining"] = (crc_raw_v27 >> 30) & 1;
                    const char* harq_comb_name_v27 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), stream_item_j["HARQ Combining"]);
                    stream_item_j["HARQ Combining"] = (harq_comb_name_v27 ? harq_comb_name_v27 : "(MI)Unknown");
                    stream_item_j["Decob TB CRC"] = (crc_raw_v27 >> 31) & 1;

                    unsigned int num_re_raw_v27 = stream_item_j["Num RE"];
                    stream_item_j["Num RE"] = (num_re_raw_v27 >> 10) & 65535;
                    stream_item_j["Codeword Index"] = (num_re_raw_v27 >> 27) & 1;

                    const char* retx_num_name_v27 = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), stream_item_j["Retransmission Number"]);
                    stream_item_j["Retransmission Number"] = (retx_num_name_v27 ? retx_num_name_v27 : "(MI)Unknown");


                    int num_energy_metric_v27 = stream_item_j["Num Code Block Plus"];
                    json energy_metrics_v27_j = json::array();
                    int current_energy_offset_v27 = current_stream_offset_v27;
                    for (int em_idx = 0; em_idx < num_energy_metric_v27; ++em_idx) {
                        json energy_item_j;
                        int em_base_offset_v27 = current_energy_offset_v27;
                        current_energy_offset_v27 += _decode_by_fmt(LtePhyPdschDecodingResult_EnergyMetric_v24,
                                                                    ARRAY_SIZE(LtePhyPdschDecodingResult_EnergyMetric_v24, Fmt),
                                                                    b, em_base_offset_v27, length - (em_base_offset_v27-start), energy_item_j);
                        unsigned int em_raw_v27 = energy_item_j["Energy Metric"];
                        energy_item_j["Energy Metric"] = em_raw_v27 & 2097151;
                        energy_item_j["Iteration Number"] = (em_raw_v27 >> 21) & 15;
                        energy_item_j["Code Block CRC Pass"] = (em_raw_v27 >> 25) & 1;
                        const char* cb_crc_name_v27 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), energy_item_j["Code Block CRC Pass"]);
                        energy_item_j["Code Block CRC Pass"] = (cb_crc_name_v27 ? cb_crc_name_v27 : "(MI)Unknown");
                        energy_item_j["Early Termination"] = (em_raw_v27 >> 26) & 1;
                        const char* et_name_v27 = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), energy_item_j["Early Termination"]);
                        energy_item_j["Early Termination"] = (et_name_v27 ? et_name_v27 : "(MI)Unknown");
                        energy_item_j["HARQ Combine Enable"] = (em_raw_v27 >> 27) & 1;
                        const char* harq_en_name_v27 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), energy_item_j["HARQ Combine Enable"]);
                        energy_item_j["HARQ Combine Enable"] = (harq_en_name_v27 ? harq_en_name_v27 : "(MI)Unknown");
                        energy_item_j["Deint Decode Bypass"] = (em_raw_v27 >> 28) & 1;
                        energy_metrics_v27_j.push_back(energy_item_j);
                    }
                    stream_item_j["Energy Metrics"] = energy_metrics_v27_j;
                    int num_padded_em_slots_v27 = 13;
                    if (num_energy_metric_v27 < num_padded_em_slots_v27) {
                        current_energy_offset_v27 += (num_padded_em_slots_v27 - num_energy_metric_v27) * 4;
                    }
                    current_stream_offset_v27 = current_energy_offset_v27;
                    streams_v27_j.push_back(stream_item_j);
                }
                record_item_j["Streams"] = streams_v27_j;
                offset = current_stream_offset_v27;
                records_v27_j.push_back(record_item_j);
            }
            j["Records"] = records_v27_j;
            break;
        }
        case 44: {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v44,
                                     ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v44, Fmt),
                                     b, offset, length, j);

            unsigned int temp_serv_cell_v44 = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell_v44 & 511;
            j["Starting Subframe Number"] = (temp_serv_cell_v44 >> 9) & 15;
            j["Starting System Frame Number"] = (temp_serv_cell_v44 >> 13) & 1023;
            j["UE Category"] = (temp_serv_cell_v44 >> 24) & 15;
            j["Num DL HARQ"] = (temp_serv_cell_v44 >> 28) & 15;

            int temp_tm_mode_v44 = j["TM Mode"];
            j["TM Mode"] = (temp_tm_mode_v44 >> 4) & 15;

            int temp_carrier_num_rec_v44 = j["Carrier Index"];
            j["Carrier Index"] = (temp_carrier_num_rec_v44 >> 7) & 15;
            const char* carrier_name_v44 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v44 ? carrier_name_v44 : "(MI)Unknown");
            int num_record_v44 = (temp_carrier_num_rec_v44 >> 11) & 31;
            j["Number of Records"] = num_record_v44;


            json records_v44_j = json::array();
            for (int k = 0; k < num_record_v44; ++k) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v44,
                                         ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v44, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("Subframe Offset") != record_item_j.end() && record_item_j["Subframe Offset"].is_number() &&
                    j.find("Starting Subframe Number") != j.end() && j["Starting Subframe Number"].is_number()){
                    record_item_j["Subframe Number"] = (record_item_j["Subframe Offset"].get<int>() + j["Starting Subframe Number"].get<int>()) % 10;
                }

                int temp_harq_v44 = record_item_j["HARQ ID"];
                record_item_j["HARQ ID"] = temp_harq_v44 & 15;
                record_item_j["RNTI Type"] = (temp_harq_v44 >> 4) & 15;
                const char* rnti_name_v44 = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), record_item_j["RNTI Type"]);
                record_item_j["RNTI Type"] = (rnti_name_v44 ? rnti_name_v44 : "(MI)Unknown");

                int temp_si_v44 = record_item_j["System Information Msg Number"];
                record_item_j["System Information Msg Number"] = temp_si_v44 & 15;
                record_item_j["System Information Mask"] = (temp_si_v44 >> 4) & 4095;

                int temp_harq_log_v44 = record_item_j["HARQ Log Status"];
                record_item_j["HARQ Log Status"] = (temp_harq_log_v44 >> 3) & 3;
                const char* harq_log_name_v44 = search_name(ValueNameHARQLogStatus, ARRAY_SIZE(ValueNameHARQLogStatus, ValueName), record_item_j["HARQ Log Status"]);
                record_item_j["HARQ Log Status"] = (harq_log_name_v44 ? harq_log_name_v44 : "(MI)Unknown");
                record_item_j["Codeword Swap"] = (temp_harq_log_v44 >> 5) & 1;
                record_item_j["Number of Streams"] = (temp_harq_log_v44 >> 6) & 3;

                int num_stream_v44 = record_item_j["Number of Streams"];
                json streams_list_j = json::array();
                for (int stream_idx = 0; stream_idx < num_stream_v44; ++stream_idx) {
                    json stream_item_j;
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v44,
                                             ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v44, Fmt),
                                             b, offset, length, stream_item_j);

                    unsigned int temp_crc_v44 = stream_item_j["Transport Block CRC"];
                    stream_item_j["Transport Block CRC"] = temp_crc_v44 & 1;
                    const char* crc_name_v44 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), stream_item_j["Transport Block CRC"]);
                    stream_item_j["Transport Block CRC"] = (crc_name_v44 ? crc_name_v44 : "(MI)Unknown");
                    stream_item_j["NDI"] = (temp_crc_v44 >> 1) & 1;
                    stream_item_j["Code Block Size Plus"] = (temp_crc_v44 >> 2) & 8191;
                    stream_item_j["Num Code Block Plus"] = (temp_crc_v44 >> 15) & 15;
                    stream_item_j["Max TDEC Iter"] = (temp_crc_v44 >> 19) & 15;
                    stream_item_j["Retransmission Number"] = (temp_crc_v44 >> 23) & 7;
                    const char* retx_num_name_v44 = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), stream_item_j["Retransmission Number"]);
                    stream_item_j["Retransmission Number"] = (retx_num_name_v44 ? retx_num_name_v44 : "(MI)Unknown");
                    stream_item_j["RVID"] = (temp_crc_v44 >> 26) & 3;
                    stream_item_j["Companding Stats"] = (temp_crc_v44 >> 28) & 3;
                    const char* comp_name_v44 = search_name(ValueNameCompandingStats, ARRAY_SIZE(ValueNameCompandingStats, ValueName), stream_item_j["Companding Stats"]);
                    stream_item_j["Companding Stats"] = (comp_name_v44 ? comp_name_v44 : "(MI)Unknown");
                    stream_item_j["HARQ Combining"] = (temp_crc_v44 >> 30) & 1;
                    const char* harq_comb_name_v44 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), stream_item_j["HARQ Combining"]);
                    stream_item_j["HARQ Combining"] = (harq_comb_name_v44 ? harq_comb_name_v44 : "(MI)Unknown");
                    stream_item_j["Decob TB CRC"] = (temp_crc_v44 >> 31) & 1;

                    unsigned int temp_re_v44 = stream_item_j["Num RE"];
                    stream_item_j["Num RE"] = (temp_re_v44 >> 10) & 65535;
                    stream_item_j["Codeword Index"] = (temp_re_v44 >> 27) & 15;

                    unsigned int temp_llr_v44 = stream_item_j["LLR Scale"];
                    stream_item_j["LLR Scale"] = temp_llr_v44 & 15;


                    int num_energy_metric_v44 = stream_item_j["Num Code Block Plus"];
                    json energy_metric_list_j = json::array();
                    for (int em_idx = 0; em_idx < num_energy_metric_v44; ++em_idx) {
                        json energy_item_j;
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_EnergyMetric_v44,
                                                 ARRAY_SIZE(LtePhyPdschDecodingResult_EnergyMetric_v44, Fmt),
                                                 b, offset, length, energy_item_j);

                        unsigned int temp_em_v44 = energy_item_j["Energy Metric"];
                        energy_item_j["Energy Metric"] = temp_em_v44 & 2097151;
                        energy_item_j["Iteration Number"] = (temp_em_v44 >> 21) & 15;
                        energy_item_j["Code Block CRC Pass"] = (temp_em_v44 >> 25) & 1;
                        const char* cb_crc_name_v44 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), energy_item_j["Code Block CRC Pass"]);
                        energy_item_j["Code Block CRC Pass"] = (cb_crc_name_v44 ? cb_crc_name_v44 : "(MI)Unknown");
                        energy_item_j["Early Termination"] = (temp_em_v44 >> 26) & 1;
                        const char* et_name_v44 = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), energy_item_j["Early Termination"]);
                        energy_item_j["Early Termination"] = (et_name_v44 ? et_name_v44 : "(MI)Unknown");
                        energy_item_j["HARQ Combine Enable"] = (temp_em_v44 >> 27) & 1;
                        const char* harq_en_name_v44 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), energy_item_j["HARQ Combine Enable"]);
                        energy_item_j["HARQ Combine Enable"] = (harq_en_name_v44 ? harq_en_name_v44 : "(MI)Unknown");
                        energy_item_j["Deint Decode Bypass"] = (temp_em_v44 >> 28) & 1;

                        energy_metric_list_j.push_back(energy_item_j);
                    }
                    stream_item_j["Energy Metrics"] = energy_metric_list_j;
                    int num_padded_energy_metrics = 13;
                    if (num_energy_metric_v44 < num_padded_energy_metrics) {
                        offset += (num_padded_energy_metrics - num_energy_metric_v44) * 4;
                    }

                    streams_list_j.push_back(stream_item_j);
                }
                record_item_j["Streams"] = streams_list_j;
                records_v44_j.push_back(record_item_j);
            }
            j["Records"] = records_v44_j;
            break;
        }
        case 105: {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v106,
                                     ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v106, Fmt),
                                     b, offset, length, j);

            unsigned int temp_serv_cell_v105 = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell_v105 & 511;
            j["Starting Subframe Number"] = (temp_serv_cell_v105 >> 9) & 15;
            j["Starting System Frame Number"] = (temp_serv_cell_v105 >> 13) & 1023;
            j["UE Category"] = (temp_serv_cell_v105 >> 24) & 15;
            j["Num DL HARQ"] = (temp_serv_cell_v105 >> 28) & 15;

            int temp_tm_mode_v105 = j["TM Mode"];
            j["TM Mode"] = (temp_tm_mode_v105 >> 4) & 15;

            unsigned int temp_nir_v105 = j["Nir"];
            j["Nir"] = temp_nir_v105 & 0x7fffff;
            j["Carrier Index"] = (temp_nir_v105 >> 23) & 15;
            const char* carrier_name_v105 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v105 ? carrier_name_v105 : "(MI)Unknown");
            int num_record_v105 = (temp_nir_v105 >> 27) & 31;
            j["Number of Records"] = num_record_v105;


            json records_v105_j = json::array();
            for (int k = 0; k < num_record_v105; ++k) {
                json record_item_j;
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v105,
                                         ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v105, Fmt),
                                         b, offset, length, record_item_j);

                if (record_item_j.find("Subframe Offset") != record_item_j.end() && record_item_j["Subframe Offset"].is_number() &&
                    j.find("Starting Subframe Number") != j.end() && j["Starting Subframe Number"].is_number()){
                    record_item_j["Subframe Number"] = (record_item_j["Subframe Offset"].get<int>() + j["Starting Subframe Number"].get<int>()) % 10;
                }

                int temp_harq_v105 = record_item_j["HARQ ID"];
                record_item_j["HARQ ID"] = temp_harq_v105 & 15;
                record_item_j["RNTI Type"] = (temp_harq_v105 >> 4) & 15;
                const char* rnti_name_v105 = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), record_item_j["RNTI Type"]);
                record_item_j["RNTI Type"] = (rnti_name_v105 ? rnti_name_v105 : "(MI)Unknown");

                int temp_si_v105 = record_item_j["System Information Msg Number"];
                record_item_j["System Information Msg Number"] = temp_si_v105 & 15;
                record_item_j["System Information Mask"] = (temp_si_v105 >> 4) & 4095;

                int temp_harq_log_v105 = record_item_j["HARQ Log Status"];
                record_item_j["HARQ Log Status"] = (temp_harq_log_v105 >> 3) & 3;
                const char* harq_log_name_v105 = search_name(ValueNameHARQLogStatus, ARRAY_SIZE(ValueNameHARQLogStatus, ValueName), record_item_j["HARQ Log Status"]);
                record_item_j["HARQ Log Status"] = (harq_log_name_v105 ? harq_log_name_v105 : "(MI)Unknown");
                record_item_j["Codeword Swap"] = (temp_harq_log_v105 >> 5) & 1;
                record_item_j["Number of Streams"] = (temp_harq_log_v105 >> 6) & 3;

                unsigned int temp_demap_v105 = record_item_j["Demap Sic Status"];
                record_item_j["Demap Sic Status"] = temp_demap_v105 & 0x1fff;


                int num_stream_v105 = record_item_j["Number of Streams"];
                json streams_list_j = json::array();
                for (int stream_idx = 0; stream_idx < num_stream_v105; ++stream_idx) {
                    json stream_item_j;
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v106,
                                             ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v106, Fmt),
                                             b, offset, length, stream_item_j);

                    unsigned int temp_crc_v105 = stream_item_j["Transport Block CRC"];
                    stream_item_j["Transport Block CRC"] = temp_crc_v105 & 1;
                    const char* crc_name_v105 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), stream_item_j["Transport Block CRC"]);
                    stream_item_j["Transport Block CRC"] = (crc_name_v105 ? crc_name_v105 : "(MI)Unknown");
                    stream_item_j["NDI"] = (temp_crc_v105 >> 1) & 1;
                    stream_item_j["Code Block Size Plus"] = (temp_crc_v105 >> 2) & 8191;
                    int numCodeBlockPlusData = (temp_crc_v105 >> 15) & 0x1f;
                    stream_item_j["Num Code Block Plus Data"] = numCodeBlockPlusData;
                    stream_item_j["Num Code Block Plus"] = numCodeBlockPlusData + 1;

                    stream_item_j["Max TDEC Iter"] = (temp_crc_v105 >> 20) & 15;
                    stream_item_j["Retransmission Number"] = (temp_crc_v105 >> 24) & 0x3f;
                    const char* retx_num_name_v105 = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), stream_item_j["Retransmission Number"]);
                    stream_item_j["Retransmission Number"] = (retx_num_name_v105 ? retx_num_name_v105 : "(MI)Unknown");
                    stream_item_j["RVID"] = (temp_crc_v105 >> 30) & 3;

                    unsigned int temp_comp_v105 = stream_item_j["Companding Stats"];
                    stream_item_j["Companding Stats"] = temp_comp_v105 & 3;
                    const char* comp_name_v105 = search_name(ValueNameCompandingStats, ARRAY_SIZE(ValueNameCompandingStats, ValueName), stream_item_j["Companding Stats"]);
                    stream_item_j["Companding Stats"] = (comp_name_v105 ? comp_name_v105 : "(MI)Unknown");
                    stream_item_j["HARQ Combining"] = (temp_comp_v105 >> 2) & 1;
                    const char* harq_comb_name_v105 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), stream_item_j["HARQ Combining"]);
                    stream_item_j["HARQ Combining"] = (harq_comb_name_v105 ? harq_comb_name_v105 : "(MI)Unknown");
                    stream_item_j["Decob TB CRC"] = (temp_comp_v105 >> 3) & 1;
                    stream_item_j["Num RE"] = (temp_comp_v105 >> 14) & 0x7fff;
                    stream_item_j["Codeword Index"] = (temp_comp_v105 >> 30) & 1;

                    unsigned int temp_llr_v105 = stream_item_j["LLR Scale"];
                    stream_item_j["LLR Scale"] = (temp_llr_v105 >> 6) & 3;

                    int num_energy_metric_v105 = stream_item_j["Num Code Block Plus"];
                    json energy_metric_list_j = json::array();
                    for (int em_idx = 0; em_idx < num_energy_metric_v105; ++em_idx) {
                        json energy_item_j;
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_EnergyMetric_v44,
                                                 ARRAY_SIZE(LtePhyPdschDecodingResult_EnergyMetric_v44, Fmt),
                                                 b, offset, length, energy_item_j);

                        unsigned int temp_em_v105 = energy_item_j["Energy Metric"];
                        energy_item_j["Energy Metric"] = temp_em_v105 & 2097151;
                        energy_item_j["Iteration Number"] = (temp_em_v105 >> 21) & 15;
                        energy_item_j["Code Block CRC Pass"] = (temp_em_v105 >> 25) & 1;
                        const char* cb_crc_name_v105 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), energy_item_j["Code Block CRC Pass"]);
                        energy_item_j["Code Block CRC Pass"] = (cb_crc_name_v105 ? cb_crc_name_v105 : "(MI)Unknown");
                        energy_item_j["Early Termination"] = (temp_em_v105 >> 26) & 1;
                        const char* et_name_v105 = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), energy_item_j["Early Termination"]);
                        energy_item_j["Early Termination"] = (et_name_v105 ? et_name_v105 : "(MI)Unknown");
                        energy_item_j["HARQ Combine Enable"] = (temp_em_v105 >> 27) & 1;
                        const char* harq_en_name_v105 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), energy_item_j["HARQ Combine Enable"]);
                        energy_item_j["HARQ Combine Enable"] = (harq_en_name_v105 ? harq_en_name_v105 : "(MI)Unknown");
                        energy_item_j["Deint Decode Bypass"] = (temp_em_v105 >> 28) & 1;

                        energy_metric_list_j.push_back(energy_item_j);
                    }
                    stream_item_j["Energy Metrics"] = energy_metric_list_j;
                    int num_padded_energy_metrics_v105 = 32;
                    if (num_energy_metric_v105 < num_padded_energy_metrics_v105) {
                        offset += (num_padded_energy_metrics_v105 - num_energy_metric_v105) * 4;
                    }
                    streams_list_j.push_back(stream_item_j);
                }
                record_item_j["Streams"] = streams_list_j;
                records_v105_j.push_back(record_item_j);
            }
            j["Records"] = records_v105_j;
            break;
        }
        case 106: {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v106,
                                     ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v106, Fmt),
                                     b, offset, length, j);

            unsigned int temp_serv_cell_v106 = j["Serving Cell ID"];
            j["Serving Cell ID"] = temp_serv_cell_v106 & 511;
            int iStartingSubframeNumber_v106 = (temp_serv_cell_v106 >> 9) & 15;
            j["Starting Subframe Number"] = iStartingSubframeNumber_v106;
            j["Starting System Frame Number"] = (temp_serv_cell_v106 >> 13) & 1023;
            j["UE Category"] = (temp_serv_cell_v106 >> 24) & 15;
            j["Num DL HARQ"] = (temp_serv_cell_v106 >> 28) & 15;

            int temp_tm_mode_v106 = j["TM Mode"];
            j["TM Mode"] = (temp_tm_mode_v106 >> 4) & 15;

            unsigned int temp_nir_v106 = j["Nir"];
            j["Nir"] = temp_nir_v106 & 0x7fffff;
            j["Carrier Index"] = (temp_nir_v106 >> 23) & 15;
            const char* carrier_name_v106 = search_name(ValueNameCarrierIndex, ARRAY_SIZE(ValueNameCarrierIndex, ValueName), j["Carrier Index"]);
            j["Carrier Index"] = (carrier_name_v106 ? carrier_name_v106 : "(MI)Unknown");
            int num_record_v106 = (temp_nir_v106 >> 27) & 31;
            j["Number of Records"] = num_record_v106;

            json records_v106_j = json::array();
            for (int k = 0; k < num_record_v106; ++k) {
                json record_item_j;
                int record_consumed = _decode_by_fmt(LtePhyPdschDecodingResult_Record_v106,
                                                     ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v106, Fmt),
                                                     b, offset, length, record_item_j);
                if (record_consumed == 0 && k < num_record_v106 -1) { LOGD("Error decoding record %d for v106", k); break; }
                offset += record_consumed;

                if (record_item_j.find("Subframe Offset") != record_item_j.end() && record_item_j["Subframe Offset"].is_number()){
                    record_item_j["Subframe Number"] = (record_item_j["Subframe Offset"].get<int>() + iStartingSubframeNumber_v106) % 10;
                }

                int temp_harq_v106 = record_item_j["HARQ ID"];
                record_item_j["HARQ ID"] = temp_harq_v106 & 15;
                record_item_j["RNTI Type"] = (temp_harq_v106 >> 4) & 15;
                const char* rnti_name_v106 = search_name(ValueNameRNTIType, ARRAY_SIZE(ValueNameRNTIType, ValueName), record_item_j["RNTI Type"]);
                record_item_j["RNTI Type"] = (rnti_name_v106 ? rnti_name_v106 : "(MI)Unknown");

                int temp_si_v106 = record_item_j["System Information Msg Number"];
                record_item_j["System Information Msg Number"] = temp_si_v106 & 15;
                record_item_j["System Information Mask"] = (temp_si_v106 >> 4) & 4095;

                int temp_harq_log_v106 = record_item_j["HARQ Log Status"];
                record_item_j["HARQ Log Status"] = (temp_harq_log_v106 >> 3) & 3;
                const char* harq_log_name_v106 = search_name(ValueNameHARQLogStatus, ARRAY_SIZE(ValueNameHARQLogStatus, ValueName), record_item_j["HARQ Log Status"]);
                record_item_j["HARQ Log Status"] = (harq_log_name_v106 ? harq_log_name_v106 : "(MI)Unknown");
                record_item_j["Codeword Swap"] = (temp_harq_log_v106 >> 5) & 1;
                record_item_j["Number of Streams"] = (temp_harq_log_v106 >> 6) & 3;

                unsigned int temp_demap_v106 = record_item_j["Demap Sic Status"];
                record_item_j["Demap Sic Status"] = temp_demap_v106 & 0x1fff;
                record_item_j["MVC Status"] = (temp_demap_v106 >> 13) & 0x1;
                record_item_j["MVC Clock Request"] = (temp_demap_v106 >> 14) & 0x3;

                int num_stream_v106 = record_item_j["Number of Streams"];
                json streams_list_j = json::array();
                for (int stream_idx = 0; stream_idx < num_stream_v106; ++stream_idx) {
                    json stream_item_j;
                    int stream_consumed = _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v106,
                                                         ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v106, Fmt),
                                                         b, offset, length, stream_item_j);
                    if (stream_consumed == 0 && stream_idx < num_stream_v106 -1) { LOGD("Error decoding stream %d for record %d v106", stream_idx, k); break; }
                    offset += stream_consumed;

                    unsigned int temp_crc_v106 = stream_item_j["Transport Block CRC"];
                    stream_item_j["Transport Block CRC"] = temp_crc_v106 & 1;
                    const char* crc_name_v106 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), stream_item_j["Transport Block CRC"]);
                    stream_item_j["Transport Block CRC"] = (crc_name_v106 ? crc_name_v106 : "(MI)Unknown");
                    stream_item_j["NDI"] = (temp_crc_v106 >> 1) & 1;
                    stream_item_j["Code Block Size Plus"] = (temp_crc_v106 >> 2) & 8191;
                    int iNumCodeBlockPlusData_v106 = (temp_crc_v106 >> 15) & 0x1f;
                    stream_item_j["Num Code Block Plus Data"] = iNumCodeBlockPlusData_v106;
                    stream_item_j["Num Code Block Plus"] = iNumCodeBlockPlusData_v106 + 1;
                    stream_item_j["Max TDEC Iter"] = (temp_crc_v106 >> 20) & 15;
                    stream_item_j["Retransmission Number"] = (temp_crc_v106 >> 24) & 0x3f;
                    const char* retx_num_name_v106 = search_name(ValueNameNumber, ARRAY_SIZE(ValueNameNumber, ValueName), stream_item_j["Retransmission Number"]);
                    stream_item_j["Retransmission Number"] = (retx_num_name_v106 ? retx_num_name_v106 : "(MI)Unknown");
                    stream_item_j["RVID"] = (temp_crc_v106 >> 30) & 3;

                    unsigned int temp_comp_v106 = stream_item_j["Companding Stats"];
                    stream_item_j["Companding Stats"] = temp_comp_v106 & 3;
                    const char* comp_name_v106 = search_name(ValueNameCompandingStats, ARRAY_SIZE(ValueNameCompandingStats, ValueName), stream_item_j["Companding Stats"]);
                    stream_item_j["Companding Stats"] = (comp_name_v106 ? comp_name_v106 : "(MI)Unknown");
                    stream_item_j["HARQ Combining"] = (temp_comp_v106 >> 2) & 1;
                    const char* harq_comb_name_v106 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), stream_item_j["HARQ Combining"]);
                    stream_item_j["HARQ Combining"] = (harq_comb_name_v106 ? harq_comb_name_v106 : "(MI)Unknown");
                    stream_item_j["Decob TB CRC"] = (temp_comp_v106 >> 3) & 1;
                    stream_item_j["Effective Code Rate Data"] = (temp_comp_v106 >> 4) & 0x3ff;
                    stream_item_j["Num RE"] = (temp_comp_v106 >> 14) & 0x7fff;
                    stream_item_j["Codeword Index"] = (temp_comp_v106 >> 30) & 1;

                    unsigned int temp_llr_v106 = stream_item_j["LLR Scale"];
                    stream_item_j["LLR Scale"] = (temp_llr_v106 >> 6) & 3;

                    int num_energy_metric_v106 = stream_item_j["Num Code Block Plus"];
                    json energy_metric_list_j = json::array();
                    for (int em_idx = 0; em_idx < num_energy_metric_v106; ++em_idx) {
                        json energy_item_j;
                        int em_consumed = _decode_by_fmt(LtePhyPdschDecodingResult_EnergyMetric_v44,
                                                         ARRAY_SIZE(LtePhyPdschDecodingResult_EnergyMetric_v44, Fmt),
                                                         b, offset, length, energy_item_j);
                        if (em_consumed == 0 && em_idx < num_energy_metric_v106 -1) { LOGD("Error decoding energy metric %d for stream %d v106", em_idx, stream_idx); break;}
                        offset += em_consumed;

                        unsigned int temp_em_v106 = energy_item_j["Energy Metric"];
                        energy_item_j["Energy Metric"] = temp_em_v106 & 2097151;
                        energy_item_j["Iteration Number"] = (temp_em_v106 >> 21) & 15;
                        energy_item_j["Code Block CRC Pass"] = (temp_em_v106 >> 25) & 1;
                        const char* cb_crc_name_v106 = search_name(ValueNamePassOrFail, ARRAY_SIZE(ValueNamePassOrFail, ValueName), energy_item_j["Code Block CRC Pass"]);
                        energy_item_j["Code Block CRC Pass"] = (cb_crc_name_v106 ? cb_crc_name_v106 : "(MI)Unknown");
                        energy_item_j["Early Termination"] = (temp_em_v106 >> 26) & 1;
                        const char* et_name_v106 = search_name(ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName), energy_item_j["Early Termination"]);
                        energy_item_j["Early Termination"] = (et_name_v106 ? et_name_v106 : "(MI)Unknown");
                        energy_item_j["HARQ Combine Enable"] = (temp_em_v106 >> 27) & 1;
                        const char* harq_en_name_v106 = search_name(ValueNameEnableOrDisable, ARRAY_SIZE(ValueNameEnableOrDisable, ValueName), energy_item_j["HARQ Combine Enable"]);
                        energy_item_j["HARQ Combine Enable"] = (harq_en_name_v106 ? harq_en_name_v106 : "(MI)Unknown");
                        energy_item_j["Deint Decode Bypass"] = (temp_em_v106 >> 28) & 1;
                        energy_metric_list_j.push_back(energy_item_j);
                    }
                    stream_item_j["Energy Metrics"] = energy_metric_list_j;
                    int num_padded_energy_metrics_v106 = 32;
                    if (num_energy_metric_v106 < num_padded_energy_metrics_v106) {
                        if (offset + (num_padded_energy_metrics_v106 - num_energy_metric_v106) * 4 <= start + length) {
                            offset += (num_padded_energy_metrics_v106 - num_energy_metric_v106) * 4;
                        } else {
                            LOGD("Not enough data to skip padded energy metrics for stream %d v106", stream_idx);
                        }
                    }
                    streams_list_j.push_back(stream_item_j);
                }
                record_item_j["Streams"] = streams_list_j;
                records_v106_j.push_back(record_item_j);
            }
            j["Records"] = records_v106_j;
            break;
        }

        // TODO Есть ещё версии этого лога

        default:
            LOGD("(MI)Unknown LTE PHY PDSCH Decoding Result version: 0x%x", pkt_ver);
            return 0;
    }

    return offset - start;
}