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
#include "lte/phy/lte_phy_serving_cell_com_loop.h"
#include "lte/rrc/lte_rrc_mib_message_log_packet.h"
#include "lte/rrc/lte_rrc_serv_cell_info.h"
#include "lte/rrc/lte_rrc_ota.h"
#include "lte/rlc/lte_rlc_dl_stats.h"
#include "lte/rlc/lte_rlc_ul_stats.h"
#include "lte/rlc/lte_rlc_dl_config.h"
#include "lte/rlc/lte_rlc_dl_am_all_pdu.h"
#include "lte/rlc/lte_rlc_ul_config.h"
#include "lte/rlc/lte_rlc_ul_am_all_pdu.h"
#include "lte/phy/lte_phy_idle_neighbor_cell_meas.h"
#include "lte/phy/lte_phy_connected_neighbor_cell_meas.h"
#include "lte/phy/lte_phy_connected_mode_intra_freq_meas_results.h"
#include "lte/phy/lte_phy_rlm_report.h"
#include "lte/phy/lte_phy_system_scan_results.h"
#include "lte/phy/lte_phy_cdrx_events_info.h"
#include "lte/phy/lte_phy_interlog.h"
#include "lte/phy/lte_phy_pucch_csf.h"
#include "lte/phy/lte_phy_pusch_tx_report.h"
#include "lte/phy/lte_phy_pusch_csf.h"
#include "lte/phy/lte_phy_bplmn_cell_request.h"
#include "lte/phy/lte_phy_bplmn_cell_confirm.h"
#include "lte/phy/lte_phy_pdsch_demapper_configuration.h"
#include "lte/phy/lte_phy_pdcch_decoding_result.h"
#include "lte/mac/lte_mac_rach_trigger.h"
#include "lte/mac/lte_mac_configuration.h"
#include "lte/mac/lte_mac_rach_attempt.h"
#include "lte/mac/lte_mac_dl_transportblock.h"
#include "lte/mac/lte_mac_ul_transportblock.h"
#include "lte/mac/lte_mac_ul_txstatistics.h"
#include "lte/mac/lte_mac_ul_bufferstatusinternal.h"
#include "lte/nas/lte_nas_emm_state.h"
#include "lte/lte_pucch_power_control.h"
#include "lte/lte_pusch_power_control.h"
#include "lte/nb1/lte_nb1_ml1_gm_tx_report.h"
#include "lte/lte_pdsch_stat_indication.h"
#include "wcdma_signaling_messages.h"
#include "srch_tng_1x_searcher_dump.h"

using json = nlohmann::json;
using namespace std;

void
payload_decode (const char *b, size_t length, LogPacketType type_id, json &j)
{
    //LOGD("in payload_decode\n");
    int offset = 0;
    json jj;
    switch (type_id) {
        case LTE_PHY_Serving_Cell_Measurement_Result: {
            LOGD("payload_decode: LTE_PHY_Serving_Cell_Measurement_Result\n");
            offset += _decode_by_fmt(LtePhySubpktFmt,
                                     ARRAY_SIZE(LtePhySubpktFmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhySubpkt"]["header"] = jj;
            offset += _decode_lte_phy_subpkt(b, offset, length, j);
            break;
        }
        case LTE_PHY_PUSCH_CSF: {
            LOGD("payload_decode: LTE_PHY_PUSCH_CSF\n");
            offset += _decode_by_fmt(LtePhyPuschCsf_Fmt,
                                     ARRAY_SIZE(LtePhyPuschCsf_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyPuschCsf"] = jj;
            offset += _decode_lte_phy_pusch_csf_payload(b, offset, length, j["payload"]["LtePhyPuschCsf"]);
            break;
        }
        case LTE_PHY_RLM_Report: {
            LOGD("payload_decode: LTE_PHY_RLM_Report\n");
            offset += _decode_by_fmt(LtePhyRlmReport_Fmt,
                                     ARRAY_SIZE(LtePhyRlmReport_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyRlmReport"] = jj;
            offset += _decode_lte_phy_rlm_report_payload(b, offset, length, j["payload"]["LtePhyRlmReport"]);
            break;
        }
        case LTE_PHY_Serving_Cell_COM_Loop: {
            LOGD("payload_decode: LTE_PHY_Serving_Cell_COM_Loop\n");
            offset += _decode_by_fmt(LtePhyServingCellComLoop_Fmt,
                                     ARRAY_SIZE(LtePhyServingCellComLoop_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyServingCellComLoop"] = jj;
            offset += _decode_lte_phy_serving_cell_com_loop_payload(b, offset, length, j["payload"]["LtePhyServingCellComLoop"]);
            break;
        }
        case LTE_PHY_Connected_Mode_Neighbor_Meas_Req_Resp: {
            LOGD("payload_decode: LTE_PHY_Connected_Mode_Neighbor_Measurement\n");
            offset += _decode_by_fmt(LtePhyCncm_Fmt,
                                     ARRAY_SIZE(LtePhyCncm_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyCncm"] = jj;
            offset += _decode_lte_phy_connected_neighbor_cell_meas_payload(b, offset, length, j["payload"]["LtePhyCncm"]);
            break;
        }
        case LTE_PHY_Idle_Neighbor_Cell_Meas: {
            LOGD("payload_decode: LTE_PHY_Idle_Neighbor_Cell_Meas\n");
            offset += _decode_by_fmt(LtePhyIncm_Fmt,
                                     ARRAY_SIZE(LtePhyIncm_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyIncm"] = jj;
            offset += _decode_lte_phy_idle_neighbor_cell_meas_payload(b, offset, length, j["payload"]["LtePhyIncm"]);

            break;
        }
        case LTE_PHY_Connected_Mode_LTE_Intra_Freq_Meas_Results: {
            LOGD("payload_decode: LTE_PHY_Connected_Mode_LTE_Intra_Freq_Meas_Results\n");
            offset += _decode_by_fmt(LtePhyCmlifmrFmt,
                                     ARRAY_SIZE(LtePhyCmlifmrFmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyCmlifmr"] = jj;
            offset += _decode_lte_phy_cmlifmr(b, offset, length, j["payload"]["LtePhyCmlifmr"]);
            break;
        }
        case LTE_RLC_DL_Stats: {
            LOGD("payload_decode: LTE_RLC_DL_Stats\n");
            int header_consumed = _decode_by_fmt(LteRlcDlStats_Fmt,
                                                 ARRAY_SIZE(LteRlcDlStats_Fmt, Fmt),
                                                 b, offset, length, jj);
            if (header_consumed == 0 || jj.find("Version") == jj.end() || jj.find("Num Subpkt") == jj.end()){
                LOGD("Error decoding RLC DL Stats header.");
                j["payload"]["LteRlcDlStats"] = {{"error", "Failed to decode header"}};
            } else {
                offset += header_consumed;
                j["payload"]["LteRlcDlStats"] = jj;
                _decode_lte_rlc_dl_stats_subpkt(b, offset, length - header_consumed, j["payload"]["LteRlcDlStats"]);
            }
            break;
        }
        case LTE_RLC_UL_Stats: {
            LOGD("payload_decode: LTE_RLC_UL_Stats\n");
            int header_consumed = _decode_by_fmt(LteRlcUlStats_Fmt,
                                                 ARRAY_SIZE(LteRlcUlStats_Fmt, Fmt),
                                                 b, offset, length, jj);
            if (header_consumed == 0 || jj.find("Version") == jj.end() || jj.find("Num Subpkt") == jj.end()){
                LOGD("Error decoding RLC UL Stats header.");
                j["payload"]["LteRlcUlStats"] = {{"error", "Failed to decode header"}};
            } else {
                offset += header_consumed;
                j["payload"]["LteRlcUlStats"] = jj;
                _decode_lte_rlc_ul_stats_subpkt(b, offset, length - header_consumed, j["payload"]["LteRlcUlStats"]);
            }
            break;
        }
        case LTE_RLC_DL_Config_Log_Packet: {
            LOGD("payload_decode: LTE_RLC_DL_Config_Log_Packet\n");
            offset += _decode_by_fmt(LteRlcDlConfigLogPacketFmt,
                                     ARRAY_SIZE(LteRlcDlConfigLogPacketFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Num SubPkt") != jj.end()) {
                j["payload"]["LteRlcDlConfigLogPacket"] = jj;
                offset += _decode_lte_rlc_dl_config_log_packet_subpkt(b, offset, length, j["payload"]["LteRlcDlConfigLogPacket"]);
            } else {
                LOGD("Error decoding LTE_RLC_DL_Config_Log_Packet base header.");
                j["payload"]["LteRlcDlConfigLogPacket"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_RLC_UL_Config_Log_Packet: {
            LOGD("payload_decode: LTE_RLC_UL_Config_Log_Packet\n");
            offset += _decode_by_fmt(LteRlcUlConfigLogPacketFmt,
                                     ARRAY_SIZE(LteRlcUlConfigLogPacketFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Num SubPkt") != jj.end()) {
                j["payload"]["LteRlcUlConfigLogPacket"] = jj;
                offset += _decode_lte_rlc_ul_config_log_packet_subpkt(b, offset, length, j["payload"]["LteRlcUlConfigLogPacket"]);
            } else {
                LOGD("Error decoding LTE_RLC_UL_Config_Log_Packet base header.");
                j["payload"]["LteRlcUlConfigLogPacket"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_RLC_UL_AM_All_PDU: {
            LOGD("payload_decode: LTE_RLC_UL_AM_All_PDU\n");
            offset += _decode_by_fmt(LteRlcUlAmAllPduFmt,
                                     ARRAY_SIZE(LteRlcUlAmAllPduFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Number of Subpackets") != jj.end()) {
                j["payload"]["LteRlcUlAmAllPdu"] = jj;
                offset += _decode_lte_rlc_ul_am_all_pdu_subpkt(b, offset, length, j["payload"]["LteRlcUlAmAllPdu"]);
            } else {
                LOGD("Error decoding LTE_RLC_UL_AM_All_PDU base header.");
                j["payload"]["LteRlcUlAmAllPdu"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_RLC_DL_AM_All_PDU: {
            LOGD("payload_decode: LTE_RLC_DL_AM_All_PDU\n");
            offset += _decode_by_fmt(LteRlcDlAmAllPduFmt,
                                     ARRAY_SIZE(LteRlcDlAmAllPduFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Number of Subpackets") != jj.end()) {
                j["payload"]["LteRlcDlAmAllPdu"] = jj;
                offset += _decode_lte_rlc_dl_am_all_pdu_subpkt(b, offset, length, j["payload"]["LteRlcDlAmAllPdu"]);
            } else {
                LOGD("Error decoding LTE_RLC_DL_AM_All_PDU base header.");
                j["payload"]["LteRlcDlAmAllPdu"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_RRC_Serv_Cell_Info_Log_Packet: {
            LOGD("payload_decode: LTE_RRC_Serv_Cell_Info_Log_Packet\n");
            int header_consumed = _decode_by_fmt(LteRrcServCellInfoLogPacketFmt,
                                                 ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt, Fmt),
                                                 b, offset, length, jj);

            if (header_consumed <= 0 || jj.find("Version") == jj.end()) {
                LOGD("Failed to decode LTE RRC Serv Cell Info header or Version key missing.");
                j["payload"]["error"] = "Header decode failed";
                break;
            }
            offset += header_consumed;

            j["payload"]["LteRrcServCellInfo"] = jj;

            int payload_consumed = _decode_lte_rrc_serv_cell_info(b, offset, length, j["payload"]["LteRrcServCellInfo"]);
            if (payload_consumed < 0) {
                LOGD("Error decoding LTE RRC Serv Cell Info payload.");
                j["payload"]["LteRrcServCellInfo"]["error"] = "Payload decode error";
            }

            break;
        }
        case LTE_RRC_MIB_Message_Log_Packet: {
            LOGD("payload_decode: LTE_RRC_MIB_Message_Log_Packet\n");
            offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt,
                                     ARRAY_SIZE(LteRrcMibMessageLogPacketFmt, Fmt),
                                     b, offset, length, jj);
            offset += _decode_lte_rrc_mib(b, offset, length, jj);
            j["payload"]["LteRrcMibMessageLogPacket"] = jj;
            break;
        }
        case LTE_RRC_OTA_Packet: {
            LOGD("payload_decode: LTE_RRC_OTA_Packet\n");
            int header_consumed = _decode_by_fmt(LteRrcOtaPacketFmt,
                                                 ARRAY_SIZE(LteRrcOtaPacketFmt, Fmt),
                                                 b, offset, length, jj);

            if (header_consumed == 0 || jj.find("Pkt Version") == jj.end()) {
                LOGD("Error decoding LTE_RRC_OTA_Packet base header.");
                j["payload"]["LteRrcOtaPacket"] = {{"error", "Failed to decode base header"}};
            } else {
                offset += header_consumed;
                j["payload"]["LteRrcOtaPacket"] = jj;
                int payload_consumed = _decode_lte_rrc_ota(b, offset, length - header_consumed, j["payload"]["LteRrcOtaPacket"]);
            }
            break;
        }
        case LTE_PDSCH_Stat_Indication: {
            LOGD("payload_decode: LTE_PDSCH_Stat_Indication\n");
            offset += _decode_by_fmt(LtePdschStatIndication_Fmt,
                                     ARRAY_SIZE(LtePdschStatIndication_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePdschStatIndication"] = jj;
            offset += _decode_lte_pdsch_stat_indication_payload(b, offset, length, j["payload"]["LtePdschStatIndication"]);
            break;
        }
        case LTE_PHY_System_Scan_Results: {
            LOGD("payload_decode: LTE_PHY_System_Scan_Results\n");
            offset += _decode_by_fmt(LtePhySystemScanResults_Fmt,
                                     ARRAY_SIZE(LtePhySystemScanResults_Fmt, Fmt),
                                     b, offset, length, jj);

            j["payload"]["LtePhySysScanResults"] = jj;

            offset += _decode_lte_phy_system_scan_results_payload(
                    b, offset,
                    length,
                    j["payload"]["LtePhySysScanResults"]
            );
            break;
        }
        case LTE_PHY_PUSCH_Tx_Report: {
            LOGD("payload_decode: LTE_PHY_PUSCH_Tx_Report\n");
            offset += _decode_by_fmt(LtePhyPuschTxReport_Fmt,
                                     ARRAY_SIZE(LtePhyPuschTxReport_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyPuschTxReport"] = jj;
            offset += _decode_lte_phy_pusch_tx_report_payload(b, offset, length, j["payload"]["LtePhyPuschTxReport"]);
            break;
        }
        case LTE_PHY_Inter_Freq_Log:
        {
            LOGD("payload_decode: LTE_PHY_InterFreq_Log\n");

            offset += _decode_by_fmt(LtePhyInterlogFmt,
                                     ARRAY_SIZE(LtePhyInterlogFmt, Fmt),
                                     b, offset, length, jj);

            j["payload"]["LtePhyInterFreqLog"] = jj;
            offset += _decode_lte_phy_interlog(b, offset, length, j["payload"]["LtePhyInterFreqLog"]);
            break;
        }
        case LTE_PHY_CDRX_Events_Info: {
            LOGD("payload_decode: LTE_PHY_CDRX_Events_Info\n");
            offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Fmt,
                                     ARRAY_SIZE(LtePhyCdrxEventsInfo_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyCdrxEventsInfo"] = jj;
            offset += _decode_lte_phy_cdrx_events_info_payload(b, offset, length, j["payload"]["LtePhyCdrxEventsInfo"]);
            break;
        }
        case LTE_PHY_BPLMN_Cell_Request: {
            LOGD("payload_decode: LTE_PHY_BPLMN_Cell_Request\n");
            offset += _decode_by_fmt(LtePhyBplmnCellRequest_Fmt,
                                     ARRAY_SIZE(LtePhyBplmnCellRequest_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyBplmnCellRequest"] = jj;
            offset += _decode_lte_phy_bplmn_cell_request_payload(b, offset, length, j["payload"]["LtePhyBplmnCellRequest"]);
            break;
        }
        case LTE_PHY_BPLMN_Cell_Confirm: {
            LOGD("payload_decode: LTE_PHY_BPLMN_Cell_Confirm\n");
            offset += _decode_by_fmt(LtePhyBplmnCellConfirm_Fmt,
                                     ARRAY_SIZE(LtePhyBplmnCellConfirm_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhyBplmnCellConfirm"] = jj;
            offset += _decode_lte_phy_bplmn_cell_confirm_payload(b, offset, length, j["payload"]["LtePhyBplmnCellConfirm"]);
            break;
        }
        case LTE_PUCCH_Power_Control: {
            LOGD("payload_decode: LTE_PUCCH_Power_Control\n");
            offset += _decode_by_fmt(LtePucchPowerControl_Fmt,
                                     ARRAY_SIZE(LtePucchPowerControl_Fmt, Fmt),
                                     b, offset, length, jj);
            offset += _decode_lte_pucch_power_control_payload(b, offset, length, jj);
            j["payload"]["LtePucchPowerControl"] = jj;
            break;
        }
        case LTE_PUSCH_Power_Control: {
            LOGD("payload_decode: LTE_PUSCH_Power_Control\n");

            offset += _decode_by_fmt(LtePuschPowerControl_Fmt,
                                     ARRAY_SIZE(LtePuschPowerControl_Fmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") == jj.end()){
                LOGD("Error decoding PUSCH Power Control: Version field missing.");
                j["payload"]["LtePuschPowerControl"] = {{"error", "Version field missing"}};
            } else {
                offset += _decode_lte_pusch_power_control_payload(b + offset, 0, length - offset, jj);
                j["payload"]["LtePuschPowerControl"] = jj;
            }
            break;
        }
        case LTE_PHY_PUCCH_CSF: {
            LOGD("payload_decode: LTE_PHY_PUCCH_CSF\n");
            offset += _decode_by_fmt(LtePhyPucchCsf_Fmt,
                                     ARRAY_SIZE(LtePhyPucchCsf_Fmt, Fmt),
                                     b, offset, length, jj);
            offset += _decode_lte_phy_pucch_csf_payload(b, offset, length, jj);
            j["payload"]["LtePhyPucchCsf"] = jj;
            break;
        }
        case LTE_PHY_PDSCH_Demapper_Configuration: {
            LOGD("payload_decode: LTE_PHY_PDSCH_Demapper_Configuration\n");

            int header_consumed = _decode_by_fmt(LtePhyPdschDemapperConfigFmt,
                                                 ARRAY_SIZE(LtePhyPdschDemapperConfigFmt, Fmt),
                                                 b, offset, length, jj);
            if (header_consumed == 0 || jj.find("Version") == jj.end()) {
                LOGD("Error decoding PDSCH Demapper Config: Version field missing or decode failed.");
                j["payload"]["LtePhyPdschDemapperConfig"] = {{"error", "Version field missing or decode failed"}};
            } else {
                offset += header_consumed;
                int payload_consumed = _decode_lte_phy_pdsch_demapper_config(
                        b,
                        offset,
                        length - header_consumed,
                        jj);
                offset += payload_consumed;
                j["payload"]["LtePhyPdschDemapperConfig"] = jj;
            }
            break;
        }
        case LTE_PHY_PDCCH_Decoding_Result: {
            LOGD("payload_decode: LTE_PHY_PDCCH_Decoding_Result\n");
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Fmt,
                                     ARRAY_SIZE(LtePhyPdcchDecodingResult_Fmt, Fmt),
                                     b, offset, length, jj);
            offset += _decode_lte_phy_pdcch_decoding_result_payload(b, offset, length, jj);
            j["payload"]["LtePhyPdcchDecodingResult"] = jj;
            break;
        }
        case LTE_NAS_EMM_State: {
            LOGD("payload_decode: LTE_NAS_EMM_State\n");

            int header_consumed = _decode_by_fmt(LteNasEmmStateFmt,
                                                 ARRAY_SIZE(LteNasEmmStateFmt, Fmt),
                                                 b, offset, length, jj);
            if (header_consumed == 0 || jj.find("Version") == jj.end()) {
                LOGD("Error decoding LTE_NAS_EMM_State header.");
                j["payload"]["LteNasEmmState"] = {{"error", "Failed to decode header"}};
            } else {
                j["payload"]["LteNasEmmState"] = jj;
                offset += header_consumed;
                int payload_consumed = _decode_lte_nas_emm_state(
                        b,
                        offset,
                        length - header_consumed,
                        j["payload"]["LteNasEmmState"]);
                offset += payload_consumed;
            }
            break;
        }
        case LTE_MAC_Rach_Attempt: {
            LOGD("payload_decode: LTE_MAC_Rach_Attempt\n");
            int initial_offset_for_this_packet = offset;
            offset += _decode_by_fmt(LteMacRachAttempt_Fmt,
                                     ARRAY_SIZE(LteMacRachAttempt_Fmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj["Version"].is_number() &&
                jj.find("Number of Subpackets") != jj.end() && jj["Number of Subpackets"].is_number()) {
                j["payload"]["LteMacRachAttempt"] = jj;
                int subpackets_consumed = _decode_lte_mac_rach_attempt_subpkt(
                        b + offset,
                        0,
                        length - offset,
                        j["payload"]["LteMacRachAttempt"]
                );
                offset += subpackets_consumed;
            } else {
                LOGD("Error decoding LTE_MAC_Rach_Attempt base header or missing essential fields.");
                if (jj.empty()) {
                    j["payload"]["LteMacRachAttempt"] = {{"error", "Failed to decode base header (empty)"}};
                } else {
                    j["payload"]["LteMacRachAttempt"] = jj;
                    j["payload"]["LteMacRachAttempt"]["error"] = "Base header missing Version or Number of Subpackets";
                }
            }
            break;
        }
        case LTE_MAC_DL_Transport_Block: {
            LOGD("payload_decode: LTE_MAC_DL_Transport_Block\n");
            int initial_offset_for_this_packet = offset;
            offset += _decode_by_fmt(LteMacDLTransportBlockFmt,
                                     ARRAY_SIZE(LteMacDLTransportBlockFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Num SubPkt") != jj.end()) {
                j["payload"]["LteMacDlTransportBlock"] = jj;
                offset += _decode_lte_mac_dl_transportblock_subpkt(b, offset, length, j["payload"]["LteMacDlTransportBlock"]);
            } else {
                LOGD("Error decoding LTE_MAC_DL_Transport_Block base header.");
                j["payload"]["LteMacDlTransportBlock"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_MAC_UL_Transport_Block: {
            LOGD("payload_decode: LTE_MAC_UL_Transport_Block\n");
            offset += _decode_by_fmt(LteMacULTransportBlockFmt,
                                     ARRAY_SIZE(LteMacULTransportBlockFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Num SubPkt") != jj.end()) {
                j["payload"]["LteMacUlTransportBlock"] = jj;
                offset += _decode_lte_mac_ul_transportblock_subpkt(b, offset, length, j["payload"]["LteMacUlTransportBlock"]);
            } else {
                LOGD("Error decoding LTE_MAC_UL_Transport_Block base header.");
                j["payload"]["LteMacUlTransportBlock"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_MAC_Rach_Trigger: {
            LOGD("payload_decode: LTE_MAC_Rach_Trigger\n");
            offset += _decode_by_fmt(LteMacRachTriggerFmt,
                                     ARRAY_SIZE(LteMacRachTriggerFmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LteMacRachTrigger"] = jj;
            offset += _decode_lte_mac_rach_trigger_subpkt(b, offset, length, j["payload"]["LteMacRachTrigger"]);
            break;
        }
        case LTE_MAC_UL_Tx_Statistics: {
            LOGD("payload_decode: LTE_MAC_UL_Tx_Statistics\n");
            offset += _decode_by_fmt(LteMacULTxStatisticsFmt,
                                     ARRAY_SIZE(LteMacULTxStatisticsFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Num SubPkt") != jj.end()) {
                j["payload"]["LteMacUlTxStatistics"] = jj;
                offset += _decode_lte_mac_ul_txstatistics_subpkt(b, offset, length, j["payload"]["LteMacUlTxStatistics"]);
            } else {
                LOGD("Error decoding LTE_MAC_UL_Tx_Statistics base header.");
                j["payload"]["LteMacUlTxStatistics"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_MAC_Configuration: {
            LOGD("payload_decode: LTE_MAC_Configuration\n");
            int header_consumed = _decode_by_fmt(LteMacConfigurationFmt,
                                                 ARRAY_SIZE(LteMacConfigurationFmt, Fmt),
                                                 b, offset, length, jj);
            if (header_consumed == 0 || jj.find("Version") == jj.end() || jj.find("Num SubPkt") == jj.end()) {
                LOGD("Error decoding LTE_MAC_Configuration header.");
                j["payload"]["LteMacConfiguration"] = {{"error", "Failed to decode header"}};
            } else {
                j["payload"]["LteMacConfiguration"] = jj;
                offset += header_consumed;
                int subpackets_consumed = _decode_lte_mac_configuration_subpkt(
                        b,
                        offset,
                        length - header_consumed,
                        j["payload"]["LteMacConfiguration"]);
                offset += subpackets_consumed;
            }
            break;
        }
        case LTE_MAC_UL_Buffer_Status_Internal: {
            LOGD("payload_decode: LTE_MAC_UL_Buffer_Status_Internal\n");
            offset += _decode_by_fmt(LteMacULBufferStatusInternalFmt,
                                     ARRAY_SIZE(LteMacULBufferStatusInternalFmt, Fmt),
                                     b, offset, length, jj);
            if (jj.find("Version") != jj.end() && jj.find("Num SubPkt") != jj.end()) {
                j["payload"]["LteMacUlBufferStatusInternal"] = jj;
                offset += _decode_lte_mac_ul_bufferstatusinternal_subpkt(b, offset, length, j["payload"]["LteMacUlBufferStatusInternal"]);
            } else {
                LOGD("Error decoding LTE_MAC_UL_Buffer_Status_Internal base header.");
                j["payload"]["LteMacUlBufferStatusInternal"] = {{"error", "Failed to decode base header"}};
            }
            break;
        }
        case LTE_NB1_ML1_GM_TX_Report: {
            LOGD("payload_decode: LTE_NB1_ML1_GM_TX_Report\n");
            offset += _decode_by_fmt(LteNb1Ml1GmTxReport,
                                     ARRAY_SIZE(LteNb1Ml1GmTxReport, Fmt),
                                     b, offset, length, jj);
            offset += _decode_lte_nb1_ml1_gm_tx_report_payload(b, offset, length, j["payload"]["LteNb1Ml1GmTxReport"]);
            j["payload"]["LteNb1Ml1GmTxReport"] = jj;
            break;
        }
        case GSM_RR_Cell_Reselection_Meas: {
            LOGD("payload_decode: GSM_RR_Cell_Reselection_Meas\n");
            offset += _decode_by_fmt(GsmRrCellResMeas_Fmt,
                                     ARRAY_SIZE(GsmRrCellResMeas_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["GsmRrCellResMeas"] = jj;
            offset += _decode_gsm_rcrm_payload(b, offset, length, j);
            break;
        }
        case WCDMA_CELL_ID: {
            LOGD("payload_decode: WCDMA_CELL_ID\n");
            offset += _decode_by_fmt(WcdmaCellIdFmt,
                                     ARRAY_SIZE(WcdmaCellIdFmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["WcdmaRrcServCellInfo"] = jj;
            break;
        }
        case WCDMA_Signaling_Messages: {
            LOGD("payload_decode: WCDMA_Signaling_Messages\n");
            offset += _decode_by_fmt(WcdmaSignalingMessagesFmt,
                                     ARRAY_SIZE(WcdmaSignalingMessagesFmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["WcdmaSigMsgs"] = jj;
            offset += _decode_wcdma_signaling_messages(b, offset, length, j);
            break;
        }
        case Srch_TNG_1x_Searcher_Dump: {
            LOGD("payload_decode: Srch_TNG_1x_Searcher_Dump\n");
            offset += _decode_by_fmt(SrchTng1xsd_Fmt,
                                     ARRAY_SIZE(SrchTng1xsd_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["SrchTng1xsd"] = jj;
            offset += _decode_srch_tng_1xsd_payload(b, offset, length, j["payload"]["SrchTng1xsd"]);
            break;
        }
        default: {
            LOGD("unknown payload type\n");
        }
    };
}



bool
is_log_packet (const char *b, size_t length) {
    return length >= 2 && b[0] == '\x10';
}

bool
is_debug_packet (const char *b, size_t length) {
    return length >=2 && (b[0] ==  '\x79' || b[0] == '\x92');
    // return length >=2 && (b[0] == '\x92');
    // return length >=2 && (b[0] ==  '\x79');
}


string decode_log_packet (const char *b, size_t length, bool skip_decoding) {

    int offset = 0;
    json j;

    offset += _decode_by_fmt(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                             b, offset, length, j["header"]);
    unsigned int type_id = j["header"]["type_id"];

    bool typeid_found = false;
    for (int i = 0; i < LogPacketTypeID_To_Name.size(); i++) {
        if(LogPacketTypeID_To_Name[i].val == type_id){
            typeid_found = true;
            break;
        }
    }

    if(typeid_found){
        payload_decode(b+offset, length-offset, (LogPacketType)type_id, j);
    }else{
        j["payload"]["typeid_unknown"] = 1;
    }

    return j.dump();
}

