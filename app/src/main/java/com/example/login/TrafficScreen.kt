package com.example.login

import android.app.DatePickerDialog
import android.content.pm.PackageManager
import android.graphics.drawable.Drawable
import android.os.Build
import android.util.Log
import android.widget.DatePicker
import android.widget.ImageView
import androidx.annotation.RequiresApi
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CalendarToday
import androidx.compose.material.icons.filled.CloudUpload
import androidx.compose.material.icons.filled.Timeline
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.util.Calendar
import java.util.concurrent.TimeUnit
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.border
import androidx.compose.foundation.Image
import androidx.compose.ui.res.painterResource
import androidx.compose.foundation.layout.Box
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Button
import androidx.compose.ui.zIndex
import androidx.compose.material.icons.filled.BarChart
import androidx.compose.foundation.layout.height
import androidx.compose.material3.Icon
import androidx.compose.material3.*
import androidx.compose.material3.Text
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.ui.res.stringResource


@RequiresApi(Build.VERSION_CODES.M)
@Composable
fun TrafficScreen(state: MainActivity.MainActivityState, onNavigateTo: (Int) -> Unit) {
    val coroutineScope = rememberCoroutineScope()
    val context = LocalContext.current
    val appTrafficData = remember { mutableStateOf(emptyList<AppTrafficData>()) }
    var days by remember { mutableStateOf("1") }
    var showError by remember { mutableStateOf(false) }
    var showAppChart by remember { mutableStateOf(false) }
    var showTotalChart by remember { mutableStateOf(false) }
    var selectedAppName by remember { mutableStateOf("") }
    val totalTrafficData = remember { mutableStateOf(TotalTrafficData(0L, 0L, 0L)) }
    var isSendingTrafficData by remember { mutableStateOf(false) }
    var selectedCalendar by remember { mutableStateOf<Calendar?>(null) }
    var showDatePicker by remember { mutableStateOf(false) }
    var activeMode by remember { mutableStateOf("days") }
    var sortCriteria by remember { mutableStateOf<SortCriteria>(SortCriteria.TOTAL) }
    var isLoading by remember { mutableStateOf(false) }


    fun onDaysChanged(newDays: String) {
        days = newDays
        activeMode = "days"
        selectedCalendar = null
    }

    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(56.dp)
            .background(Color.White)
            .border(width = 2.dp, color = Color(0x809E9E9E), shape = RoundedCornerShape(0.dp))
    ) {
        Row(
            modifier = Modifier.fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            IconButton(onClick = { onNavigateTo(5) }) {
                Image(
                    painter = painterResource(id = R.drawable.transition_light),
                    contentDescription = "back",
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            Text(
                text = stringResource(id = R.string.traffic),
                color = Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold
            )
            Spacer(modifier = Modifier.weight(1f))
        }
    }

    LaunchedEffect(days, selectedCalendar, activeMode, sortCriteria) {
        isLoading = true
        coroutineScope.launch(Dispatchers.IO) {
            try {
                if (activeMode == "calendar" && selectedCalendar != null) {
                    val daysList = listOf(
                        TimeUnit.MILLISECONDS.toDays(
                            Calendar.getInstance().timeInMillis - selectedCalendar!!.timeInMillis
                        ).toInt()
                    )
                    appTrafficData.value = when (sortCriteria) {
                        SortCriteria.TOTAL -> getAppTrafficDataForDays(context, daysList).sortedByDescending { it.totalBytes }
                        SortCriteria.MOBILE -> getAppTrafficDataForDays(context, daysList).sortedByDescending { it.mobileBytes }
                        SortCriteria.WIFI -> getAppTrafficDataForDays(context, daysList).sortedByDescending { it.wifiBytes }
                    }
                    totalTrafficData.value = getTotalTrafficDataForDays(context, daysList)
                } else if (activeMode == "days") {
                    val daysList = days.split(",").mapNotNull { it.toIntOrNull() }
                    if (daysList.isNotEmpty()) {
                        showError = false
                        appTrafficData.value = when (sortCriteria) {
                            SortCriteria.TOTAL -> getAppTrafficData(context, daysList.sum()).sortedByDescending { it.totalBytes }
                            SortCriteria.MOBILE -> getAppTrafficData(context, daysList.sum()).sortedByDescending { it.mobileBytes }
                            SortCriteria.WIFI -> getAppTrafficData(context, daysList.sum()).sortedByDescending { it.wifiBytes }
                        }
                        totalTrafficData.value = getTotalTrafficData(context, daysList.sum())
                    } else {
                        showError = true
                    }
                }
            } catch (e: Exception) {
                Log.e(MainActivity.TAG, "Error fetching app traffic data: ${e.message}", e)
                showError = true
            } finally {
                isLoading = false
            }
        }
    }

    LazyColumn(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        item {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    modifier = Modifier
                        .weight(1f)
                        .background(Color.White, shape = RoundedCornerShape(8.dp))
                        .border(
                            1.5.dp,
                            if (sortCriteria == SortCriteria.TOTAL) Color(0xFF132C86) else Color(0x809E9E9E),
                            shape = RoundedCornerShape(8.dp)
                        )
                        .clickable { sortCriteria = SortCriteria.TOTAL }
                        .padding(8.dp)
                ) {
                    Text(
                        text = stringResource(id = R.string.total),
                        fontWeight = FontWeight.SemiBold,
                        color = Color(0xFF34204C)
                    )
                    Text(text = "${(totalTrafficData.value.totalBytes / 1024)} Kb")
                }

                Spacer(modifier = Modifier.width(8.dp))

                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    modifier = Modifier
                        .weight(1f)
                        .background(Color.White, shape = RoundedCornerShape(8.dp))
                        .border(
                            1.5.dp,
                            if (sortCriteria == SortCriteria.MOBILE) Color(0xFF132C86) else Color(0x809E9E9E),
                            shape = RoundedCornerShape(8.dp)
                        )
                        .clickable { sortCriteria = SortCriteria.MOBILE }
                        .padding(8.dp)
                ) {
                    Text(
                        text = stringResource(id = R.string.mobile),
                        fontWeight = FontWeight.SemiBold,
                        color = Color(0xFF34204C)
                    )
                    Text(text = "${(totalTrafficData.value.mobileBytes / 1024)} Kb")
                }

                Spacer(modifier = Modifier.width(8.dp))

                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    modifier = Modifier
                        .weight(1f)
                        .background(Color.White, shape = RoundedCornerShape(8.dp))
                        .border(
                            1.5.dp,
                            if (sortCriteria == SortCriteria.WIFI) Color(0xFF132C86) else Color(0x809E9E9E),
                            shape = RoundedCornerShape(8.dp)
                        )
                        .clickable { sortCriteria = SortCriteria.WIFI }
                        .padding(8.dp)
                ) {
                    Text(
                        text = stringResource(id = R.string.wifi),
                        fontWeight = FontWeight.SemiBold,
                        color = Color(0xFF34204C)
                    )
                    Text(text = "${(totalTrafficData.value.wifiBytes / 1024)} Kb")
                }
            }
        }


        item {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Box(
                    modifier = Modifier
                        .fillMaxSize()
                        .weight(1f)
                        .padding(8.dp)
                ) {
                    OutlinedTextField(
                        value = days,
                        onValueChange = { onDaysChanged(it) },
                        label = { Text(stringResource(id = R.string.days_count), color = Color(0xFF7B7B7B)) },
                        modifier = Modifier.fillMaxWidth()
                    )
                }

                Box(
                    modifier = Modifier
                        .size(56.dp)
                        .offset(y = 4.dp)
                        .background(Color(0xCC132C86), shape = RoundedCornerShape(8.dp))
                )
                {

                    IconButton(
                        onClick = { showDatePicker = true },
                        modifier = Modifier
                            .size(56.dp)
                            .align(Alignment.Center)
                            .zIndex(1f)
                    ) {
                        Icon(
                            imageVector = Icons.Default.CalendarToday,
                            contentDescription = "Select date",
                            tint = Color.White,
                            modifier = Modifier.size(30.dp)
                        )
                    }
                }
            }
        }

        item {
            if (showError) {
                Text(
                    text = stringResource(id = R.string.incorrect_days_count),
                    color = Color(0xCCAF1027),
                    modifier = Modifier.padding(16.dp)
                )
            }
        }

        item {
            if (!PermissionUtils.hasUsageStatsPermission(context)) {
                Button(
                    onClick = { PermissionUtils.requestUsageStatsPermission(context) },
                    colors = ButtonDefaults.buttonColors(containerColor = Color(0xCC132C86)),
                    shape = RoundedCornerShape(8.dp),
                    modifier = Modifier
                        .padding(16.dp)
                        .fillMaxWidth()
                ) {
                    Text(
                        text = stringResource(id = R.string.grant_permission),
                        color = Color.White,
                        fontSize = 16.sp
                    )
                }
            }
        }

        item {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalArrangement = Arrangement.SpaceAround
            ) {
                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Button(
                        onClick = { showTotalChart = true },
                        colors = ButtonDefaults.buttonColors(containerColor = Color(0xCC132C86)),
                        shape = RoundedCornerShape(8.dp),
                        modifier = Modifier
                            .wrapContentWidth()
                            .wrapContentHeight()
                            .padding(2.dp)
                    ) {
                        Text(
                            text = stringResource(id = R.string.total_traffic),
                            color = Color.White,
                            fontSize = 16.sp,
                            fontWeight = FontWeight.SemiBold
                        )
                    }
                }

                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Button(
                        onClick = {
                            coroutineScope.launch {
                                isSendingTrafficData = true

                                val authResponse = try {
                                    MainActivity.networkManager.authenticateForTraffic(state.Email, state.Password).await()
                                } catch (e: Exception) {
                                    Log.e(MainActivity.TAG, "Authentication failed: ${e.message}", e)
                                    null
                                }

                                if (authResponse != null) {
                                    val sortedApps = appTrafficData.value.sortedByDescending { it.totalBytes }
                                    val top10Apps = sortedApps.take(10)
                                    try {
                                        MainActivity.networkManager.sendTrafficDataToServer(authResponse.jwt, top10Apps)
                                        Log.d(MainActivity.TAG, "Traffic data sent successfully!")
                                    } catch (e: Exception) {
                                        Log.e(MainActivity.TAG, "Failed to send traffic data: ${e.message}", e)
                                    }
                                } else {
                                    Log.e(MainActivity.TAG, "Authentication for traffic data failed")
                                }
                                isSendingTrafficData = false
                            }
                        },
                        colors = ButtonDefaults.buttonColors(containerColor = Color(0xCC132C86)),
                        shape = RoundedCornerShape(8.dp),
                        modifier = Modifier
                            .wrapContentWidth()
                            .wrapContentHeight()
                            .padding(2.dp)
                    ) {
                        Text(
                            text = stringResource(id = R.string.send_data),
                            color = Color.White,
                            fontSize = 16.sp,
                            fontWeight = FontWeight.SemiBold
                        )
                    }
                }
            }
        }

        items(appTrafficData.value, key = { it.appName }) { appData ->
            TrafficItem(appData) { appName ->
                selectedAppName = appName
                showAppChart = true
            }
        }
    }

    if (showAppChart) {
        HourlyTrafficChart(
            appName = selectedAppName,
            onClose = { showAppChart = false },
            context = context,
            selectedDate = selectedCalendar
        )
    }

    if (showTotalChart) {
        TotalHourlyTrafficChart(
            onClose = { showTotalChart = false },
            context = context,
            selectedDate = if (activeMode == "calendar") selectedCalendar else null
        )
    }

    if (showDatePicker) {
        val calendar = Calendar.getInstance()
        val year = calendar.get(Calendar.YEAR)
        val month = calendar.get(Calendar.MONTH)
        val day = calendar.get(Calendar.DAY_OF_MONTH)

        val datePickerDialog = DatePickerDialog(
            context,
            { _: DatePicker, year: Int, month: Int, dayOfMonth: Int ->
                selectedCalendar = Calendar.getInstance().apply {
                    set(year, month, dayOfMonth)
                }
                activeMode = "calendar"
                showDatePicker = false

            },
            year,
            month,
            day
        )

        datePickerDialog.setOnCancelListener {
            showDatePicker = false
            activeMode = "days"
            selectedCalendar = null
        }

        datePickerDialog.setOnDismissListener {
            showDatePicker = false
        }

        datePickerDialog.show()
    }
}

@Composable
fun TrafficItem(appData: AppTrafficData, onShowChart: (String) -> Unit) {
    val context = LocalContext.current
    var icon by remember { mutableStateOf<Drawable?>(null) }
    val coroutineScope = rememberCoroutineScope()
    var isSelected by remember { mutableStateOf(false) }
    var isHighlighted by remember { mutableStateOf(false) }

    LaunchedEffect(appData) {
        coroutineScope.launch(Dispatchers.IO) {
            icon = try {
                context.packageManager.getApplicationIcon(appData.packageName)
            } catch (e: PackageManager.NameNotFoundException) {
                Log.e("TrafficItem", "Error loading icon for ${appData.packageName}", e)
                null
            }
        }
    }

    LaunchedEffect(isSelected) {
        if (isSelected) {
            isHighlighted = true
            delay(2000)
            isHighlighted = false
        }
    }

    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            if (icon != null) {
                AndroidView(
                    factory = { context ->
                        ImageView(context).apply {
                            setImageDrawable(icon)
                        }
                    },
                    modifier = Modifier.size(48.dp)
                )
                Spacer(modifier = Modifier.width(8.dp))
            } else {
                CircularProgressIndicator(modifier = Modifier.size(48.dp))
            }

            Column(modifier = Modifier.width(200.dp)) {
                val textColor = Color(0xFF34204C)
                Text(
                    text = appData.appName,
                    color = textColor,
                    fontWeight = FontWeight.SemiBold,
                )
                Text(text = "${stringResource(id = R.string.total)} ${(appData.totalBytes / 1024)} Kb", color = textColor)
                Text(text = "${stringResource(id = R.string.mobile)} ${(appData.mobileBytes / 1024)} Kb", color = textColor)
                Text(text = "${stringResource(id = R.string.wifi)} ${(appData.wifiBytes / 1024)} Kb", color = textColor)
                Text(text = "${stringResource(id = R.string.downlink)} ${(appData.rxBytes / 1024)} Kb", color = textColor)
                Text(text = "${stringResource(id = R.string.uplink)} ${(appData.txBytes / 1024)} Kb", color = textColor)
            }
        }

        Box(
            modifier = Modifier
                .size(30.dp)
                .background(
                    if (isHighlighted) Color(0xCC132C86) else Color(0x809E9E9E).copy(alpha = 0.5f),
                    shape = CircleShape
                )
                .clickable {
                    isSelected = !isSelected
                    onShowChart(appData.appName)
                },
            contentAlignment = Alignment.Center
        ) {
            Icon(
                imageVector = Icons.Default.BarChart,
                contentDescription = "Show graph",
                tint = if (isHighlighted) Color.White else Color.Transparent,
                modifier = Modifier.size(24.dp)
            )
        }
    }
}
enum class SortCriteria {
    TOTAL, MOBILE, WIFI
}