@file:Suppress("DEPRECATION")

package com.example.login

import android.Manifest
import android.annotation.SuppressLint
import android.app.usage.NetworkStats
import android.app.usage.NetworkStatsManager
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.ConnectivityManager
import android.os.Build
import android.os.Bundle
import android.provider.Settings
import android.telephony.CellInfoLte
import android.telephony.TelephonyManager
import android.telephony.cdma.CdmaCellLocation
import android.telephony.gsm.GsmCellLocation
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.annotation.RequiresApi
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Button
import androidx.compose.material.Checkbox
import androidx.compose.material.Icon
import androidx.compose.material.IconButton
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.Scaffold
import androidx.compose.material.SnackbarDuration
import androidx.compose.material.Surface
import androidx.compose.material.Tab
import androidx.compose.material.TabRow
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.rememberScaffoldState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.nativeCanvas
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.lifecycleScope
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationCallback
import com.google.android.gms.location.LocationRequest
import com.google.android.gms.location.LocationResult
import com.google.android.gms.location.LocationServices
import com.google.android.gms.maps.model.CameraPosition
import com.google.android.gms.maps.model.LatLng
import com.google.maps.android.compose.Circle
import com.google.maps.android.compose.GoogleMap
import com.google.maps.android.compose.Polyline
import com.google.maps.android.compose.rememberCameraPositionState
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.serialization.Serializable
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import okhttp3.Call
import okhttp3.Callback
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import java.io.IOException
import java.time.Instant
import java.time.format.DateTimeFormatter
import java.util.concurrent.TimeUnit


@Suppress("NAME_SHADOWING")
class MainActivity : ComponentActivity(), ActivityCompat.OnRequestPermissionsResultCallback {

    companion object {
        const val REQUEST_CODE_PERMISSIONS = 101
        private const val LOCATION_PERMISSION_REQUEST_CODE = 1001
        private const val READ_PHONE_STATE_PERMISSION_REQUEST_CODE = 1002

        const val TAG = "com.example.login.MainActivity"
        const val UPDATE_INTERVAL = 2000L // 2 секунды
        private const val SERVER_URL = "http://45.90.218.73:8080"
        const val SERVER_URL1 = "ws://45.90.218.73:8080"

        const val WEBSOCKET_ENDPOINT = "/api/sockets/thermalmap"

        // Имя ключа для SharedPreferences
        private const val SHARED_PREFS_NAME = "login_prefs"
        private const val EMAIL_KEY = "email"
        private const val PASSWORD_KEY = "password"
        private const val JWT_TOKEN_KEY = "jwt_token"
        private const val UUID_KEY = "uuid" // Ключ для UUID
        private const val REMEMBER_ME_KEY = "remember_me"

        // ID уведомления и имя канала
        const val NOTIFICATION_ID = 1
        const val CHANNEL_ID = "location_service_channel"

        // Intent action для управления сервисом
        const val ACTION_START_SERVICE = "com.example.login.ACTION_START_SERVICE"
        const val ACTION_STOP_SERVICE = "com.example.login.ACTION_STOP_SERVICE"
    }

    private lateinit var state: MainActivityState
    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private lateinit var httpClient: OkHttpClient
    private var webSocket: WebSocket? = null
    private var isWebSocketConnected by mutableStateOf(false)
    private var isSendingData by mutableStateOf(false) // Флаг отправки данных

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        state = MainActivityState(applicationContext)
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(this)
        httpClient = OkHttpClient()
        getLocation(state, applicationContext)
        setContent {
            MainContent(state)
        }
        checkAndRequestPermissions()
        // Загрузка данных из SharedPreferences при запуске
        state.loadLoginData()
    }

    private fun registerUser(email: String, password: String, onComplete: (RegisterResponse?) -> Unit) {
        val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val request = Request.Builder()
            .url("$SERVER_URL/api/user/register")
            .post(requestBody)
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to register user", e)
                onComplete(null)
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful) {
                        Log.e(TAG, "Failed to register user: ${response.code}")
                        onComplete(null)
                        return
                    }
                    val responseBody = response.body?.string()
                    try {
                        val registerResponse = Json.decodeFromString<RegisterResponse>(responseBody ?: "")
                        Log.d(TAG, "Register response: $registerResponse")
                        onComplete(registerResponse)
                    } catch (e: Exception) {
                        Log.e(TAG, "Failed to parse register response: ${e.message}")
                        onComplete(null)
                    }
                }
            }
        })
    }

    private fun verifyToken(token: String, onComplete: (Boolean) -> Unit) {
        val request = Request.Builder()
            .url("$SERVER_URL/api/jwt/verify")
            .header("Authorization", "Bearer $token")
            .post("".toRequestBody(null))
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to verify token", e)
                onComplete(false)
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful) {
                        Log.e(TAG, "Failed to verify token: ${response.code}")
                        onComplete(false)
                        return
                    }
                    val responseBody = response.body?.string()
                    val jsonResponse = Json.decodeFromString<Map<String, String>>(responseBody ?: "")
                    val result = jsonResponse["result"]
                    if (result == "valid") {
                        Log.d(TAG, "Token is valid")
                        onComplete(true)
                    } else {
                        Log.d(TAG, "Token is invalid")
                        onComplete(false)
                    }
                }
            }
        })
    }

    private fun authenticateUser(email: String, password: String, token: String) {
        verifyToken(token) { isValid ->
            if (isValid) {
                val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
                val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

                val request = Request.Builder()
                    .url("$SERVER_URL/api/user/auth")
                    .header("Authorization", "Bearer $token")
                    .post(requestBody)
                    .build()

                httpClient.newCall(request).enqueue(object : Callback {
                    override fun onFailure(call: Call, e: IOException) {
                        Log.e(TAG, "Failed to authenticate user (1)", e)
                    }

                    override fun onResponse(call: Call, response: Response) {
                        response.use {
                            if (!response.isSuccessful) {
                                if (response.code == 401) {
                                    Log.e(TAG, "Failed to authenticate user: Unauthorized")
                                } else {
                                    Log.e(TAG, "Failed to authenticate user (2): ${response.code}")
                                }
                                return
                            }
                            val responseBody = response.body?.string()
                            try {
                                val authResponse = Json.decodeFromString<AuthResponse>(responseBody ?: "")
                                Log.d(TAG, "Auth response: $authResponse")
                                state.JwtToken = authResponse.jwt
                                state.Uuid = authResponse.uuid
                                state.saveLoginData()
                                connectWebSocket(authResponse.jwt)
                                isWebSocketConnected = true // Устанавливаем флаг подключения
                            } catch (e: Exception) {
                                Log.e(TAG, "Failed to parse auth response: ${e.message}")
                            }
                        }
                    }
                })
            } else {
                Log.e(TAG, "Token is invalid, cannot authenticate user")
            }
        }
    }

    private fun connectWebSocket(jwt: String) {
        if (webSocket != null) {
            // Проверяем, был ли WebSocket уже создан
            return
        }
        val client = OkHttpClient.Builder()
            .pingInterval(5, TimeUnit.SECONDS)
            .build()
        val request = Request.Builder()
            .url("$SERVER_URL1$WEBSOCKET_ENDPOINT")
            .header("Authorization", "Bearer $jwt")
            .build()

        webSocket = client.newWebSocket(request, object : WebSocketListener() {
            @RequiresApi(Build.VERSION_CODES.O)
            override fun onOpen(webSocket: WebSocket, response: Response) {
                Log.d(TAG, "WebSocket connection established")
                lifecycleScope.launch {
                    while (true) {
                        delay(UPDATE_INTERVAL)
                        val jsonString = generateJSON(state)
                        webSocket.send(jsonString)
                        isSendingData = true // Устанавливаем флаг отправки
                        delay(500) // Задержка для отображения индикатора
                        isSendingData = false // Сбрасываем флаг отправки
                        Log.d(TAG, "Sent JSON to server: $jsonString")
                    }
                }
            }

            override fun onMessage(webSocket: WebSocket, text: String) {
                Log.d(TAG, "Received message from server: $text")
            }

            override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                Log.d(TAG, "WebSocket connection closed: $code, $reason")
                isWebSocketConnected = false // Сбрасываем флаг подключения
                this@MainActivity.webSocket = null // Обнуляем ссылку на WebSocket
            }

            override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                Log.e(TAG, "WebSocket connection failure", t)
                if (response != null) {
                    Log.e(TAG, "Response message: ${response.message}")
                    Log.e(TAG, "Response code: ${response.code}")
                }
                isWebSocketConnected = false // Сбрасываем флаг подключения
                this@MainActivity.webSocket = null // Обнуляем ссылку на WebSocket
            }
        })
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun generateJSON(state: MainActivityState): String {
        val currentTimestamp = Instant.now().toString()
        val formattedTimestamp = String.format(
            DateTimeFormatter.ISO_INSTANT.format(
                Instant.parse(
                    currentTimestamp
                )
            )
        )

        val rsrp = state.Rsrp.replace(" dBm", "").toLongOrNull() ?: 0L
        val rssi = state.Rssi.replace(" dBm", "").toLongOrNull() ?: 0L
        val rsrq = state.Rsrq.replace(" dB", "").toLongOrNull() ?: 0L
        val rssnr = state.Rssnr.replace(" dB", "").toLongOrNull() ?: 0L
        val cqi = state.Cqi.toLongOrNull() ?: 0L
        val bandwidth = state.Bandwidth.toLongOrNull() ?: 0L
        val cellid = state.Cellid.toLongOrNull() ?: 0L

        val messageData = MessageData(
            state.JwtToken,  // JWT
            state.Uuid,       // UUID
            formattedTimestamp,
            state.Latitude.toDoubleOrNull() ?: 0.0,
            state.Longtitude.toDoubleOrNull() ?: 0.0,
            rsrp,
            rssi,
            rsrq,
            rssnr,
            cqi,
            bandwidth,
            cellid
        )

        return Json.encodeToString(messageData)
    }

    private fun requestUsageStatsPermission() {
        val intent = Intent(Settings.ACTION_USAGE_ACCESS_SETTINGS)
        startActivity(intent)
    }

    // Функция для проверки разрешения USAGE STATS
    @RequiresApi(Build.VERSION_CODES.M)
    private fun hasUsageStatsPermission(context: Context): Boolean {
        val appOps = context.getSystemService(Context.APP_OPS_SERVICE) as android.app.AppOpsManager
        val mode = appOps.checkOpNoThrow(android.app.AppOpsManager.OPSTR_GET_USAGE_STATS,
            android.os.Process.myUid(), context.packageName)
        return mode == android.app.AppOpsManager.MODE_ALLOWED
    }

    private fun checkAndRequestPermissions() {
        val context = applicationContext
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) { // Android 11 (API 30) и ниже
            if (!checkPermissions(context)) {
                Log.d(TAG, "Requesting permissions (API <= 30)")
                ActivityCompat.requestPermissions(
                    this,
                    arrayOf(
                        Manifest.permission.ACCESS_COARSE_LOCATION,
                        Manifest.permission.ACCESS_FINE_LOCATION,
                        Manifest.permission.READ_PHONE_STATE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE
                    ),
                    REQUEST_CODE_PERMISSIONS
                )
            } else {
                getLocation(state, applicationContext)
            }
        } else { // Android 13 (API 33) и выше
            if (if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    !checkPermissionsForAndroid13(context)
                } else {
                    TODO("VERSION.SDK_INT < TIRAMISU")
                }
            ) {
                Log.d(TAG, "Requesting permissions (API >= 33)")
                ActivityCompat.requestPermissions(
                    this,
                    arrayOf(
                        Manifest.permission.ACCESS_COARSE_LOCATION,
                        Manifest.permission.ACCESS_FINE_LOCATION,
                        Manifest.permission.READ_PHONE_STATE,
                        Manifest.permission.READ_MEDIA_IMAGES, // Для доступа к изображениям
                        Manifest.permission.READ_MEDIA_VIDEO, // Для доступа к видео
                        Manifest.permission.READ_MEDIA_AUDIO  // Для доступа к аудио
                    ),
                    REQUEST_CODE_PERMISSIONS
                )
            } else {
                getLocation(state, applicationContext)
            }
        }
    }

    private fun checkPermissions(context: Context): Boolean { // Для Android 11 (API 30) и ниже
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.ACCESS_COARSE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_PHONE_STATE
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
                ) == PackageManager.PERMISSION_GRANTED
    }

    @RequiresApi(Build.VERSION_CODES.TIRAMISU) // Для Android 13 (API 33) и выше
    private fun checkPermissionsForAndroid13(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.ACCESS_COARSE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_PHONE_STATE
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_MEDIA_IMAGES // Для доступа к изображениям
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_MEDIA_VIDEO // Для доступа к видео
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_MEDIA_AUDIO // Для доступа к аудио
                ) == PackageManager.PERMISSION_GRANTED
    }

    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun MainContent(state: MainActivityState) {
        val context = LocalContext.current
        var permissionsGranted by remember {
            mutableStateOf(
                if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) checkPermissions(context)
                else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    checkPermissionsForAndroid13(context)
                } else {
                    TODO("VERSION.SDK_INT < TIRAMISU")
                }
            )
        }
        val scaffoldState = rememberScaffoldState()
        var showConnectionSnackbar by remember { mutableStateOf(false) }
        var showChart by remember { mutableStateOf(false) } // Состояние для отображения графика
        var selectedAppName by remember { mutableStateOf("") } // Состояние для имени выбранного приложения

        // Запускаем корутину для показа Snackbar
        LaunchedEffect(isWebSocketConnected) {
            if (isWebSocketConnected) {
                showConnectionSnackbar = true
                // Отображаем Snackbar
                scaffoldState.snackbarHostState.showSnackbar(
                    message = "WebSocket connected!",
                    duration = SnackbarDuration.Short
                )
                delay(2000) // Прячем Snackbar через 2 секунды
                showConnectionSnackbar = false
            }
        }

        LaunchedEffect(context) {
            permissionsGranted =
                if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) checkPermissions(context)
                else checkPermissionsForAndroid13(context)
        }

        Box(modifier = Modifier.fillMaxSize()) { // Используем Box для позиционирования
            Scaffold(
                scaffoldState = scaffoldState
            ) { innerPadding ->
                if (permissionsGranted) {
                    LaunchedEffect(Unit) {
                        while (true) {
                            getLocation(state, applicationContext)
                            getSignalStrength(state)
                            delay(UPDATE_INTERVAL)
                        }
                    }

                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(innerPadding), // Добавляем отступы от Scaffold
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        TabRow(selectedTabIndex = state.selectedTabIndex) {
                            Tab(
                                selected = state.selectedTabIndex == 0,
                                onClick = { state.selectedTabIndex = 0 },
                                text = { Text("Вход") }
                            )
                            Tab(
                                selected = state.selectedTabIndex == 1,
                                onClick = { state.selectedTabIndex = 1 },
                                text = { Text("Данные") }
                            )
                            Tab(
                                selected = state.selectedTabIndex == 2,
                                onClick = { state.selectedTabIndex = 2 },
                                text = { Text("Графики") }
                            )
                            Tab(
                                selected = state.selectedTabIndex == 3,
                                onClick = { state.selectedTabIndex = 3 },
                                text = { Text("Карта") }
                            )
                            Tab(
                                selected = state.selectedTabIndex == 4,
                                onClick = { state.selectedTabIndex = 4 },
                                text = { Text("Трафик") }
                            )
                        }
                        when (state.selectedTabIndex) {
                            0 -> LoginScreen(state)
                            1 -> DataScreen(state)
                            2 -> RSRPGraph(state)
                            3 -> MapScreen(state)
                            4 -> TrafficScreen(state)
                        }
                    }
                } else {
                    Text("Waiting for permissions...")
                }
            }
            // Индикатор отправки данных в правом нижнем углу
            if (isSendingData) {
                Box(
                    modifier = Modifier
                        .align(Alignment.BottomEnd) // Выравнивание по правому нижнему углу
                        .padding(16.dp)
                        .size(16.dp)
                        .background(Color.Green, CircleShape)
                )
            }
        }
    }

    @Composable
    fun TrafficScreen(state: MainActivityState) {
        val context = LocalContext.current
        val appTrafficData = remember { mutableStateOf(emptyList<AppTrafficData>()) }
        var days by remember { mutableStateOf("1") }
        var showError by remember { mutableStateOf(false) }
        var showChart by remember { mutableStateOf(false) }
        var selectedAppName by remember { mutableStateOf("") }

        // Получаем общую статистику трафика
        val totalTrafficData = remember { mutableStateOf(TotalTrafficData(0L, 0L, 0L)) }

        LaunchedEffect(appTrafficData.value, days) {
            val numDays = days.toIntOrNull()
            if (numDays != null) {
                showError = false
                while (true) {
                    appTrafficData.value =
                        getAppTrafficData(context, numDays).sortedByDescending { it.totalBytes }
                    // Обновляем общую статистику
                    totalTrafficData.value = getTotalTrafficData(context, numDays)
                    delay(5000) // Раз 5 секунд
                }
            } else {
                showError = true
            }
        }

        Column(modifier = Modifier.fillMaxSize()) {
            // Выводим общую статистику
            Text(
                text = "Total Traffic: ${(totalTrafficData.value.totalBytes / 1024)} Kb, " +
                        "Mobile: ${(totalTrafficData.value.mobileBytes / 1024)} Kb, " +
                        "Wi-Fi: ${(totalTrafficData.value.wifiBytes / 1024)} Kb",
                modifier = Modifier.padding(16.dp)
            )

            // Поле для ввода количества дней
            OutlinedTextField(
                value = days,
                onValueChange = { days = it },
                label = { Text("Days") },
                modifier = Modifier.padding(16.dp)
            )

            // Сообщение об ошибке
            if (showError) {
                Text(
                    text = "Invalid number of days",
                    color = Color.Red,
                    modifier = Modifier.padding(16.dp)
                )
            }

            // Проверяем, есть ли разрешение
            if (!hasUsageStatsPermission(context)) {
                Button(onClick = {
                    requestUsageStatsPermission()
                }) {
                    Text("Grant Usage Stats Permission")
                }
            }

            LazyColumn(modifier = Modifier.fillMaxSize()) {
                items(appTrafficData.value) { appData ->
                    TrafficItem(appData, onShowChart = { appName ->
                        selectedAppName = appName
                        showChart = true
                    })
                }
            }
            // Отображаем график, если showChart = true
            if (showChart) {
                HourlyTrafficChart(
                    appName = selectedAppName,
                    onClose = { showChart = false },
                    context = context
                )
            }
        }
    }


    // Функция для получения общей статистики трафика
    @RequiresApi(Build.VERSION_CODES.M)
    private fun getTotalTrafficData(context: Context, days: Int): TotalTrafficData {
        var mobileBytes = 0L
        var wifiBytes = 0L
        val currentTime = System.currentTimeMillis()
        val startTime = currentTime - TimeUnit.DAYS.toMillis(days.toLong())

        val networkStatsManager = context.getSystemService(NETWORK_STATS_SERVICE) as NetworkStatsManager

        try {
            // Мобильный трафик
            val mobileStats = networkStatsManager.querySummaryForDevice(ConnectivityManager.TYPE_MOBILE, null, startTime, currentTime)
            mobileBytes = mobileStats.rxBytes + mobileStats.txBytes

            // Wi-Fi трафик
            val wifiStats = networkStatsManager.querySummaryForDevice(ConnectivityManager.TYPE_WIFI, null, startTime, currentTime)
            wifiBytes = wifiStats.rxBytes + wifiStats.txBytes
        } catch (e: Exception) {
            Log.e(TAG, "Error fetching network stats", e)
        }

        return TotalTrafficData(mobileBytes + wifiBytes, mobileBytes, wifiBytes)
    }

    // Класс для хранения данных общей статистики
    data class TotalTrafficData(
        val totalBytes: Long,
        val mobileBytes: Long,
        val wifiBytes: Long
    )

    private fun getLocation(state: MainActivityState, context: Context) {
        Log.d(TAG, "getLocation() called")

        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED
        ) {

            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.ACCESS_FINE_LOCATION),
                LOCATION_PERMISSION_REQUEST_CODE
            )
            return
        }

        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_COARSE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            Log.d(TAG, "No permission to write to external storage")
            return
        }

        val locationRequest = LocationRequest.create().apply {
            interval = UPDATE_INTERVAL
            fastestInterval = UPDATE_INTERVAL
            priority = LocationRequest.PRIORITY_HIGH_ACCURACY
        }

        fusedLocationClient.lastLocation.addOnSuccessListener { location ->
            location?.let {
                state.Latitude = location.latitude.toString()
                state.Longtitude = location.longitude.toString()
                Log.d(TAG, "Location received: Lat=${state.Latitude}, Lon=${state.Longtitude}")
            }
        }.addOnFailureListener { e ->
            Log.e(TAG, "Failed to get last known location", e)
        }

        fusedLocationClient.requestLocationUpdates(locationRequest, object : LocationCallback() {
            override fun onLocationResult(locationResult: LocationResult) {
                super.onLocationResult(locationResult)
                locationResult.lastLocation?.let { location ->
                    state.Latitude = location.latitude.toString()
                    state.Longtitude = location.longitude.toString()
                    Log.d(TAG, "Location updated: Lat=${state.Latitude}, Lon=${state.Longtitude}")
                }
            }
        }, null)
    }

    private fun getSignalStrength(state: MainActivityState) {
        if (checkPhoneStatePermission(state.context)) {
            val telephonyManager =
                state.context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
            val cellInfoList = telephonyManager.allCellInfo
            if (cellInfoList.isNullOrEmpty()) {
                state.Rsrp = "CellInfo list is empty"
                state.Rssi = "CellInfo list is empty"
                state.Rsrq = "CellInfo list is empty"
                state.Rssnr = "CellInfo list is empty"
                state.Cqi = "CellInfo list is empty"
                state.Bandwidth = "CellInfo list is empty"
                state.Cellid = "Cell Info not available"
                state.Mcc = "N/A"
                state.Mnc = "N/A"
                state.Lac = "N/A"
                state.Tac = "N/A"
                state.Pci = "N/A"
                state.Earfcn = "N/A"
                state.Ci = "N/A"
                state.NetworkType = "N/A"
                state.SignalStrength = "N/A"
                state.BitErrorRate = "N/A"
                state.TimingAdvance = "N/A"

            } else {
                for (info in cellInfoList) {
                    if (info is CellInfoLte) {
                        val cellSignalStrengthLte = info.cellSignalStrength
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                            state.Rsrp = "${cellSignalStrengthLte.rsrp} dBm"
                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                                state.Rssi = "${cellSignalStrengthLte.rssi} dBm"
                            }
                            state.Rsrq = "${cellSignalStrengthLte.rsrq} dB"
                            state.Rssnr = "${cellSignalStrengthLte.rssnr} dB"
                            state.Cqi = "${cellSignalStrengthLte.cqi}"
                            state.Bandwidth = "${telephonyManager.dataNetworkType}"
                            state.Cellid = when (val cellLocation = telephonyManager.cellLocation) {
                                is GsmCellLocation -> cellLocation.cid.toString()
                                is CdmaCellLocation -> cellLocation.baseStationId.toString()
                                else -> "Cell ID not available"
                            }
                            state.Mcc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                                info.cellIdentity.mccString ?: "N/A"
                            } else {
                                "N/A"
                            }
                            state.Mnc = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                                info.cellIdentity.mncString ?: "N/A"
                            } else {
                                "N/A"
                            }
                            state.Lac = try {
                                (telephonyManager.cellLocation as? GsmCellLocation)?.lac?.toString()
                                    ?: "N/A"
                            } catch (e: Exception) {
                                "N/A"
                            }
                            state.Tac =
                                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
                                    info.cellIdentity.tac.toString()
                                } else {
                                    "N/A"
                                }
                            state.Pci = info.cellIdentity.pci.toString()
                            state.Earfcn = info.cellIdentity.earfcn.toString()
                            state.Ci = info.cellIdentity.ci.toString()
                            state.NetworkType = telephonyManager.networkType.toString()
                            state.SignalStrength =
                                cellSignalStrengthLte.dbm.toString() // Используем уровень сигнала в dBm
                            state.BitErrorRate = "N/A" // Недоступно для LTE
                            state.TimingAdvance =
                                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                                    cellSignalStrengthLte.timingAdvance.toString()
                                } else {
                                    "N/A"
                                }
                            Log.d(TAG, "RSRP value: ${state.Rsrp}")
                            Log.d(TAG, "Rssi value: ${state.Rssi}")
                            Log.d(TAG, "Rsrq value: ${state.Rsrq}")
                            Log.d(TAG, "Rssnr value: ${state.Rssnr}")
                            Log.d(TAG, "Cqi value: ${state.Cqi}")
                            Log.d(TAG, "Bandwidth value: ${state.Bandwidth}")
                            Log.d(TAG, "Cell ID value: ${state.Cellid}")
                            Log.d(TAG, "MCC: ${state.Mcc}")
                            Log.d(TAG, "MNC: ${state.Mnc}")
                            Log.d(TAG, "LAC: ${state.Lac}")
                            Log.d(TAG, "TAC: ${state.Tac}")
                            Log.d(TAG, "PCI: ${state.Pci}")
                            Log.d(TAG, "EARFCN: ${state.Earfcn}")
                            Log.d(TAG, "CI: ${state.Ci}")
                            Log.d(TAG, "Network Type: ${state.NetworkType}")
                            Log.d(TAG, "Signal Strength: ${state.SignalStrength}")
                            Log.d(TAG, "Bit Error Rate: ${state.BitErrorRate}")
                            Log.d(TAG, "Timing Advance: ${state.TimingAdvance}")
                        }
                        break
                    }
                }
            }
        } else {
            state.Rsrp = "No READ_PHONE_STATE permission"
            state.Rssi = "No READ_PHONE_STATE permission"
            state.Rsrq = "No READ_PHONE_STATE permission"
            state.Rssnr = "No READ_PHONE_STATE permission"
            state.Cqi = "No READ_PHONE_STATE permission"
            state.Bandwidth = "No READ_PHONE_STATE permission"
            state.Cellid = "No READ_PHONE_STATE permission"
            state.Mcc = "No READ_PHONE_STATE permission"
            state.Mnc = "No READ_PHONE_STATE permission"
            state.Lac = "No READ_PHONE_STATE permission"
            state.Tac = "No READ_PHONE_STATE permission"
            state.Pci = "No READ_PHONE_STATE permission"
            state.Earfcn = "No READ_PHONE_STATE permission"
            state.Ci = "No READ_PHONE_STATE permission"
            state.NetworkType = "No READ_PHONE_STATE permission"
            state.SignalStrength = "No READ_PHONE_STATE permission"
            state.BitErrorRate = "No READ_PHONE_STATE permission"
            state.TimingAdvance = "No READ_PHONE_STATE permission"
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun LoginScreen(state: MainActivityState) {
        var email by remember { mutableStateOf(state.Email) }
        var password by remember { mutableStateOf(state.Password) }
        var jwtToken by remember { mutableStateOf(state.JwtToken) }
        var rememberMe by remember { mutableStateOf(state.RememberMe) }
        var showRegistration by remember { mutableStateOf(false) }
        var showSuccessMessage by remember { mutableStateOf(false) }
        var showErrorMessage by remember { mutableStateOf(false) }

        Column(
            modifier = Modifier.fillMaxSize(),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = androidx.compose.foundation.layout.Arrangement.Center
        ) {
            if (showRegistration) {
                OutlinedTextField(
                    value = email,
                    onValueChange = { email = it },
                    label = { Text("Email") }
                )
                OutlinedTextField(
                    value = password,
                    onValueChange = { password = it },
                    label = { Text("Password") },
                    visualTransformation = PasswordVisualTransformation()
                )
                Button(onClick = {
                    registerUser(email, password) { response ->
                        if (response != null) {
                            jwtToken = response.jwt
                            state.JwtToken = response.jwt // Сохраняем JWT в state
                            state.Uuid = response.uuid // Сохраняем UUID в state
                            state.saveLoginData() // Сохраняем данные в SharedPreferences
                            showSuccessMessage = true
                            showRegistration = false
                        } else {
                            showErrorMessage = true
                        }
                    }
                }) {
                    Text("Register")
                }
            } else {
                OutlinedTextField(
                    value = email,
                    onValueChange = { email = it },
                    label = { Text("Email") }
                )
                OutlinedTextField(
                    value = password,
                    onValueChange = { password = it },
                    label = { Text("Password") },
                    visualTransformation = PasswordVisualTransformation()
                )
                OutlinedTextField(
                    value = jwtToken,
                    onValueChange = { jwtToken = it },
                    label = { Text("JWT Token") },
                    enabled = true // Делаем поле JWT Token редактируемым
                )
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Checkbox(
                        checked = rememberMe,
                        onCheckedChange = { rememberMe = it }
                    )
                    Text("Remember me")
                }
                Button(onClick = {
                    state.Email = email
                    state.Password = password
                    state.JwtToken = jwtToken
                    state.RememberMe = rememberMe
                    state.saveLoginData()
                    authenticateUser(email, password, jwtToken)
                    lifecycleScope.launch {
                        isSendingData = true // Устанавливаем флаг отправки
                        delay(500) // Задержка для отображения индикатора
                        isSendingData = false // Сбрасываем флаг отправки
                    }
                }) {
                    Text("Login")
                }
                Button(onClick = { showRegistration = true }) {
                    Text("Register")
                }
            }
            if (showSuccessMessage) {
                Text("Registration successful! Your JWT token is: $jwtToken")
            }
            if (showErrorMessage) {
                Text("Registration failed. Please try again.")
            }
        }
    }

    @Composable
    fun DataScreen(state: MainActivityState) {
        Column(
            modifier = Modifier.fillMaxSize(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = "RSRP value: ${state.Rsrp}"
            )
            Text(
                text = "Rssi value: ${state.Rssi}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Rsrq value: ${state.Rsrq}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Rssnr value: ${state.Rssnr}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Cqi value: ${state.Cqi}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Bandwidth: ${state.Bandwidth}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Cell ID: ${state.Cellid}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "LAT: ${state.Latitude}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "LON: ${state.Longtitude}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "MCC: ${state.Mcc}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "MNC: ${state.Mnc}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "LAC: ${state.Lac}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "TAC: ${state.Tac}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "PCI: ${state.Pci}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "EARFCN: ${state.Earfcn}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "CI: ${state.Ci}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Network Type: ${state.NetworkType}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Signal Strength: ${state.SignalStrength}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Bit Error Rate: ${state.BitErrorRate}",
                modifier = Modifier.padding(16.dp)
            )
            Text(
                text = "Timing Advance: ${state.TimingAdvance}",
                modifier = Modifier.padding(16.dp)
            )
        }
    }

    private fun checkPhoneStatePermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.READ_PHONE_STATE
        ) == PackageManager.PERMISSION_GRANTED
    }

    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun RSRPGraph(state: MainActivityState) {
        val rsrpData = remember { mutableStateListOf<Pair<Long, Float>>() }

        // Запускаем корутину для добавления новых данных в список
        LaunchedEffect(state.Rsrp, state.Cellid) {
            val timestamp = System.currentTimeMillis()
            val rsrpValue = state.Rsrp.replace(" dBm", "").toFloatOrNull() ?: 0f
            rsrpData.add(Pair(timestamp, rsrpValue))
        }

        // Используем Box для компоновки Canvas и осей
        Box(modifier = Modifier.fillMaxSize()) {

            // Оси графика размещаем внизу Canvas
            Column(
                modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .fillMaxWidth()
                    .padding(top = 16.dp) // Добавлен отступ сверху
            ) {
                Text(
                    "Время", modifier = Modifier
                        .padding(8.dp).align(Alignment.CenterHorizontally)
                )
            }
            Row(
                modifier = Modifier
                    .align(Alignment.BottomStart)
                    .fillMaxHeight()
                    .padding(end = 16.dp) // Добавлен отступ справа
            ) {
                Text(
                    "RSRP (dBm)", modifier = Modifier
                        .padding(8.dp)
                        .align(Alignment.CenterVertically)
                )
            }

            // Canvas для отрисовки графика (сверху осей)
            Canvas(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(
                        bottom = 48.dp,
                        start = 48.dp,
                        end = 16.dp,
                        top = 48.dp
                    ) // Отступы для осей и сверху
            ) {
                // Определяем максимальное значение RSRP для масштабирования
                val maxRSRP = rsrpData.maxOfOrNull { it.second } ?: 0f

                // Рисуем график
                rsrpData.forEachIndexed { index, (timestamp, rsrp) ->
                    // Вычисляем координаты точки на графике
                    val x = (index * size.width / (rsrpData.size - 1)).toFloat()
                    val y = size.height - (rsrp / maxRSRP * size.height)

                    // Рисуем точку
                    drawCircle(
                        color = Color.Blue,
                        center = Offset(x, y),
                        radius = 5f
                    )

                    // Соединяем точки линией, если это не первая точка
                    if (index > 0) {
                        val previousX = ((index - 1) * size.width / (rsrpData.size - 1)).toFloat()
                        val previousY =
                            size.height - (rsrpData[index - 1].second / maxRSRP * size.height)
                        drawLine(
                            start = Offset(previousX, previousY),
                            end = Offset(x, y),
                            color = Color.Red,
                            strokeWidth = 2f
                        )
                    }

                    // Отображаем Cell ID под точкой
                    drawContext.canvas.nativeCanvas.drawText(
                        state.Cellid,
                        x,
                        y + 20.dp.toPx(), // Сдвигаем текст вниз от точки
                        android.graphics.Paint().apply {
                            textSize = 10.sp.toPx()
                            color = android.graphics.Color.BLACK
                        }
                    )
                }
            }
        }
    }

    @Composable
    fun MapScreen(state: MainActivityState) {
        LaunchedEffect(state.Latitude, state.Longtitude, state.Rsrp) {
            val lat = state.Latitude.toDoubleOrNull()
            val lng = state.Longtitude.toDoubleOrNull()
            if (lat != null && lng != null) {
                val color =
                    generateColorFromRSRP(state.Rsrp.replace(" dBm", "").toIntOrNull() ?: -140)
                state.locations.add(Pair(LatLng(lat, lng), color))
            }
        }
        val cameraPositionState = rememberCameraPositionState {
            position = CameraPosition.fromLatLngZoom(LatLng(55.0415, 82.9346), 10f)
        }
        GoogleMap(
            modifier = Modifier.fillMaxSize(),
            cameraPositionState = cameraPositionState
        ) {
            // Линия
            Polyline(
                points = state.locations.map { it.first }, // используем state.locations
                color = Color.Red,
                width = 5f
            )
            // Точки
            state.locations.forEach { (location, color) -> // используем state.locations
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

    data class AppTrafficData(
        val appName: String,
        val totalBytes: Long,
        val mobileBytes: Long,
        val wifiBytes: Long,
        val rxBytes: Long, // Downlink
        val txBytes: Long  // Uplink
    )

    @Composable
    fun TrafficItem(appData: AppTrafficData, onShowChart: (String) -> Unit) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            horizontalArrangement = Arrangement.SpaceBetween, // Располагаем элементы по краям
            verticalAlignment = Alignment.CenterVertically // Выравниваем элементы по вертикали
        ) {
            Column {
                Text(text = appData.appName)
                Text(text = "Total: ${(appData.totalBytes / 1024).toString()} Kb")
                Text(text = "Mobile: ${(appData.mobileBytes / 1024).toString()} Kb")
                Text(text = "Wi-Fi: ${(appData.wifiBytes / 1024).toString()} Kb")
                Text(text = "Downlink: ${(appData.rxBytes / 1024).toString()} Kb")
                Text(text = "Uplink: ${(appData.txBytes / 1024).toString()} Kb")
            }
            // Кнопка "Show Chart" - для отображения почасового графика
            Button(onClick = { onShowChart(appData.appName) }) {
                Text("Show Chart")
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.M)
    private fun getAppTrafficData(context: Context, days: Int): List<AppTrafficData> {
        val appTrafficDataList = mutableListOf<AppTrafficData>()
        val packageManager = context.packageManager
        val networkStatsManager = context.getSystemService(Context.NETWORK_STATS_SERVICE) as NetworkStatsManager

        val currentTime = System.currentTimeMillis()
        val startTime = currentTime - TimeUnit.DAYS.toMillis(days.toLong())

        for (packageInfo in packageManager.getInstalledApplications(PackageManager.GET_META_DATA)) {
            val uid = packageInfo.uid
            val appName = packageManager.getApplicationLabel(packageInfo).toString()

            var mobileBytes = 0L
            var wifiBytes = 0L
            var totalDownlinkBytes = 0L  // Суммарный Downlink
            var totalUplinkBytes = 0L    // Суммарный Uplink

            try {
                // Получаем мобильный трафик для UID
                val mobileStats = networkStatsManager.queryDetailsForUid(
                    ConnectivityManager.TYPE_MOBILE,
                    null,
                    startTime,
                    currentTime,
                    uid
                )
                var bucket = NetworkStats.Bucket()
                while (mobileStats.hasNextBucket()) {
                    mobileStats.getNextBucket(bucket)
                    if (bucket.uid == uid) {
                        mobileBytes += bucket.rxBytes + bucket.txBytes
                        totalDownlinkBytes += bucket.rxBytes
                        totalUplinkBytes += bucket.txBytes
                    }
                }
                mobileStats.close()

                // Получаем Wi-Fi трафик для UID
                val wifiStats = networkStatsManager.queryDetailsForUid(
                    ConnectivityManager.TYPE_WIFI,
                    null,
                    startTime,
                    currentTime,
                    uid
                )
                bucket = NetworkStats.Bucket()
                while (wifiStats.hasNextBucket()) {
                    wifiStats.getNextBucket(bucket)
                    if (bucket.uid == uid) {
                        wifiBytes += bucket.rxBytes + bucket.txBytes
                        totalDownlinkBytes += bucket.rxBytes
                        totalUplinkBytes += bucket.txBytes
                    }
                }
                wifiStats.close()

                val totalBytes = mobileBytes + wifiBytes
                appTrafficDataList.add(
                    AppTrafficData(
                        appName,
                        totalBytes,
                        mobileBytes,
                        wifiBytes,
                        totalDownlinkBytes, // Downlink
                        totalUplinkBytes  // Uplink
                    )
                )

            } catch (e: Exception) {
                Log.e("AppTraffic", "Error getting traffic data for $appName: ${e.message}", e)
            }
        }

        return appTrafficDataList
    }
    @RequiresApi(Build.VERSION_CODES.M)
    @Composable
    fun HourlyTrafficChart(appName: String, onClose: () -> Unit, context: Context) {
        val hourlyTrafficData = remember { mutableStateListOf<Pair<Int, Long>>() }
        val packageName = getPackageNameForApp(context, appName)

        // Получаем почасовую статистику трафика для приложения
        LaunchedEffect(packageName) {
            if (packageName != null) {
                hourlyTrafficData.addAll(getHourlyTrafficData(context, packageName))
            }
        }

        // Диалог с графиком
        Dialog(onDismissRequest = onClose) {
            Surface(shape = RoundedCornerShape(8.dp)) {
                Column(modifier = Modifier.padding(16.dp)) {
                    // Заголовок с названием приложения
                    Text(
                        text = "Hourly Traffic for $appName",
                        fontSize = 18.sp,
                        fontWeight = FontWeight.Bold
                    )

                    // Кнопка "Close"
                    IconButton(onClick = onClose, modifier = Modifier.align(Alignment.End)) {
                        Icon(imageVector = Icons.Default.Close, contentDescription = "Close")
                    }

                    // График (отображаем, только если имя пакета найдено)
                    if (packageName != null) {
                        HourlyTrafficChartContent(hourlyTrafficData)
                    } else {
                        Text("Error: Package not found", color = Color.Red)
                    }
                }
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.M)
    private fun getPackageNameForApp(context: Context, appName: String): String? {
        val packageManager = context.packageManager
        for (packageInfo in packageManager.getInstalledApplications(PackageManager.GET_META_DATA)) {
            val currentAppName = packageManager.getApplicationLabel(packageInfo).toString()
            if (currentAppName == appName) {
                return packageInfo.packageName
            }
        }
        return null // Возвращаем null, если пакет не найден
    }

    @RequiresApi(Build.VERSION_CODES.M)
    @Composable
    fun HourlyTrafficChartContent(hourlyTrafficData: List<Pair<Int, Long>>) {
        val scrollState = rememberScrollState()
        val hourWidth = 50.dp
        val chartWidth = hourWidth * 24
        val maxTraffic = hourlyTrafficData.maxOfOrNull { it.second } ?: 1L
        val maxTrafficKb = (maxTraffic / 1024).toFloat()

        Box(modifier = Modifier.horizontalScroll(scrollState)) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(8.dp)
            ) {
                Canvas(modifier = Modifier.height(200.dp).width(40.dp)) {
                    val stepSize = maxTrafficKb / 5
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
                    hourlyTrafficData.forEachIndexed { index, (hour, traffic) ->
                        val x = index * hourWidth.toPx()
                        val trafficKb = (traffic / 1024).toFloat()
                        val barHeight = (trafficKb / maxTrafficKb * size.height).coerceAtLeast(0f) // Ограничиваем минимальную высоту столбца

                        drawRect(
                            color = Color.Blue,
                            topLeft = Offset(x, size.height - barHeight),
                            size = androidx.compose.ui.geometry.Size(
                                hourWidth.toPx() - 4.dp.toPx(),
                                barHeight
                            )
                        )

                        // Отображение времени
                        drawContext.canvas.nativeCanvas.drawText(
                            String.format("%02d:00", hour),
                            x + hourWidth.toPx() / 2,
                            size.height + 15f,
                            android.graphics.Paint().apply {
                                textSize = 10.sp.toPx()
                                color = android.graphics.Color.BLACK
                                textAlign = android.graphics.Paint.Align.CENTER
                            }
                        )

                        // Отображение значения трафика
                        drawContext.canvas.nativeCanvas.drawText(
                            String.format("%.1f Kb", trafficKb),
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

    @RequiresApi(Build.VERSION_CODES.M)
    private fun getHourlyTrafficData(context: Context, packageName: String): List<Pair<Int, Long>> {
        val currentHour = java.util.Calendar.getInstance().get(java.util.Calendar.HOUR_OF_DAY)
        val hourlyTrafficData = MutableList(24) { i -> Pair((i + currentHour) % 24, 0L) } // Заполняем список нулями
        val networkStatsManager = context.getSystemService(Context.NETWORK_STATS_SERVICE) as NetworkStatsManager
        val packageManager = context.packageManager

        val currentTime = System.currentTimeMillis()
        val startTime = currentTime - TimeUnit.DAYS.toMillis(1)

        val uid = try {
            packageManager.getApplicationInfo(packageName, 0).uid
        } catch (e: PackageManager.NameNotFoundException) {
            Log.e(TAG, "Error: Package $packageName not found", e)
            return emptyList()
        }

        try {
            // Мобильный трафик
            val mobileStats = networkStatsManager.queryDetailsForUid(
                ConnectivityManager.TYPE_MOBILE,
                null,
                startTime,
                currentTime,
                uid
            )
            sumTrafficForHours(mobileStats, hourlyTrafficData, currentHour)
            mobileStats.close()

            // Wi-Fi трафик
            val wifiStats = networkStatsManager.queryDetailsForUid(
                ConnectivityManager.TYPE_WIFI,
                null,
                startTime,
                currentTime,
                uid
            )
            sumTrafficForHours(wifiStats, hourlyTrafficData, currentHour)
            wifiStats.close()
        } catch (e: Exception) {
            Log.e(TAG, "Error fetching network stats: ${e.message}", e)
        }

        return hourlyTrafficData
    }

    @RequiresApi(Build.VERSION_CODES.M)
    private fun sumTrafficForHours(networkStats: NetworkStats, hourlyTrafficData: MutableList<Pair<Int, Long>>, currentHour: Int) {
        while (networkStats.hasNextBucket()) {
            val bucket = NetworkStats.Bucket()
            networkStats.getNextBucket(bucket)

            val bucketStartHour = java.util.Calendar.getInstance().apply {
                timeInMillis = bucket.startTimeStamp
            }.get(java.util.Calendar.HOUR_OF_DAY)

            val bucketEndHour = java.util.Calendar.getInstance().apply {
                timeInMillis = bucket.endTimeStamp
            }.get(java.util.Calendar.HOUR_OF_DAY)

            // Определяем, в какие часы попадает bucket
            val hours = if (bucketStartHour <= bucketEndHour) {
                bucketStartHour..bucketEndHour
            } else {
                (bucketStartHour..23) + (0..bucketEndHour)
            }

            hours.forEach { hour ->
                val displayHour = (hour + currentHour) % 24
                val index = (displayHour - currentHour + 24) % 24
                val traffic = hourlyTrafficData[index].second
                hourlyTrafficData[index] = Pair(displayHour, traffic + bucket.rxBytes + bucket.txBytes)
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

    @SuppressLint("AutoboxingStateCreation")
    class MainActivityState(val context: Context) {
        var Latitude by mutableStateOf("")
        var Longtitude by mutableStateOf("")
        var Rsrp by mutableStateOf("")
        var Rssi by mutableStateOf("")
        var Rsrq by mutableStateOf("")
        var Rssnr by mutableStateOf("")
        var Cqi by mutableStateOf("")
        var Bandwidth by mutableStateOf("")
        var Cellid by mutableStateOf("")
        var Mcc by mutableStateOf("")
        var Mnc by mutableStateOf("")
        var Lac by mutableStateOf("")
        var Tac by mutableStateOf("")
        var Pci by mutableStateOf("")
        var Earfcn by mutableStateOf("")
        var Ci by mutableStateOf("")
        var NetworkType by mutableStateOf("")
        var SignalStrength by mutableStateOf("")
        var BitErrorRate by mutableStateOf("")
        var TimingAdvance by mutableStateOf("")
        var selectedTabIndex by mutableStateOf(0)

        //Для тепловой карты (точки)
        val locations = mutableStateListOf<Pair<LatLng, Color>>()

        var Email by mutableStateOf("")
        var Password by mutableStateOf("")
        var JwtToken by mutableStateOf("")
        var Uuid by mutableStateOf("") // Добавляем UUID в state
        var RememberMe by mutableStateOf(false)

        fun saveLoginData() {
            val sharedPreferences =
                context.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE)
            with(sharedPreferences.edit()) {
                putString(EMAIL_KEY, Email)
                putString(PASSWORD_KEY, Password)
                putString(JWT_TOKEN_KEY, JwtToken)
                putString(UUID_KEY, Uuid) // Сохраняем UUID
                putBoolean(REMEMBER_ME_KEY, RememberMe)
                apply()
            }
        }

        fun loadLoginData() {
            val sharedPreferences =
                context.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE)
            Email = sharedPreferences.getString(EMAIL_KEY, "") ?: ""
            Password = sharedPreferences.getString(PASSWORD_KEY, "") ?: ""
            JwtToken = sharedPreferences.getString(JWT_TOKEN_KEY, "") ?: ""
            Uuid = sharedPreferences.getString(UUID_KEY, "") ?: "" // Загружаем UUID
            RememberMe = sharedPreferences.getBoolean(REMEMBER_ME_KEY, false)
        }
    }

    @Serializable
    data class LocationData(
        val Latitude: String,
        val longitude: String,
        val rsrp: String,
        val Rssi: String,
        val Rsrq: String,
        val Rssnr: String,
        val Cqi: String,
        val Bandwidth: String,
        val Cellid: String,
        val timestamp: String
    )

    @Serializable
    data class MessageData(
        val jwt: String,    // JWT
        val uuid: String,   // UUID
        val time: String,
        val latitude: Double,
        val Longitude: Double,
        val rsrp: Long,
        val rssi: Long,
        val rsrq: Long,
        val rssnr: Long,
        val cqi: Long,
        val bandwidth: Long,
        val cellID: Long
    )

    @Serializable
    data class RegisterResponse(
        val email: String,
        val jwt: String,
        val message: String,
        val uuid: String
    )

    @Serializable
    data class AuthResponse(
        val email: String,
        val jwt: String,
        val uuid: String
    )
}