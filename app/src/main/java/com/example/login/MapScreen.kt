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
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import com.google.android.gms.maps.model.CameraPosition
import com.google.android.gms.maps.model.LatLng
import com.google.maps.android.compose.Circle
import com.google.maps.android.compose.GoogleMap
import com.google.maps.android.compose.Polyline
import com.google.maps.android.compose.rememberCameraPositionState

@Composable
fun MapScreen(state: MainActivity.MainActivityState) {
    val cameraPositionState = rememberCameraPositionState {
        position = CameraPosition.fromLatLngZoom(LatLng(55.0415, 82.9346), 10f)
    }
    GoogleMap(
        modifier = Modifier.fillMaxSize(),
        cameraPositionState = cameraPositionState
    ) {
        for (i in 0 until state.locations.size - 1) {
            val (location1, color1) = state.locations[i]
            val (location2, color2) = state.locations[i + 1]

            Polyline(
                points = listOf(location1, LatLng((location1.latitude + location2.latitude) / 2, (location1.longitude + location2.longitude) / 2)),
                color = color1,
                width = 5f
            )
            Polyline(
                points = listOf(LatLng((location1.latitude + location2.latitude) / 2, (location1.longitude + location2.longitude) / 2), location2),
                color = color2,
                width = 5f
            )
        }

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