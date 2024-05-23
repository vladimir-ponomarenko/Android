@file:Suppress("DEPRECATION")

package com.example.login

//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
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
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.Button
import androidx.compose.material.Checkbox
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.Tab
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.TabRow
//noinspection UsingMaterialAndMaterial3Libraries
import androidx.compose.material.Text
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
import androidx.core.content.ContextCompat
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

        private const val TAG = "com.example.login.MainActivity"
        private const val UPDATE_INTERVAL = 2000L // 2 секунды
        private const val SERVER_URL = "http://45.90.218.73:8080"
        private const val SERVER_URL1 = "ws://45.90.218.73:8080"

        private const val WEBSOCKET_ENDPOINT = "/api/sockets/termalmap"

        // Имя ключа для SharedPreferences
        private const val SHARED_PREFS_NAME = "login_prefs"
        private const val EMAIL_KEY = "email"
        private const val PASSWORD_KEY = "password"
        private const val JWT_TOKEN_KEY = "jwt_token"
        private const val REMEMBER_ME_KEY = "remember_me"
    }

    private lateinit var state: MainActivityState
    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private lateinit var httpClient: OkHttpClient
    private lateinit var webSocket: WebSocket
    private var isWebSocketConnected = false

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
                            val jsonResponse = Json.decodeFromString<Map<String, String>>(responseBody ?: "")
                            val email = jsonResponse["email"]
                            val jwt = jsonResponse["jwt"]
                            if (email != null && jwt != null) {
                                Log.d(TAG, "User authenticated successfully")
                                connectWebSocket(jwt)
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
            }

            override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                Log.e(TAG, "WebSocket connection failure", t)
                if (response != null) {
                    Log.e(TAG, "Response message: ${response.message}")
                    Log.e(TAG, "Response code: ${response.code}")
                }
            }
        })
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun generateJSON(state: MainActivityState): String {
        val currentTimestamp = Instant.now().toString() // Преобразование Instant в String

        // Форматирование строки
        val formattedTimestamp = String.format(DateTimeFormatter.ISO_INSTANT.format(Instant.parse(currentTimestamp)))

        // Преобразование строк в числа
        val rsrp = state.Rsrp.replace(" dBm", "").toLongOrNull() ?: 0L
        val rssi = state.Rssi.replace(" dBm", "").toLongOrNull() ?: 0L
        val rsrq = state.Rsrq.replace(" dB", "").toLongOrNull() ?: 0L
        val rssnr = state.Rssnr.replace(" dB", "").toLongOrNull() ?: 0L
        val cqi = state.Cqi.toLongOrNull() ?: 0L
        val bandwidth = state.Bandwidth.toLongOrNull() ?: 0L
        val cellid = state.Cellid.toLongOrNull() ?: 0L

        // Создание объекта MessageData
        val messageData = MessageData(
            formattedTimestamp, // Передача отформатированного времени
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

        LaunchedEffect(context) {
            permissionsGranted = checkPermissions(context)
        }

        if (permissionsGranted) {
            LaunchedEffect(Unit) {
                while (true) {
                    getLocation(state, applicationContext)
                    getSignalStrength(state)
                    delay(UPDATE_INTERVAL)
                }
            }

            Column(
                modifier = Modifier.fillMaxSize(),
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

    private fun getLocation(state: MainActivityState, context: Context) {
        Log.d(TAG, "getLocation() called")

        // Проверка разрешения на доступ к местоположению
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
                    // Сохранение данных после авторизации
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
                    drawLine(start = Offset(startX, startY), end = Offset(endX, endY), color = Color.Red, strokeWidth = 2f)
                }
            }
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

        // Функции для сохранения и загрузки данных из SharedPreferences
        fun saveLoginData() {
            val sharedPreferences = context.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE)
            with(sharedPreferences.edit()) {
                putString(EMAIL_KEY, Email)
                putString(PASSWORD_KEY, Password)
                putString(JWT_TOKEN_KEY, JwtToken)
                putBoolean(REMEMBER_ME_KEY, RememberMe)
                apply()
            }
        }

        fun loadLoginData() {
            val sharedPreferences = context.getSharedPreferences(SHARED_PREFS_NAME, Context.MODE_PRIVATE)
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