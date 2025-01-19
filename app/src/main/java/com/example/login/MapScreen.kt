@file:Suppress("DEPRECATION")

package com.example.login

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.IntrinsicSize
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.DropdownMenuItem
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.BarChart
import androidx.compose.material.icons.filled.Close
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.google.android.gms.maps.model.CameraPosition
import com.google.android.gms.maps.model.LatLng
import com.google.android.gms.maps.model.MapStyleOptions
import com.google.maps.android.compose.GoogleMap
import com.google.maps.android.compose.Polyline
import com.google.maps.android.compose.rememberCameraPositionState
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale


@Composable
fun MapScreen(state: MainActivity.MainActivityState, onNavigateTo: (Int) -> Unit) {
    val isDarkTheme = isSystemInDarkTheme()
    val cameraPositionState = rememberCameraPositionState {
        position = CameraPosition.fromLatLngZoom(LatLng(55.0415, 82.9346), 10f)
    }

    val context = LocalContext.current
    val mapStyleOptions = remember(context, isDarkTheme) {
        if (isDarkTheme) {
            try {
                val mapStyleJson = context.resources.openRawResource(R.raw.map_style_d)
                    .bufferedReader()
                    .use { it.readText() }
                MapStyleOptions(mapStyleJson)
            } catch (e: Exception) {
                e.printStackTrace()
                null
            }
        } else {
            null
        }
    }

    val locations = remember(state.selectedNetworkType) {
        when (state.selectedNetworkType) {
            "LTE" -> state.lteLocations
            "GSM" -> state.gsmLocations
            "WCDMA" -> state.wcdmaLocations
            "CDMA" -> state.cdmaLocations
            "NR" -> state.nrLocations
            else -> emptyList()
        }
    }

    val lastUpdateTime = remember { mutableStateOf(getCurrentTime()) }
    val showChartDialog = remember { mutableStateOf(false) }
    val signalPercentages = rememberSaveable { mutableStateOf<List<Int>>(listOf()) }

    LaunchedEffect(locations) {
        lastUpdateTime.value = getCurrentTime()
        signalPercentages.value = calculateSignalDistribution(locations)
    }

    Scaffold(
        topBar = {
            MapTopBar(state, onNavigateTo = onNavigateTo, onNetworkTypeChanged = { networkType ->
                state.selectedNetworkType = networkType
            })
        }
    ) { innerPadding ->
        Box(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
                .background(if (isDarkTheme) Color(0xFF1C1C1E) else Color(0xFFF8F8F8))
        ) {
            GoogleMap(
                modifier = Modifier.fillMaxSize(),
                cameraPositionState = cameraPositionState,
                properties = com.google.maps.android.compose.MapProperties(
                    mapStyleOptions = mapStyleOptions
                )
            ) {
                if (locations.size > 1) {
                    locations.windowed(2).forEach { (location1, location2) ->
                        Polyline(
                            points = listOf(location1.first, location2.first),
                            color = location1.second,
                            width = 15f
                        )
                    }
                }
            }

            ColorScaleColumn(state)

            Box(
                modifier = Modifier
                    .align(Alignment.TopEnd)
                    .padding(16.dp)
                    .offset(y = 315.dp, x = 5.dp)
                    .background(
                        if (isDarkTheme) Color(0xFF3C3C3E) else Color(0xFFFFFFFF),
                        shape = CircleShape
                    )
                    .border(
                        1.dp,
                        if (isDarkTheme) Color(0x809E9E9E) else Color(0x4D9E9E9E),
                        shape = CircleShape
                    )
                    .clickable { showChartDialog.value = !showChartDialog.value }
                    .padding(12.dp)
            ) {
                Icon(
                    imageVector = Icons.Default.BarChart,
                    contentDescription = "Показать график",
                    tint = if (isDarkTheme) Color(0xFF9E9E9E) else Color(0x809E9E9E),
                    modifier = Modifier.size(20.dp)
                )
            }

            if (showChartDialog.value) {
                SignalBarChart(
                    signalStrengthBuckets = signalPercentages.value,
                    selectedNetworkType = state.selectedNetworkType,
                    onClose = { showChartDialog.value = false }
                )
            }

            Box(
                modifier = Modifier
                    .align(Alignment.BottomStart)
                    .padding(16.dp)
                    .background(
                        if (isDarkTheme) Color(0xFF3C3C3E) else Color(0xFFE2E2E2),
                        shape = RoundedCornerShape(8.dp)
                    )
                    .padding(8.dp)
            ) {
                Column(
                    horizontalAlignment = Alignment.Start,
                    verticalArrangement = Arrangement.Center
                ) {
                    Text(
                        text = stringResource(id = R.string.last_update_time) + ": ${lastUpdateTime.value} ${getCurrentDate()}",
                        color = if (isDarkTheme) Color.White else Color(0xFF34204C),
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Normal
                    )
                }
            }
        }
    }
}

fun getCurrentTime(): String {
    val currentTime = System.currentTimeMillis()
    val formatter = SimpleDateFormat("HH:mm", Locale.getDefault())
    return formatter.format(Date(currentTime))
}
fun getCurrentDate(): String {
    val currentDate = System.currentTimeMillis()
    val formatter = SimpleDateFormat("dd/MM/yyyy", Locale.getDefault())
    return formatter.format(Date(currentDate))
}

@Composable
fun MapTopBar(state: MainActivity.MainActivityState, onNavigateTo: (Int) -> Unit, onNetworkTypeChanged: (String) -> Unit) {
    val isDarkTheme = isSystemInDarkTheme()
    var expanded by remember { mutableStateOf(false) }
    val networkSelectorText = stringResource(id = R.string.network_selector)
    val selectedNetworkText = remember { mutableStateOf(networkSelectorText) }

    LaunchedEffect(state.selectedNetworkType) {
        selectedNetworkText.value = state.selectedNetworkType
    }

    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(56.dp)
            .background(if (isDarkTheme) Color(0xFF2C2C2E) else Color(0xFFF8F8F8))
            .border(width = 2.dp, color = if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E), shape = RoundedCornerShape(0.dp))
    ) {
        Row(
            modifier = Modifier.fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            IconButton(onClick = { onNavigateTo(5) }) {
                Image(
                    painter = painterResource(id = if (isDarkTheme) R.drawable.transition_dark else R.drawable.transition_light),
                    contentDescription = "Назад",
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            Text(
                text = stringResource(id = R.string.map_screen_title),
                color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold
            )
            Spacer(modifier = Modifier.weight(1f))

            Box {
                Button(
                    onClick = { expanded = true },
                    colors = ButtonDefaults.buttonColors(
                        containerColor = if (isDarkTheme) Color(0xFF3C3C3E) else Color(0xFFFFFFFF)
                    ),
                    border = BorderStroke(1.dp, if (isDarkTheme) Color(0x809E9E9E) else Color(0x4D9E9E9E)),
                    shape = RoundedCornerShape(5.dp),
                    modifier = Modifier
                        .width(130.dp)
                        .height(36.dp)
                        .padding(end = 20.dp)
                ) {
                    Text(
                        text = selectedNetworkText.value,
                        color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xB334204C),
                        fontSize = 16.sp,
                        textAlign = TextAlign.Center,
                        maxLines = 1
                    )
                }

                DropdownMenu(
                    expanded = expanded,
                    onDismissRequest = { expanded = false },
                    modifier = Modifier
                        .width(110.dp)
                        .background(if (isDarkTheme) Color(0xFF3C3C3E) else Color(0xFFFFFFFF))
                        .align(Alignment.CenterStart)
                ) {
                    DropdownMenuItem(onClick = {
                        onNetworkTypeChanged("LTE")
                        expanded = false
                    }) {
                        Text("LTE", color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xB334204C), fontSize = 14.sp, textAlign = TextAlign.Center)
                    }
                    DropdownMenuItem(onClick = {
                        onNetworkTypeChanged("GSM")
                        expanded = false
                    }) {
                        Text("GSM", color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xB334204C), fontSize = 14.sp, textAlign = TextAlign.Center)
                    }
                    DropdownMenuItem(onClick = {
                        onNetworkTypeChanged("WCDMA")
                        expanded = false
                    }) {
                        Text("WCDMA", color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xB334204C), fontSize = 14.sp, textAlign = TextAlign.Center)
                    }
                    DropdownMenuItem(onClick = {
                        onNetworkTypeChanged("CDMA")
                        expanded = false
                    }) {
                        Text("CDMA", color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xB334204C), fontSize = 14.sp, textAlign = TextAlign.Center)
                    }
                    DropdownMenuItem(onClick = {
                        onNetworkTypeChanged("NR")
                        expanded = false
                    }) {
                        Text("NR", color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xB334204C), fontSize = 14.sp, textAlign = TextAlign.Center)
                    }
                }
            }
        }
    }
}

data class ColorScaleItemData(val color: Color, val text: String)

@Composable
fun ColorScaleColumn(state: MainActivity.MainActivityState) {
    val isDarkTheme = isSystemInDarkTheme()
    val colorScaleItems = when (state.selectedNetworkType) {
        "LTE" -> listOf(
            ColorScaleItemData(color = Color(0xFF980000), text = "-40"),
            ColorScaleItemData(color = Color(0xFFF20000), text = "-50"),
            ColorScaleItemData(color = Color(0xFFFF9000), text = "-60"),
            ColorScaleItemData(color = Color(0xFFFFE500), text = "-70"),
            ColorScaleItemData(color = Color(0xFFB7FF00), text = "-80"),
            ColorScaleItemData(color = Color(0xFF00FF00), text = "-90"),
            ColorScaleItemData(color = Color(0xFF00FFC8), text = "-100"),
            ColorScaleItemData(color = Color(0xFF00EAFF), text = "-110"),
            ColorScaleItemData(color = Color(0xFF009DFF), text = "-120"),
            ColorScaleItemData(color = Color(0xFF0011FF), text = "-130"),
            ColorScaleItemData(color = Color(0xFF00008B), text = "-140")
        )
        "GSM" -> listOf(
            ColorScaleItemData(color = Color(0xFFF20000), text = "-51"),
            ColorScaleItemData(color = Color(0xFFFF9000), text = "-60"),
            ColorScaleItemData(color = Color(0xFFFFE500), text = "-70"),
            ColorScaleItemData(color = Color(0xFF00FF00), text = "-80"),
            ColorScaleItemData(color = Color(0xFF00FFC8), text = "-90"),
            ColorScaleItemData(color = Color(0xFF009DFF), text = "-100"),
            ColorScaleItemData(color = Color(0xFF0011FF), text = "-110"),
            ColorScaleItemData(color = Color(0xFF00008B), text = "-113")
        )
        "WCDMA" -> listOf(
            ColorScaleItemData(color = Color(0xFF980000), text = "-24"),
            ColorScaleItemData(color = Color(0xFFF20000), text = "-30"),
            ColorScaleItemData(color = Color(0xFFFF9000), text = "-40"),
            ColorScaleItemData(color = Color(0xFFFFE500), text = "-50"),
            ColorScaleItemData(color = Color(0xFFB7FF00), text = "-60"),
            ColorScaleItemData(color = Color(0xFF00FF00), text = "-70"),
            ColorScaleItemData(color = Color(0xFF00FFC8), text = "-80"),
            ColorScaleItemData(color = Color(0xFF00EAFF), text = "-90"),
            ColorScaleItemData(color = Color(0xFF009DFF), text = "-100"),
            ColorScaleItemData(color = Color(0xFF0011FF), text = "-110"),
            ColorScaleItemData(color = Color(0xFF00008B), text = "-120")
        )
        "CDMA" -> listOf(
            ColorScaleItemData(color = Color(0xFFF20000), text = "-75"),
            ColorScaleItemData(color = Color(0xFFFF9000), text = "-80"),
            ColorScaleItemData(color = Color(0xFFFFE500), text = "-85"),
            ColorScaleItemData(color = Color(0xFF00FF00), text = "-90"),
            ColorScaleItemData(color = Color(0xFF00EAFF), text = "-95"),
            ColorScaleItemData(color = Color(0xFF0011FF), text = "-100"),
            ColorScaleItemData(color = Color(0xFF00008B), text = "-101")
        )
        "NR" -> listOf(
            ColorScaleItemData(color = Color(0xFF980000), text = "-31"),
            ColorScaleItemData(color = Color(0xFFF20000), text = "-40"),
            ColorScaleItemData(color = Color(0xFFFF9000), text = "-50"),
            ColorScaleItemData(color = Color(0xFFFFE500), text = "-60"),
            ColorScaleItemData(color = Color(0xFF99FF33), text = "-70"),
            ColorScaleItemData(color = Color(0xFF33FF99), text = "-80"),
            ColorScaleItemData(color = Color(0xFF00FFFF), text = "-90"),
            ColorScaleItemData(color = Color(0xFF00CCFF), text = "-100"),
            ColorScaleItemData(color = Color(0xFF0099FF), text = "-110"),
            ColorScaleItemData(color = Color(0xFF0066FF), text = "-120"),
            ColorScaleItemData(color = Color(0xFF0033FF), text = "-130"),
            ColorScaleItemData(color = Color(0xFF0000FF), text = "-140"),
            ColorScaleItemData(color = Color(0xFF00008B), text = "-150"),
            ColorScaleItemData(color = Color(0xFF000033), text = "-156")
        )
        else -> emptyList()
    }
    Box(modifier = Modifier.fillMaxSize()) {
        Column(
            modifier = Modifier
                .align(Alignment.TopEnd)
                .offset(y = 30.dp)
                .padding(10.dp)
                .background(if (isDarkTheme)  Color(0xFF3C3C3E) else  Color(0xFFFFFFFF), shape = RoundedCornerShape(15.dp))
                .border(1.dp, if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E), shape = RoundedCornerShape(15.dp))
                .padding(5.dp)
                .width(IntrinsicSize.Max)
        ) {
            colorScaleItems.forEachIndexed { index, item ->
                val shape = when (index) {
                    0 -> RoundedCornerShape(topStart = 10.dp, topEnd = 10.dp)
                    colorScaleItems.lastIndex -> RoundedCornerShape(
                        bottomStart = 10.dp,
                        bottomEnd = 10.dp
                    )
                    else -> RoundedCornerShape(0.dp)
                }

                Box(
                    modifier = Modifier
                        .background(item.color, shape = shape)
                        .fillMaxWidth()
                        .padding(2.dp)
                ) {
                    Text(
                        text = item.text,
                        color =  Color.White,
                        textAlign = TextAlign.Center,
                        modifier = Modifier.fillMaxWidth().padding(0.dp)
                    )
                }
                if (index < colorScaleItems.lastIndex) {
                    Spacer(modifier = Modifier.height(3.dp))
                }
            }
        }
    }
}


fun generateColorFromRSRP(rsrp: Int): Color {
    return when {
        rsrp >= -50 -> Color(0xFF980000)
        rsrp in -60..-51 -> Color(0xFFF20000)
        rsrp in -70..-61 -> Color(0xFFFF9000)
        rsrp in -80..-71 -> Color(0xFFFFE500)
        rsrp in -90..-81 -> Color(0xFFB7FF00)
        rsrp in -100..-91 -> Color(0xFF00FF00)
        rsrp in -110..-101 -> Color(0xFF00FFC8)
        rsrp in -120..-111 -> Color(0xFF00EAFF)
        rsrp in -130..-121 -> Color(0xFF009DFF)
        rsrp in -140..-131 ->Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromRSSIGsm(rssi: Int): Color {
    return when {
        rssi >= -51 -> Color(0xFFF20000)
        rssi in -60..-52 -> Color(0xFFFF9000)
        rssi in -70..-61 -> Color(0xFFFFE500)
        rssi in -80..-71 -> Color(0xFF00FF00)
        rssi in -90..-81 -> Color(0xFF00FFC8)
        rssi in -100..-91 -> Color(0xFF009DFF)
        rssi in -110..-101 -> Color(0xFF0011FF)
        rssi in -113..-111 -> Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromRSCPWcdma(rscp: Int): Color {
    return when {
        rscp >= -24 -> Color(0xFF980000)
        rscp in -30..-25 -> Color(0xFFF20000)
        rscp in -40..-31 -> Color(0xFFFF9000)
        rscp in -50..-41 -> Color(0xFFFFE500)
        rscp in -60..-51 -> Color(0xFFB7FF00)
        rscp in -70..-61 -> Color(0xFF00FF00)
        rscp in -80..-71 -> Color(0xFF00FFC8)
        rscp in -90..-81 -> Color(0xFF00EAFF)
        rscp in -100..-91 -> Color(0xFF009DFF)
        rscp in -110..-101 -> Color(0xFF0011FF)
        rscp in -120..-111 -> Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromRSSICdma(rssi: Int): Color {
    return when {
        rssi >= -75 -> Color(0xFFF20000)
        rssi in -80..-76 -> Color(0xFFFF9000)
        rssi in -85..-81 -> Color(0xFFFFE500)
        rssi in -90..-86 -> Color(0xFF00FF00)
        rssi in -95..-91 -> Color(0xFF00EAFF)
        rssi in -100..-96 -> Color(0xFF0011FF)
        rssi in -101..-101 -> Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromCSIRsrpNr(csiRsrp: Int): Color {
    return when {
        csiRsrp >= -31 -> Color(0xFF980000)
        csiRsrp in -40..-32 -> Color(0xFFF20000)
        csiRsrp in -50..-41 -> Color(0xFFFF9000)
        csiRsrp in -60..-51 -> Color(0xFFFFE500)
        csiRsrp in -70..-61 -> Color(0xFF99FF33)
        csiRsrp in -80..-71 -> Color(0xFF33FF99)
        csiRsrp in -90..-81 -> Color(0xFF00FFFF)
        csiRsrp in -100..-91 -> Color(0xFF00CCFF)
        csiRsrp in -110..-101 -> Color(0xFF0099FF)
        csiRsrp in -120..-111 -> Color(0xFF0066FF)
        csiRsrp in -130..-121 -> Color(0xFF0033FF)
        csiRsrp in -140..-131 -> Color(0xFF0000FF)
        csiRsrp in -150..-141 -> Color(0xFF0000CC)
        csiRsrp in -156..-151 -> Color(0xFF000099)
        else -> Color.Black
    }
}

fun calculateSignalStrengthAndColor(color: Color? = null, rsrp: Int? = null): Pair<Int, Color> {
    return when {
        color != null -> {
            val signalStrength = when (color) {
                Color(0xFF980000) -> -50
                Color(0xFFF20000) -> -55
                Color(0xFFFF9000) -> -65
                Color(0xFFFFE500) -> -75
                Color(0xFFB7FF00) -> -85
                Color(0xFF00FF00) -> -95
                Color(0xFF00FFC8) -> -105
                Color(0xFF00EAFF) -> -115
                Color(0xFF009DFF) -> -125
                Color(0xFF00008B) -> -135
                else -> -150
            }
            Pair(signalStrength, color)
        }

        rsrp != null -> {
            val signalColor = when {
                rsrp >= -50 -> Color(0xFF980000)
                rsrp in -60..-51 -> Color(0xFFF20000)
                rsrp in -70..-61 -> Color(0xFFFF9000)
                rsrp in -80..-71 -> Color(0xFFFFE500)
                rsrp in -90..-81 -> Color(0xFFB7FF00)
                rsrp in -100..-91 -> Color(0xFF00FF00)
                rsrp in -110..-101 -> Color(0xFF00FFC8)
                rsrp in -120..-111 -> Color(0xFF00EAFF)
                rsrp in -130..-121 -> Color(0xFF009DFF)
                rsrp in -140..-131 -> Color(0xFF00008B)
                else -> Color.Black
            }
            Pair(rsrp, signalColor)
        }
        else -> Pair(-150, Color.Black)
    }
}

fun calculateSignalDistribution(locations: List<Pair<LatLng, Color>>): List<Int> {
    val signalStrengths = locations.map { calculateSignalStrengthAndColor(color = it.second).first }
    val totalSignals = signalStrengths.size

    val signalBuckets = IntArray(16)

    signalStrengths.forEach {
        when (it) {
            in -150..-141 -> signalBuckets[0]++
            in -140..-131 -> signalBuckets[1]++
            in -130..-121 -> signalBuckets[2]++
            in -120..-111 -> signalBuckets[3]++
            in -110..-101 -> signalBuckets[4]++
            in -100..-91 -> signalBuckets[5]++
            in -90..-81 -> signalBuckets[6]++
            in -80..-71 -> signalBuckets[7]++
            in -70..-61 -> signalBuckets[8]++
            in -60..-51 -> signalBuckets[9]++
            in -50..-41 -> signalBuckets[10]++
            in -40..-31 -> signalBuckets[11]++
            in -30..-21 -> signalBuckets[12]++
            in -20..-11 -> signalBuckets[13]++
            in -10..-1 -> signalBuckets[14]++
            else -> signalBuckets[15]++
        }
    }
    return signalBuckets.map { (it.toFloat() / totalSignals * 100).toInt() }
}

@Composable
fun SignalBarChart(
    signalStrengthBuckets: List<Int>,
    selectedNetworkType: String,
    onClose: () -> Unit
) {
    if (signalStrengthBuckets.isEmpty()) {
        Text(text = stringResource(id = R.string.no_data_message), color = Color.Red)
        return
    }

    val isDarkTheme = isSystemInDarkTheme()
    val signalValues = when (selectedNetworkType) {
        "LTE" -> listOf("-140", "-130", "-120", "-110", "-100", "-90", "-80", "-70", "-60", "-50", "-40")
        "GSM" -> listOf("-113", "-110", "-100", "-90", "-80", "-70", "-60", "-51")
        "WCDMA" -> listOf("-120", "-110", "-100", "-90", "-80", "-70", "-60", "-50", "-40", "-30", "-24")
        "CDMA" -> listOf("-101", "-100", "-95", "-90", "-85", "-80", "-75")
        "NR" -> listOf("-156", "-150", "-140", "-130", "-120", "-110", "-100", "-90", "-80", "-70", "-60", "-50", "-40", "-31")
        else -> emptyList()
    }

    val maxWidth = 300.dp
    val barHeight = 30.dp
    val spacing = 10.dp
    val density = LocalDensity.current
    val maxWidthPx = with(density) { maxWidth.toPx() }

    val totalSignals = signalStrengthBuckets.sum()
    val signalPercentages = signalStrengthBuckets.map { (it.toFloat() / totalSignals * 100).toInt() }

    Column(
        modifier = Modifier
            .padding(10.dp)
            .background(if (isDarkTheme) Color(0xFF3C3C3E) else  Color(0xFFFFFFFF), shape = RoundedCornerShape(10.dp))
            .padding(10.dp)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(bottom = 4.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Spacer(modifier = Modifier.weight(1f))

            Text(
                text = stringResource(id = R.string.signal_distribution_title),
                color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                fontWeight = FontWeight.Bold,
                fontSize = 18.sp,
                textAlign = TextAlign.Center,
                modifier = Modifier.weight(8f)
            )

            IconButton(
                onClick = { onClose() },
                modifier = Modifier.weight(1f)
            ) {
                Icon(
                    imageVector = Icons.Filled.Close,
                    contentDescription = "Close",
                    modifier = Modifier.size(20.dp),
                    tint = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                )
            }
        }

        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp)
        ) {
            signalStrengthBuckets.forEachIndexed { index, bucketCount ->
                if (bucketCount > 0 && index < signalValues.size) {
                    val barWidthPx = (signalPercentages[index] / 100f) * maxWidthPx
                    val color = getNetworkColor(selectedNetworkType, signalValues[index])

                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(bottom = spacing)
                            .height(barHeight)
                    ) {
                        Text(
                            text = signalValues[index],
                            fontSize = 10.sp,
                            fontWeight = FontWeight.Normal,
                            color = if (isDarkTheme) Color.White else Color.Black,
                            modifier = Modifier
                                .align(Alignment.CenterVertically)
                                .padding(end = 8.dp)
                        )

                        Box(
                            modifier = Modifier
                                .fillMaxHeight()
                                .width(barWidthPx.dp)
                                .background(color, RoundedCornerShape(4.dp))
                        ) {
                            Text(
                                text = "${signalPercentages[index]}%",
                                modifier = Modifier
                                    .align(Alignment.Center)
                                    .padding(start = 8.dp),
                                fontSize = 12.sp,
                                color = if (isDarkTheme) Color.Black else Color.White,
                                fontWeight = FontWeight.Bold
                            )
                        }
                    }
                }
            }
        }
    }
}


fun getNetworkColor(networkType: String, signalStrength: String): Color {
    return when (networkType) {
        "LTE" -> when (signalStrength) {
            "-40" -> Color(0xFF980000)
            "-50" -> Color(0xFFF20000)
            "-60" -> Color(0xFFFF9000)
            "-70" -> Color(0xFFFFE500)
            "-80" -> Color(0xFFB7FF00)
            "-90" -> Color(0xFF00FF00)
            "-100" -> Color(0xFF00FFC8)
            "-110" -> Color(0xFF00EAFF)
            "-120" -> Color(0xFF009DFF)
            "-130" -> Color(0xFF0011FF)
            "-140" -> Color(0xFF00008B)
            else -> Color.Gray
        }
        "GSM" -> when (signalStrength) {
            "-51" -> Color(0xFFF20000)
            "-60" -> Color(0xFFFF9000)
            "-70" -> Color(0xFFFFE500)
            "-80" -> Color(0xFF00FF00)
            "-90" -> Color(0xFF00FFC8)
            "-100" -> Color(0xFF009DFF)
            "-110" -> Color(0xFF0011FF)
            "-113" -> Color(0xFF00008B)
            else -> Color.Gray
        }
        "WCDMA" -> when (signalStrength) {
            "-24" -> Color(0xFF980000)
            "-30" -> Color(0xFFF20000)
            "-40" -> Color(0xFFFF9000)
            "-50" -> Color(0xFFFFE500)
            "-60" -> Color(0xFFB7FF00)
            "-70" -> Color(0xFF00FF00)
            "-80" -> Color(0xFF00FFC8)
            "-90" -> Color(0xFF00EAFF)
            "-100" -> Color(0xFF009DFF)
            "-110" -> Color(0xFF0011FF)
            "-120" -> Color(0xFF00008B)
            else -> Color.Gray
        }
        "CDMA" -> when (signalStrength) {
            "-75" -> Color(0xFFF20000)
            "-80" -> Color(0xFFFF9000)
            "-85" -> Color(0xFFFFE500)
            "-90" -> Color(0xFF00FF00)
            "-95" -> Color(0xFF00EAFF)
            "-100" -> Color(0xFF0011FF)
            "-101" -> Color(0xFF00008B)
            else -> Color.Gray
        }
        "NR" -> when (signalStrength) {
            "-31" -> Color(0xFF980000)
            "-40" -> Color(0xFFF20000)
            "-50" -> Color(0xFFFF9000)
            "-60" -> Color(0xFFFFE500)
            "-70" -> Color(0xFF99FF33)
            "-80" -> Color(0xFF33FF99)
            "-90" -> Color(0xFF00FFFF)
            "-100" -> Color(0xFF00CCFF)
            "-110" -> Color(0xFF0099FF)
            "-120" -> Color(0xFF0066FF)
            "-130" -> Color(0xFF0033FF)
            "-140" -> Color(0xFF0000FF)
            "-150" -> Color(0xFF00008B)
            "-156" -> Color(0xFF000033)
            else -> Color.Gray
        }
        else -> Color.Gray
    }
}