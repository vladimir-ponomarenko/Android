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
#include "lte_phy_serving_cell_meas_res.h"
#include "lte_rrc_mib_message_log_packet.h"
#include "lte_phy_idle_neighbor_cell_meas.h"
#include "lte_phy_system_scan_results.h"
#include "lte_phy_interlog.h"
#include "lte_phy_pusch_tx_report.h"
#include "lte_nb1_ml1_gm_tx_report.h"
#include "lte_pdsch_stat_indication.h"
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
        case LTE_RRC_MIB_Message_Log_Packet: {
            LOGD("payload_decode: LTE_RRC_MIB_Message_Log_Packet\n");
            offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt,
                                     ARRAY_SIZE(LteRrcMibMessageLogPacketFmt, Fmt),
                                     b, offset, length, jj);
            offset += _decode_lte_rrc_mib(b, offset, length, jj);
            j["payload"]["LteRrcMibMessageLogPacket"] = jj;
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
    // return length >=2 && (b[0] == '\x92');  //Yuanjie: optimization for iCellular, avoid unuseful debug msg
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

