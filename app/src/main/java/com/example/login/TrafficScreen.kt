package com.example.login

import android.content.pm.PackageManager
import android.graphics.drawable.Drawable
import android.os.Build
import android.util.Log
import android.widget.ImageView
import androidx.annotation.RequiresApi
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.Button
import androidx.compose.material.CircularProgressIndicator
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

@Composable
fun TrafficScreen(state: MainActivity.MainActivityState) {
    val context = LocalContext.current
    val appTrafficData = remember { mutableStateOf(emptyList<AppTrafficData>()) }
    var days by remember { mutableStateOf("1") }
    var showError by remember { mutableStateOf(false) }
    var showAppChart by remember { mutableStateOf(false) }
    var showTotalChart by remember { mutableStateOf(false) }
    var showLineChart by remember { mutableStateOf(false) }
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

    LazyColumn(modifier = Modifier.fillMaxSize(), horizontalAlignment = Alignment.CenterHorizontally) {
        item {
            Text(
                text = "Total Traffic: ${(totalTrafficData.value.totalBytes / 1024)} Kb, " +
                        "Mobile: ${(totalTrafficData.value.mobileBytes / 1024)} Kb, " +
                        "Wi-Fi: ${(totalTrafficData.value.wifiBytes / 1024)} Kb",
                modifier = Modifier.padding(16.dp)
            )
        }

        item {
            OutlinedTextField(
                value = days,
                onValueChange = { days = it },
                label = { Text("Days") },
                modifier = Modifier.padding(16.dp)
            )
        }

        item {
            if (showError) {
                Text(
                    text = "Invalid number of days",
                    color = Color.Red,
                    modifier = Modifier.padding(16.dp)
                )
            }
        }

        item {
            if (!PermissionUtils.hasUsageStatsPermission(context)) {
                Button(onClick = { PermissionUtils.requestUsageStatsPermission(context) }) {
                    Text("Grant Usage Stats Permission")
                }
            }
        }

        item {
            Button(onClick = {
                isSendingTrafficData = true
                MainActivity.networkManager.authenticateForTraffic(state.Email, state.Password) { authResponse ->
                    if (authResponse != null) {
                        val top5Apps = appTrafficData.value.take(10)
                        MainActivity.networkManager.sendTrafficDataToServer(authResponse.jwt, top5Apps) { success ->
                            if (success) {
                                Log.d(MainActivity.TAG, "Traffic data sent successfully!")
                            } else {
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
        }

        item {
            Button(
                onClick = { showTotalChart = true },
                modifier = Modifier.padding(16.dp)
            ) {
                Text("Show Total Hourly Traffic")
            }
        }
        item {
            Button(
                onClick = { showLineChart = true },
                modifier = Modifier.padding(16.dp)
            ) {
                Text("Show Total Hourly Traffic (Line Chart)")
            }
        }

        items(appTrafficData.value, key = { appData -> appData.packageName }) { appData ->
            TrafficItem(appData) { appName ->
                selectedAppName = appName
                showAppChart = true
            }
        }
    }

    if (showAppChart) {
        HourlyTrafficChart(
            appName = selectedAppName,
            onClose = { showAppChart = false },
            context = context
        )
    }

    if (showTotalChart) {
        TotalHourlyTrafficChart(
            onClose = { showTotalChart = false },
            context = context
        )
    }
    if (showLineChart) {
        TotalHourlyTrafficLineChart(
            onClose = { showLineChart = false },
            context = context,
            days = 1
        )
    }
}


@RequiresApi(Build.VERSION_CODES.M)
@Composable
fun TrafficItem(appData: AppTrafficData, onShowChart: (String) -> Unit) {
    val context = LocalContext.current
    var icon by remember { mutableStateOf<Drawable?>(null) }
    val coroutineScope = rememberCoroutineScope()

    LaunchedEffect(appData) {
        coroutineScope.launch(Dispatchers.IO) {
            icon = try {
                context.packageManager.getApplicationIcon(appData.packageName)
            } catch (e: PackageManager.NameNotFoundException) {
                Log.e("TrafficItem", "Error loading icon for ${appData.packageName}", e)
                null
            }
        }
    }

    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            if (icon != null) {
                AndroidView(
                    factory = { context ->
                        ImageView(context).apply {
                            setImageDrawable(icon)
                        }
                    },
                    modifier = Modifier.size(48.dp)
                )
                Spacer(modifier = Modifier.width(8.dp))
            } else {
                CircularProgressIndicator(modifier = Modifier.size(48.dp))
            }
            Column {
                Text(text = appData.appName)
                Text(text = "Total: ${(appData.totalBytes / 1024).toString()} Kb")
                Text(text = "Mobile: ${(appData.mobileBytes / 1024).toString()} Kb")
                Text(text = "Wi-Fi: ${(appData.wifiBytes / 1024).toString()} Kb")
                Text(text = "Downlink: ${(appData.rxBytes / 1024).toString()} Kb")
                Text(text = "Uplink: ${(appData.txBytes / 1024).toString()} Kb")
            }
        }
        Button(onClick = { onShowChart(appData.appName) }) {
            Text("Show Chart")
        }
    }
}