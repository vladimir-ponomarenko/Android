    @file:Suppress("DEPRECATION")

    package com.example.login

    //noinspection UsingMaterialAndMaterial3Libraries
    //noinspection UsingMaterialAndMaterial3Libraries
    //noinspection UsingMaterialAndMaterial3Libraries
    //noinspection UsingMaterialAndMaterial3Libraries
    import android.annotation.SuppressLint
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
import androidx.compose.foundation.layout.fillMaxSize
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
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.nativeCanvas
    import androidx.compose.ui.graphics.toArgb
    import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.util.Calendar
import java.util.concurrent.TimeUnit

    @Composable
    fun HourlyTrafficChart(appName: String, onClose: () -> Unit, context: Context) {
        val hourlyTrafficData = remember { mutableStateListOf<Pair<Int, Long>>() }
        val packageName = getPackageNameForApp(context, appName)

        LaunchedEffect(packageName) {
            launch(Dispatchers.IO) {
                if (packageName != null) {
                    val data = getHourlyTrafficData(context, packageName)
                    withContext(Dispatchers.Main) {
                        hourlyTrafficData.addAll(data)
                    }
                }
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
    @Composable
    fun TotalHourlyTrafficChart(onClose: () -> Unit, context: Context) {
        val hourlyTrafficData = remember { mutableStateListOf<Pair<Int, AppTrafficData>>() }

        LaunchedEffect(Unit) {
            launch(Dispatchers.IO) {
                val data = getTotalHourlyTrafficData(context)
                withContext(Dispatchers.Main) {
                    hourlyTrafficData.addAll(data)
                }
            }
        }

        Dialog(onDismissRequest = onClose) {
            Surface(shape = RoundedCornerShape(8.dp)) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text(
                        text = "Total Hourly Traffic",
                        fontSize = 18.sp,
                        fontWeight = FontWeight.Bold
                    )

                    IconButton(onClick = onClose, modifier = Modifier.align(Alignment.End)) {
                        Icon(imageVector = Icons.Default.Close, contentDescription = "Close")
                    }
                    TotalHourlyTrafficChartContent(hourlyTrafficData)
                }
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.M)
    @Composable
    fun TotalHourlyTrafficChartContent(hourlyTrafficData: List<Pair<Int, AppTrafficData>>) {
        val scrollState = rememberScrollState()
        val hourWidth = 50.dp
        val chartWidth = hourWidth * 24
        val maxTraffic = hourlyTrafficData.maxOfOrNull { it.second.totalBytes } ?: 1L
        val maxTrafficKb = (maxTraffic / 1024).toFloat()

        Row(modifier = Modifier.fillMaxWidth()) {
            Column(modifier = Modifier.height(200.dp).width(40.dp)) {
                Canvas(modifier = Modifier.fillMaxSize()) {
                    val stepSize = maxTrafficKb / 5
                    for (i in 0..5) {
                        val y = size.height - i * (size.height / 5)
                        drawLine(
                            color = Color.LightGray,
                            start = Offset(0f, y),
                            end = Offset(chartWidth.toPx(), y),
                            strokeWidth = 1f
                        )
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
            }

            Box(modifier = Modifier.horizontalScroll(scrollState)) {
                Canvas(modifier = Modifier.width(chartWidth).height(200.dp)) {
                    hourlyTrafficData.forEachIndexed { index, (hour, appTrafficData) ->
                        val x = index * hourWidth.toPx() + 40.dp.toPx()
                        val trafficKb = (appTrafficData.totalBytes / 1024).toFloat()
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
                            appTrafficData.appName,
                            x + hourWidth.toPx() / 2,
                            size.height - barHeight - 25.dp.toPx(),
                            android.graphics.Paint().apply {
                                textSize = 10.sp.toPx()
                                color = android.graphics.Color.BLACK
                                textAlign = android.graphics.Paint.Align.CENTER
                            }
                        )

                        drawContext.canvas.nativeCanvas.drawText(
                            "%.1f Kb".format(trafficKb),
                            x + hourWidth.toPx() / 2,
                            size.height - barHeight - 10.dp.toPx(),
                            android.graphics.Paint().apply {
                                textSize = 10.sp.toPx()
                                color = android.graphics.Color.BLACK
                                textAlign = android.graphics.Paint.Align.CENTER
                            }
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
                    }
                }
            }
        }
    }
    @RequiresApi(Build.VERSION_CODES.M)
    @Composable
    fun TotalHourlyTrafficLineChart(onClose: () -> Unit, context: Context) {
        val hourlyTrafficData = remember { mutableStateListOf<Pair<Int, AppTrafficData>>() }

        LaunchedEffect(Unit) {
            launch(Dispatchers.IO) {
                val data = getTotalHourlyTrafficData(context)
                withContext(Dispatchers.Main) {
                    hourlyTrafficData.addAll(data)
                }
            }
        }

        Dialog(onDismissRequest = onClose) {
            Surface(shape = RoundedCornerShape(8.dp)) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text(
                        text = "Total Hourly Traffic (Line Chart)",
                        fontSize = 18.sp,
                        fontWeight = FontWeight.Bold
                    )

                    IconButton(onClick = onClose, modifier = Modifier.align(Alignment.End)) {
                        Icon(imageVector = Icons.Default.Close, contentDescription = "Close")
                    }
                    TotalHourlyTrafficLineChartContent(hourlyTrafficData)
                }
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.M)
    @Composable
    fun TotalHourlyTrafficLineChartContent(hourlyTrafficData: List<Pair<Int, AppTrafficData>>) {
        val scrollState = rememberScrollState()
        val hourWidth = 60.dp
        val chartWidth = hourWidth * 24
        val maxTraffic = hourlyTrafficData.maxOfOrNull { it.second.totalBytes } ?: 1L
        val maxTrafficKb = (maxTraffic / 1024).toFloat()

        val currentHour = Calendar.getInstance().get(Calendar.HOUR_OF_DAY)

        val totalTrafficData = (0 until 24).map { hour ->
            Pair(hour, hourlyTrafficData.filter { it.first == (hour + currentHour) % 24 }.sumOf { it.second.totalBytes })
        }

        Box(modifier = Modifier.horizontalScroll(scrollState)) {
            Canvas(modifier = Modifier.width(chartWidth + 150.dp).height(200.dp)) {

                val stepSize = maxTrafficKb / 5
                for (i in 0..5) {
                    val y = size.height - i * (size.height / 5)
                    drawLine(
                        color = Color.LightGray,
                        start = Offset(0f, y),
                        end = Offset(chartWidth.toPx(), y),
                        strokeWidth = 1f
                    )
                    drawContext.canvas.nativeCanvas.drawText(
                        String.format("%.0f Kb", i * stepSize),
                        10f,
                        y,
                        android.graphics.Paint().apply {
                            textSize = 10.sp.toPx()
                            color = android.graphics.Color.BLACK
                            textAlign = android.graphics.Paint.Align.LEFT
                        }
                    )
                }

                drawTrafficLine(totalTrafficData, maxTrafficKb, Color.Black, "Total")

                val topAppNames = hourlyTrafficData
                    .groupBy { it.second.appName }
                    .mapValues { it.value.sumOf { it.second.totalBytes } }
                    .entries
                    .sortedByDescending { it.value }
                    .take(10)
                    .map { it.key }

                val lineColors = topAppNames.mapIndexed { index, _ ->
                    when (index) {
                        0 -> Color.Red
                        1 -> Color.Green
                        2 -> Color.Blue
                        else -> Color(
                            (0..255).random(),
                            (0..255).random(),
                            (0..255).random()
                        )
                    }
                }

                topAppNames.forEachIndexed { index, appName ->
                    val appTrafficData = hourlyTrafficData.filter { it.second.appName == appName }

                    val appTrafficByHour = (0 until 24).map { hour ->
                        val dataForHour = appTrafficData.find { it.first == (hour + currentHour) % 24 }
                        dataForHour?.let { Pair(it.first, it.second.totalBytes) } ?: Pair(hour, 0L)
                    }

                    drawTrafficLine(appTrafficByHour, maxTrafficKb, lineColors[index], appName)
                }

                val legendSpacing = 15.dp
                var legendY = 10.dp
                drawContext.canvas.nativeCanvas.drawText(
                    "Apps:",
                    chartWidth.toPx() + 10.dp.toPx(),
                    legendY.toPx(),
                    android.graphics.Paint().apply {
                        textSize = 10.sp.toPx()
                        color = android.graphics.Color.BLACK
                        textAlign = android.graphics.Paint.Align.LEFT
                        typeface = android.graphics.Typeface.DEFAULT_BOLD
                    }
                )
                legendY += legendSpacing * 2

                topAppNames.forEachIndexed { index, appName ->
                    drawRect(
                        color = lineColors[index],
                        topLeft = Offset(chartWidth.toPx() + 10.dp.toPx(), legendY.toPx()),
                        size = Size(20.dp.toPx(), 10.dp.toPx())
                    )

                    drawContext.canvas.nativeCanvas.drawText(
                        appName,
                        chartWidth.toPx() + 40.dp.toPx(),
                        legendY.toPx() + 8.dp.toPx(),
                        android.graphics.Paint().apply {
                            textSize = 8.sp.toPx()
                            color = android.graphics.Color.BLACK
                            textAlign = android.graphics.Paint.Align.LEFT
                        }
                    )
                    legendY += legendSpacing
                }

                for (hour in 0 until 24) {
                    val adjustedHour = (hour + currentHour) % 24
                    val x = hour * hourWidth.toPx()
                    drawContext.canvas.nativeCanvas.drawText(
                        String.format("%02d:00", adjustedHour),
                        x + hourWidth.toPx() / 2,
                        size.height + 15f,
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

    @RequiresApi(Build.VERSION_CODES.M)
    private fun DrawScope.drawTrafficLine(
        trafficData: List<Pair<Int, Long>>,
        maxTrafficKb: Float,
        lineColor: Color,
        lineLabel: String = ""
    ) {
        var previousPoint: Offset? = null
        trafficData.forEachIndexed { index, (hour, traffic) ->
            val x = index * 60.dp.toPx() + 40.dp.toPx()
            val trafficKb = (traffic / 1024).toFloat()
            val y = size.height - (trafficKb / maxTrafficKb * size.height).coerceAtLeast(0f)
            val currentPoint = Offset(x, y)

            if (previousPoint != null) {
                drawLine(
                    color = lineColor,
                    start = previousPoint!!,
                    end = currentPoint,
                    strokeWidth = 2f
                )
            }
            previousPoint = currentPoint
        }

        if (lineLabel.isNotBlank()) {
            val lastHourData = trafficData.lastOrNull()
            if (lastHourData != null) {
                val lastHourIndex = lastHourData.first
                val lastX = lastHourIndex * 60.dp.toPx() + 40.dp.toPx()
                drawContext.canvas.nativeCanvas.drawText(
                    lineLabel,
                    lastX + 10.dp.toPx(),
                    size.height - (lastHourData.second / 1024f / maxTrafficKb * size.height).coerceAtLeast(0f),
                    android.graphics.Paint().apply {
                        textSize = 8.sp.toPx()
                        color = lineColor.toArgb()
                        textAlign = android.graphics.Paint.Align.LEFT
                    }
                )
            }
        }
    }
    @RequiresApi(Build.VERSION_CODES.M)
    fun getTotalHourlyTrafficData(context: Context): List<Pair<Int, AppTrafficData>> {
        val result = mutableListOf<Pair<Int, AppTrafficData>>()
        val packageManager = context.packageManager
        val networkStatsManager = context.getSystemService(Context.NETWORK_STATS_SERVICE) as android.app.usage.NetworkStatsManager

        val calendarStart = Calendar.getInstance()
        calendarStart.add(Calendar.DAY_OF_YEAR, -1)
        val startTime = calendarStart.timeInMillis

        val trafficData = mutableListOf<TrafficDataPoint>()
        val endTime = startTime + TimeUnit.DAYS.toMillis(1)

        for (packageInfo in packageManager.getInstalledApplications(PackageManager.GET_META_DATA)) {
            val uid = packageInfo.uid
            val appName = packageManager.getApplicationLabel(packageInfo).toString()

            for (networkType in listOf(ConnectivityManager.TYPE_MOBILE, ConnectivityManager.TYPE_WIFI)) {
                try {
                    val networkStats = networkStatsManager.queryDetailsForUid(networkType, null, startTime, endTime, uid)
                    while (networkStats.hasNextBucket()) {
                        val bucket = android.app.usage.NetworkStats.Bucket()
                        networkStats.getNextBucket(bucket)
                        trafficData.add(TrafficDataPoint(appName, bucket.rxBytes + bucket.txBytes, bucket.endTimeStamp))
                    }
                    networkStats.close()
                } catch (e: Exception) {
                    Log.e(MainActivity.TAG, "Error fetching traffic data: ${e.message}", e)
                    continue
                }
            }
        }

        trafficData.sortBy { it.timestamp }

        var currentHour = (calendarStart.get(Calendar.HOUR_OF_DAY) + 1) % 24
        var currentHourTraffic = mutableMapOf<String, Long>()
        var maxTrafficApp: AppTrafficData? = null
        var maxTraffic = 0L

        val hours = (0..23).toList()

        for (hour in hours.subList(currentHour, hours.size) + hours.subList(0, currentHour)) {
            val hourData = trafficData.filter { dataPoint ->
                val calendar = Calendar.getInstance()
                calendar.timeInMillis = dataPoint.timestamp
                val dataHour = calendar.get(Calendar.HOUR_OF_DAY)
                dataHour == hour || (dataHour == (hour - 1 + 24) % 24 && calendar.get(Calendar.MINUTE) >= 55)
            }

            for (dataPoint in hourData) {
                currentHourTraffic[dataPoint.appName] = (currentHourTraffic[dataPoint.appName] ?: 0L) + dataPoint.bytes

                if (currentHourTraffic[dataPoint.appName]!! > maxTraffic) {
                    maxTraffic = currentHourTraffic[dataPoint.appName]!!
                    maxTrafficApp = AppTrafficData(
                        dataPoint.appName,
                        getPackageNameForApp(context, dataPoint.appName) ?: "",
                        maxTraffic,
                        0L,
                        0L,
                        0L,
                        0L
                    )
                }
            }

            if (maxTrafficApp != null) {
                val resultHour = (hour - 1 + 24) % 24
                result.add(Pair(resultHour, maxTrafficApp))
            }

            currentHourTraffic = mutableMapOf()
            maxTrafficApp = null
            maxTraffic = 0L
        }

        return result
    }

    data class TrafficDataPoint(val appName: String, val bytes: Long, val timestamp: Long)

    @SuppressLint("QueryPermissionsNeeded")
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

        val filteredData = hourlyTrafficData.filter { it.second > 0 }

        val chartWidth = hourWidth * filteredData.size
        val maxTraffic = filteredData.maxOfOrNull { it.second } ?: 1L
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
                    filteredData.forEachIndexed { index, (hour, traffic) ->
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
    @SuppressLint("QueryPermissionsNeeded")
    @RequiresApi(Build.VERSION_CODES.M)
    fun getAppTrafficData(context: Context, days: Int): List<AppTrafficData> {
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
                        packageInfo.packageName,
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

        return appTrafficDataList
    }