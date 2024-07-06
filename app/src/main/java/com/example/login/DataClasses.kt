package com.example.login

import kotlinx.serialization.Serializable

@Serializable
data class LocationData(
    val Latitude: String,
    val longitude: String,
    val rsrp: String,
    val Rssi: String,
    val Rsrq: String,
    val Rssnr: String,
    val Cqi: String,
    val Bandwidth: String,
    val Cellid: String,
    val timestamp: String
)

@Serializable
data class MessageData(
    val jwt: String,
    val uuid: String,
    val time: String,
    val latitude: Double,
    val Longitude: Double,
    val rsrp: Long,
    val rssi: Long,
    val rsrq: Long,
    val rssnr: Long,
    val cqi: Long,
    val bandwidth: Long,
    val cellID: Long,
    val physcellid: Long
)

@Serializable
data class RegisterResponse(
    val email: String,
    val jwt: String,
    val message: String,
    val uuid: String
)

@Serializable
data class AuthResponse(
    val email: String,
    val jwt: String,
    val uuid: String
)

data class AppTrafficData(
    val appName: String,
    val totalBytes: Long,
    val mobileBytes: Long,
    val wifiBytes: Long,
    val rxBytes: Long,
    val txBytes: Long
)

data class TotalTrafficData(
    val totalBytes: Long,
    val mobileBytes: Long,
    val wifiBytes: Long
)

@Serializable
data class CellInfoData(
    val type: String,
    val timestamp: Long,
    val registered: Boolean,
    val mcc: String? = null,
    val mnc: String? = null,
    val lac: String? = null,
    val cid: String? = null,
    val arfcn: String? = null,
    val bsic: String? = null,
    val ci: String? = null,
    val pci: String? = null,
    val tac: String? = null,
    val earfcn: String? = null,
    val bandwidth: String? = null,
    val rsrp: String? = null,
    val rssi: String? = null,
    val rsrq: String? = null,
    val rssnr: String? = null,
    val cqi: String? = null,
    val timingAdvance: String? = null,
    val psc: String? = null,
    val uarfcn: String? = null,
    val rscp: String? = null,
    val ecno: String? = null,
    val sid: String? = null,
    val nid: String? = null,
    val bsid: String? = null,
    val ecIo: String? = null,
    val evdoDbm: String? = null,
    val evdoEcio: String? = null,
    val evdoSnr: String? = null,
    val nci: String? = null,
    val nrarfcn: String? = null,
    val csiRsrp: String? = null,
    val csiRsrq: String? = null,
    val csiSinr: String? = null,
    val ssRsrp: String? = null,
    val ssRsrq: String? = null,
    val ssSinr: String? = null,
    val level: String? = null,
    val parametersUseForLevel: String? = null,
    val cqiTableIndex: String? = null,
    val bitErrorRate: String? = null
)