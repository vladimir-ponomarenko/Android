//@file:Suppress("DEPRECATION")
//
//package com.example.login
//
////noinspection UsingMaterialAndMaterial3Libraries
////noinspection UsingMaterialAndMaterial3Libraries
////noinspection UsingMaterialAndMaterial3Libraries
////noinspection UsingMaterialAndMaterial3Libraries
////noinspection UsingMaterialAndMaterial3Libraries
////noinspection UsingMaterialAndMaterial3Libraries
////noinspection UsingMaterialAndMaterial3Libraries
//import android.annotation.SuppressLint
//import androidx.compose.foundation.Canvas
//import androidx.compose.foundation.background
//import androidx.compose.foundation.horizontalScroll
//import androidx.compose.foundation.layout.Arrangement
//import androidx.compose.foundation.layout.Box
//import androidx.compose.foundation.layout.Column
//import androidx.compose.foundation.layout.Row
//import androidx.compose.foundation.layout.Spacer
//import androidx.compose.foundation.layout.fillMaxSize
//import androidx.compose.foundation.layout.fillMaxWidth
//import androidx.compose.foundation.layout.height
//import androidx.compose.foundation.layout.padding
//import androidx.compose.foundation.layout.size
//import androidx.compose.foundation.layout.width
//import androidx.compose.foundation.rememberScrollState
//import androidx.compose.foundation.verticalScroll
//import androidx.compose.material.IconButton
//import androidx.compose.material.Tab
//import androidx.compose.material.TabRow
//import androidx.compose.material.Text
//import androidx.compose.material.icons.Icons
//import androidx.compose.material.icons.filled.ArrowForward
//import androidx.compose.material.icons.filled.Close
//import androidx.compose.material3.AlertDialog
//import androidx.compose.material3.Button
//import androidx.compose.material3.Icon
//import androidx.compose.runtime.Composable
//import androidx.compose.runtime.LaunchedEffect
//import androidx.compose.runtime.getValue
//import androidx.compose.runtime.mutableStateMapOf
//import androidx.compose.runtime.mutableStateOf
//import androidx.compose.runtime.remember
//import androidx.compose.runtime.rememberCoroutineScope
//import androidx.compose.runtime.saveable.Saver
//import androidx.compose.runtime.saveable.rememberSaveable
//import androidx.compose.runtime.setValue
//import androidx.compose.ui.Alignment
//import androidx.compose.ui.Modifier
//import androidx.compose.ui.geometry.Offset
//import androidx.compose.ui.geometry.Size
//import androidx.compose.ui.graphics.Color
//import androidx.compose.ui.graphics.nativeCanvas
//import androidx.compose.ui.platform.LocalContext
//import androidx.compose.ui.text.TextStyle
//import androidx.compose.ui.text.drawText
//import androidx.compose.ui.text.font.FontWeight
//import androidx.compose.ui.text.rememberTextMeasurer
//import androidx.compose.ui.unit.dp
//import androidx.compose.ui.unit.sp
//import androidx.compose.ui.unit.times
//import kotlinx.coroutines.delay
//import kotlinx.coroutines.launch
//import java.text.SimpleDateFormat
//import java.util.Date
//import java.util.Locale
//import kotlin.math.ceil
//import kotlin.math.floor
//
//@SuppressLint("AutoboxingStateCreation")
//@Composable
//fun RSRPGraph(state: MainActivity.MainActivityState) {
//    var selectedTabIndex by remember { mutableStateOf(0) }
//
//    Column(modifier = Modifier.fillMaxSize()) {
//        TabRow(selectedTabIndex = selectedTabIndex) {
//            Tab(
//                text = { Text("LTE") },
//                selected = selectedTabIndex == 0,
//                onClick = { selectedTabIndex = 0 }
//            )
//            Tab(
//                text = { Text("GSM") },
//                selected = selectedTabIndex == 1,
//                onClick = { selectedTabIndex = 1 }
//            )
//            Tab(
//                text = { Text("WCDMA") },
//                selected = selectedTabIndex == 2,
//                onClick = { selectedTabIndex = 2 }
//            )
//            Tab(
//                text = { Text("CDMA") },
//                selected = selectedTabIndex == 3,
//                onClick = { selectedTabIndex = 3 }
//            )
//            Tab(
//                text = { Text("NR") },
//                selected = selectedTabIndex == 4,
//                onClick = { selectedTabIndex = 4 }
//            )
//        }
//
//        when (selectedTabIndex) {
//            0 -> LteChartsContent(state)
//            1 -> GsmChartsContent(state)
//            2 -> WcdmaChartsContent(state)
//            3 -> CdmaChartsContent(state)
//            4 -> NrChartsContent(state)
//        }
//    }
//}
//
//@Composable
//fun LteChartsContent(state: MainActivity.MainActivityState) {
//    var lastTimestamp by remember { mutableStateOf(0L) }
//
//    LaunchedEffect(Unit) {
//        while (true) {
//            val currentTimestamp = System.currentTimeMillis()
//            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
//                lastTimestamp = currentTimestamp
//
//                val cellInfo = state.messageToData2?.lte?.cellInfoList?.firstOrNull()
//                cellInfo?.let {
//                    addChartData(state.rsrpData, it.rsrp?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.rssiData, it.rssi?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.rsrqData, it.rsrq?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.asuLevelLTE, it.asuLevel?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.levelLTE, it.level?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.earfcnLTE, it.earfcn?.toString() ?: "0", currentTimestamp)
///*                    addChartDataForDetailedChart(state.rsrpDetailedData, it.rsrp?.toString() ?: "0", currentTimestamp, it.ci.toString())
//                    addChartDataForDetailedChart(state.rssiDetailedData, it.rssi?.toString() ?: "0", currentTimestamp, it.ci.toString())
//                    addChartDataForDetailedChart(state.rsrqDetailedData, it.rsrq?.toString() ?: "0", currentTimestamp, it.ci.toString())
//                    addChartDataForDetailedChart(state.asuLevelLTEDetailed, it.asuLevel?.toString() ?: "0", currentTimestamp, it.ci.toString())
//                    addChartDataForDetailedChart(state.levelLTEDetailed, it.level?.toString() ?: "0", currentTimestamp, it.ci.toString())
//                    addChartDataForDetailedChart(state.earfcnLTEDetailed, it.earfcn?.toString() ?: "0", currentTimestamp, it.ci.toString())*/
//                }
//            }
//            delay(MainActivity.UPDATE_INTERVAL)
//        }
//    }
//
//    Column(modifier = Modifier.verticalScroll(rememberScrollState())) {
//        ChartContent(state.rsrpData, "RSRP ", state, state.rsrpDetailedData, "LTE")
//        ChartContent(state.rssiData, "RSSI ", state, state.rssiDetailedData, "LTE")
//        ChartContent(state.rsrqData, "RSRQ ", state, state.rsrqDetailedData, "LTE")
//        ChartContent(state.asuLevelLTE, "AsuLevel ", state, state.asuLevelLTEDetailed, "LTE")
//        ChartContent(state.levelLTE, "Level ", state, state.levelLTEDetailed, "LTE")
//        ChartContent(state.earfcnLTE, "Earfcn ", state, state.earfcnLTEDetailed, "LTE")
//    }
//}
//
//@Composable
//fun GsmChartsContent(state: MainActivity.MainActivityState) {
//    var lastTimestamp by remember { mutableStateOf(0L) }
//
//    LaunchedEffect(Unit) {
//        while (true) {
//            val currentTimestamp = System.currentTimeMillis()
//            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
//                lastTimestamp = currentTimestamp
//
//                val cellInfo = state.messageToData2?.gsm?.cellInfoList?.firstOrNull()
//                cellInfo?.let {
//                    addChartData(state.rssiDataGsm, it.rssi?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.arfcnGsm, it.arfcn?.toString() ?: "0", currentTimestamp)
///*                    addChartDataForDetailedChart(state.rssiDataGsmDetailed, it.rssi?.toString() ?: "0", currentTimestamp, it.cid.toString())
//                    addChartDataForDetailedChart(state.arfcnGsmDetailed, it.arfcn?.toString() ?: "0", currentTimestamp, it.cid.toString())*/
//                }
//            }
//            delay(MainActivity.UPDATE_INTERVAL)
//        }
//    }
//    Column(modifier = Modifier.verticalScroll(rememberScrollState())) {
//        ChartContent(state.rssiDataGsm, "RSSI ", state, state.rssiDataGsmDetailed, "GSM")
//        ChartContent(state.arfcnGsm, "ARFCN ", state, state.arfcnGsmDetailed, "GSM")
//    }
//}
//
//@Composable
//fun WcdmaChartsContent(state: MainActivity.MainActivityState) {
//    var lastTimestamp by remember { mutableStateOf(0L) }
//
//    LaunchedEffect(Unit) {
//        while (true) {
//            val currentTimestamp = System.currentTimeMillis()
//            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
//                lastTimestamp = currentTimestamp
//
//                val cellInfo = state.messageToData2?.wcdma?.cellInfoList?.firstOrNull()
//                cellInfo?.let {
//                    addChartData(state.rscpDataWcdma, it.rscp?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.levelWcdma, it.level?.toString() ?: "0", currentTimestamp)
///*                    addChartDataForDetailedChart(state.rscpDataWcdmaDetailed, it.rscp?.toString() ?: "0", currentTimestamp, it.cid.toString())
//                    addChartDataForDetailedChart(state.levelWcdmaDetailed, it.level?.toString() ?: "0", currentTimestamp, it.cid.toString())*/
//                }
//            }
//            delay(MainActivity.UPDATE_INTERVAL)
//        }
//    }
//    Column(modifier = Modifier.verticalScroll(rememberScrollState())) {
//        ChartContent(state.rscpDataWcdma, "RSCP ", state, state.rscpDataWcdmaDetailed, "WCDMA")
//        ChartContent(state.levelWcdma, "Level ", state, state.levelWcdmaDetailed, "WCDMA")
//    }
//}
//
//@Composable
//fun CdmaChartsContent(state: MainActivity.MainActivityState) {
//    var lastTimestamp by remember { mutableStateOf(0L) }
//
//    LaunchedEffect(Unit) {
//        while (true) {
//            val currentTimestamp = System.currentTimeMillis()
//            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
//                lastTimestamp = currentTimestamp
//
//                val cellInfo = state.messageToData2?.cdma?.cellInfoList?.firstOrNull()
//                cellInfo?.let {
//                    addChartData(state.rssiDataCdma, it.rssi?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.levelCdma, it.level?.toString() ?: "0", currentTimestamp)
///*                    addChartDataForDetailedChart(state.rssiDataCdmaDetailed, it.rssi?.toString() ?: "0", currentTimestamp, it.bsid.toString())
//                    addChartDataForDetailedChart(state.levelCdmaDetailed, it.level?.toString() ?: "0", currentTimestamp, it.bsid.toString())*/
//                }
//            }
//            delay(MainActivity.UPDATE_INTERVAL)
//        }
//    }
//    Column(modifier = Modifier.verticalScroll(rememberScrollState())) {
//        ChartContent(state.rssiDataCdma, "RSSI ", state, state.rssiDataCdmaDetailed, "CDMA")
//        ChartContent(state.levelCdma, "Level ", state, state.levelCdmaDetailed, "CDMA")
//    }
//}
//
//@Composable
//fun NrChartsContent(state: MainActivity.MainActivityState) {
//    var lastTimestamp by remember { mutableStateOf(0L) }
//
//    LaunchedEffect(Unit) {
//        while (true) {
//            val currentTimestamp = System.currentTimeMillis()
//            if (currentTimestamp - lastTimestamp >= MainActivity.UPDATE_INTERVAL) {
//                lastTimestamp = currentTimestamp
//
//                val cellInfo = state.messageToData2?.nr?.cellInfoList?.firstOrNull()
//                cellInfo?.let {
//                    addChartData(state.rssiDataNr, it.csiRsrp?.toString() ?: "0", currentTimestamp)
//                    addChartData(state.asuLevelNr, it.asuLevel?.toString() ?: "0", currentTimestamp)
///*                    addChartDataForDetailedChart(state.rssiDataNrDetailed, it.csiRsrp?.toString() ?: "0", currentTimestamp, it.nci.toString())
//                    addChartDataForDetailedChart(state.asuLevelNrDetailed, it.asuLevel?.toString() ?: "0", currentTimestamp, it.nci.toString())*/
//                }
//            }
//            delay(MainActivity.UPDATE_INTERVAL)
//        }
//    }
//    Column(modifier = Modifier.verticalScroll(rememberScrollState())) {
//        ChartContent(state.rssiDataNr, "RSSI ", state, state.rssiDataNrDetailed, "NR")
//        ChartContent(state.asuLevelNr, "AsuLevel ", state, state.asuLevelNrDetailed, "NR")
//    }
//}
//internal fun addChartData(chartData: MutableList<Pair<Long, Float>>, value: String, timestamp: Long) {
//    val chartValue = value.replace(" dBm", "").replace(" dB", "").toFloatOrNull() ?: 0f
//
//    val lastIndex = chartData.indexOfLast { it.first < timestamp }
//
//    if (lastIndex != -1) {
//        val lastTimestamp = chartData[lastIndex].first
//        val timeDifference = timestamp - lastTimestamp
//
//        if (timeDifference > MainActivity.UPDATE_INTERVAL) {
//            val missedUpdatesCount = (timeDifference / MainActivity.UPDATE_INTERVAL).toInt() - 1
//            for (i in 1..missedUpdatesCount) {
//                val missedTimestamp = lastTimestamp + i * MainActivity.UPDATE_INTERVAL
//                chartData.add(lastIndex + i, Pair(missedTimestamp, chartValue))
//            }
//        } else {
//            chartData.add(Pair(timestamp, chartValue))
//        }
//    } else {
//        chartData.add(Pair(timestamp, chartValue))
//    }
//
//    while (chartData.size > 6) {
//        chartData.removeAt(0)
//    }
//}
//data class ChartDataPoint(val timestamp: Long, val value: Float, val cellId: String)
//
//internal fun addChartDataForDetailedChart(chartData: MutableList<ChartDataPoint>, value: String, timestamp: Long, cellId: String) {
//    val chartValue = value.replace(" dBm", "").replace(" dB", "").toFloatOrNull() ?: 0f
//
//    chartData.add(ChartDataPoint(timestamp, chartValue, cellId))
//
//    while (chartData.size > 1500) {
//        chartData.removeAt(0)
//    }
//}
//
//@Composable
//fun ChartContent(
//    chartData: List<Pair<Long, Float>>,
//    chartTitle: String,
//    state: MainActivity.MainActivityState,
//    detailedChartData: List<ChartDataPoint> = emptyList(),
//    networkType: String
//) {
//    val scrollState = rememberScrollState()
//    val context = LocalContext.current
//    var showDialog by remember { mutableStateOf(false) }
//
//    val hourWidth = 50.dp
//    val chartWidth = hourWidth * 24
//
//    val maxAbsChartValue = chartData.maxOfOrNull { kotlin.math.abs(it.second) } ?: 0f
//
//    val maxValue = if (chartData.any { it.second > 0 }) {
//        ceil(maxAbsChartValue / 10) * 10
//    } else {
//        -ceil(maxAbsChartValue / 10) * 10
//    }
//
//    Column(modifier = Modifier.padding(16.dp)) {
//        Row(modifier = Modifier.fillMaxWidth()) {
//            Text(
//                text = chartTitle,
//                fontSize = 18.sp,
//                fontWeight = FontWeight.Bold
//            )
//            Spacer(modifier = Modifier.weight(1f))
//            Button(onClick = { showDialog = true }) {
//                Text("График")
//            }
//        }
//
//        if (showDialog) {
//            AlertDialog(
//                onDismissRequest = { showDialog = false },
//                title = { Text(chartTitle) },
//                text = {
//                    DetailedChartContent(detailedChartData, state, onDismiss = { showDialog = false }, networkType)
//                },
//                confirmButton = {
//                }
//            )
//        }
//    }
//    Box {
//        Row(
//            modifier = Modifier
//                .fillMaxWidth()
//                .padding(8.dp)
//        ) {
//            Canvas(modifier = Modifier.height(200.dp).width(40.dp)) {
//                val stepSize = maxValue / 5f
//                for (i in 0..5) {
//                    val y = size.height - i * (size.height / 5)
//                    drawContext.canvas.nativeCanvas.drawText(
//                        String.format("%.0f", i * stepSize),
//                        10f,
//                        y,
//                        android.graphics.Paint().apply {
//                            textSize = 10.sp.toPx()
//                            color = android.graphics.Color.BLACK
//                            textAlign = android.graphics.Paint.Align.LEFT
//                        }
//                    )
//                }
//            }
//
//            Canvas(modifier = Modifier.width(chartWidth).height(200.dp)) {
//                chartData.forEachIndexed { index, (timestamp, chartValue) ->
//                    val x = index * hourWidth.toPx()
//
//                    val barHeight = (chartValue / maxValue * size.height).coerceAtLeast(0f)
//
//                    drawRect(
//                        color = Color.Blue,
//                        topLeft = Offset(x, size.height - barHeight),
//                        size = Size(
//                            hourWidth.toPx() - 4.dp.toPx(),
//                            barHeight
//                        )
//                    )
//
//                    val date = Date(timestamp)
//                    val format = SimpleDateFormat("HH:mm:ss", Locale.getDefault())
//                    val formattedTime = format.format(date)
//                    drawContext.canvas.nativeCanvas.drawText(
//                        formattedTime,
//                        x + hourWidth.toPx() / 2,
//                        size.height + 15f,
//                        android.graphics.Paint().apply {
//                            textSize = 10.sp.toPx()
//                            color = android.graphics.Color.BLACK
//                            textAlign = android.graphics.Paint.Align.CENTER
//                        }
//                    )
//
//                    val textValue = String.format("%.1f", chartValue)
//                    drawContext.canvas.nativeCanvas.drawText(
//                        textValue,
//                        x + hourWidth.toPx() / 2,
//                        size.height - barHeight - 10.dp.toPx(),
//                        android.graphics.Paint().apply {
//                            textSize = 11.sp.toPx()
//                            color = android.graphics.Color.BLACK
//                            textAlign = android.graphics.Paint.Align.CENTER
//                        }
//                    )
//                    if (state.NetworkType == networkType){
//                        val textId = if (detailedChartData.isNotEmpty() && index < detailedChartData.size) {
//                            "ID: ${detailedChartData[index].cellId}"
//                        } else {
//                            "ID: -"
//                        }
//
//                        drawContext.canvas.nativeCanvas.drawText(
//                            textId,
//                            x + hourWidth.toPx() / 2,
//                            size.height - barHeight - 2.dp.toPx(),
//                            android.graphics.Paint().apply {
//                                textSize = 7.sp.toPx()
//                                color = android.graphics.Color.BLACK
//                                textAlign = android.graphics.Paint.Align.CENTER
//                            }
//                        )
//                    }
//                }
//            }
//        }
//    }
//}
//
//@Composable
//fun DetailedChartContent(chartData: List<ChartDataPoint>, state: MainActivity.MainActivityState, onDismiss: () -> Unit, networkType: String) {
//    val scrollState = rememberScrollState()
//    val coroutineScope = rememberCoroutineScope()
//
//    val mapSaver = Saver<MutableMap<String, Color>, List<Pair<String, Long>>>(
//        save = { map ->
//            map.map { entry -> Pair(entry.key, entry.value.value.toLong()) }
//        },
//        restore = { list ->
//            mutableMapOf<String, Color>().apply {
//                list.forEach { pair -> this[pair.first] = Color(pair.second) }
//            }
//        }
//    )
//
//    val cellIdToColor = rememberSaveable(saver = mapSaver) { mutableStateMapOf<String, Color>() }
//
//    val textMeasurer = rememberTextMeasurer()
//
//    val maxChartValue = chartData.maxOfOrNull { it.value } ?: 0f
//    val minChartValue = chartData.minOfOrNull { it.value } ?: 0f
//
//
//    val yAxisMaxValue = if (maxChartValue == minChartValue) {
//        maxChartValue + 10
//    } else {
//        ceil(maxChartValue / 10) * 10
//    }
//
//    val yAxisMinValue = if (maxChartValue == minChartValue) {
//        minChartValue - 10
//    } else {
//        floor(minChartValue / 10) * 10
//    }
//
//    val pointWidth = 10.dp
//    val chartWidth = chartData.size * pointWidth
//    val chartHeight = 300.dp
//
//    val predefinedColors = listOf(
//        Color(0xFFF44336),
//        Color(0xFF9C27B0),
//        Color(0xFF673AB7),
//        Color(0xFF3F51B5),
//        Color(0xFF2196F3),
//        Color(0xFF009688),
//        Color(0xFF4CAF50),
//        Color(0xFFFFEB3B),
//        Color(0xFFFF9800)
//    )
//
//    fun generateUniqueColor(): Color {
//        var color: Color
//        do {
//            color = Color(
//                (0..255).random(),
//                (0..255).random(),
//                (0..255).random()
//            )
//        } while (cellIdToColor.containsValue(color))
//        return color
//    }
//
//    fun getColorForCellId(cellId: String): Color {
//        return cellIdToColor.getOrPut(cellId) {
//            if (cellIdToColor.size < predefinedColors.size) {
//                predefinedColors[cellIdToColor.size]
//            } else {
//                generateUniqueColor()
//            }
//        }
//    }
//
//    Column {
//        Row(
//            horizontalArrangement = Arrangement.End,
//            modifier = Modifier
//                .fillMaxWidth()
//                .padding(8.dp)
//        ) {
//            IconButton(onClick = {
//                coroutineScope.launch {
//                    scrollState.animateScrollTo(scrollState.maxValue)
//                }
//            }) {
//                Icon(
//                    imageVector = Icons.Default.ArrowForward,
//                    contentDescription = "Scroll to End"
//                )
//            }
//            IconButton(onClick = onDismiss) {
//                Icon(imageVector = Icons.Default.Close, contentDescription = "Close")
//            }
//        }
//        Row {
//            Box(
//                modifier = Modifier
//                    .width(40.dp)
//                    .height(chartHeight)
//            ) {
//                Canvas(modifier = Modifier.fillMaxSize()) {
//                    val stepSize = (yAxisMaxValue - yAxisMinValue) / 5f
//                    for (i in 0..5) {
//                        val y = chartHeight.toPx() - i * (chartHeight.toPx() / 5)
//                        drawLine(
//                            color = Color.LightGray,
//                            start = Offset(0f, y),
//                            end = Offset(chartWidth.toPx(), y),
//                            strokeWidth = 1f
//                        )
//                        drawText(
//                            textMeasurer = textMeasurer,
//                            text = String.format("%.0f", yAxisMinValue + i * stepSize),
//                            topLeft = Offset(5.dp.toPx(), y - 5.dp.toPx()),
//                            style = TextStyle(color = Color.Black, fontSize = 8.sp)
//                        )
//                    }
//                }
//            }
//
//            Box(
//                modifier = Modifier
//                    .horizontalScroll(scrollState)
//                    .width(chartWidth)
//                    .height(chartHeight)
//            ) {
//                Canvas(modifier = Modifier.fillMaxSize()) {
//                    var previousPoint: Offset? = null
//                    var previousCellId: String? = null
//                    var previousColor: Color? = null
//
//                    chartData.forEachIndexed { index, dataPoint ->
//                        val x = index * pointWidth.toPx()
//                        val y = chartHeight.toPx() - ((dataPoint.value - yAxisMinValue) / (yAxisMaxValue - yAxisMinValue) * chartHeight.toPx())
//                        val currentPoint = Offset(x, y)
//
//                        val cellId = dataPoint.cellId
//                        val color = getColorForCellId(cellId)
//
//                        if (previousPoint != null) {
//                            if (previousCellId != cellId) {
//                                val middleX = (previousPoint!!.x + currentPoint.x) / 2
//                                drawLine(
//                                    color = previousColor!!,
//                                    start = previousPoint!!,
//                                    end = Offset(middleX, previousPoint!!.y),
//                                    strokeWidth = 2f
//                                )
//                                drawLine(
//                                    color = color,
//                                    start = Offset(middleX, previousPoint!!.y),
//                                    end = currentPoint,
//                                    strokeWidth = 2f
//                                )
//                            } else {
//                                drawLine(
//                                    color = color,
//                                    start = previousPoint!!,
//                                    end = currentPoint,
//                                    strokeWidth = 2f
//                                )
//                            }
//                        }
//
//                        drawCircle(
//                            color = color,
//                            center = currentPoint,
//                            radius = 4f
//                        )
//
//                        val textOffset = if (index % 2 == 0) {
//                            Offset(x, y - 10.dp.toPx())
//                        } else {
//                            Offset(x, y + 10.dp.toPx())
//                        }
//                        if (index != 0 && index % 5 == 0) {
//                            drawText(
//                                textMeasurer = textMeasurer,
//                                text = String.format("%.0f", dataPoint.value),
//                                topLeft = textOffset,
//                                style = TextStyle(color = Color.Black, fontSize = 8.sp)
//                            )
//                        }
//
//                        drawLine(
//                            color = Color.LightGray,
//                            start = Offset(x, 0f),
//                            end = Offset(x, chartHeight.toPx()),
//                            strokeWidth = 1f
//                        )
//
//                        if (index != 0 && index % 5 == 0) {
//                            val date = Date(dataPoint.timestamp)
//                            val format = SimpleDateFormat("HH:mm:ss", Locale.getDefault())
//                            val formattedTime = format.format(date)
//                            drawText(
//                                textMeasurer = textMeasurer,
//                                text = formattedTime,
//                                topLeft = Offset(x - 15.dp.toPx(), chartHeight.toPx() - 10.dp.toPx()),
//                                style = TextStyle(color = Color.Black, fontSize = 6.sp)
//                            )
//                        }
//
//                        previousPoint = currentPoint
//                        previousCellId = cellId
//                        previousColor = color
//                    }
//                }
//            }
//        }
//
//        Row(modifier = Modifier.padding(start = 16.dp, top = 8.dp)) {
//            Box(
//                modifier = Modifier
//                    .horizontalScroll(rememberScrollState())
//                    .height(40.dp)
//            ) {
//                Row {
//                    cellIdToColor.forEach { (cellId, color) ->
//                        Row(
//                            verticalAlignment = Alignment.CenterVertically,
//                            modifier = Modifier.padding(end = 8.dp)
//                        ) {
//                            Box(
//                                modifier = Modifier
//                                    .size(10.dp)
//                                    .background(color)
//                            )
//                            Spacer(modifier = Modifier.width(4.dp))
//                            Text("Cell ID: $cellId", fontSize = 8.sp)
//                        }
//                    }
//                }
//            }
//        }
//    }
//}