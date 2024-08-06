@file:Suppress("DEPRECATION")

package com.example.login

//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
import android.annotation.SuppressLint
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
import androidx.compose.material.Tab
import androidx.compose.material.TabRow
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.nativeCanvas
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import kotlin.math.ceil

@SuppressLint("AutoboxingStateCreation")
@Composable
fun RSRPGraph(state: MainActivity.MainActivityState) {
    var selectedTabIndex by remember { mutableStateOf(0) }

    Column(modifier = Modifier.fillMaxSize()) {
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

        when (selectedTabIndex) {
            0 -> LteChartsContent(state)
            1 -> GsmChartsContent(state)
            2 -> WcdmaChartsContent(state)
            3 -> CdmaChartsContent(state)
            4 -> NrChartsContent(state)
        }
    }
}
@Composable
fun LteChartsContent(state: MainActivity.MainActivityState) {
    var lastTimestamp by remember { mutableStateOf(0L) }

    LaunchedEffect(Unit) {
        while (true) {
            val currentTimestamp = System.currentTimeMillis()
            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
                lastTimestamp = currentTimestamp

                val cellInfo = state.messageToData2?.lte?.cellInfoList?.firstOrNull()
                cellInfo?.let {
                    addChartData(state.rsrpData, it.rsrp?.toString() ?: "0", currentTimestamp)
                    addChartData(state.rssiData, it.rssi?.toString() ?: "0", currentTimestamp)
                    addChartData(state.rsrqData, it.rsrq?.toString() ?: "0", currentTimestamp)
                }
            }
            delay(MainActivity.UPDATE_INTERVAL)
        }
    }

    Column(modifier = Modifier.fillMaxSize()) {
        ChartContent(state.rsrpData, "RSRP", state)
        ChartContent(state.rssiData, "RSSI", state)
        ChartContent(state.rsrqData, "RSRQ", state)
    }
}

@Composable
fun GsmChartsContent(state: MainActivity.MainActivityState) {
    var lastTimestamp by remember { mutableStateOf(0L) }

    LaunchedEffect(Unit) {
        while (true) {
            val currentTimestamp = System.currentTimeMillis()
            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
                lastTimestamp = currentTimestamp

                val cellInfo = state.messageToData2?.gsm?.cellInfoList?.firstOrNull()
                cellInfo?.let {
                    addChartData(state.rssiDataGsm, it.rssi?.toString() ?: "0", currentTimestamp)
                }
            }
            delay(MainActivity.UPDATE_INTERVAL)
        }
    }
    Column(modifier = Modifier.fillMaxSize()) {
        ChartContent(state.rssiDataGsm, "RSSI", state)
    }
}

@Composable
fun WcdmaChartsContent(state: MainActivity.MainActivityState) {
    var lastTimestamp by remember { mutableStateOf(0L) }

    LaunchedEffect(Unit) {
        while (true) {
            val currentTimestamp = System.currentTimeMillis()
            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
                lastTimestamp = currentTimestamp

                val cellInfo = state.messageToData2?.wcdma?.cellInfoList?.firstOrNull()
                cellInfo?.let {
                    addChartData(state.rssiDataWcdma, it.rssi?.toString() ?: "0", currentTimestamp)
                    addChartData(state.rscpDataWcdma, it.rscp?.toString() ?: "0", currentTimestamp)
                }
            }
            delay(MainActivity.UPDATE_INTERVAL)
        }
    }
    Column(modifier = Modifier.fillMaxSize()) {
        ChartContent(state.rssiDataWcdma, "RSSI", state)
        ChartContent(state.rscpDataWcdma, "RSCP", state)
    }
}

@Composable
fun CdmaChartsContent(state: MainActivity.MainActivityState) {
    var lastTimestamp by remember { mutableStateOf(0L) }

    LaunchedEffect(Unit) {
        while (true) {
            val currentTimestamp = System.currentTimeMillis()
            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
                lastTimestamp = currentTimestamp

                val cellInfo = state.messageToData2?.cdma?.cellInfoList?.firstOrNull()
                cellInfo?.let {
                    addChartData(state.rssiDataCdma, it.rssi?.toString() ?: "0", currentTimestamp)
                }
            }
            delay(MainActivity.UPDATE_INTERVAL)
        }
    }
    Column(modifier = Modifier.fillMaxSize()) {
        ChartContent(state.rssiDataCdma, "RSSI", state)
    }
}

@Composable
fun NrChartsContent(state: MainActivity.MainActivityState) {
    var lastTimestamp by remember { mutableStateOf(0L) }

    LaunchedEffect(Unit) {
        while (true) {
            val currentTimestamp = System.currentTimeMillis()
            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
                lastTimestamp = currentTimestamp

                val cellInfo = state.messageToData2?.nr?.cellInfoList?.firstOrNull()
                cellInfo?.let {
                    addChartData(state.rssiDataNr, it.csiRsrp?.toString() ?: "0", currentTimestamp)
                }
            }
            delay(MainActivity.UPDATE_INTERVAL)
        }
    }
    Column(modifier = Modifier.fillMaxSize()) {
        ChartContent(state.rssiDataNr, "RSSI", state)
    }
}

internal fun addChartData(chartData: MutableList<Pair<Long, Float>>, value: String, timestamp: Long) {
    val chartValue = value.replace(" dBm", "").replace(" dB", "").toFloatOrNull() ?: 0f

    val lastIndex = chartData.indexOfLast { it.first < timestamp }

    if (lastIndex != -1) {
        val lastTimestamp = chartData[lastIndex].first
        val timeDifference = timestamp - lastTimestamp

        if (timeDifference > MainActivity.UPDATE_INTERVAL) {
            val missedUpdatesCount = (timeDifference / MainActivity.UPDATE_INTERVAL).toInt() - 1
            for (i in 1..missedUpdatesCount) {
                val missedTimestamp = lastTimestamp + i * MainActivity.UPDATE_INTERVAL
                chartData.add(lastIndex + i, Pair(missedTimestamp, chartValue))
            }
        } else {
            chartData.add(Pair(timestamp, chartValue))
        }
    } else {
        chartData.add(Pair(timestamp, chartValue))
    }

    while (chartData.size > 6) {
        chartData.removeAt(0)
    }
}

@Composable
fun ChartContent(chartData: List<Pair<Long, Float>>, chartTitle: String, state: MainActivity.MainActivityState) {
    val scrollState = rememberScrollState()
    val context = LocalContext.current

    val hourWidth = 50.dp
    val chartWidth = hourWidth * 24

    val maxAbsChartValue = chartData.maxOfOrNull { kotlin.math.abs(it.second) } ?: 0f

    val maxValue = -ceil(maxAbsChartValue / 10) * 10

    Column(modifier = Modifier.padding(16.dp)) {
        Text(
            text = chartTitle,
            fontSize = 18.sp,
            fontWeight = FontWeight.Bold
        )

        Box(modifier = Modifier.horizontalScroll(scrollState)) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(8.dp)
            ) {
                Canvas(modifier = Modifier.height(200.dp).width(40.dp)) {
                    val stepSize = maxValue / 5f
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
                    chartData.forEachIndexed { index, (timestamp, chartValue) ->
                        val x = index * hourWidth.toPx()

                        val barHeight = (chartValue / maxValue * size.height).coerceAtLeast(0f)

                        drawRect(
                            color = Color.Blue,
                            topLeft = Offset(x, size.height - barHeight),
                            size = Size(
                                hourWidth.toPx() - 4.dp.toPx(),
                                barHeight
                            )
                        )

                        val date = Date(timestamp)
                        val format = SimpleDateFormat("HH:mm:ss", Locale.getDefault())
                        val formattedTime = format.format(date)
                        drawContext.canvas.nativeCanvas.drawText(
                            formattedTime,
                            x + hourWidth.toPx() / 2,
                            size.height + 15f,
                            android.graphics.Paint().apply {
                                textSize = 10.sp.toPx()
                                color = android.graphics.Color.BLACK
                                textAlign = android.graphics.Paint.Align.CENTER
                            }
                        )

                        val textValue = String.format("%.1f", chartValue)
                        drawContext.canvas.nativeCanvas.drawText(
                            textValue,
                            x + hourWidth.toPx() / 2,
                            size.height - barHeight - 10.dp.toPx(),
                            android.graphics.Paint().apply {
                                textSize = 11.sp.toPx()
                                color = android.graphics.Color.BLACK
                                textAlign = android.graphics.Paint.Align.CENTER
                            }
                        )

                        val textId = "ID: ${state.Cellid}"
                        drawContext.canvas.nativeCanvas.drawText(
                            textId,
                            x + hourWidth.toPx() / 2,
                            size.height - barHeight - 2.dp.toPx(),
                            android.graphics.Paint().apply {
                                textSize = 7.sp.toPx()
                                color = android.graphics.Color.BLACK
                                textAlign = android.graphics.Paint.Align.CENTER
                            }
                        )
                    }
                }
            }
        }
    }
}