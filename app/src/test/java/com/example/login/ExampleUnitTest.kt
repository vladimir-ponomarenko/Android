package com.example.login

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
import android.telephony.CellSignalStrengthGsm
import android.telephony.CellSignalStrengthLte
import android.telephony.CellSignalStrengthNr
import android.telephony.CellSignalStrengthWcdma
import android.telephony.PhoneStateListener
import android.telephony.TelephonyManager
import android.util.Log
import androidx.test.core.app.ApplicationProvider
import androidx.test.filters.SdkSuppress
import com.example.login.MainActivity.Companion.TAG
import io.mockk.Runs
import io.mockk.every
import io.mockk.just
import io.mockk.mockk
import io.mockk.spyk
import io.mockk.verify
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config
import org.robolectric.shadows.ShadowLooper
import java.time.Instant
import java.time.format.DateTimeFormatter


@RunWith(RobolectricTestRunner::class)
@Config(minSdk = Build.VERSION_CODES.R, instrumentedPackages = ["com.example.login"])
class CellInfoUnitTest {

    private lateinit var context: Context
    private lateinit var telephonyManager: TelephonyManager
    private lateinit var state: MainActivity.MainActivityState

    @Before
    fun setUp() {
        context = ApplicationProvider.getApplicationContext()
        state = MainActivity.MainActivityState(context)
        telephonyManager = mockk(relaxed = true)
    }

    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.O)
    @Test
    fun testGetCellInfo_validData() {
        val cellInfoLte = mockk<CellInfoLte>()
        val cellIdentityLte = mockk<android.telephony.CellIdentityLte>()
        val signalStrengthLte = mockk<CellSignalStrengthLte>()

        every { cellInfoLte.isRegistered } returns true
        every { cellInfoLte.cellSignalStrength } returns signalStrengthLte
        every { cellInfoLte.cellIdentity } returns cellIdentityLte
        every { cellIdentityLte.mccString } returns "310"
        every { cellIdentityLte.mncString } returns "410"
        every { cellIdentityLte.ci } returns 123456
        every { cellIdentityLte.pci } returns 234
        every { cellIdentityLte.tac } returns 56789
        every { cellIdentityLte.earfcn } returns 67890
        every { cellIdentityLte.bandwidth } returns 15
        every { signalStrengthLte.rsrp } returns -90
        every { signalStrengthLte.rssi } returns -70
        every { signalStrengthLte.rsrq } returns -10
        every { signalStrengthLte.rssnr } returns 10
        every { signalStrengthLte.cqi } returns 12
        every { signalStrengthLte.timingAdvance } returns 5

        val cellInfoGsm = mockk<CellInfoGsm>()
        val cellIdentityGsm = mockk<android.telephony.CellIdentityGsm>()
        val signalStrengthGsm = mockk<CellSignalStrengthGsm>()

        every { cellInfoGsm.isRegistered } returns false
        every { cellInfoGsm.cellSignalStrength } returns signalStrengthGsm
        every { cellInfoGsm.cellIdentity } returns cellIdentityGsm
        every { cellIdentityGsm.mccString } returns "311"
        every { cellIdentityGsm.mncString } returns "411"
        every { cellIdentityGsm.lac } returns 65432
        every { cellIdentityGsm.cid } returns 98765
        every { cellIdentityGsm.arfcn } returns 4567
        every { cellIdentityGsm.bsic } returns 34
        every { signalStrengthGsm.dbm } returns -85
        every { signalStrengthGsm.bitErrorRate } returns 3
        every { signalStrengthGsm.timingAdvance } returns 8

        val cellInfoWcdma = mockk<CellInfoWcdma>()
        val cellIdentityWcdma = mockk<android.telephony.CellIdentityWcdma>()
        val signalStrengthWcdma = mockk<CellSignalStrengthWcdma>()

        every { cellInfoWcdma.isRegistered } returns true
        every { cellInfoWcdma.cellSignalStrength } returns signalStrengthWcdma
        every { cellInfoWcdma.cellIdentity } returns cellIdentityWcdma
        every { cellIdentityWcdma.mccString } returns "312"
        every { cellIdentityWcdma.mncString } returns "412"
        every { cellIdentityWcdma.lac } returns 54321
        every { cellIdentityWcdma.cid } returns 87654
        every { cellIdentityWcdma.psc } returns 123
        every { cellIdentityWcdma.uarfcn } returns 3456
        every { signalStrengthWcdma.dbm } returns -80
        every { signalStrengthWcdma.asuLevel } returns 5
        every { signalStrengthWcdma.level } returns 3

        val cellInfoList = listOf(cellInfoLte, cellInfoGsm, cellInfoWcdma)

        val telephonyManager: TelephonyManager = mockk(relaxed = true)
        val context: Context = mockk()
        every { context.getSystemService(Context.TELEPHONY_SERVICE) } returns telephonyManager
        every { context.checkPermission(any(), any(), any()) } returns PackageManager.PERMISSION_GRANTED

        val testListener = object : PhoneStateListener() {
            override fun onCellInfoChanged(cellInfoList: List<CellInfo>?) {
                val cellInfoDataByType = cellInfoList?.groupBy {
                    when (it) {
                        is CellInfoLte -> "LTE"
                        is CellInfoGsm -> "GSM"
                        is CellInfoWcdma -> "WCDMA"
                        is CellInfoCdma -> "CDMA"
                        is CellInfoNr -> "NR"
                        else -> "Unknown"
                    }
                }?.mapValues {
                    it.value.mapNotNull { cellInfo ->
                        when (cellInfo) {
                            is CellInfoLte -> extractLteCellInfoToJson(cellInfo)
                            is CellInfoGsm -> extractGsmCellInfoToJson(cellInfo)
                            is CellInfoWcdma -> extractWcdmaCellInfoToJson(cellInfo)
                            is CellInfoCdma -> extractCdmaCellInfoToJson(cellInfo)
                            is CellInfoNr -> extractNrCellInfoToJson(cellInfo)
                            else -> null
                        }
                    }
                } ?: emptyMap()

                state.messageToData2 = MessageToData2(
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

            @SdkSuppress(minSdkVersion = Build.VERSION_CODES.P)
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

            @SdkSuppress(minSdkVersion = Build.VERSION_CODES.Q)
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

        testListener.onCellInfoChanged(cellInfoList)

        assert(state.messageToData2?.lte?.cellInfoList?.isNotEmpty() == true)
        assert(state.messageToData2?.gsm?.cellInfoList?.isNotEmpty() == true)
        assert(state.messageToData2?.wcdma?.cellInfoList?.isNotEmpty() == true)
    }


@SdkSuppress(minSdkVersion = Build.VERSION_CODES.O)
    @Test
    fun testGetCellInfo_emptyData() {
        context = mockk()
        every { context.getSystemService(Context.TELEPHONY_SERVICE as String) } returns telephonyManager
        every { context.checkPermission(any(), any(), any()) } returns PackageManager.PERMISSION_GRANTED
        every { telephonyManager.allCellInfo } returns emptyList<CellInfo>()
        every { telephonyManager.listen(any(), any()) } just Runs
        DataManager.getCellInfo(context, state)

        val phoneStateListener = spyk<PhoneStateListener>()
        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_CELL_INFO)
        phoneStateListener.onCellInfoChanged(emptyList())

        ShadowLooper.idleMainLooper()
        Thread.sleep(100)

        assert(state.messageToData2?.lte?.cellInfoList?.isEmpty() ?: true)
        assert(state.messageToData2?.gsm?.cellInfoList?.isEmpty() ?: true)
        assert(state.messageToData2?.wcdma?.cellInfoList?.isEmpty() ?: true)
        assert(state.messageToData2?.cdma?.cellInfoList?.isEmpty() ?: true)
        assert(state.messageToData2?.nr?.cellInfoList?.isEmpty() ?: true)

        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_NONE)
    }
    @Test
    fun testGetCellInfo_registersListener() {
        val telephonyManager: TelephonyManager = mockk(relaxed = true)
        val context: Context = mockk()
        val state = MainActivity.MainActivityState(context)
        every { context.getSystemService(Context.TELEPHONY_SERVICE) } returns telephonyManager
        every { context.checkPermission(any(), any(), any()) } returns PackageManager.PERMISSION_GRANTED

        DataManager.getCellInfo(context, state)

        verify { telephonyManager.listen(any<PhoneStateListener>(), PhoneStateListener.LISTEN_CELL_INFO) }
    }

    @Test
    fun testGetCellInfo_unregistersListener() {
        val telephonyManager: TelephonyManager = mockk(relaxed = true)
        val context: Context = mockk()
        val state = MainActivity.MainActivityState(context)
        every { context.getSystemService(Context.TELEPHONY_SERVICE) } returns telephonyManager
        every { context.checkPermission(any(), any(), any()) } returns PackageManager.PERMISSION_GRANTED
        every { telephonyManager.listen(any(), any()) } just Runs

        DataManager.getCellInfo(context, state)
        DataManager.getCellInfo(context, state)

        verify { telephonyManager.listen(any<PhoneStateListener>(), PhoneStateListener.LISTEN_NONE) }
    }
}
