//
// Created by Vladimir Ponomarenko on 22.05.2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "consts.h"
#include "utils.h"
#include "log_packet_utils.h"

using json = nlohmann::json;
using namespace std;





static int _decode_lte_rlc_ul_am_all_pdu_subpkt(const char* b, int offset, size_t length, json& j) {
    int start = offset;
    int pkt_ver = 0;
    int n_subpkt = 0;

    if (j.find("Version") != j.end() && j["Version"].is_number()) {
        pkt_ver = j["Version"].get<int>();
    } else {
        LOGD("LTE_RLC_UL_AM_All_PDU: Missing or invalid 'Version'");
        j["error"] = "Missing Version in main header";
        return 0;
    }
    if (j.find("Number of Subpackets") != j.end() && j["Number of Subpackets"].is_number()) {
        n_subpkt = j["Number of Subpackets"].get<int>();
    } else {
        LOGD("LTE_RLC_UL_AM_All_PDU: Missing or invalid 'Number of Subpackets'");
        j["error"] = "Missing Number of Subpackets in main header";
        return 0;
    }

   // LOGD("Decoding LTE_RLC_UL_AM_All_PDU with version: %d, num_subpackets: %d", pkt_ver, n_subpkt);

    switch (pkt_ver) {
        case 1: {
            json subpackets_j = json::array();
            for (int i = 0; i < n_subpkt; ++i) {
                json subpkt_j;
                int start_subpkt = offset;

                int header_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_SubpktHeader,
                                                     ARRAY_SIZE(LteRlcUlAmAllPdu_SubpktHeader, Fmt),
                                                     b, offset, length, subpkt_j);

                int subpkt_id = -1, subpkt_payload_ver = -1, subpkt_size = 0;
                bool header_ok = false;

                if (header_consumed > 0 &&
                    subpkt_j.find("Subpacket ID") != subpkt_j.end() && subpkt_j["Subpacket ID"].is_number() &&
                    subpkt_j.find("Subpacket Version") != subpkt_j.end() && subpkt_j["Subpacket Version"].is_number() &&
                    subpkt_j.find("Subpacket Size") != subpkt_j.end() && subpkt_j["Subpacket Size"].is_number()) {
                    subpkt_id = subpkt_j["Subpacket ID"].get<int>();
                    subpkt_payload_ver = subpkt_j["Subpacket Version"].get<int>();
                    subpkt_size = subpkt_j["Subpacket Size"].get<int>();
                    if (subpkt_size >= header_consumed && start_subpkt + subpkt_size <= start + length) {
                        header_ok = true;
                    }
                }

                if (!header_ok) {
                    LOGD("LTE_RLC_UL_AM_All_PDU: Error decoding subpacket %d header or missing fields/invalid size.", i);
                    if (subpkt_j.find("Subpacket Size") != subpkt_j.end() && subpkt_j["Subpacket Size"].is_number()) {
                        int skip_size_check = subpkt_j["Subpacket Size"].get<int>();
                        if (start_subpkt + skip_size_check <= start + length && skip_size_check > 0) {
                            offset = start_subpkt + skip_size_check;
                        } else { return offset - start; }
                    } else { return offset - start; }
                    continue;
                }
                offset += header_consumed;
                int payload_offset_start = offset;
                int payload_consumed_total = 0;


                if (subpkt_id == 70) {
                    if (subpkt_payload_ver == 3) {
                        payload_consumed_total += _decode_by_fmt(LteRlcUlAmAllPdu_SubpktPayload,
                                                                 ARRAY_SIZE(LteRlcUlAmAllPdu_SubpktPayload, Fmt),
                                                                 b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset = payload_offset_start + payload_consumed_total;

                        int rb_cfg_idx = subpkt_j["RB Cfg Idx"];
                        const char* rb_mode_name = search_name(LteRlcUlAmAllPdu_Subpkt_RBMode, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_RBMode, ValueName), subpkt_j["RB Mode"]);
                        subpkt_j["RB Mode"] = (rb_mode_name ? rb_mode_name : "(MI)Unknown");

                        int enabled_pdu = subpkt_j["Enabled PDU Log Packets"];
                        std::string strEnabledPDU = "";
                        if ((enabled_pdu) & (1 << 1)) strEnabledPDU += "RLCUL Config (0xB091), ";
                        if ((enabled_pdu) & (1 << 2)) strEnabledPDU += "RLCUL AM ALL PDU (0xB092), ";
                        if ((enabled_pdu) & (1 << 3)) strEnabledPDU += "RLCUL AM CONTROL PDU (0xB093), ";
                        if ((enabled_pdu) & (1 << 4)) strEnabledPDU += "RLCUL AM POLLING PDU (0xB094), ";
                        if ((enabled_pdu) & (1 << 5)) strEnabledPDU += "RLCUL AM SIGNALING PDU (0xB095), ";
                        if ((enabled_pdu) & (1 << 6)) strEnabledPDU += "RLCUL UM DATA PDU (0xB096), ";
                        if ((enabled_pdu) & (1 << 7)) strEnabledPDU += "RLCUL STATISTICS (0xB097), ";
                        if ((enabled_pdu) & (1 << 8)) strEnabledPDU += "RLCUL AM STATE (0xB098), ";
                        if ((enabled_pdu) & (1 << 9)) strEnabledPDU += "RLCUL UM STATE (0xB099), ";
                        if (strEnabledPDU.length() > 2) {
                            strEnabledPDU = strEnabledPDU.substr(0, strEnabledPDU.length() - 2);
                            strEnabledPDU += ".";
                        }
                        subpkt_j["Enabled PDU Log Packets"] = strEnabledPDU;

                        int n_pdu = subpkt_j["Number of PDUs"];
                        json pdu_list_j = json::array();
                        int current_pdu_offset = offset;

                        for (int pdu_idx = 0; pdu_idx < n_pdu; ++pdu_idx) {
                            json pdu_item_j;
                            int pdu_basic_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_Basic,
                                                                    ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_Basic, Fmt),
                                                                    b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);

                            if (pdu_basic_consumed == 0) { LOGD("Error decoding PDU basic part for PDU %d", pdu_idx); break;}
                            current_pdu_offset += pdu_basic_consumed;

                            pdu_item_j["rb_cfg_idx"] = rb_cfg_idx;
                            int DCLookAhead = pdu_item_j["D/C LookAhead"];
                            int iNonDecodeFN = pdu_item_j["sys_fn"];
                            int iLoggedBytes = pdu_item_j["logged_bytes"];
                            iLoggedBytes -= 2;

                            const unsigned int SFN_RSHIFT_PDU = 4, SFN_MASK_PDU = (1 << 12) - 1;
                            const unsigned int SUBFRAME_RSHIFT_PDU = 0, SUBFRAME_MASK_PDU = (1 << 4) - 1;
                            pdu_item_j["sys_fn"] = (iNonDecodeFN >> SFN_RSHIFT_PDU) & SFN_MASK_PDU;
                            pdu_item_j["sub_fn"] = (iNonDecodeFN >> SUBFRAME_RSHIFT_PDU) & SUBFRAME_MASK_PDU;

                            int iNonDecodeSN = pdu_item_j["SN"];

                            if (DCLookAhead < 16) {
                                pdu_item_j["PDU TYPE"] = "RLCUL CTRL";
                                std::string strAckSN = "ACK_SN = ";
                                int iAckSN = DCLookAhead * 64 + iNonDecodeSN / 4;
                                strAckSN += SSTR(iAckSN);
                                int iHeadFromPadding_ctrl = (iNonDecodeSN & 1) * 512;
                                pdu_item_j["SN"] = strAckSN;

                                int ctrl_pdu_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_Control,
                                                                       ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_Control, Fmt),
                                                                       b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);
                                current_pdu_offset += ctrl_pdu_consumed;
                                pdu_item_j["cpt"] = "STATUS(0)";

                                if (iLoggedBytes > 0) {
                                    int numNack = static_cast<int>(floor(iLoggedBytes / 1.5));
                                    int iAllign = 0;
                                    int iHeadFromAllign = 0;
                                    json nack_list_j = json::array();
                                    for (int indexNack = 0; indexNack < numNack; ++indexNack) {
                                        json nack_item_j;
                                        int nack_consumed_bytes = 0;
                                        if (iAllign == 0) {
                                            iAllign = 1;
                                            nack_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN, Fmt), b, current_pdu_offset, length - (current_pdu_offset-start), nack_item_j);
                                            iLoggedBytes -= 2;
                                            int iNonDecodeNACK = nack_item_j["NACK_SN"];
                                            int iPart3 = iNonDecodeNACK / 4096;
                                            int iPart4 = (iNonDecodeNACK - iPart3 * 4096) / 256;
                                            int iPart1 = (iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256) / 16;
                                            int iPart2 = iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                            nack_item_j["NACK_SN"] = iPart1 * 32 + iPart2 * 2 + iPart3 / 8 + iHeadFromPadding_ctrl;
                                            iHeadFromAllign = iPart4 + (iPart3 & 1) * 16;
                                            if ((iPart3 & 2) == 2) indexNack = numNack - 1;
                                        } else {
                                            iAllign = 0;
                                            nack_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING, Fmt), b, current_pdu_offset, length - (current_pdu_offset-start), nack_item_j);
                                            iLoggedBytes -= 1;
                                            int iNonDecodeNACK = nack_item_j["NACK_SN"];
                                            nack_item_j["NACK_SN"] = iHeadFromAllign * 32 + iNonDecodeNACK / 8;
                                            if ((iNonDecodeNACK & 2) == 2) indexNack = numNack - 1;
                                            iHeadFromPadding_ctrl = (iNonDecodeNACK & 1) * 512;
                                        }
                                        current_pdu_offset += nack_consumed_bytes;
                                        nack_list_j.push_back(nack_item_j);
                                        if (indexNack >= numNack -1) break;
                                    }
                                    pdu_item_j["RLC CTRL NACK"] = nack_list_j;
                                }
                            } else {
                                pdu_item_j["PDU TYPE"] = "RLCUL DATA";
                                std::string strRF = ((DCLookAhead) & (1 << 6)) ? "1" : "0";
                                std::string strP  = ((DCLookAhead) & (1 << 5)) ? "1" : "0";
                                std::string strFI = "";
                                strFI += ((DCLookAhead) & (1 << 4)) ? "1" : "0";
                                strFI += ((DCLookAhead) & (1 << 3)) ? "1" : "0";
                                std::string strE  = ((DCLookAhead) & (1 << 2)) ? "1" : "0";

                                if ((DCLookAhead) & (1 << 1)) iNonDecodeSN += 512;
                                if ((DCLookAhead) & 1) iNonDecodeSN += 256;
                                pdu_item_j["SN"] = iNonDecodeSN;

                                int data_pdu_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_DATA,
                                                                       ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_DATA, Fmt),
                                                                       b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);
                                current_pdu_offset += data_pdu_consumed;

                                pdu_item_j["RF"] = strRF;
                                pdu_item_j["P"] = strP;
                                pdu_item_j["FI"] = strFI;
                                pdu_item_j["E"] = strE;

                                if (strRF == "1") {
                                    iLoggedBytes -= 2;
                                    int lsf_so_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO,
                                                                         ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO, Fmt),
                                                                         b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);
                                    current_pdu_offset += lsf_so_consumed;
                                    int temp_lsf_so = pdu_item_j["LSF"];
                                    pdu_item_j["LSF"] = temp_lsf_so >> 7;
                                    pdu_item_j["SO"] = ((temp_lsf_so & 127) * 256) + pdu_item_j["SO"].get<int>();
                                }

                                if (strE == "1") {
                                    int numLI = static_cast<int>(floor(iLoggedBytes / 1.5));
                                    iLoggedBytes = 0;
                                    int iAllign = 0;
                                    int iHeadFromAllign = 0;
                                    json li_list_j = json::array();
                                    for (int indexLI = 0; indexLI < numLI; ++indexLI) {
                                        json li_item_j;
                                        int li_consumed_bytes = 0;
                                        if (iAllign == 0) {
                                            iAllign = 1;
                                            li_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN, Fmt), b, current_pdu_offset, length-(current_pdu_offset-start), li_item_j);
                                            int iNonDecodeLI = li_item_j["LI"];
                                            int iPart3 = iNonDecodeLI / 4096;
                                            int iPart4 = (iNonDecodeLI - iPart3 * 4096) / 256;
                                            int iPart1 = (iNonDecodeLI - iPart3 * 4096 - iPart4 * 256) / 16;
                                            int iPart2 = iNonDecodeLI - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                            li_item_j["LI"] = (iPart1 % 8) * 256 + iPart2 * 16 + iPart3;
                                            iHeadFromAllign = (iPart4 % 8) * 256;
                                        } else {
                                            iAllign = 0;
                                            li_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING, Fmt), b, current_pdu_offset, length-(current_pdu_offset-start), li_item_j);
                                            int iNonDecodeLI = li_item_j["LI"];
                                            li_item_j["LI"] = iHeadFromAllign + iNonDecodeLI;
                                        }
                                        current_pdu_offset += li_consumed_bytes;
                                        li_list_j.push_back(li_item_j);
                                    }
                                    pdu_item_j["RLC DATA LI"] = li_list_j;
                                }
                            }

                            if (iLoggedBytes > 0 && current_pdu_offset + iLoggedBytes <= start + length) {
                                current_pdu_offset += iLoggedBytes;
                            } else if (iLoggedBytes < 0) {
                                LOGD("RLC UL PDU: iLoggedBytes is negative (%d), likely error in parsing LIs/NACKs", iLoggedBytes);
                            }

                            pdu_list_j.push_back(pdu_item_j);
                        }
                        subpkt_j["RLCUL PDUs"] = pdu_list_j;
                        offset = current_pdu_offset;

                    } else if (subpkt_id == 70 && subpkt_payload_ver == 4) {

                        payload_consumed_total += _decode_by_fmt(LteRlcUlAmAllPdu_SubpktPayload_v4,
                                                                 ARRAY_SIZE(LteRlcUlAmAllPdu_SubpktPayload_v4, Fmt),
                                                                 b, payload_offset_start, length - (payload_offset_start - start), subpkt_j);
                        offset = payload_offset_start + payload_consumed_total;

                        int rb_cfg_idx = subpkt_j["RB Cfg Idx"];
                        const char* rb_mode_name = search_name(LteRlcUlAmAllPdu_Subpkt_RBMode, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_RBMode, ValueName), subpkt_j["RB Mode"]);
                        subpkt_j["RB Mode"] = (rb_mode_name ? rb_mode_name : "(MI)Unknown");

                        int enabled_pdu = subpkt_j["Enabled PDU Log Packets"];
                        std::string strEnabledPDU = "";
                        if ((enabled_pdu) & (1 << 1)) strEnabledPDU += "RLCUL Config (0xB091), ";
                        if ((enabled_pdu) & (1 << 2)) strEnabledPDU += "RLCUL AM ALL PDU (0xB092), ";
                        if ((enabled_pdu) & (1 << 3)) strEnabledPDU += "RLCUL AM CONTROL PDU (0xB093), ";
                        if ((enabled_pdu) & (1 << 4)) strEnabledPDU += "RLCUL AM POLLING PDU (0xB094), ";
                        if ((enabled_pdu) & (1 << 5)) strEnabledPDU += "RLCUL AM SIGNALING PDU (0xB095), ";
                        if ((enabled_pdu) & (1 << 6)) strEnabledPDU += "RLCUL UM DATA PDU (0xB096), ";
                        if ((enabled_pdu) & (1 << 7)) strEnabledPDU += "RLCUL STATISTICS (0xB097), ";
                        if ((enabled_pdu) & (1 << 8)) strEnabledPDU += "RLCUL AM STATE (0xB098), ";
                        if ((enabled_pdu) & (1 << 9)) strEnabledPDU += "RLCUL UM STATE (0xB099), ";
                        if (strEnabledPDU.length() > 2) {
                            strEnabledPDU = strEnabledPDU.substr(0, strEnabledPDU.length() - 2);
                            strEnabledPDU += ".";
                        }
                        subpkt_j["Enabled PDU Log Packets"] = strEnabledPDU;

                        int n_pdu = subpkt_j["Number of PDUs"];
                        json pdu_list_j = json::array();
                        int current_pdu_offset = offset;

                        for (int pdu_idx = 0; pdu_idx < n_pdu; ++pdu_idx) {
                            json pdu_item_j;
                            int pdu_basic_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_Basic_v4,
                                                                    ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_Basic_v4, Fmt),
                                                                    b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);
                            if (pdu_basic_consumed == 0) { LOGD("Error decoding PDU basic_v4 part for PDU %d", pdu_idx); break;}
                            current_pdu_offset += pdu_basic_consumed;

                            pdu_item_j["rb_cfg_idx"] = rb_cfg_idx;
                            // pdu_item_j["SFN"] = pdu_item_j["sys_fn"];

                            int DCLookAhead = pdu_item_j["D/C LookAhead"];
                            int iNonDecodeFN = pdu_item_j["sys_fn"];
                            int iLoggedBytes = pdu_item_j["logged_bytes"];
                            iLoggedBytes -= 2;

                            const unsigned int SFN_RSHIFT_PDU_V4 = 4, SFN_MASK_PDU_V4 = (1 << 12) - 1;
                            const unsigned int SUBFRAME_RSHIFT_PDU_V4 = 0, SUBFRAME_MASK_PDU_V4 = (1 << 4) - 1;
                            pdu_item_j["sys_fn_combined"] = iNonDecodeFN;
                            pdu_item_j["sys_fn"] = (iNonDecodeFN >> SFN_RSHIFT_PDU_V4) & SFN_MASK_PDU_V4;
                            pdu_item_j["sub_fn"] = (iNonDecodeFN >> SUBFRAME_RSHIFT_PDU_V4) & SUBFRAME_MASK_PDU_V4;


                            int iNonDecodeSN = pdu_item_j["SN"];

                            if (DCLookAhead < 16) {
                                pdu_item_j["PDU TYPE"] = "RLCUL CTRL";
                                std::string strAckSN = "ACK_SN = ";
                                int iAckSN = DCLookAhead * 64 + iNonDecodeSN / 4;
                                strAckSN += SSTR(iAckSN);
                                int iHeadFromPadding_ctrl = (iNonDecodeSN & 1) * 512;
                                pdu_item_j["SN"] = strAckSN;

                                int ctrl_pdu_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_Control_v4,
                                                                       ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_Control_v4, Fmt),
                                                                       b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);
                                current_pdu_offset += ctrl_pdu_consumed;
                                pdu_item_j["cpt"] = "STATUS(0)";

                                if (iLoggedBytes > 0) {
                                    int numNack = static_cast<int>(floor(iLoggedBytes / 1.5));
                                    int iAllign = 0;
                                    int iHeadFromAllign = 0;
                                    json nack_list_j = json::array();
                                    for (int indexNack = 0; indexNack < numNack; ++indexNack) {
                                        json nack_item_j;
                                        int nack_consumed_bytes = 0;
                                        if (iAllign == 0) {
                                            iAllign = 1;
                                            nack_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN_v4, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN_v4, Fmt), b, current_pdu_offset, length - (current_pdu_offset-start), nack_item_j);
                                            iLoggedBytes -= 2;
                                            int iNonDecodeNACK = nack_item_j["NACK_SN"];
                                            int iPart3 = iNonDecodeNACK / 4096;
                                            int iPart4 = (iNonDecodeNACK - iPart3 * 4096) / 256;
                                            int iPart1 = (iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256) / 16;
                                            int iPart2 = iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                            nack_item_j["NACK_SN"] = iPart1 * 32 + iPart2 * 2 + iPart3 / 8 + iHeadFromPadding_ctrl;
                                            iHeadFromAllign = iPart4 + (iPart3 & 1) * 16;
                                            if ((iPart3 & 2) == 2) indexNack = numNack - 1;
                                        } else {
                                            iAllign = 0;
                                            nack_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING_v4, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING_v4, Fmt), b, current_pdu_offset, length - (current_pdu_offset-start), nack_item_j);
                                            iLoggedBytes -= 1;
                                            int iNonDecodeNACK = nack_item_j["NACK_SN"];
                                            nack_item_j["NACK_SN"] = iHeadFromAllign * 32 + iNonDecodeNACK / 8;
                                            if ((iNonDecodeNACK & 2) == 2) indexNack = numNack - 1;
                                            iHeadFromPadding_ctrl = (iNonDecodeNACK & 1) * 512;
                                        }
                                        current_pdu_offset += nack_consumed_bytes;
                                        nack_list_j.push_back(nack_item_j);
                                        if (indexNack >= numNack -1) break;
                                    }
                                    pdu_item_j["RLC CTRL NACK"] = nack_list_j;
                                }
                            } else {
                                pdu_item_j["PDU TYPE"] = "RLCUL DATA";
                                std::string strRF = ((DCLookAhead) & (1 << 6)) ? "1" : "0";
                                std::string strP  = ((DCLookAhead) & (1 << 5)) ? "1" : "0";
                                std::string strFI = "";
                                strFI += ((DCLookAhead) & (1 << 4)) ? "1" : "0";
                                strFI += ((DCLookAhead) & (1 << 3)) ? "1" : "0";
                                std::string strE  = ((DCLookAhead) & (1 << 2)) ? "1" : "0";

                                if ((DCLookAhead) & (1 << 1)) iNonDecodeSN += 512;
                                if ((DCLookAhead) & 1) iNonDecodeSN += 256;
                                pdu_item_j["SN"] = iNonDecodeSN;

                                int data_pdu_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_DATA_v4,
                                                                       ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_DATA_v4, Fmt),
                                                                       b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);
                                current_pdu_offset += data_pdu_consumed;

                                pdu_item_j["RF"] = strRF;
                                pdu_item_j["P"] = strP;
                                pdu_item_j["FI"] = strFI;
                                pdu_item_j["E"] = strE;

                                if (strRF == "1") {
                                    iLoggedBytes -= 2;
                                    int lsf_so_consumed = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO_v4,
                                                                         ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO_v4, Fmt),
                                                                         b, current_pdu_offset, length - (current_pdu_offset - start), pdu_item_j);
                                    current_pdu_offset += lsf_so_consumed;
                                    int temp_lsf_so = pdu_item_j["LSF"];
                                    pdu_item_j["LSF"] = temp_lsf_so >> 7;
                                    pdu_item_j["SO"] = ((temp_lsf_so & 127) * 256) + pdu_item_j["SO"].get<int>();
                                }

                                if (strE == "1") {
                                    int numLI = static_cast<int>(floor(iLoggedBytes / 1.5));
                                    iLoggedBytes = 0;
                                    int iAllign = 0;
                                    int iHeadFromAllign = 0;
                                    json li_list_j = json::array();
                                    for (int indexLI = 0; indexLI < numLI; ++indexLI) {
                                        json li_item_j;
                                        int li_consumed_bytes = 0;
                                        if (iAllign == 0) {
                                            iAllign = 1;
                                            li_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN_v4, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN_v4, Fmt), b, current_pdu_offset, length-(current_pdu_offset-start), li_item_j);
                                            int iNonDecodeLI = li_item_j["LI"];
                                            int iPart3 = iNonDecodeLI / 4096;
                                            int iPart4 = (iNonDecodeLI - iPart3 * 4096) / 256;
                                            int iPart1 = (iNonDecodeLI - iPart3 * 4096 - iPart4 * 256) / 16;
                                            int iPart2 = iNonDecodeLI - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                            li_item_j["LI"] = (iPart1 % 8) * 256 + iPart2 * 16 + iPart3;
                                            iHeadFromAllign = (iPart4 % 8) * 256;
                                        } else {
                                            iAllign = 0;
                                            li_consumed_bytes = _decode_by_fmt(LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING_v4, ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING_v4, Fmt), b, current_pdu_offset, length-(current_pdu_offset-start), li_item_j);
                                            int iNonDecodeLI = li_item_j["LI"];
                                            li_item_j["LI"] = iHeadFromAllign + iNonDecodeLI;
                                        }
                                        current_pdu_offset += li_consumed_bytes;
                                        li_list_j.push_back(li_item_j);
                                    }
                                    pdu_item_j["RLC DATA LI"] = li_list_j;
                                }
                            }
                            if (iLoggedBytes > 0 && current_pdu_offset + iLoggedBytes <= start + length) {
                                current_pdu_offset += iLoggedBytes;
                            } else if (iLoggedBytes < 0) {
                                LOGD("RLC UL PDU v4: iLoggedBytes is negative (%d)", iLoggedBytes);
                            }
                            pdu_list_j.push_back(pdu_item_j);
                        }
                        subpkt_j["RLCUL PDUs"] = pdu_list_j;
                        offset = current_pdu_offset;

                    } else {
                        LOGD("(MI)Unknown LTE RLC UL AM All PDU subpkt version for ID 70: %d", subpkt_payload_ver);
                        int remaining_payload_size = subpkt_size - header_consumed;
                        if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                            offset += remaining_payload_size;
                        }
                    }
                } else {
                    LOGD("(MI)Unknown LTE RLC UL AM All PDU Subpacket ID: 0x%x", subpkt_id);
                    int remaining_payload_size = subpkt_size - header_consumed;
                    if (remaining_payload_size > 0 && offset + remaining_payload_size <= start + length) {
                        offset += remaining_payload_size;
                    }
                }

                int total_consumed_by_subpkt = offset - start_subpkt;
                if (total_consumed_by_subpkt != subpkt_size) {
                    LOGD("Subpacket ID %d Ver %d: Consumed %d, SubPktSize %d. Adjusting offset.", subpkt_id, subpkt_payload_ver, total_consumed_by_subpkt, subpkt_size);
                    offset = start_subpkt + subpkt_size;
                    if (offset > start + length) {
                        LOGD("Error: Adjusted offset for subpacket ID %d Ver %d exceeds buffer length. Stopping.", subpkt_id, subpkt_payload_ver);
                        return offset - start;
                    }
                }
                subpackets_j.push_back(subpkt_j);
            }
            j["Subpackets"] = subpackets_j;
            return offset - start;
        }
        default:
            LOGD("(MI)Unknown LTE RLC UL AM All PDU packet version: 0x%x", pkt_ver);
            return 0;
    }
}
