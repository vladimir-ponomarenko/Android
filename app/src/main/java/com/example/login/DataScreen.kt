package com.example.login

import android.annotation.SuppressLint
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.Tab
import androidx.compose.material.TabRow
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Divider
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.delay
import kotlinx.serialization.json.Json
import kotlin.reflect.full.declaredMemberProperties


@RequiresApi(Build.VERSION_CODES.O)
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun DataScreen(state: MainActivity.MainActivityState) {
    val context = LocalContext.current
    var selectedTabIndex by remember { mutableStateOf(0) }
    val cellInfoJsonByType = remember { mutableStateOf(mutableMapOf<String, List<CellInfoData>>()) }

    LaunchedEffect(Unit) {
        while (true) {
            cellInfoJsonByType.value = state.cellInfoJson.value.mapValues { entry ->
                entry.value.map { jsonString ->
                    Json.decodeFromString<CellInfoData>(jsonString)
                }
            }.toMutableMap()
            delay(500) // Задержка в 0,5 секунд для обновления экрана
        }
    }

    Scaffold { innerPadding ->
        Column(
            modifier = Modifier
                .padding(innerPadding)
                .fillMaxSize()
        ) {
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
                0 -> CellInfoTabContent("LTE", cellInfoJsonByType.value["LTE"] ?: emptyList())
                1 -> CellInfoTabContent("GSM", cellInfoJsonByType.value["GSM"] ?: emptyList())
                2 -> CellInfoTabContent("WCDMA", cellInfoJsonByType.value["WCDMA"] ?: emptyList())
                3 -> CellInfoTabContent("CDMA", cellInfoJsonByType.value["CDMA"] ?: emptyList())
                4 -> CellInfoTabContent("NR", cellInfoJsonByType.value["NR"] ?: emptyList())
            }
        }
    }
}

@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CellInfoTabContent(cellType: String, cellInfoList: List<CellInfoData>) {
    Scaffold { innerPadding ->
        Column(modifier = Modifier.padding(innerPadding)) {
            if (cellInfoList.isEmpty()) {
                Text("No $cellType cell information available.", modifier = Modifier.padding(16.dp))
            } else {
                LazyColumn(modifier = Modifier.padding(8.dp)) {
                    items(cellInfoList) { cellInfo ->
                        Card(
                            elevation = CardDefaults.cardElevation(defaultElevation = 2.dp),
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(vertical = 4.dp)
                        ) {
                            Column(modifier = Modifier.padding(16.dp)) {
                                Text("Cell Type: $cellType")
                                for (field in CellInfoData::class.declaredMemberProperties) {
                                    val fieldValue = field.getter.call(cellInfo)
                                    if (field.name != "type" &&
                                        field.name != "timestamp" &&
                                        fieldValue != null &&
                                        fieldValue != "N/A"
                                    ) {
                                        Text("${field.name.capitalize()}: $fieldValue")
                                    }
                                }
                                Divider(Modifier.padding(vertical = 8.dp))
                            }
                        }
                    }
                }
            }
        }
    }
}