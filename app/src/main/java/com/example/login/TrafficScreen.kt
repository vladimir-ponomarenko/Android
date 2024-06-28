package com.example.login

import android.util.Log
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.Button
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.delay


@Composable
fun TrafficScreen(state: MainActivity.MainActivityState) {
    val context = LocalContext.current
    val appTrafficData = remember { mutableStateOf(emptyList<AppTrafficData>()) }
    var days by remember { mutableStateOf("1") }
    var showError by remember { mutableStateOf(false) }
    var showChart by remember { mutableStateOf(false) }
    var selectedAppName by remember { mutableStateOf("") }
    val totalTrafficData = remember { mutableStateOf(TotalTrafficData(0L, 0L, 0L)) }
    var isSendingTrafficData by remember { mutableStateOf(false) }

    LaunchedEffect(appTrafficData.value, days) {
        val numDays = days.toIntOrNull()
        if (numDays != null) {
            showError = false
            while (true) {
                appTrafficData.value = getAppTrafficData(context, numDays).sortedByDescending { it.totalBytes }
                totalTrafficData.value = getTotalTrafficData(context, numDays)
                delay(5000)
            }
        } else {
            showError = true
        }
    }

    Column(modifier = Modifier.fillMaxSize()) {
        Text(
            text = "Total Traffic: ${(totalTrafficData.value.totalBytes / 1024)} Kb, " +
                    "Mobile: ${(totalTrafficData.value.mobileBytes / 1024)} Kb, " +
                    "Wi-Fi: ${(totalTrafficData.value.wifiBytes / 1024)} Kb",
            modifier = Modifier.padding(16.dp)
        )

        OutlinedTextField(
            value = days,
            onValueChange = { days = it },
            label = { Text("Days") },
            modifier = Modifier.padding(16.dp)
        )

        if (showError) {
            Text(
                text = "Invalid number of days",
                color = Color.Red,
                modifier = Modifier.padding(16.dp)
            )
        }

        if (!PermissionUtils.hasUsageStatsPermission(context)) {
            Button(onClick = { PermissionUtils.requestUsageStatsPermission(context) }) {
                Text("Grant Usage Stats Permission")
            }
        }

        Button(onClick = {
            isSendingTrafficData = true
            MainActivity.networkManager.authenticateForTraffic(state.Email, state.Password) { authResponse ->
                if (authResponse != null) {
                    val top5Apps = appTrafficData.value.take(5)
                    MainActivity.networkManager.sendTrafficDataToServer(authResponse.jwt, top5Apps) { success ->
                        if (success) {
                            // Обработка успешной отправки данных
                            Log.d(MainActivity.TAG, "Traffic data sent successfully!")
                        } else {
                            // Обработка ошибки отправки данных
                            Log.e(MainActivity.TAG, "Failed to send traffic data")
                        }
                    }
                } else {
                    Log.e(MainActivity.TAG, "Authentication for traffic data failed")
                }
                isSendingTrafficData = false
            }
        }) {
            Text("Send Data to Server")
        }

        LazyColumn(modifier = Modifier.fillMaxSize()) {
            items(appTrafficData.value) { appData ->
                TrafficItem(appData) { appName ->
                    selectedAppName = appName
                    showChart = true
                }
            }
        }

        if (showChart) {
            HourlyTrafficChart(
                appName = selectedAppName,
                onClose = { showChart = false },
                context = context
            )
        }
    }
}


@Composable
fun TrafficItem(appData: AppTrafficData, onShowChart: (String) -> Unit) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Column {
            Text(text = appData.appName)
            Text(text = "Total: ${(appData.totalBytes / 1024).toString()} Kb")
            Text(text = "Mobile: ${(appData.mobileBytes / 1024).toString()} Kb")
            Text(text = "Wi-Fi: ${(appData.wifiBytes / 1024).toString()} Kb")
            Text(text = "Downlink: ${(appData.rxBytes / 1024).toString()} Kb")
            Text(text = "Uplink: ${(appData.txBytes / 1024).toString()} Kb")
        }
        Button(onClick = { onShowChart(appData.appName) }) {
            Text("Show Chart")
        }
    }
}