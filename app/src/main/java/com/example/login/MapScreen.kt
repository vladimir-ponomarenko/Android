@file:Suppress("DEPRECATION")

package com.example.login

//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import com.google.android.gms.maps.model.CameraPosition
import com.google.android.gms.maps.model.LatLng
import com.google.maps.android.compose.Circle
import com.google.maps.android.compose.GoogleMap
import com.google.maps.android.compose.Polyline
import com.google.maps.android.compose.rememberCameraPositionState
import kotlinx.coroutines.delay

@Composable
fun MapScreen(state: MainActivity.MainActivityState) {
    LaunchedEffect(Unit) {
        while (true) {
            val lat = state.Latitude.toDoubleOrNull()
            val lng = state.Longtitude.toDoubleOrNull()
            if (lat != null && lng != null) {
                val color = generateColorFromRSRP(state.Rsrp.replace(" dBm", "").toIntOrNull() ?: -140)
                state.locations.add(Pair(LatLng(lat, lng), color))
            }
            delay(MainActivity.UPDATE_INTERVAL)
        }
    }
    val cameraPositionState = rememberCameraPositionState {
        position = CameraPosition.fromLatLngZoom(LatLng(55.0415, 82.9346), 10f)
    }
    GoogleMap(
        modifier = Modifier.fillMaxSize(),
        cameraPositionState = cameraPositionState
    ) {
        Polyline(
            points = state.locations.map { it.first },
            color = Color.Red,
            width = 5f
        )
        state.locations.forEach { (location, color) ->
            Circle(
                center = location,
                radius = 10.0,
                fillColor = color,
                strokeColor = color,
                strokeWidth = 3f
            )
        }
    }
}

fun generateColorFromRSRP(rsrp: Int): Color {
    return when {
        rsrp >= -80 -> Color.Red
        rsrp in -90..-81 -> Color.Blue
        else -> Color.Cyan
    }
}