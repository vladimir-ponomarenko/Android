@file:Suppress("DEPRECATION")

package com.example.login

//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.os.Build
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.annotation.RequiresApi
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
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
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.lifecycle.lifecycleScope
import com.example.login.PermissionUtils.checkPermissions
import com.example.login.PermissionUtils.checkPermissionsForAndroid13
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import com.google.android.gms.maps.model.LatLng
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import okhttp3.WebSocket

@Suppress("NAME_SHADOWING")
class MainActivity : ComponentActivity(), ActivityCompat.OnRequestPermissionsResultCallback {

    companion object {
        private const val LOCATION_PERMISSION_REQUEST_CODE = 1001

        const val TAG = "com.example.login.MainActivity"
        const val UPDATE_INTERVAL = 2000L
        private const val SERVER_URL =  "http://78.24.222.170:8080" //"http://45.90.218.73:8080"

        private const val SHARED_PREFS_NAME = "login_prefs"
        private const val EMAIL_KEY = "email"
        private const val PASSWORD_KEY = "password"
        private const val JWT_TOKEN_KEY = "jwt_token"
        private const val UUID_KEY = "uuid"
        private const val REMEMBER_ME_KEY = "remember_me"

        lateinit var state: MainActivityState
        lateinit var networkManager: NetworkManager<Any?>
        private var webSocket: WebSocket? = null
    }

    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private var webSocket: WebSocket? = null
    private var isWebSocketConnected by mutableStateOf(false)
    var isSendingData by mutableStateOf(false)

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        state = MainActivityState(applicationContext)
        networkManager = NetworkManager(this, SERVER_URL, "/api/sockets/thermalmap")
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(this)
        DataManager.getLocation(this, state)
        setContent {
            Box(modifier = Modifier.fillMaxSize()) {
                SendingIndicator(isSendingData)
                MainScreen(isSendingData)
            }
        }
        PermissionUtils.checkAndRequestPermissions(this)
        state.loadLoginData()
        lifecycleScope.launch {
            while (true) {
                val lat = state.Latitude.toDoubleOrNull()
                val lng = state.Longtitude.toDoubleOrNull()
                if (lat != null && lng != null) {
                    val color = generateColorFromRSRP(state.Rsrp.replace(" dBm", "").toIntOrNull() ?: -140)
                    state.locations.add(Pair(LatLng(lat, lng), color))
                }
                delay(UPDATE_INTERVAL)
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun MainScreen(isSendingData: Boolean) {
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
        var isLoggedIn by remember { mutableStateOf(false) }
        LaunchedEffect(isWebSocketConnected) {
            if (isWebSocketConnected) {
                showConnectionSnackbar = true
                scaffoldState.snackbarHostState.showSnackbar(
                    message = "WebSocket connected!",
                    duration = SnackbarDuration.Short
                )
                delay(2000)
                showConnectionSnackbar = false
            }
        }

        LaunchedEffect(context) {
            permissionsGranted =
                if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) checkPermissions(context)
                else checkPermissionsForAndroid13(context)
        }

        Scaffold(
            scaffoldState = scaffoldState
        ) { innerPadding ->
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(innerPadding)
            ) {
                if (permissionsGranted) {
                    LaunchedEffect(Unit) {
                        while (true) {
                            DataManager.getLocation(this@MainActivity, state)
                            DataManager.getSignalStrength(state)
                            delay(UPDATE_INTERVAL)
                        }
                    }

                    MainContent(state, isLoggedIn) { isLoggedIn = true }
                } else {
                    Text("Waiting for permissions...")
                }
                LaunchedEffect(isSendingData) {
                    // Небольшая задержка, чтобы индикатор был виден
                    delay(500)
                }

                SendingIndicator(isSendingData)
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun MainContent(state: MainActivity.MainActivityState, isLoggedIn: Boolean, onLoginSuccess: () -> Unit) {
        val context = LocalContext.current
        val coroutineScope = rememberCoroutineScope()
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

        LaunchedEffect(isWebSocketConnected) {
            if (isWebSocketConnected) {
                showConnectionSnackbar = true
                scaffoldState.snackbarHostState.showSnackbar(
                    message = "WebSocket connected!",
                    duration = SnackbarDuration.Short
                )
                delay(2000)
                showConnectionSnackbar = false
            }
        }

        LaunchedEffect(context) {
            permissionsGranted =
                if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) checkPermissions(context)
                else checkPermissionsForAndroid13(context)
        }

        Box(modifier = Modifier.fillMaxSize()) {
            Scaffold(
                scaffoldState = scaffoldState
            ) { innerPadding ->
                if (permissionsGranted) {
                    LaunchedEffect(Unit) {
                        while (true) {
                            DataManager.getLocation(this@MainActivity, state)
                            DataManager.getSignalStrength(state)
                            DataManager.getCellInfo(context, state)
                            delay(UPDATE_INTERVAL)
                        }
                    }

                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(innerPadding),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        TabRow(selectedTabIndex = state.selectedTabIndex) {
                            Tab(
                                selected = state.selectedTabIndex == 0,
                                onClick = { state.selectedTabIndex = 0 },
                                text = { Text("Сервер") }
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
/*                            Tab(
                                selected = state.selectedTabIndex == 5,
                                onClick = { state.selectedTabIndex = 5 },
                                text = { Text("Тест") }
                            )*/
                        }
                        when (state.selectedTabIndex) {
                            0 -> LoginScreen(
                                state,
                                onLoginSuccess = onLoginSuccess,
                                onData1Click = {
                                    coroutineScope.launch {
                                        MainActivity.networkManager.authenticateUser(state.Email, state.Password, state.JwtToken) { authResponse ->
                                            if (authResponse != null) {
                                                (context as? Activity)?.runOnUiThread {
                                                    state.JwtToken = authResponse.jwt
                                                    state.Uuid = authResponse.uuid
                                                    state.saveLoginData()
                                                }
                                                connectWebSocket(authResponse.jwt)
                                            } else {
                                                Log.e(MainActivity.TAG, "Authentication failed")
                                            }
                                        }
                                    }
                                },
                                onCellInfoDataClick = {
                                    coroutineScope.launch {
                                        if (!state.isSendingCellInfoData) {
                                            state.isSendingCellInfoData = true

                                            MainActivity.networkManager.authenticateUser(state.Email, state.Password, state.JwtToken) { authResponse ->
                                                if (authResponse != null) {
                                                    (context as? Activity)?.runOnUiThread {
                                                        state.JwtToken = authResponse.jwt
                                                        state.Uuid = authResponse.uuid
                                                        state.saveLoginData()
                                                    }
                                                } else {
                                                    Log.e(MainActivity.TAG, "Authentication failed")
                                                    state.isSendingCellInfoData = false
                                                }
                                            }
                                        } else {
                                            state.isSendingCellInfoData = false
                                        }
                                    }
                                }
                            )
                            1 -> DataScreen(state)
                            2 -> RSRPGraph(state)
                            3 -> MapScreen(state)
                            4 -> TrafficScreen(state)
                            5 -> SpeedTestScreen(state)
                        }
                    }
                } else {
                    Text("Waiting for permissions...")
                }
            }
        }
    }



    @RequiresApi(Build.VERSION_CODES.O)
    fun connectWebSocket(jwt: String) {
        if (webSocket != null) {
            return
        }
        webSocket = networkManager.connectWebSocket(
            jwt,
            onOpen = { webSocket, _ ->
                Log.d(TAG, "WebSocket connection established")
                isWebSocketConnected = true
                lifecycleScope.launch {
                    while (true) {
                        delay(UPDATE_INTERVAL)
                        val jsonString = DataManager.generateJSON(state)
                        webSocket.send(jsonString)
                        isSendingData = true
                        if (isSendingData == true){
                            Log.d(TAG, "SendingIndicator = TRUE")
                        }
                        delay(2000)
                        isSendingData = false
                        if (isSendingData == false){
                            Log.d(TAG, "SendingIndicator = FALSE")
                        }
                        Log.d(TAG, "Sent JSON to server: $jsonString")
                    }
                }
            },
            onMessage = { _, text ->
                Log.d(TAG, "Received message from server: $text")
            },
            onClosed = { webSocket, code, reason ->
                Log.d(TAG, "WebSocket connection closed: $code, $reason")
                isWebSocketConnected = false
                this@MainActivity.webSocket = null
            },
            onFailure = { _, t, response ->
                Log.e(TAG, "WebSocket connection failure", t)
                if (response != null) {
                    Log.e(TAG, "Response message: ${response.message}")
                    Log.e(TAG, "Response code: ${response.code}")
                }
                isWebSocketConnected = false
                this@MainActivity.webSocket = null
            }
        )
    }

    @Composable
    fun SendingIndicator(isSendingData: Boolean) {
        Log.d(TAG, "Rendering SendingIndicator, isSendingData: $isSendingData")
        if (isSendingData) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(16.dp)
            ) {
                Box(
                    modifier = Modifier
                        .align(Alignment.BottomEnd)
                        .size(16.dp)
                        .background(Color.Green, CircleShape)
                )
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
        var messageToData2 by mutableStateOf<MessageToData2?>(null)

        // Переменные состояния для отображения графиков
        var showRSRPChart by mutableStateOf(false)
        var showRSSIChart by mutableStateOf(false)
        var showRSRQChart by mutableStateOf(false)

        //Для тепловой карты (точки)
        val locations = mutableStateListOf<Pair<LatLng, Color>>()

        var Email by mutableStateOf("")
        var Password by mutableStateOf("")
        var JwtToken by mutableStateOf("")
        var Uuid by mutableStateOf("")
        var RememberMe by mutableStateOf(false)

        var isSendingCellInfoData by mutableStateOf(false)

        val cellInfoJson = mutableStateOf(mutableMapOf<String, List<String>>())
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

}