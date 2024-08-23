@file:Suppress("DEPRECATION")

package com.example.login

//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material.DropdownMenuItem
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
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

@Composable
fun MapScreen(state: MainActivity.MainActivityState) {
    val cameraPositionState = rememberCameraPositionState {
        position = CameraPosition.fromLatLngZoom(LatLng(55.0415, 82.9346), 10f)
    }

    var expanded by remember { mutableStateOf(false) }

    val locations = when (state.selectedNetworkType) {
        "LTE" -> state.lteLocations
        "GSM" -> state.gsmLocations
        "WCDMA" -> state.wcdmaLocations
        "CDMA" -> state.cdmaLocations
        "NR" -> state.nrLocations
        else -> emptyList()
    }

    Box(modifier = Modifier.fillMaxSize()) {
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
                    width = 15f
                )
            }
        }

        Column(
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(8.dp)
        ) {
            Button(onClick = { expanded = true }) {
                Text(state.selectedNetworkType)
            }
            DropdownMenu(
                expanded = expanded,
                onDismissRequest = { expanded = false }
            ) {
                DropdownMenuItem(onClick = { state.selectedNetworkType = "LTE"; expanded = false }) {
                    Text("LTE")
                }
                DropdownMenuItem(onClick = { state.selectedNetworkType = "GSM"; expanded = false }) {
                    Text("GSM")
                }
                DropdownMenuItem(onClick = { state.selectedNetworkType = "WCDMA"; expanded = false }) {
                    Text("WCDMA")
                }
                DropdownMenuItem(onClick = { state.selectedNetworkType = "CDMA"; expanded = false }) {
                    Text("CDMA")
                }
                DropdownMenuItem(onClick = { state.selectedNetworkType = "NR"; expanded = false }) {
                    Text("NR")
                }
            }
        }

        Column(
            modifier = Modifier
                .align(Alignment.CenterEnd)
                .padding(8.dp)
        ) {
            when (state.selectedNetworkType) {
                "LTE" -> {
                    ColorScaleItem(color = Color.Red, text = "-40")
                    ColorScaleItem(color = Color(0xFFFFA500), text = "-50")
                    ColorScaleItem(color = Color(0xFFFFFF00), text = "-60")
                    ColorScaleItem(color = Color(0xFF90EE90), text = "-70")
                    ColorScaleItem(color = Color(0xFF00FFFF), text = "-80")
                    ColorScaleItem(color = Color(0xFFADD8E6), text = "-90")
                    ColorScaleItem(color = Color(0xFF87CEEB), text = "-100")
                    ColorScaleItem(color = Color(0xFF6495ED), text = "-110")
                    ColorScaleItem(color = Color(0xFF4682B4), text = "-120")
                    ColorScaleItem(color = Color.Blue, text = "-130")
                    ColorScaleItem(color = Color(0xFF00008B), text = "-140")
                }
                "GSM" -> {
                    ColorScaleItem(color = Color.Red, text = "-51")
                    ColorScaleItem(color = Color(0xFFFFA500), text = "-60")
                    ColorScaleItem(color = Color(0xFFFFFF00), text = "-70")
                    ColorScaleItem(color = Color(0xFF90EE90), text = "-80")
                    ColorScaleItem(color = Color(0xFF00FFFF), text = "-90")
                    ColorScaleItem(color = Color(0xFFADD8E6), text = "-100")
                    ColorScaleItem(color = Color(0xFF87CEEB), text = "-110")
                    ColorScaleItem(color = Color(0xFF00008B), text = "-113")
                }
                "WCDMA" -> {
                    ColorScaleItem(color = Color.Red, text = "-24")
                    ColorScaleItem(color = Color(0xFFFFA500), text = "-30")
                    ColorScaleItem(color = Color(0xFFFFFF00), text = "-40")
                    ColorScaleItem(color = Color(0xFF90EE90), text = "-50")
                    ColorScaleItem(color = Color(0xFF00FFFF), text = "-60")
                    ColorScaleItem(color = Color(0xFFADD8E6), text = "-70")
                    ColorScaleItem(color = Color(0xFF87CEEB), text = "-80")
                    ColorScaleItem(color = Color(0xFF6495ED), text = "-90")
                    ColorScaleItem(color = Color(0xFF4682B4), text = "-100")
                    ColorScaleItem(color = Color.Blue, text = "-110")
                    ColorScaleItem(color = Color(0xFF00008B), text = "-120")
                }
                "CDMA" -> {
                    ColorScaleItem(color = Color.Red, text = "-75")
                    ColorScaleItem(color = Color(0xFFFFA500), text = "-80")
                    ColorScaleItem(color = Color(0xFFFFFF00), text = "-85")
                    ColorScaleItem(color = Color(0xFF90EE90), text = "-90")
                    ColorScaleItem(color = Color(0xFF00FFFF), text = "-95")
                    ColorScaleItem(color = Color(0xFFADD8E6), text = "-100")
                    ColorScaleItem(color = Color(0xFF00008B), text = "-101")
                }
                "NR" -> {
                    ColorScaleItem(color = Color(0xFFE60000), text = "-31")
                    ColorScaleItem(color = Color(0xFFFF6600), text = "-40")
                    ColorScaleItem(color = Color(0xFFFFCC00), text = "-50")
                    ColorScaleItem(color = Color(0xFFFFFF00), text = "-60")
                    ColorScaleItem(color = Color(0xFF99FF33), text = "-70")
                    ColorScaleItem(color = Color(0xFF33FF99), text = "-80")
                    ColorScaleItem(color = Color(0xFF00FFFF), text = "-90")
                    ColorScaleItem(color = Color(0xFF00CCFF), text = "-100")
                    ColorScaleItem(color = Color(0xFF0099FF), text = "-110")
                    ColorScaleItem(color = Color(0xFF0066FF), text = "-120")
                    ColorScaleItem(color = Color(0xFF0033FF), text = "-130")
                    ColorScaleItem(color = Color(0xFF0000FF), text = "-140")
                    ColorScaleItem(color = Color(0xFF0000CC), text = "-150")
                    ColorScaleItem(color = Color(0xFF000099), text = "-156")
                }
            }
        }
    }
}

@Composable
fun ColorScaleItem(color: Color, text: String) {
    Box(
        modifier = Modifier
            .size(20.dp)
            .background(color)
            .padding(2.dp),
        contentAlignment = Alignment.Center
    ) {
        Text(
            text = text,
            fontSize = 7.sp,
            color = Color.Black,
            fontWeight = FontWeight.SemiBold
        )
    }
}

fun generateColorFromRSRP(rsrp: Int): Color {
    return when {
        rsrp >= -50 -> Color.Red
        rsrp in -60..-51 -> Color(0xFFFFA500)
        rsrp in -70..-61 -> Color(0xFFFFFF00)
        rsrp in -80..-71 -> Color(0xFF90EE90)
        rsrp in -90..-81 -> Color(0xFF00FFFF)
        rsrp in -100..-91 -> Color(0xFFADD8E6)
        rsrp in -110..-101 -> Color(0xFF87CEEB)
        rsrp in -120..-111 -> Color(0xFF6495ED)
        rsrp in -130..-121 -> Color.Blue
        rsrp in -140..-131 -> Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromRSSIGsm(rssi: Int): Color {
    return when {
        rssi >= -51 -> Color.Red
        rssi in -60..-52 -> Color(0xFFFFA500)
        rssi in -70..-61 -> Color(0xFFFFFF00)
        rssi in -80..-71 -> Color(0xFF90EE90)
        rssi in -90..-81 -> Color(0xFF00FFFF)
        rssi in -100..-91 -> Color(0xFFADD8E6)
        rssi in -110..-101 -> Color(0xFF87CEEB)
        rssi in -113..-111 -> Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromRSCPWcdma(rscp: Int): Color {
    return when {
        rscp >= -24 -> Color.Red
        rscp in -30..-25 -> Color(0xFFFFA500)
        rscp in -40..-31 -> Color(0xFFFFFF00)
        rscp in -50..-41 -> Color(0xFF90EE90)
        rscp in -60..-51 -> Color(0xFF00FFFF)
        rscp in -70..-61 -> Color(0xFFADD8E6)
        rscp in -80..-71 -> Color(0xFF87CEEB)
        rscp in -90..-81 -> Color(0xFF6495ED)
        rscp in -100..-91 -> Color(0xFF4682B4)
        rscp in -110..-101 -> Color.Blue
        rscp in -120..-111 -> Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromRSSICdma(rssi: Int): Color {
    return when {
        rssi >= -75 -> Color.Red
        rssi in -80..-76 -> Color(0xFFFFA500)
        rssi in -85..-81 -> Color(0xFFFFFF00)
        rssi in -90..-86 -> Color(0xFF90EE90)
        rssi in -95..-91 -> Color(0xFF00FFFF)
        rssi in -100..-96 -> Color(0xFFADD8E6)
        rssi in -101..-101 -> Color(0xFF00008B)
        else -> Color.Black
    }
}

fun generateColorFromCSIRsrpNr(csiRsrp: Int): Color {
    return when {
        csiRsrp >= -31 -> Color(0xFFE60000)
        csiRsrp in -40..-32 -> Color(0xFFFF6600)
        csiRsrp in -50..-41 -> Color(0xFFFFCC00)
        csiRsrp in -60..-51 -> Color(0xFFFFFF00)
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