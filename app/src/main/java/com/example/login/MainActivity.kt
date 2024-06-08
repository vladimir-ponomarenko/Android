@file:Suppress("DEPRECATION")

package com.example.login

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.net.TrafficStats
import android.os.Build
import android.os.Bundle
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
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material.Button
import androidx.compose.material.Checkbox
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.Scaffold
import androidx.compose.material.SnackbarDuration
import androidx.compose.material.Tab
import androidx.compose.material.TabRow
import androidx.compose.material.Text
import androidx.compose.material.rememberScaffoldState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateMapOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.nativeCanvas
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
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

    @RequiresApi(Build.VERSION_CODES.N)
    private fun getSignalStrength(state: MainActivityState) {
        if (checkPhoneStatePermission(state.context)) {
            val telephonyManager =
                state.context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
            val cellInfoList = telephonyManager.allCellInfo

            if (cellInfoList.isNullOrEmpty()) {
                // Очищаем данные, если список пуст
                state.Rsrp = "N/A"
                state.Rssi = "N/A"
                state.Rsrq = "N/A"
                state.Rssnr = "N/A"
                state.Cqi = "N/A"
                state.Bandwidth = "N/A"
                state.Cellid = "N/A"
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
                state.Band = "N/A"
                state.Operator = "N/A"
                state.Technology = "N/A"
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
                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                                state.Bandwidth = "${info.cellIdentity.bandwidth / 1000} MHz"
                            }
                            state.Cellid = when (val cellLocation = telephonyManager.cellLocation) {
                                is GsmCellLocation -> cellLocation.cid.toString()
                                is CdmaCellLocation -> cellLocation.baseStationId.toString()
                                else -> "N/A"
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
                            state.Band = getBandFromEarfcn(info.cellIdentity.earfcn)
                            state.Operator = telephonyManager.networkOperatorName
                            state.Technology = "LTE"
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
                            Log.d(TAG, "Band: ${state.Band}")
                            Log.d(TAG, "Operator: ${state.Operator}")
                            Log.d(TAG, "Technology: ${state.Technology}")
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
            state.Band = "No READ_PHONE_STATE permission"
            state.Operator = "No READ_PHONE_STATE permission"
            state.Technology = "No READ_PHONE_STATE permission"
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
                            state.Uuid = response.uuid
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
            modifier = Modifier.fillMaxSize(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            items(dataItems) { item ->
                Text(text = item, modifier = Modifier.padding(16.dp))
            }
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
                    .padding(top = 16.dp)
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
                    .padding(end = 16.dp)
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




    private val totalRxBytes = mutableStateMapOf<Int, Long>()
    private val totalTxBytes = mutableStateMapOf<Int, Long>()
    private val totalWifiRxBytes = mutableStateMapOf<Int, Long>()
    private val totalWifiTxBytes = mutableStateMapOf<Int, Long>()
    private val totalMobileRxBytes = mutableStateMapOf<Int, Long>()
    private val totalMobileTxBytes = mutableStateMapOf<Int, Long>()

    @Composable
    fun TrafficScreen(state: MainActivityState) {
        val context = LocalContext.current
        val appTrafficData = remember { mutableStateOf(emptyList<AppTrafficData>()) }

        LaunchedEffect(Unit) {
            while (true) {
                appTrafficData.value = getAppTrafficData(context).sortedByDescending { it.totalBytes }
                state.mobileTraffic =
                    TrafficStats.getMobileRxBytes() + TrafficStats.getMobileTxBytes()
                state.wifiTraffic =
                    TrafficStats.getTotalRxBytes() - TrafficStats.getMobileRxBytes() +
                            TrafficStats.getTotalTxBytes() - TrafficStats.getMobileTxBytes()
                delay(1000) // Обновляем каждую секунду
            }
        }

        Column(modifier = Modifier.fillMaxSize()) {
            // Общий
            Text(
                text = "Total Traffic: ${(state.mobileTraffic + state.wifiTraffic) / 1024} Kb, Mobile: ${(state.mobileTraffic / 1024).toString()} Kb, Wi-Fi: ${(state.wifiTraffic / 1024).toString()} Kb",
                modifier = Modifier.padding(16.dp)
            )

            LazyColumn(modifier = Modifier.fillMaxSize()) {
                items(appTrafficData.value) { appData ->
                    TrafficItem(appData)
                }
            }
        }
    }

    @Composable
    fun TrafficItem(appData: AppTrafficData) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp)
        ) {
            Text(text = appData.appName)
            Text(text = "Total: ${(appData.totalBytes / 1024).toString()} Kb")
            Text(text = "Wi-Fi Total: ${(appData.wifiTotalBytes / 1024).toString()} Kb")
            Text(text = "Mobile Total: ${(appData.mobileTotalBytes / 1024).toString()} Kb")
            Text(text = "Wi-Fi Downlink: ${(appData.wifiRxBytes / 1024).toString()} Kb")
            Text(text = "Wi-Fi Uplink: ${(appData.wifiTxBytes / 1024).toString()} Kb")
            Text(text = "Mobile Downlink: ${(appData.mobileRxBytes / 1024).toString()} Kb")
            Text(text = "Mobile Uplink: ${(appData.mobileTxBytes / 1024).toString()} Kb")
        }
    }
    //        TODO: настроить анализ трафика для каждого приложения. Сейчас у каждого приложения все поля равны 0.
//        Возможные варианты: использовать скрытый API; использовать NetStats.
    private fun getAppTrafficData(context: Context): List<AppTrafficData> {
        val appTrafficDataList = mutableListOf<AppTrafficData>()
        val packageManager = context.packageManager
        val sharedPreferences = context.getSharedPreferences("traffic_stats", Context.MODE_PRIVATE)

        for (packageInfo in packageManager.getInstalledApplications(PackageManager.GET_META_DATA)) {
            val uid = packageInfo.uid
            val appName = packageManager.getApplicationLabel(packageInfo).toString()

            // Получаем текущие значения трафика
            val rxBytes = TrafficStats.getUidRxBytes(uid)
            val txBytes = TrafficStats.getUidTxBytes(uid)

            // Получаем предыдущие значения трафика из SharedPreferences
            val prevRxBytes = sharedPreferences.getLong("rxBytes_$uid", 0)
            val prevTxBytes = sharedPreferences.getLong("txBytes_$uid", 0)

            // Вычисляем разницу
            val diffRxBytes = rxBytes - prevRxBytes
            val diffTxBytes = txBytes - prevTxBytes

            // Сохраняем текущие значения как предыдущие для следующей итерации
            sharedPreferences.edit()
                .putLong("rxBytes_$uid", rxBytes)
                .putLong("txBytes_$uid", txBytes)
                .apply()

            // Мобильный и Wi-Fi трафик
            val mobileRxBytes = TrafficStats.getUidRxBytes(uid)
            val mobileTxBytes = TrafficStats.getUidTxBytes(uid)
            val wifiRxBytes = diffRxBytes - mobileRxBytes
            val wifiTxBytes = diffTxBytes - mobileTxBytes

            appTrafficDataList.add(
                AppTrafficData(
                    appName,
                    diffRxBytes,
                    diffTxBytes,
                    wifiRxBytes,
                    wifiTxBytes,
                    mobileRxBytes,
                    mobileTxBytes
                )
            )
        }

        return appTrafficDataList
    }


    data class AppTrafficData(
        val appName: String,
        val rxBytes: Long,
        val txBytes: Long,
        val wifiRxBytes: Long,
        val wifiTxBytes: Long,
        val mobileRxBytes: Long,
        val mobileTxBytes: Long
    ) {
        val totalBytes: Long = rxBytes + txBytes
        val wifiTotalBytes: Long = wifiRxBytes + wifiTxBytes
        val mobileTotalBytes: Long = mobileRxBytes + mobileTxBytes
    }

    fun generateColorFromRSRP(rsrp: Int): Color {
        // Генерация цвета
        return when {
            rsrp >= -80 -> Color.Red // Хороший сигнал
            rsrp in -90..-81 -> Color.Blue // Средний сигнал
            else -> Color.Cyan // Слабый сигнал
        }
    }
    // Добавляем функцию для определения диапазона частот (Band)
    private fun getBandFromEarfcn(earfcn: Int): String {
        return when (earfcn) {
            in 1..600 -> "Band 1 (2100 MHz)"
            in 1200..1950 -> "Band 3 (1800 MHz)"
            in 1951..2650 -> "Band 4 (1700/2100 MHz AWS)"
            in 2750..3450 -> "Band 7 (2600 MHz)"
            in 3600..4150 -> "Band 8 (900 MHz)"
            in 6600..7100 -> "Band 20 (800 DD)"
            in 9210..9660 -> "Band 28 (700 APT)"
            in 9770..10280 -> "Band 38 (TD 2600)"
            in 25700..26200 -> "Band 41 (TD 2500)"
            in 65536..67535 -> "Band 71 (600 MHz)"
            else -> "Unknown"
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
        var Band by mutableStateOf("")
        var Technology by mutableStateOf("")
        var Operator by mutableStateOf("")
        var selectedTabIndex by mutableStateOf(0)

        //Для тепловой карты (точки)
        val locations = mutableStateListOf<Pair<LatLng, Color>>()

        // Для общего трафика
        var mobileTraffic by mutableStateOf(
            TrafficStats.getMobileRxBytes() + TrafficStats.getMobileTxBytes()
        )
        var wifiTraffic by mutableStateOf(
            TrafficStats.getTotalRxBytes() - TrafficStats.getMobileRxBytes() +
                    TrafficStats.getTotalTxBytes() - TrafficStats.getMobileTxBytes()
        )

        var Email by mutableStateOf("")
        var Password by mutableStateOf("")
        var JwtToken by mutableStateOf("")
        var Uuid by mutableStateOf("")
        var RememberMe by mutableStateOf(false)

        fun saveLoginData() {
            val sharedPreferences =
                context.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE)
            with(sharedPreferences.edit()) {
                putString(EMAIL_KEY, Email)
                putString(PASSWORD_KEY, Password)
                putString(JWT_TOKEN_KEY, JwtToken)
                putString(UUID_KEY, Uuid)
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
            Uuid = sharedPreferences.getString(UUID_KEY, "") ?: ""
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
        val cellID: Long,
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