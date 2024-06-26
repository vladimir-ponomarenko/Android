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
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Button
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
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import kotlinx.coroutines.delay
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
@SuppressLint("AutoboxingStateCreation")
@Composable
fun RSRPGraph(state: MainActivity.MainActivityState) {
    val rsrpData = remember { mutableStateListOf<Pair<Long, Float>>() }
    val rssiData = remember { mutableStateListOf<Pair<Long, Float>>() }
    val rsrqData = remember { mutableStateListOf<Pair<Long, Float>>() }

    LaunchedEffect(state.Rsrp, state.Rssi, state.Rsrq, state.Cellid) {
        while (true) {
            val timestamp = System.currentTimeMillis()

            addChartData(rsrpData, state.Rsrp, timestamp)
            addChartData(rssiData, state.Rssi, timestamp)
            addChartData(rsrqData, state.Rsrq, timestamp)

            delay(MainActivity.UPDATE_INTERVAL)
        }
    }

    Column(modifier = Modifier.fillMaxSize()) {
        Button(onClick = { state.showRSRPChart = true }, modifier = Modifier.padding(8.dp)) {
            Text("Show RSRP Chart")
        }
        Button(onClick = { state.showRSSIChart = true }, modifier = Modifier.padding(8.dp)) {
            Text("Show RSSI Chart")
        }
        Button(onClick = { state.showRSRQChart = true }, modifier = Modifier.padding(8.dp)) {
            Text("Show RSRQ Chart")
        }
    }

    if (state.showRSRPChart) {
        ChartDialog(
            chartData = rsrpData,
            chartTitle = "RSRP Chart",
            onClose = { state.showRSRPChart = false },
            state = state
        )
    }

    if (state.showRSSIChart) {
        ChartDialog(
            chartData = rssiData,
            chartTitle = "RSSI Chart",
            onClose = { state.showRSSIChart = false },
            state = state
        )
    }

    if (state.showRSRQChart) {
        ChartDialog(
            chartData = rsrqData,
            chartTitle = "RSRQ Chart",
            onClose = { state.showRSRQChart = false },
            state = state
        )
    }
}

private fun addChartData(chartData: MutableList<Pair<Long, Float>>, value: String, timestamp: Long) {
    val chartValue = value.replace(" dBm", "").replace(" dB", "").toFloatOrNull() ?: 0f
    chartData.add(Pair(timestamp, chartValue))
    if (chartData.size > 10) {
        chartData.removeAt(0)
    }
}

@Composable
fun ChartDialog(chartData: List<Pair<Long, Float>>, chartTitle: String, onClose: () -> Unit, state: MainActivity.MainActivityState) {
    Dialog(onDismissRequest = onClose) {
        Surface(shape = RoundedCornerShape(8.dp)) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(
                    text = chartTitle,
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold
                )

                IconButton(onClick = onClose, modifier = Modifier.align(Alignment.End)) {
                    Icon(imageVector = Icons.Default.Close, contentDescription = "Close")
                }

                ChartContent(chartData, state)
            }
        }
    }
}

@Composable
fun ChartContent(chartData: List<Pair<Long, Float>>, state: MainActivity.MainActivityState) {
    val scrollState = rememberScrollState()
    val context = LocalContext.current

    val hourWidth = 50.dp
    val chartWidth = hourWidth * 24
    val maxChartValue = chartData.maxOfOrNull { it.second } ?: 1f
    val maxValue = (maxChartValue / 10).toInt() * 10 + 10

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

                    val text = String.format("%.1f (ID: %s)", chartValue, state.Cellid)
                    drawContext.canvas.nativeCanvas.drawText(
                        text,
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