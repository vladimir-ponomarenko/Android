package com.example.login

import android.annotation.SuppressLint
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material.Button
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.launch

@SuppressLint("CoroutineCreationDuringComposition")
@Composable
fun SpeedTestScreen(state: MainActivity.MainActivityState) {
    var testResult by remember { mutableStateOf<TestResult?>(null) }
    val lifecycleScope = rememberCoroutineScope()
    val lifecycleOwner = LocalLifecycleOwner.current
    val forceUpdate = remember { mutableStateOf(false) }

    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Button(
            onClick = {
                lifecycleScope.launch {
                    SpeedTestManager.startSpeedTest(lifecycleScope, forceUpdate) { result ->
                        testResult = result
                    }
                }
            },
            enabled = !SpeedTestManager.testRunning,
            modifier = Modifier.padding(16.dp)
        ) {
            Text("Start Speed Test", fontSize = 18.sp)
        }

        Spacer(modifier = Modifier.height(32.dp))

        if (SpeedTestManager.testRunning) {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Text("Download Speed: ${String.format("%.2f", SpeedTestManager.downloadSpeed)} Mbps", fontSize = 16.sp)
                Text("Upload Speed: ${String.format("%.2f", SpeedTestManager.uploadSpeed)} Mbps", fontSize = 16.sp)
                Text("Ping: ${SpeedTestManager.ping} ms", fontSize = 16.sp)
            }
        }

        Spacer(modifier = Modifier.height(32.dp))

        testResult?.let {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Text("Test Results:", fontSize = 18.sp)
                Text("Download Speed: ${String.format("%.2f", it.downloadSpeed)} Mbps", fontSize = 16.sp)
                Text("Upload Speed: ${String.format("%.2f", it.uploadSpeed)} Mbps", fontSize = 16.sp)
                Text("Ping: ${it.ping} ms", fontSize = 16.sp)
                Text("Duration: ${it.duration / 1000} seconds", fontSize = 16.sp)
            }
        }
    }

    LaunchedEffect(key1 = forceUpdate.value) {
        if (SpeedTestManager.testRunning) {
        }
    }

    DisposableEffect(lifecycleOwner) {
        onDispose {
            SpeedTestManager.stopSpeedTest()
        }
    }
}