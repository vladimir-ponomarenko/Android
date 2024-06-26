package com.example.login

import android.app.Activity
import android.content.Context
import android.content.pm.PackageManager
import android.net.ConnectivityManager
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Icon
import androidx.compose.material.IconButton
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Close
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.nativeCanvas
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import java.util.Calendar
import java.util.concurrent.TimeUnit

@RequiresApi(Build.VERSION_CODES.M)
@Composable
fun HourlyTrafficChart(appName: String, onClose: () -> Unit, context: Context) {
    val hourlyTrafficData = remember { mutableStateListOf<Pair<Int, Long>>() }
    val packageName = getPackageNameForApp(context, appName)

    LaunchedEffect(packageName) {
        if (packageName != null) {
            hourlyTrafficData.addAll(getHourlyTrafficData(context, packageName))
        }
    }

    Dialog(onDismissRequest = onClose) {
        Surface(shape = RoundedCornerShape(8.dp)) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(
                    text = "Hourly Traffic for $appName",
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold
                )

                IconButton(onClick = onClose, modifier = Modifier.align(Alignment.End)) {
                    Icon(imageVector = Icons.Default.Close, contentDescription = "Close")
                }

                if (packageName != null) {
                    HourlyTrafficChartContent(hourlyTrafficData)
                } else {
                    Text("Error: Package not found", color = Color.Red)
                }
            }
        }
    }
}


@RequiresApi(Build.VERSION_CODES.M)
fun getPackageNameForApp(context: Context, appName: String): String? {
    val packageManager = context.packageManager
    for (packageInfo in packageManager.getInstalledApplications(PackageManager.GET_META_DATA)) {
        val currentAppName = packageManager.getApplicationLabel(packageInfo).toString()
        if (currentAppName == appName) {
            return packageInfo.packageName
        }
    }
    return null
}

@RequiresApi(Build.VERSION_CODES.M)
@Composable
fun HourlyTrafficChartContent(hourlyTrafficData: List<Pair<Int, Long>>) {
    val scrollState = rememberScrollState()
    val hourWidth = 50.dp
    val chartWidth = hourWidth * 24
    val maxTraffic = hourlyTrafficData.maxOfOrNull { it.second } ?: 1L
    val maxTrafficKb = (maxTraffic / 1024).toFloat()

    Box(modifier = Modifier.horizontalScroll(scrollState)) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(8.dp)
        ) {
            Canvas(modifier = Modifier.height(200.dp).width(40.dp)) {
                val stepSize = maxTrafficKb / 5
                for (i in 0..5) {
                    val y = size.height - i * (size.height / 5)
                    drawContext.canvas.nativeCanvas.drawText(
                        String.format("%.0f", i * stepSize),
                        10f,
                        y,
                        android.graphics.Paint().apply {
                            textSize = 10.sp.toPx()
                            color = android.graphics.Color.BLACK
                            textAlign = android.graphics.Paint.Align.LEFT
                        }
                    )
                }
            }

            Canvas(modifier = Modifier.width(chartWidth).height(200.dp)) {
                hourlyTrafficData.forEachIndexed { index, (hour, traffic) ->
                    val x = index * hourWidth.toPx()
                    val trafficKb = (traffic / 1024).toFloat()
                    val barHeight = (trafficKb / maxTrafficKb * size.height).coerceAtLeast(0f)

                    drawRect(
                        color = Color.Blue,
                        topLeft = Offset(x, size.height - barHeight),
                        size = Size(
                            hourWidth.toPx() - 4.dp.toPx(),
                            barHeight
                        )
                    )

                    drawContext.canvas.nativeCanvas.drawText(
                        String.format("%02d:00", hour),
                        x + hourWidth.toPx() / 2,
                        size.height + 15f,
                        android.graphics.Paint().apply {
                            textSize = 10.sp.toPx()
                            color = android.graphics.Color.BLACK
                            textAlign = android.graphics.Paint.Align.CENTER
                        }
                    )

                    drawContext.canvas.nativeCanvas.drawText(
                        String.format("%.1f Kb", trafficKb),
                        x + hourWidth.toPx() / 2,
                        size.height - barHeight - 5.dp.toPx(),
                        android.graphics.Paint().apply {
                            textSize = 10.sp.toPx()
                            color = android.graphics.Color.BLACK
                            textAlign = android.graphics.Paint.Align.CENTER
                        }
                    )
                }
            }
        }
    }
}

@RequiresApi(Build.VERSION_CODES.M)
fun getHourlyTrafficData(context: Context, packageName: String): List<Pair<Int, Long>> {
    val calendar = Calendar.getInstance()
    val currentHour = calendar.get(Calendar.HOUR_OF_DAY)

    calendar.add(Calendar.DAY_OF_YEAR, -1)
    calendar.set(Calendar.HOUR_OF_DAY, currentHour)
    calendar.set(Calendar.MINUTE, 0)
    calendar.set(Calendar.SECOND, 0)
    calendar.set(Calendar.MILLISECOND, 0)
    val startTime = calendar.timeInMillis

    val endTime = System.currentTimeMillis()

    val hourlyTrafficData = MutableList(24) { 0L }
    val networkStatsManager = context.getSystemService(Context.NETWORK_STATS_SERVICE) as android.app.usage.NetworkStatsManager
    val packageManager = context.packageManager

    val uid = try {
        packageManager.getApplicationInfo(packageName, 0).uid
    } catch (e: PackageManager.NameNotFoundException) {
        Log.e(MainActivity.TAG, "Error: Package $packageName not found", e)
        return emptyList()
    }

    try {
        var previousTotalBytes = 0L

        for (hour in 0 until 24) {
            calendar.add(Calendar.HOUR_OF_DAY, 1)
            val currentHourEnd = calendar.timeInMillis

            var totalBytes = 0L
            totalBytes += getTrafficForHour(networkStatsManager, uid, startTime, currentHourEnd, android.net.ConnectivityManager.TYPE_MOBILE)
            totalBytes += getTrafficForHour(networkStatsManager, uid, startTime, currentHourEnd, android.net.ConnectivityManager.TYPE_WIFI)

            hourlyTrafficData[hour] = totalBytes - previousTotalBytes
            previousTotalBytes = totalBytes
        }
    } catch (e: Exception) {
        Log.e(MainActivity.TAG, "Error fetching network stats: ${e.message}", e)
    }

    return hourlyTrafficData.mapIndexed { index, traffic ->
        Pair((index + currentHour) % 24, traffic)
    }
}

@RequiresApi(Build.VERSION_CODES.M)
internal fun getTotalTrafficData(context: Context, days: Int): TotalTrafficData {
    var mobileBytes = 0L
    var wifiBytes = 0L
    val currentTime = System.currentTimeMillis()
    val startTime = currentTime - TimeUnit.DAYS.toMillis(days.toLong())

    val networkStatsManager = context.getSystemService(Activity.NETWORK_STATS_SERVICE) as android.app.usage.NetworkStatsManager

    try {
        val mobileStats = networkStatsManager.querySummaryForDevice(ConnectivityManager.TYPE_MOBILE, null, startTime, currentTime)
        mobileBytes = mobileStats.rxBytes + mobileStats.txBytes

        val wifiStats = networkStatsManager.querySummaryForDevice(ConnectivityManager.TYPE_WIFI, null, startTime, currentTime)
        wifiBytes = wifiStats.rxBytes + wifiStats.txBytes
    } catch (e: Exception) {
        Log.e(MainActivity.TAG, "Error fetching network stats", e)
    }

    return TotalTrafficData(mobileBytes + wifiBytes, mobileBytes, wifiBytes)
}
@RequiresApi(Build.VERSION_CODES.M)
private fun getTrafficForHour(networkStatsManager: android.app.usage.NetworkStatsManager, uid: Int, startTime: Long, endTime: Long, networkType: Int): Long {
    var totalBytes = 0L
    try {
        val networkStats = networkStatsManager.queryDetailsForUid(
            networkType,
            null,
            startTime,
            endTime,
            uid
        )
        while (networkStats.hasNextBucket()) {
            val bucket = android.app.usage.NetworkStats.Bucket()
            networkStats.getNextBucket(bucket)
            totalBytes += bucket.rxBytes + bucket.txBytes
        }
        networkStats.close()
    } catch (e: Exception) {
        Log.e(MainActivity.TAG, "Error fetching network stats: ${e.message}", e)
    }
    return totalBytes
}
@RequiresApi(Build.VERSION_CODES.M)
fun getAppTrafficData(context: Context, days: Int) {
    val appTrafficDataList = mutableListOf<AppTrafficData>()
    val packageManager = context.packageManager
    val networkStatsManager =
        context.getSystemService(Context.NETWORK_STATS_SERVICE) as android.app.usage.NetworkStatsManager

    val currentTime = System.currentTimeMillis()
    val startTime = currentTime - TimeUnit.DAYS.toMillis(days.toLong())

    for (packageInfo in packageManager.getInstalledApplications(PackageManager.GET_META_DATA)) {
        val uid = packageInfo.uid
        val appName = packageManager.getApplicationLabel(packageInfo).toString()

        var mobileBytes = 0L
        var wifiBytes = 0L
        var totalDownlinkBytes = 0L
        var totalUplinkBytes = 0L

        try {
            val mobileStats = networkStatsManager.queryDetailsForUid(
                ConnectivityManager.TYPE_MOBILE,
                null,
                startTime,
                currentTime,
                uid
            )
            var bucket = android.app.usage.NetworkStats.Bucket()
            while (mobileStats.hasNextBucket()) {
                mobileStats.getNextBucket(bucket)
                if (bucket.uid == uid) {
                    mobileBytes += bucket.rxBytes + bucket.txBytes
                    totalDownlinkBytes += bucket.rxBytes
                    totalUplinkBytes += bucket.txBytes
                }
            }
            mobileStats.close()

            val wifiStats = networkStatsManager.queryDetailsForUid(
                ConnectivityManager.TYPE_WIFI,
                null,
                startTime,
                currentTime,
                uid
            )
            bucket = android.app.usage.NetworkStats.Bucket()
            while (wifiStats.hasNextBucket()) {
                wifiStats.getNextBucket(bucket)
                if (bucket.uid == uid) {
                    wifiBytes += bucket.rxBytes + bucket.txBytes
                    totalDownlinkBytes += bucket.rxBytes
                    totalUplinkBytes += bucket.txBytes
                }
            }
            wifiStats.close()

            val totalBytes = mobileBytes + wifiBytes
            appTrafficDataList.add(
                AppTrafficData(
                    appName,
                    totalBytes,
                    mobileBytes,
                    wifiBytes,
                    totalDownlinkBytes,
                    totalUplinkBytes
                )
            )

        } catch (e: Exception) {
            Log.e("AppTraffic", "Error getting traffic data for $appName: ${e.message}", e)
        }
    }
}
