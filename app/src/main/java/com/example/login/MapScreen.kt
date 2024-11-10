@file:Suppress("DEPRECATION")

package com.example.login

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.border
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.DropdownMenuItem
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.Text
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.google.android.gms.maps.model.CameraPosition
import com.google.android.gms.maps.model.LatLng
import com.google.maps.android.compose.GoogleMap
import com.google.maps.android.compose.Polyline
import com.google.maps.android.compose.rememberCameraPositionState
import androidx.compose.material3.*
import androidx.compose.foundation.Image
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.foundation.BorderStroke
import androidx.compose.runtime.Composable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.runtime.LaunchedEffect
import java.text.SimpleDateFormat
import java.util.*

@Composable
fun MapScreen(state: MainActivity.MainActivityState, onNavigateTo: (Int) -> Unit) {
    val cameraPositionState = rememberCameraPositionState {
        position = CameraPosition.fromLatLngZoom(LatLng(55.0415, 82.9346), 10f)
    }

    val locations = when (state.selectedNetworkType) {
        "LTE" -> state.lteLocations
        "GSM" -> state.gsmLocations
        "WCDMA" -> state.wcdmaLocations
        "CDMA" -> state.cdmaLocations
        "NR" -> state.nrLocations
        else -> emptyList()
    }

    val lastUpdateTime = remember { mutableStateOf(getCurrentTime()) }

    LaunchedEffect(locations) {
        lastUpdateTime.value = getCurrentTime()
    }

    Scaffold(
        topBar = {
            MapTopBar(state, onNavigateTo = onNavigateTo)
        }
    ) { innerPadding ->
        Box(modifier = Modifier.fillMaxSize().padding(innerPadding)) {
            GoogleMap(
                modifier = Modifier.fillMaxSize(),
                cameraPositionState = cameraPositionState
            ) {
                for (i in 1 until locations.size) {
                    val (location1, color1) = locations[i - 1]
                    val (location2, color2) = locations[i]
                    Polyline(
                        points = listOf(location1, location2),
                        color = color1,
                        width = 10f
                    )
                }
            }
            ColorScaleColumn(state )

            Box(
                modifier = Modifier
                    .align(Alignment.BottomStart)
                    .padding(16.dp)
                    .background(Color.White, shape = RoundedCornerShape(8.dp))
                    .padding(8.dp)
            ) {
                Column(
                    horizontalAlignment = Alignment.Start,
                    verticalArrangement = Arrangement.Center
                ) {

                    Text(
                        text = "Последнее обновление: ${lastUpdateTime.value} ${getCurrentDate()}",
                        color = Color(0xFF34204C),
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
fun MapTopBar(state: MainActivity.MainActivityState, onNavigateTo: (Int) -> Unit) {
    var expanded by remember { mutableStateOf(false) }
    var selectedNetworkText by remember { mutableStateOf("сеть") }

    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(56.dp)
            .background(Color(0xFFF8F8F8))
            .border(width = 2.dp, color = Color(0x809E9E9E), shape = RoundedCornerShape(0.dp))
    ) {
        Row(
            modifier = Modifier.fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            IconButton(onClick = { onNavigateTo(5) }) {
                Image(
                    painter = painterResource(id = R.drawable.transition_light),
                    contentDescription = "Назад",
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            Text(
                text = "Карта",
                color = Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold
            )
            Spacer(modifier = Modifier.weight(1f))

            Box {
                Button(
                    onClick = { expanded = true },
                    colors = ButtonDefaults.buttonColors(containerColor = Color.White),
                    border = BorderStroke(1.dp, Color(0x809E9E9E)),
                    shape = RoundedCornerShape(5.dp),
                    modifier = Modifier
                        .width(110.dp)
                        .height(36.dp)
                        .padding(end = 20.dp)
                ) {
                    Text(
                        text = selectedNetworkText,
                        color = Color(0xB334204C),
                        fontSize = 16.sp,
                        textAlign = TextAlign.Center,
                        maxLines = 1
                    )
                }

                DropdownMenu(
                    expanded = expanded,
                    onDismissRequest = { expanded = false },
                    modifier = Modifier
                        .width(90.dp)
                        .background(Color.White)
                        .align(Alignment.CenterStart)

                ) {
                    DropdownMenuItem(onClick = {
                        state.selectedNetworkType = "LTE"
                        selectedNetworkText = "LTE"
                        expanded = false
                    }) {
                        Text(
                            "LTE",
                            color = Color(0xB334204C),
                            fontSize = 14.sp,
                            textAlign = TextAlign.Center
                        )
                    }
                    DropdownMenuItem(onClick = {
                        state.selectedNetworkType = "GSM"
                        selectedNetworkText = "GSM"
                        expanded = false
                    }) {
                        Text(
                            "GSM",
                            color = Color(0xB334204C),
                            fontSize = 14.sp,
                            textAlign = TextAlign.Center
                        )
                    }
                    DropdownMenuItem(onClick = {
                        state.selectedNetworkType = "WCDMA"
                        selectedNetworkText = "WCDMA"
                        expanded = false
                    }) {
                        Text(
                            "WCDMA",
                            color = Color(0xB334204C),
                            fontSize = 14.sp,
                            textAlign = TextAlign.Center)
                    }
                    DropdownMenuItem(onClick = {
                        state.selectedNetworkType = "CDMA"
                        selectedNetworkText = "CDMA"
                        expanded = false
                    }) {
                        Text("CDMA",
                            color = Color(0xB334204C),
                            fontSize = 14.sp,
                            textAlign = TextAlign.Center
                        )
                    }
                    DropdownMenuItem(onClick = {
                        state.selectedNetworkType = "NR"
                        selectedNetworkText = "NR"
                        expanded = false
                    }) {
                        Text("NR",
                            color = Color(0xB334204C),
                            fontSize = 14.sp,
                            textAlign = TextAlign.Center
                        )
                    }
                }
            }
        }
    }
}

data class ColorScaleItemData(val color: Color, val text: String)

@Composable
fun ColorScaleColumn(state: MainActivity.MainActivityState) {

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
                    ColorScaleItemData(color = Color(0xFF0000CC), text = "-150"),
                    ColorScaleItemData(color = Color(0xFF000099), text = "-156")
                )
        else -> emptyList()
            }

    Box(modifier = Modifier.fillMaxSize()) {
        Column(
            modifier = Modifier
                .align(Alignment.CenterEnd)
                .offset(y = (-210).dp)
                .padding( 10.dp)
                .background(Color.White, shape = RoundedCornerShape(15.dp))
                .border(1.dp, Color(0x809E9E9E), shape = RoundedCornerShape(15.dp))
                .padding(5.dp)
                .width(IntrinsicSize.Max)
        ) {
            colorScaleItems.forEachIndexed { index, item ->
                val shape = when (index) {
                    0 -> RoundedCornerShape(topStart = 10.dp, topEnd = 10.dp)
                    colorScaleItems.lastIndex -> RoundedCornerShape(bottomStart = 10.dp, bottomEnd = 10.dp)
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
                        color = Color.White,
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