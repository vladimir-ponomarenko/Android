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
    val jwt: String,    // JWT
    val uuid: String,   // UUID
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
    val rxBytes: Long, // Downlink
    val txBytes: Long  // Uplink
)

data class TotalTrafficData(
    val totalBytes: Long,
    val mobileBytes: Long,
    val wifiBytes: Long
)