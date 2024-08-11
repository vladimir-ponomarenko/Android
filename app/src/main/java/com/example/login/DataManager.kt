package com.example.login

import android.Manifest
import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.telephony.CellIdentityNr
import android.telephony.CellInfo
import android.telephony.CellInfoCdma
import android.telephony.CellInfoGsm
import android.telephony.CellInfoLte
import android.telephony.CellInfoNr
import android.telephony.CellInfoWcdma
import android.telephony.CellSignalStrengthNr
import android.telephony.PhoneStateListener
import android.telephony.TelephonyManager
import android.telephony.cdma.CdmaCellLocation
import android.telephony.gsm.GsmCellLocation
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationCallback
import com.google.android.gms.location.LocationRequest
import com.google.android.gms.location.LocationResult
import com.google.android.gms.location.LocationServices
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import java.time.Instant
import java.time.format.DateTimeFormatter


object DataManager {
    private const val LOCATION_PERMISSION_REQUEST_CODE = 1001
    private const val TAG = "DataManager"
    private var locationCallback: LocationCallback? = null
    private var locationUpdatesCount = 0
    private const val MAX_LOCATION_UPDATES = 1

    @RequiresApi(Build.VERSION_CODES.O)
    fun getLocation(activity: Activity, state: MainActivity.MainActivityState) {
        val context = activity.applicationContext
        Log.d(MainActivity.TAG, "getLocation() called")

        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED
        ) {

            ActivityCompat.requestPermissions(
                activity,
                arrayOf(Manifest.permission.ACCESS_FINE_LOCATION),
                LOCATION_PERMISSION_REQUEST_CODE
            )
            return
        }

        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_COARSE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            Log.d(MainActivity.TAG, "No permission to write to external storage")
            return
        }

        val locationRequest = LocationRequest.create().apply {
            interval = 0
            fastestInterval = 0
            priority = LocationRequest.PRIORITY_HIGH_ACCURACY
            smallestDisplacement = 0f
            numUpdates = Int.MAX_VALUE
        }

        val fusedLocationClient: FusedLocationProviderClient = LocationServices.getFusedLocationProviderClient(activity)

        fusedLocationClient.lastLocation.addOnSuccessListener { location ->
            location?.let {
                state.Latitude = location.latitude.toString()
                state.Longtitude = location.longitude.toString()
                Log.d(MainActivity.TAG, "Location received: Lat=${state.Latitude}, Lon=${state.Longtitude}")
            }
        }.addOnFailureListener { e ->
            Log.e(MainActivity.TAG, "Failed to get last known location", e)
        }

        if (locationUpdatesCount < MAX_LOCATION_UPDATES) {
            locationUpdatesCount++
            fusedLocationClient.requestLocationUpdates(locationRequest, object : LocationCallback() {
                override fun onLocationResult(locationResult: LocationResult) {
                    super.onLocationResult(locationResult)
                    locationResult.lastLocation?.let { location ->
                        state.Latitude = location.latitude.toString()
                        state.Longtitude = location.longitude.toString()
                        Log.d(MainActivity.TAG, "Location updated: Lat=${state.Latitude}, Lon=${state.Longtitude}")
                    }
                }
            }, null)
        } else {
            Log.w(TAG, "Maximum number of location updates reached.")
        }
    }
    @RequiresApi(Build.VERSION_CODES.O)
    fun getCellInfo(context: Context, state: MainActivity.MainActivityState) {
        if (ContextCompat.checkSelfPermission(context, android.Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
            Log.d(MainActivity.TAG, "No READ_PHONE_STATE permission for cell info")
            return
        }

        val telephonyManager = context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager

        val phoneStateListener = object : PhoneStateListener() {
            private var lastUpdateTime = 0L

            @RequiresApi(Build.VERSION_CODES.Q)
            @SuppressLint("MissingPermission")
            override fun onCellInfoChanged(cellInfoList: List<CellInfo>?) {
                try {
                    super.onCellInfoChanged(cellInfoList)
                    val currentTime = System.currentTimeMillis()

                    if (currentTime - lastUpdateTime >= 2000) {
                        lastUpdateTime = currentTime

                        val cellInfoDataByType = mutableMapOf<String, MutableList<CellInfoData>>(
                            "CDMA" to mutableListOf(),
                            "GSM" to mutableListOf(),
                            "LTE" to mutableListOf(),
                            "WCDMA" to mutableListOf(),
                            "NR" to mutableListOf()
                        )

                        if (cellInfoList != null) {
                            for (cellInfo in cellInfoList) {
                                val cellData = when (cellInfo) {
                                    is CellInfoLte -> extractLteCellInfoToJson(cellInfo)
                                    is CellInfoGsm -> extractGsmCellInfoToJson(cellInfo)
                                    is CellInfoWcdma -> extractWcdmaCellInfoToJson(cellInfo)
                                    is CellInfoCdma -> extractCdmaCellInfoToJson(cellInfo)
                                    is CellInfoNr -> if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                                        extractNrCellInfoToJson(cellInfo)
                                    } else null
                                    else -> {
                                        Log.d(TAG, "Unknown cell info type: ${cellInfo.javaClass.simpleName}")
                                        null
                                    }
                                }

                                if (cellData != null) {
                                    val cellType = when (cellInfo) {
                                        is CellInfoLte -> "LTE"
                                        is CellInfoGsm -> "GSM"
                                        is CellInfoWcdma -> "WCDMA"
                                        is CellInfoCdma -> "CDMA"
                                        is CellInfoNr -> "NR"
                                        else -> "Unknown"
                                    }
                                    cellInfoDataByType[cellType]?.add(cellData)
                                }
                            }
                        }

                        val messageToData2 = MessageToData2(
                            jwt = state.JwtToken,
                            UUID = state.Uuid,
                            time = DateTimeFormatter.ISO_INSTANT.format(Instant.now()),
                            latitude = state.Latitude.toDoubleOrNull() ?: 0.0,
                            longitude = state.Longtitude.toDoubleOrNull() ?: 0.0,
                            operator = state.Operator,
                            cdma = CdmaData(cellInfoDataByType["CDMA"] ?: emptyList()),
                            gsm = GsmData(cellInfoDataByType["GSM"] ?: emptyList()),
                            wcdma = WcdmaData(cellInfoDataByType["WCDMA"] ?: emptyList()),
                            lte = LteData(cellInfoDataByType["LTE"] ?: emptyList()),
                            nr = NRData(cellInfoDataByType["NR"] ?: emptyList())
                        )

                        (context as? Activity)?.runOnUiThread {
                            state.messageToData2 = messageToData2
                        }

                        val jsonMessageToData2 = Json.encodeToString(messageToData2)
                        val maxLogLength = 4000
                        for (i in jsonMessageToData2.indices step maxLogLength) {
                            val chunk = jsonMessageToData2.substring(i, minOf(i + maxLogLength, jsonMessageToData2.length))
                            Log.d(TAG, "CellInfo JSON (part ${i / maxLogLength}): $chunk")
                        }
                        if (state.isSendingCellInfoData) {
                            MainActivity.networkManager.sendMessageToData2ToServer(messageToData2) { success ->
                                if (success) {
                                    Log.d(TAG, "CellInfo SENT TO SERVER")
                                } else {
                                    Log.e(TAG, "FAILED TO SEND CellInfo")
                                }
                            }
                        }
                    }
                } catch (e: SecurityException) {
                    Log.e(MainActivity.TAG, "SecurityException in onCellInfoChanged: ", e)
                }
            }
            @RequiresApi(Build.VERSION_CODES.Q)
            private fun logCellInfo(info: CellInfo) {
                val type = when (info) {
                    is CellInfoLte -> "LTE"
                    is CellInfoGsm -> "GSM"
                    is CellInfoWcdma -> "WCDMA"
                    is CellInfoCdma -> "CDMA"
                    is CellInfoNr -> "5G NR"
                    else -> "Unknown"
                }
                Log.d(MainActivity.TAG, "CellInfo$type: $info")
            }

            private fun extractLteCellInfoToJson(info: CellInfoLte): CellInfoData? {
                return try {
                    val signalStrength = info.cellSignalStrength
                    val identity = info.cellIdentity
                    val timestamp = System.currentTimeMillis()
                    CellInfoData(
                        type = "LTE",
                        timestamp = timestamp,
                        registered = info.isRegistered,
                        mcc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) identity.mccString?.toLongOrNull() else null,
                        mnc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) identity.mncString?.toLongOrNull() else null,
                        ci = identity.ci.toLong().takeIf { it != 2147483647L },
                        pci = identity.pci.toLong().takeIf { it != 2147483647L },
                        tac = identity.tac.toLong().takeIf { it != 2147483647L },
                        earfcn = identity.earfcn.toLong().takeIf { it != 2147483647L },
                        bandwidth = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) identity.bandwidth.toLong().takeIf { it != 2147483647L } else null,
                        rsrp = signalStrength.rsrp.toLong().takeIf { it != 2147483647L },
                        rssi = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) signalStrength.rssi.toLong().takeIf { it != 2147483647L } else null,
                        rsrq = signalStrength.rsrq.toLong().takeIf { it != 2147483647L },
                        rssnr = signalStrength.rssnr.toLong().takeIf { it != 2147483647L },
                        cqi = signalStrength.cqi.toLong().takeIf { it != 2147483647L },
                        timingAdvance = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) signalStrength.timingAdvance.toLong().takeIf { it != 2147483647L } else null
                    )
                } catch (e: Exception) {
                    Log.e(TAG, "Error encoding LTE cell info to JSON: ", e)
                    null
                }
            }

            private fun extractGsmCellInfoToJson(info: CellInfoGsm): CellInfoData? {
                return try {
                    val signalStrength = info.cellSignalStrength
                    val identity = info.cellIdentity
                    val timestamp = System.currentTimeMillis()
                    CellInfoData(
                        type = "GSM",
                        timestamp = timestamp,
                        registered = info.isRegistered,
                        mcc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) identity.mccString?.toLongOrNull() else null,
                        mnc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) identity.mncString?.toLongOrNull() else null,
                        lac = identity.lac.toLong().takeIf { it != 2147483647L },
                        cid = identity.cid.toLong().takeIf { it != 2147483647L },
                        arfcn = identity.arfcn.toLong().takeIf { it != 2147483647L },
                        bsic = identity.bsic.toLong().takeIf { it != 2147483647L },
                        rssi = signalStrength.dbm.toLong().takeIf { it != 2147483647L },
                        bitErrorRate = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) signalStrength.bitErrorRate.toLong().takeIf { it != 2147483647L } else null,
                        timingAdvance = signalStrength.timingAdvance.toLong().takeIf { it != 2147483647L }
                    )
                } catch (e: Exception) {
                    Log.e(TAG, "Error encoding GSM cell info to JSON: ", e)
                    null
                }
            }

            @RequiresApi(Build.VERSION_CODES.P)
            private fun extractWcdmaCellInfoToJson(info: CellInfoWcdma): CellInfoData? {
                return try {
                    val signalStrength = info.cellSignalStrength
                    val identity = info.cellIdentity
                    val timestamp = System.currentTimeMillis()

                    CellInfoData(
                        type = "WCDMA",
                        timestamp = timestamp,
                        registered = info.isRegistered,
                        mcc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) identity.mccString?.toLongOrNull() else null,
                        mnc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) identity.mncString?.toLongOrNull() else null,
                        lac = identity.lac.toLong().takeIf { it != 2147483647L },
                        cid = identity.cid.toLong().takeIf { it != 2147483647L },
                        psc = identity.psc.toLong().takeIf { it != 2147483647L },
                        uarfcn = identity.uarfcn.toLong().takeIf { it != 2147483647L },
                        rssi = signalStrength.dbm.toLong().takeIf { it != 2147483647L },
                        bitErrorRate = signalStrength.asuLevel.toLong().takeIf { it != 2147483647L },
                        rscp = signalStrength.dbm.toLong().takeIf { it != 2147483647L },
                        level = signalStrength.level.toLong().takeIf { it != 2147483647L }
                    )
                } catch (e: Exception) {
                    Log.e(TAG, "Error encoding WCDMA cell info to JSON: ", e)
                    null
                }
            }

            private fun extractCdmaCellInfoToJson(info: CellInfoCdma): CellInfoData? {
                return try {
                    val signalStrength = info.cellSignalStrength
                    val identity = info.cellIdentity
                    val timestamp = System.currentTimeMillis()
                    CellInfoData(
                        type = "CDMA",
                        timestamp = timestamp,
                        registered = info.isRegistered,
                        sid = identity.systemId.toLong().takeIf { it != 2147483647L },
                        nid = identity.networkId.toLong().takeIf { it != 2147483647L },
                        bsid = identity.basestationId.toLong().takeIf { it != 2147483647L },
                        rssi = signalStrength.cdmaDbm.toLong().takeIf { it != 2147483647L },
                        ecIo = signalStrength.cdmaEcio.toLong().takeIf { it != 2147483647L },
                        evdoDbm = signalStrength.evdoDbm.toLong().takeIf { it != 2147483647L },
                        evdoEcio = signalStrength.evdoEcio.toLong().takeIf { it != 2147483647L },
                        evdoSnr = signalStrength.evdoSnr.toLong().takeIf { it != 2147483647L }
                    )
                } catch (e: Exception) {
                    Log.e(TAG, "Error encoding CDMA cell info to JSON: ", e)
                    null
                }
            }

            @RequiresApi(Build.VERSION_CODES.Q)
            private fun extractNrCellInfoToJson(info: CellInfoNr): CellInfoData? {
                return try {
                    val signalStrength = info.cellSignalStrength as CellSignalStrengthNr
                    val identity = info.cellIdentity as CellIdentityNr
                    val timestamp = System.currentTimeMillis()

                    CellInfoData(
                        type = "5G NR",
                        timestamp = timestamp,
                        registered = info.isRegistered,
                        mcc = identity.mccString?.toLongOrNull(),
                        mnc = identity.mncString?.toLongOrNull(),
                        nci = identity.nci.toLong().takeIf { it != 2147483647L },
                        pci = identity.pci.toLong().takeIf { it != 2147483647L },
                        tac = identity.tac.toLong().takeIf { it != 2147483647L },
                        nrarfcn = identity.nrarfcn.toLong().takeIf { it != 2147483647L },
                        csiRsrp = signalStrength.csiRsrp.toLong().takeIf { it != 2147483647L },
                        csiRsrq = signalStrength.csiRsrq.toLong().takeIf { it != 2147483647L },
                        csiSinr = signalStrength.csiSinr.toLong().takeIf { it != 2147483647L },
                        ssRsrp = signalStrength.ssRsrp.toLong().takeIf { it != 2147483647L },
                        ssRsrq = signalStrength.ssRsrq.toLong().takeIf { it != 2147483647L },
                        ssSinr = signalStrength.ssSinr.toLong().takeIf { it != 2147483647L }
                    )
                } catch (e: Exception) {
                    Log.e(TAG, "Error encoding NR cell info to JSON: ", e)
                    null
                }
            }
        }
        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_CELL_INFO)
        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_NONE)
    }



    @RequiresApi(Build.VERSION_CODES.O)
    internal fun getSignalStrength(state: MainActivity.MainActivityState) {
        val context = state.context
        if (PermissionUtils.checkPhoneStatePermission(context) &&
            ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
            val telephonyManager =
                context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
            val cellInfoList = telephonyManager.allCellInfo
            if (cellInfoList.isNullOrEmpty()) {
                setEmptyCellInfo(state)
            } else {
                for (info in cellInfoList) {
                    if (info is CellInfoLte) {
                        extractLteCellInfo(state, info, telephonyManager)
                        break
                    }
                }
            }
        } else {
            setNoPermissionCellInfo(state)
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun extractLteCellInfo(state: MainActivity.MainActivityState, info: CellInfoLte, telephonyManager: TelephonyManager) {
        val context = state.context
        if (ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
            val cellSignalStrengthLte = info.cellSignalStrength
            state.Rsrp = "${cellSignalStrengthLte.rsrp} dBm"
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                state.Rssi = "${cellSignalStrengthLte.rssi} dBm"
            }
            state.Rsrq = "${cellSignalStrengthLte.rsrq} dB"
            state.Rssnr = "${cellSignalStrengthLte.rssnr} dB"
            state.Cqi = "${cellSignalStrengthLte.cqi}"
            state.Bandwidth = "${telephonyManager.dataNetworkType}"
            state.Cellid = when (val cellLocation = telephonyManager.cellLocation) {
                is GsmCellLocation -> cellLocation.cid.toString()
                is CdmaCellLocation -> cellLocation.baseStationId.toString()
                else -> "Cell ID not available"
            }
            state.Mcc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                info.cellIdentity.mccString ?: "N/A"
            } else {
                "N/A"
            }
            state.Mnc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                info.cellIdentity.mncString ?: "N/A"
            } else {
                "N/A"
            }
            state.Lac = try {
                (telephonyManager.cellLocation as? GsmCellLocation)?.lac?.toString() ?: "N/A"
            } catch (e: Exception) {
                "N/A"
            }
            state.Tac =
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
                    info.cellIdentity.tac.toString()
                } else {
                    "N/A"
                }
            state.Pci = info.cellIdentity.pci.toString()
            state.Earfcn = info.cellIdentity.earfcn.toString()
            state.Ci = info.cellIdentity.ci.toString()
            state.NetworkType = telephonyManager.networkType.toString()
            state.SignalStrength = cellSignalStrengthLte.dbm.toString()
            state.BitErrorRate = "N/A"
            state.TimingAdvance =
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                    cellSignalStrengthLte.timingAdvance.toString()
                } else {
                    "N/A"
                }
            state.Band = getBandFromEarfcn(info.cellIdentity.earfcn)
            state.Operator = telephonyManager.networkOperatorName
            state.Technology = "LTE"
            logCellInfo(state)
        } else {
            setNoPermissionCellInfo(state)
        }
    }

    private fun setEmptyCellInfo(state: MainActivity.MainActivityState) {
        state.Rsrp = "CellInfo list is empty"
        state.Rssi = "CellInfo list is empty"
        state.Rsrq = "CellInfo list is empty"
        state.Rssnr = "CellInfo list is empty"
        state.Cqi = "CellInfo list is empty"
        state.Bandwidth = "CellInfo list is empty"
        state.Cellid = "Cell Info not available"
        state.Mcc = "N/A"
        state.Mnc = "N/A"
        state.Lac = "N/A"
        state.Tac = "N/A"
        state.Pci = "N/A"
        state.Earfcn = "N/A"
        state.Ci = "N/A"
        state.NetworkType = "N/A"
        state.SignalStrength = "N/A"
        state.BitErrorRate = "N/A"
        state.TimingAdvance = "N/A"
        state.Band = "N/A"
        state.Operator = "N/A"
        state.Technology = "N/A"
    }

    private fun setNoPermissionCellInfo(state: MainActivity.MainActivityState) {
        state.Rsrp = "No READ_PHONE_STATE permission"
        state.Rssi = "No READ_PHONE_STATE permission"
        state.Rsrq = "No READ_PHONE_STATE permission"
        state.Rssnr = "No READ_PHONE_STATE permission"
        state.Cqi = "No READ_PHONE_STATE permission"
        state.Bandwidth = "No READ_PHONE_STATE permission"
        state.Cellid = "No READ_PHONE_STATE permission"
        state.Mcc = "No READ_PHONE_STATE permission"
        state.Mnc = "No READ_PHONE_STATE permission"
        state.Lac = "No READ_PHONE_STATE permission"
        state.Tac = "No READ_PHONE_STATE permission"
        state.Pci = "No READ_PHONE_STATE permission"
        state.Earfcn = "No READ_PHONE_STATE permission"
        state.Ci = "No READ_PHONE_STATE permission"
        state.NetworkType = "No READ_PHONE_STATE permission"
        state.SignalStrength = "No READ_PHONE_STATE permission"
        state.BitErrorRate = "No READ_PHONE_STATE permission"
        state.TimingAdvance = "No READ_PHONE_STATE permission"
        state.Band = "No READ_PHONE_STATE permission"
        state.Operator = "No READ_PHONE_STATE permission"
        state.Technology = "No READ_PHONE_STATE permission"
    }

    private fun logCellInfo(state: MainActivity.MainActivityState) {
        Log.d(MainActivity.TAG, "RSRP value: ${state.Rsrp}")
        Log.d(MainActivity.TAG, "Rssi value: ${state.Rssi}")
        Log.d(MainActivity.TAG, "Rsrq value: ${state.Rsrq}")
        Log.d(MainActivity.TAG, "Rssnr value: ${state.Rssnr}")
        Log.d(MainActivity.TAG, "Cqi value: ${state.Cqi}")
        Log.d(MainActivity.TAG, "Bandwidth value: ${state.Bandwidth}")
        Log.d(MainActivity.TAG, "Cell ID value: ${state.Cellid}")
        Log.d(MainActivity.TAG, "MCC: ${state.Mcc}")
        Log.d(MainActivity.TAG, "MNC: ${state.Mnc}")
        Log.d(MainActivity.TAG, "LAC: ${state.Lac}")
        Log.d(MainActivity.TAG, "TAC: ${state.Tac}")
        Log.d(MainActivity.TAG, "PCI: ${state.Pci}")
        Log.d(MainActivity.TAG, "EARFCN: ${state.Earfcn}")
        Log.d(MainActivity.TAG, "CI: ${state.Ci}")
        Log.d(MainActivity.TAG, "Network Type: ${state.NetworkType}")
        Log.d(MainActivity.TAG, "Signal Strength: ${state.SignalStrength}")
        Log.d(MainActivity.TAG, "Bit Error Rate: ${state.BitErrorRate}")
        Log.d(MainActivity.TAG, "Timing Advance: ${state.TimingAdvance}")
        Log.d(MainActivity.TAG, "Band: ${state.Band}")
        Log.d(MainActivity.TAG, "Operator: ${state.Operator}")
        Log.d(MainActivity.TAG, "Technology: ${state.Technology}")
    }

    @RequiresApi(Build.VERSION_CODES.O)
    fun generateJSON(state: MainActivity.MainActivityState): String {
        val currentTimestamp = Instant.now().toString()
        val formattedTimestamp = String.format(
            DateTimeFormatter.ISO_INSTANT.format(
                Instant.parse(currentTimestamp)
            )
        )

        val rsrp = state.Rsrp.replace(" dBm", "").toLongOrNull() ?: 0L
        val rssi = state.Rssi.replace(" dBm", "").toLongOrNull() ?: 0L
        val rsrq = state.Rsrq.replace(" dB", "").toLongOrNull() ?: 0L
        val rssnr = state.Rssnr.replace(" dB", "").toLongOrNull() ?: 0L
        val cqi = state.Cqi.toLongOrNull() ?: 0L
        val bandwidth = state.Bandwidth.toLongOrNull() ?: 0L
        val cellid = state.Cellid.toLongOrNull() ?: 0L
        val physcellid = state.Pci.toLongOrNull() ?: 0L

        val messageData = MessageData(
            state.JwtToken,
            state.Uuid,
            formattedTimestamp,
            state.Latitude.toDoubleOrNull() ?: 0.0,
            state.Longtitude.toDoubleOrNull() ?: 0.0,
            rsrp,
            rssi,
            rsrq,
            rssnr,
            cqi,
            bandwidth,
            cellid,
            physcellid
        )

        return Json.encodeToString(messageData)
    }

    private fun getBandFromEarfcn(earfcn: Int): String {
        return when (earfcn) {
            in 1..600 -> "Band 1 (2100 MHz)"
            in 1200..1950 -> "Band 3 (1800 MHz)"
            in 1951..2650 -> "Band 4 (1700/2100 MHz AWS)"
            in 2750..3450 -> "Band 7 (2600 MHz)"
            in 3600..4150 -> "Band 8 (900 MHz)"
            in 6600..7100 -> "Band 20 (800 DD)"
            in 9210..9660 -> "Band 28 (700 APT)"
            in 9770..10280 -> "Band 38 (TD 2600)"
            in 25700..26200 -> "Band 41 (TD 2500)"
            in 65536..67535 -> "Band 71 (600 MHz)"
            else -> "Unknown"
        }
    }
}
