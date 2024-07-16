package com.example.login

import android.annotation.SuppressLint
import android.content.Context
import android.os.Build
import android.telephony.SubscriptionManager
import android.telephony.TelephonyManager
import androidx.annotation.RequiresApi
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.Tab
import androidx.compose.material.TabRow
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Divider
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.delay
import kotlinx.serialization.json.Json
import kotlin.reflect.full.declaredMemberProperties


@RequiresApi(Build.VERSION_CODES.O)
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun DataScreen(state: MainActivity.MainActivityState) {
    val context = LocalContext.current
    var selectedTabIndex by remember { mutableStateOf(0) }
    val cellInfoJsonByType = remember { mutableStateOf(mutableMapOf<String, List<CellInfoData>>()) }

    LaunchedEffect(Unit) {
        while (true) {
            cellInfoJsonByType.value = state.cellInfoJson.value.mapValues { entry ->
                entry.value.map { jsonString ->
                    Json.decodeFromString<CellInfoData>(jsonString)
                }
            }.toMutableMap()
            delay(500) // Задержка в 0,5 секунд для обновления экрана
        }
    }

    Scaffold { innerPadding ->
        Column(
            modifier = Modifier
                .padding(innerPadding)
                .fillMaxSize()
        ) {
            TabRow(selectedTabIndex = selectedTabIndex) {
                Tab(
                    text = { Text("LTE") },
                    selected = selectedTabIndex == 0,
                    onClick = { selectedTabIndex = 0 }
                )
                Tab(
                    text = { Text("GSM") },
                    selected = selectedTabIndex == 1,
                    onClick = { selectedTabIndex = 1 }
                )
                Tab(
                    text = { Text("WCDMA") },
                    selected = selectedTabIndex == 2,
                    onClick = { selectedTabIndex = 2 }
                )
                Tab(
                    text = { Text("CDMA") },
                    selected = selectedTabIndex == 3,
                    onClick = { selectedTabIndex = 3 }
                )
                Tab(
                    text = { Text("NR") },
                    selected = selectedTabIndex == 4,
                    onClick = { selectedTabIndex = 4 }
                )
            }

            LazyColumn(modifier = Modifier.padding(8.dp)) {
                item { PhoneInfoCard() }

                when (selectedTabIndex) {
                    0 -> items(cellInfoJsonByType.value["LTE"] ?: emptyList()) { cellInfo ->
                        CellInfoCard(cellType = "LTE", cellInfo = cellInfo)
                    }
                    1 -> items(cellInfoJsonByType.value["GSM"] ?: emptyList()) { cellInfo ->
                        CellInfoCard(cellType = "GSM", cellInfo = cellInfo)
                    }
                    2 -> items(cellInfoJsonByType.value["WCDMA"] ?: emptyList()) { cellInfo ->
                        CellInfoCard(cellType = "WCDMA", cellInfo = cellInfo)
                    }
                    3 -> items(cellInfoJsonByType.value["CDMA"] ?: emptyList()) { cellInfo ->
                        CellInfoCard(cellType = "CDMA", cellInfo = cellInfo)
                    }
                    4 -> items(cellInfoJsonByType.value["NR"] ?: emptyList()) { cellInfo ->
                        CellInfoCard(cellType = "NR", cellInfo = cellInfo)
                    }
                }
            }
        }
    }
}

@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CellInfoCard(cellType: String, cellInfo: CellInfoData) {
    Card(
        elevation = CardDefaults.cardElevation(defaultElevation = 2.dp),
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 4.dp)
    ) {
        Column(modifier = Modifier.padding(16.dp)) {
            Text("Cell Type: $cellType")
            for (field in CellInfoData::class.declaredMemberProperties) {
                val fieldValue = field.getter.call(cellInfo)
                if (field.name != "type" &&
                    field.name != "timestamp" &&
                    fieldValue != null &&
                    fieldValue != "N/A"
                ) {
                    Text("${field.name.capitalize()}: $fieldValue")
                }
            }
            Divider(Modifier.padding(vertical = 8.dp))
        }
    }
}

@Composable
fun PhoneInfoCard() {
    val context = LocalContext.current
    val telephonyManager = context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
    val cardBackgroundColor = MaterialTheme.colorScheme.surfaceVariant

    Card(
        elevation = CardDefaults.cardElevation(defaultElevation = 2.dp),
        colors = CardDefaults.cardColors(containerColor = cardBackgroundColor),
        modifier = Modifier
            .fillMaxWidth()
            .padding(8.dp)
    ) {
        Column(modifier = Modifier.padding(16.dp)) {
            Text("Phone Information", style = MaterialTheme.typography.titleMedium)
            Divider(Modifier.padding(vertical = 8.dp))

            Text("Device Model: ${Build.MODEL}")
            Text("Manufacturer: ${Build.MANUFACTURER}")
            Text("Brand: ${Build.BRAND}")
            Text("Android Version: ${Build.VERSION.RELEASE} (SDK: ${Build.VERSION.SDK_INT})")
            Text("Board: ${Build.BOARD}")
            Text("Bootloader: ${Build.BOOTLOADER}")
            Text("Device: ${Build.DEVICE}")
            Text("Hardware: ${Build.HARDWARE}")
            Text("Product: ${Build.PRODUCT}")

            Text("Operator: ${telephonyManager.networkOperatorName}")
            Text("Network Type: ${getNetworkType(telephonyManager.networkType)}")
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                Text("SIM Operator: ${telephonyManager.simOperatorName}")
                Text("SIM State: ${getSimState(telephonyManager.simState)}")
            }
            Text("Phone Type: ${getPhoneType(telephonyManager.phoneType)}")
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                Text("Data Network Type: ${getDataNetworkType(telephonyManager.dataNetworkType)}")
            }


            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1) {
                val subscriptionManager = context.getSystemService(Context.TELEPHONY_SUBSCRIPTION_SERVICE) as SubscriptionManager
                val subscriptions = subscriptionManager.activeSubscriptionInfoList
                for (subscription in subscriptions) {
                    Text("SIM ${subscription.simSlotIndex + 1} Info:")
                    Text("  Carrier Name: ${subscription.carrierName}")
                    Text("  Carrier id: ${subscription.carrierId}")
                    Text("  Country ISO: ${subscription.countryIso}")
                    Text("  Number: ${subscription.number}")
                }
            }
        }
    }
}

private fun getNetworkType(networkType: Int): String {
    return when (networkType) {
        TelephonyManager.NETWORK_TYPE_UNKNOWN -> "Unknown"
        TelephonyManager.NETWORK_TYPE_GPRS -> "GPRS"
        TelephonyManager.NETWORK_TYPE_EDGE -> "EDGE"
        TelephonyManager.NETWORK_TYPE_UMTS -> "UMTS"
        TelephonyManager.NETWORK_TYPE_CDMA -> "CDMA"
        TelephonyManager.NETWORK_TYPE_EVDO_0 -> "EVDO rev. 0"
        TelephonyManager.NETWORK_TYPE_EVDO_A -> "EVDO rev. A"
        TelephonyManager.NETWORK_TYPE_1xRTT -> "1xRTT"
        TelephonyManager.NETWORK_TYPE_HSDPA -> "HSDPA"
        TelephonyManager.NETWORK_TYPE_HSUPA -> "HSUPA"
        TelephonyManager.NETWORK_TYPE_HSPA -> "HSPA"
        TelephonyManager.NETWORK_TYPE_IDEN -> "iDen"
        TelephonyManager.NETWORK_TYPE_EVDO_B -> "EVDO rev. B"
        TelephonyManager.NETWORK_TYPE_LTE -> "LTE"
        TelephonyManager.NETWORK_TYPE_EHRPD -> "eHRPD"
        TelephonyManager.NETWORK_TYPE_HSPAP -> "HSPA+"
        TelephonyManager.NETWORK_TYPE_GSM -> "GSM"
        TelephonyManager.NETWORK_TYPE_TD_SCDMA -> "TD-SCDMA"
        TelephonyManager.NETWORK_TYPE_IWLAN -> "IWLAN"
        TelephonyManager.NETWORK_TYPE_NR -> "NR (5G)"
        else -> "Unknown ($networkType)"
    }
}

private fun getSimState(simState: Int): String {
    return when (simState) {
        TelephonyManager.SIM_STATE_UNKNOWN -> "Unknown"
        TelephonyManager.SIM_STATE_ABSENT -> "Absent"
        TelephonyManager.SIM_STATE_PIN_REQUIRED -> "PIN Required"
        TelephonyManager.SIM_STATE_PUK_REQUIRED -> "PUK Required"
        TelephonyManager.SIM_STATE_NETWORK_LOCKED -> "Network Locked"
        TelephonyManager.SIM_STATE_READY -> "Ready"
        TelephonyManager.SIM_STATE_NOT_READY -> "Not Ready"
        TelephonyManager.SIM_STATE_PERM_DISABLED -> "Permanently Disabled"
        TelephonyManager.SIM_STATE_CARD_IO_ERROR -> "Card IO Error"
        TelephonyManager.SIM_STATE_CARD_RESTRICTED -> "Card Restricted"
        else -> "Unknown ($simState)"
    }
}

private fun getPhoneType(phoneType: Int): String {
    return when (phoneType) {
        TelephonyManager.PHONE_TYPE_NONE -> "None"
        TelephonyManager.PHONE_TYPE_GSM -> "GSM"
        TelephonyManager.PHONE_TYPE_CDMA -> "CDMA"
        TelephonyManager.PHONE_TYPE_SIP -> "SIP"
        else -> "Unknown ($phoneType)"
    }
}

@RequiresApi(Build.VERSION_CODES.N)
private fun getDataNetworkType(dataNetworkType: Int): String {
    return when (dataNetworkType) {
        TelephonyManager.NETWORK_TYPE_UNKNOWN -> "Unknown"
        TelephonyManager.NETWORK_TYPE_GPRS -> "GPRS"
        TelephonyManager.NETWORK_TYPE_EDGE -> "EDGE"
        TelephonyManager.NETWORK_TYPE_UMTS -> "UMTS"
        TelephonyManager.NETWORK_TYPE_CDMA -> "CDMA"
        TelephonyManager.NETWORK_TYPE_EVDO_0 -> "EVDO rev. 0"
        TelephonyManager.NETWORK_TYPE_EVDO_A -> "EVDO rev. A"
        TelephonyManager.NETWORK_TYPE_1xRTT -> "1xRTT"
        TelephonyManager.NETWORK_TYPE_HSDPA -> "HSDPA"
        TelephonyManager.NETWORK_TYPE_HSUPA -> "HSUPA"
        TelephonyManager.NETWORK_TYPE_HSPA -> "HSPA"
        TelephonyManager.NETWORK_TYPE_IDEN -> "iDen"
        TelephonyManager.NETWORK_TYPE_EVDO_B -> "EVDO rev. B"
        TelephonyManager.NETWORK_TYPE_LTE -> "LTE"
        TelephonyManager.NETWORK_TYPE_EHRPD -> "eHRPD"
        TelephonyManager.NETWORK_TYPE_HSPAP -> "HSPA+"
        TelephonyManager.NETWORK_TYPE_GSM -> "GSM"
        TelephonyManager.NETWORK_TYPE_TD_SCDMA -> "TD-SCDMA"
        TelephonyManager.NETWORK_TYPE_IWLAN -> "IWLAN"
        TelephonyManager.NETWORK_TYPE_NR -> "NR (5G)"
        else -> "Unknown Network Type"    }
}
