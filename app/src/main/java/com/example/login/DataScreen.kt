package com.example.login

import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun DataScreen(state: MainActivity.MainActivityState) {
    val dataItems = listOf(
        "RSRP value: ${state.Rsrp}",
        "Rssi value: ${state.Rssi}",
        "Rsrq value: ${state.Rsrq}",
        "Rssnr value: ${state.Rssnr}",
        "Cqi value: ${state.Cqi}",
        "Bandwidth: ${state.Bandwidth}",
        "Cell ID: ${state.Cellid}",
        "LAT: ${state.Latitude}",
        "LON: ${state.Longtitude}",
        "MCC: ${state.Mcc}",
        "MNC: ${state.Mnc}",
        "LAC: ${state.Lac}",
        "TAC: ${state.Tac}",
        "PCI: ${state.Pci}",
        "EARFCN: ${state.Earfcn}",
        "CI: ${state.Ci}",
        "Network Type: ${state.NetworkType}",
        "Signal Strength: ${state.SignalStrength}",
        "Bit Error Rate: ${state.BitErrorRate}",
        "Timing Advance: ${state.TimingAdvance}",
        "Band: ${state.Band}",
        "Technology: ${state.Technology}",
        "Operator: ${state.Operator}"
    )

    LazyColumn(
        modifier = Modifier.padding(16.dp)
    ) {
        items(dataItems) { item ->
            Text(text = item, modifier = Modifier.padding(8.dp))
        }
    }
}