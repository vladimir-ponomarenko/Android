@file:Suppress("DEPRECATION")

package com.example.login

import android.Manifest
import android.annotation.SuppressLint
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.os.IBinder
import android.telephony.CellInfoLte
import android.telephony.PhoneStateListener
import android.telephony.SignalStrength
import android.telephony.TelephonyManager
import android.telephony.cdma.CdmaCellLocation
import android.telephony.gsm.GsmCellLocation
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.annotation.RequiresApi
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
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
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.nativeCanvas
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleService
import androidx.lifecycle.lifecycleScope
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationCallback
import com.google.android.gms.location.LocationRequest
import com.google.android.gms.location.LocationResult
import com.google.android.gms.location.LocationServices
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

        const val WEBSOCKET_ENDPOINT = "/api/sockets/termalmap"

        // Имя ключа для SharedPreferences
        private const val SHARED_PREFS_NAME = "login_prefs"
        private const val EMAIL_KEY = "email"
        private const val PASSWORD_KEY = "password"
        private const val JWT_TOKEN_KEY = "jwt_token"
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

    private fun registerUser(email: String, password: String, onComplete: (String?) -> Unit) {
        val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val request = Request.Builder()
            .url("$SERVER_URL/api/user/register")
            .post(requestBody)
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to register user", e)
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful) {
                        Log.e(TAG, "Failed to register user: ${response.code}")
                        onComplete(null)
                        return
                    }
                    val jwt = response.body?.string()
                    Log.d(TAG, "JWT token received: $jwt")
                    onComplete(jwt)
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
                            val jsonResponse =
                                Json.decodeFromString<Map<String, String>>(responseBody ?: "")
                            val email = jsonResponse["email"]
                            val jwt = jsonResponse["jwt"]
                            if (email != null && jwt != null) {
                                Log.d(TAG, "User authenticated successfully")
                                // Запускаем сервис после успешной авторизации
                                startBackgroundService(email, jwt)
                                isWebSocketConnected = true // Устанавливаем флаг подключения
                            } else {
                                Log.e(TAG, "Failed to authenticate user: Invalid response format")
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
        if (!checkPermissions(context)) {
            Log.d(TAG, "Requesting permissions")
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
    }

    private fun checkPermissions(context: Context): Boolean {
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

    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun MainContent(state: MainActivityState) {
        val context = LocalContext.current
        var permissionsGranted by remember { mutableStateOf(checkPermissions(context)) }
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
            permissionsGranted = checkPermissions(context)
        }

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
                    }
                    when (state.selectedTabIndex) {
                        0 -> LoginScreen(state)
                        1 -> DataScreen(state)
                        2 -> RSRPGraph(state)
                    }
                }
            } else {
                Text("Waiting for permissions...")
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
                            Log.d(TAG, "RSRP value: ${state.Rsrp}")
                            Log.d(TAG, "Rssi value: ${state.Rssi}")
                            Log.d(TAG, "Rsrq value: ${state.Rsrq}")
                            Log.d(TAG, "Rssnr value: ${state.Rssnr}")
                            Log.d(TAG, "Cqi value: ${state.Cqi}")
                            Log.d(TAG, "Bandwidth value: ${state.Bandwidth}")
                            Log.d(TAG, "Cell ID value: ${state.Cellid}")
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
                    registerUser(email, password) { jwt ->
                        jwt?.let {
                            jwtToken = it
                            showSuccessMessage = true
                            showRegistration = false
                        } ?: run {
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
                    label = { Text("JWT Token") }
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
        }
    }

    private fun checkPhoneStatePermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.READ_PHONE_STATE
        ) == PackageManager.PERMISSION_GRANTED
    }

    @Composable
    fun RSRPGraph(state: MainActivityState) {
        val cellData = remember { mutableStateListOf<Pair<String, Float>>() }

        LaunchedEffect(state.Cellid) {
            cellData.add(Pair(state.Cellid, state.Rsrp.toFloatOrNull() ?: 0f))
        }

        Canvas(modifier = Modifier.fillMaxSize()) {
            drawLine(
                start = Offset(0f, size.height),
                end = Offset(size.width, size.height),
                color = Color.Black
            )

            if (cellData.size >= 2) {
                val xInterval = size.width / (cellData.size - 1)

                val paint = androidx.compose.ui.graphics.Paint().asFrameworkPaint()
                paint.color = Color.Black.toArgb()
                paint.textSize = 30f

                cellData.forEachIndexed { index, pair ->
                    val x = index * xInterval
                    val y = size.height - pair.second
                    drawCircle(color = Color.Blue, center = Offset(x, y), radius = 5f)
                    drawContext.canvas.nativeCanvas.drawText(pair.first, x, size.height + 20f, paint)
                }

                (0 until cellData.size - 1).forEach { index ->
                    val startX = index * xInterval
                    val startY = size.height - cellData[index].second
                    val endX = (index + 1) * xInterval
                    val endY = size.height - cellData[index + 1].second
                    drawLine(
                        start = Offset(startX, startY),
                        end = Offset(endX, endY),
                        color = Color.Red,
                        strokeWidth = 2f
                    )
                }
            }
        }
    }

    // Метод для запуска BackgroundService
    private fun startBackgroundService(email: String, jwt: String) {
        Intent(this, BackgroundService::class.java).also {
            it.action = ACTION_START_SERVICE
            it.putExtra("email", email) // Передаем email в сервис
            it.putExtra("jwt", jwt) // Передаем JWT в сервис
            startService(it)
        }
    }

    // Метод для остановки BackgroundService (при необходимости)
    private fun stopBackgroundService() {
        Intent(this, BackgroundService::class.java).also {
            it.action = ACTION_STOP_SERVICE
            startService(it)
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
        var selectedTabIndex by mutableStateOf(0)

        var Email by mutableStateOf("")
        var Password by mutableStateOf("")
        var JwtToken by mutableStateOf("")
        var RememberMe by mutableStateOf(false)

        fun saveLoginData() {
            val sharedPreferences =
                context.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE)
            with(sharedPreferences.edit()) {
                putString(EMAIL_KEY, Email)
                putString(PASSWORD_KEY, Password)
                putString(JWT_TOKEN_KEY, JwtToken)
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
}

class ForegroundService : LifecycleService() {
    @SuppressLint("ForegroundServiceType")
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        super.onStartCommand(intent, flags, startId)

        when (intent?.action) {
            MainActivity.ACTION_START_SERVICE -> {
                // Создаем канал уведомлений (для Android 8.0 и выше)
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    val channel = NotificationChannel(
                        MainActivity.CHANNEL_ID,
                        "Location Service Channel",
                        NotificationManager.IMPORTANCE_LOW
                    )
                    val manager = getSystemService(NotificationManager::class.java)
                    manager?.createNotificationChannel(channel)
                }

                val notificationIntent = Intent(this, MainActivity::class.java)
                val pendingIntent = PendingIntent.getActivity(
                    this,
                    0,
                    notificationIntent,
                    PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
                )

                val notification = NotificationCompat.Builder(this, MainActivity.CHANNEL_ID)
                    .setContentTitle("Location Service")
                    .setContentText("Running in background...")
                    .setSmallIcon(R.drawable.ic_launcher_foreground) // Замените на ваш значок
                    .setContentIntent(pendingIntent)
                    .build()

                startForeground(MainActivity.NOTIFICATION_ID, notification)
                // Запускаем BackgroundService
                Intent(this, BackgroundService::class.java).also {
                    it.action = MainActivity.ACTION_START_SERVICE
                    startService(it)
                }
            }
            MainActivity.ACTION_STOP_SERVICE -> {
                stopForeground(true)
                stopSelf()
            }
        }

        return START_NOT_STICKY // Сервис не будет перезапускаться после остановки системой
    }

    override fun onBind(intent: Intent): IBinder? {
        super.onBind(intent)
        return null
    }
}

class BackgroundService : LifecycleService() {
    private lateinit var state: MainActivity.MainActivityState
    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private lateinit var httpClient: OkHttpClient
    private var webSocket: WebSocket? = null
    private var isWebSocketConnected = false
    private lateinit var telephonyManager: TelephonyManager

    private var email: String = ""
    private var jwt: String = ""

    private val locationCallback = object : LocationCallback() {
        override fun onLocationResult(locationResult: LocationResult) {
            super.onLocationResult(locationResult)
            locationResult.lastLocation?.let { location ->
                state.Latitude = location.latitude.toString()
                state.Longtitude = location.longitude.toString()
                Log.d(
                    MainActivity.TAG,
                    "Location updated (from service): Lat=${state.Latitude}, Lon=${state.Longtitude}"
                )
            }
        }
    }
    // PhoneStateListener для получения информации о сигнале
    private val phoneStateListener = object : PhoneStateListener() {
        @RequiresApi(Build.VERSION_CODES.O)
        override fun onSignalStrengthsChanged(signalStrength: SignalStrength) {
            super.onSignalStrengthsChanged(signalStrength)
            getSignalStrength(state, signalStrength)
        }
    }

    override fun onCreate() {
        super.onCreate()
        state = MainActivity.MainActivityState(this)
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(this)
        httpClient = OkHttpClient()
        telephonyManager = getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager

        // Загрузка данных из SharedPreferences
        state.loadLoginData()
// Регистрация слушателя изменений сигнала
        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS)
    }

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        super.onStartCommand(intent, flags, startId)

        when (intent?.action) {
            MainActivity.ACTION_START_SERVICE -> {
                email = intent.getStringExtra("email") ?: ""
                jwt = intent.getStringExtra("jwt") ?: ""
                Log.d(MainActivity.TAG, "Background Service started with email: $email, jwt: $jwt")
                // Запуск фоновой работы
                startBackgroundWork()
            }
            MainActivity.ACTION_STOP_SERVICE -> {
                stopBackgroundWork()
                stopSelf()
            }
        }

        return START_NOT_STICKY
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun startBackgroundWork() {
        lifecycleScope.launch {
            while (true) {
                if (jwt.isNotEmpty()) {
                    getLocation(state, this@BackgroundService)
                    // getSignalStrength(state) // Теперь получаем данные о сигнале через PhoneStateListener
                    connectWebSocket(jwt)
                    if (isWebSocketConnected) {
                        sendLocationData(state)
                    }
                }
                delay(MainActivity.UPDATE_INTERVAL)
            }
        }
    }

    private fun stopBackgroundWork() {
        // Отмена регистрации PhoneStateListener
        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_NONE)

        // Остановка обновлений местоположения
        fusedLocationClient.removeLocationUpdates(locationCallback)
        // Закрытие WebSocket
        webSocket?.close(1000, "Service stopped")
        webSocket = null
        isWebSocketConnected = false
    }

    override fun onDestroy() {
        super.onDestroy()
        stopBackgroundWork()
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun sendLocationData(state: MainActivity.MainActivityState) {
        val jsonString = generateJSON(state)
        webSocket?.send(jsonString)
        Log.d(MainActivity.TAG, "Sent JSON to server (from service): $jsonString")
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
            .url("${MainActivity.SERVER_URL1}${MainActivity.WEBSOCKET_ENDPOINT}")
            .header("Authorization", "Bearer $jwt")
            .build()

        webSocket = client.newWebSocket(request, object : WebSocketListener() {
            override fun onOpen(webSocket: WebSocket, response: Response) {
                Log.d(MainActivity.TAG, "WebSocket connection established (from service)")
                isWebSocketConnected = true
            }

            override fun onMessage(webSocket: WebSocket, text: String) {
                Log.d(MainActivity.TAG, "Received message from server (from service): $text")
            }

            override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                Log.d(
                    MainActivity.TAG,
                    "WebSocket connection closed (from service): $code, $reason"
                )
                isWebSocketConnected = false
                this@BackgroundService.webSocket = null
            }

            override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                Log.e(MainActivity.TAG, "WebSocket connection failure (from service)", t)
                if (response != null) {
                    Log.e(MainActivity.TAG, "Response message (from service): ${response.message}")
                    Log.e(MainActivity.TAG, "Response code (from service): ${response.code}")
                }
                isWebSocketConnected = false
                this@BackgroundService.webSocket = null
            }
        })
    }

    private fun getLocation(state: MainActivity.MainActivityState, context: Context) {
        Log.d(MainActivity.TAG, "getLocation() called (from service)")

        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            return
        }

        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_COARSE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            Log.d(
                MainActivity.TAG,
                "No permission to write to external storage (from service)"
            )
            return
        }

        val locationRequest = LocationRequest.create().apply {
            interval = MainActivity.UPDATE_INTERVAL
            fastestInterval = MainActivity.UPDATE_INTERVAL
            priority = LocationRequest.PRIORITY_HIGH_ACCURACY
        }

        fusedLocationClient.lastLocation.addOnSuccessListener { location ->
            location?.let {
                state.Latitude = location.latitude.toString()
                state.Longtitude = location.longitude.toString()
                Log.d(
                    MainActivity.TAG,
                    "Location received (from service): Lat=${state.Latitude}, Lon=${state.Longtitude}"
                )
            }
        }.addOnFailureListener { e ->
            Log.e(
                MainActivity.TAG,
                "Failed to get last known location (from service)",
                e
            )
        }
        fusedLocationClient.requestLocationUpdates(locationRequest, locationCallback, null)
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun getSignalStrength(state: MainActivity.MainActivityState, signalStrength: SignalStrength) {
        if (checkPhoneStatePermission(state.context)) {
            val cellInfoList = telephonyManager.allCellInfo
            if (cellInfoList.isNullOrEmpty()) {
                state.Rsrp = "CellInfo list is empty"
                state.Rssi = "CellInfo list is empty"
                state.Rsrq = "CellInfo list is empty"
                state.Rssnr = "CellInfo list is empty"
                state.Cqi = "CellInfo list is empty"
                state.Bandwidth = "CellInfo list is empty"
                state.Cellid = "Cell Info not available"
            } else {
                for (info in cellInfoList) {
                    if (info is CellInfoLte) {
                        val cellSignalStrengthLte = info.cellSignalStrength
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
                        Log.d(MainActivity.TAG, "RSRP value (from service): ${state.Rsrp}")
                        Log.d(MainActivity.TAG, "Rssi value (from service): ${state.Rssi}")
                        Log.d(MainActivity.TAG, "Rsrq value (from service): ${state.Rsrq}")
                        Log.d(MainActivity.TAG, "Rssnr value (from service): ${state.Rssnr}")
                        Log.d(MainActivity.TAG, "Cqi value (from service): ${state.Cqi}")
                        Log.d(MainActivity.TAG, "Bandwidth value (from service): ${state.Bandwidth}")
                        Log.d(MainActivity.TAG, "Cell ID value (from service): ${state.Cellid}")
                        break
                    }
                }
            }
        } else {
            // Обработка отсутствия разрешения READ_PHONE_STATE
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun generateJSON(state: MainActivity.MainActivityState): String {
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

        val messageData = MainActivity.MessageData(
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

    override fun onBind(intent: Intent): IBinder? {
        super.onBind(intent)
        return null
    }

    private fun checkPhoneStatePermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.READ_PHONE_STATE
        ) == PackageManager.PERMISSION_GRANTED
    }
}