@file:Suppress("DEPRECATION")

package com.example.login

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
import android.net.ConnectivityManager
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
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.lifecycleScope
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationCallback
import com.google.android.gms.location.LocationRequest
import com.google.android.gms.location.LocationResult
import com.google.android.gms.location.LocationServices
import com.google.android.gms.maps.model.LatLng
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import okhttp3.WebSocket
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
        const val UPDATE_INTERVAL = 2000L
        private const val SERVER_URL = "http://45.90.218.73:8080"

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
        getLocation(state, applicationContext)
        setContent {
            Box(modifier = Modifier.fillMaxSize()) {
                SendingIndicator(isSendingData)
                MainScreen(isSendingData)
            }
        }
        checkAndRequestPermissions()
        state.loadLoginData()
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
                            getLocation(state, applicationContext)
                            getSignalStrength(state)
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
                            0 -> LoginScreen(state, onLoginSuccess)
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
                                cellSignalStrengthLte.dbm.toString()
                            state.BitErrorRate = "N/A"
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

    private fun checkPhoneStatePermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.READ_PHONE_STATE
        ) == PackageManager.PERMISSION_GRANTED
    }


    private fun checkAndRequestPermissions() {
        val context = applicationContext
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) {
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
        } else if (Build.VERSION.SDK_INT == Build.VERSION_CODES.S) {
            if (!checkPermissionsForAndroid12(context)) {
                Log.d(TAG, "Requesting permissions (API == 31)")
                ActivityCompat.requestPermissions(
                    this,
                    arrayOf(
                        Manifest.permission.ACCESS_COARSE_LOCATION,
                        Manifest.permission.ACCESS_FINE_LOCATION,
                        Manifest.permission.READ_PHONE_STATE,
                        Manifest.permission.READ_EXTERNAL_STORAGE
                    ),
                    REQUEST_CODE_PERMISSIONS
                )
            } else {
                getLocation(state, applicationContext)
            }
        } else {
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
                        Manifest.permission.READ_MEDIA_IMAGES,
                        Manifest.permission.READ_MEDIA_VIDEO,
                        Manifest.permission.READ_MEDIA_AUDIO
                    ),
                    REQUEST_CODE_PERMISSIONS
                )
            } else {
                getLocation(state, applicationContext)
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.S)
    private fun checkPermissionsForAndroid12(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.ACCESS_COARSE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context, Manifest.permission.ACCESS_FINE_LOCATION
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_PHONE_STATE
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_EXTERNAL_STORAGE
                ) == PackageManager.PERMISSION_GRANTED
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

    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
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
                    Manifest.permission.READ_MEDIA_IMAGES
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_MEDIA_VIDEO
                ) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(
                    context,
                    Manifest.permission.READ_MEDIA_AUDIO
                ) == PackageManager.PERMISSION_GRANTED
    }

    @RequiresApi(Build.VERSION_CODES.O)
    fun generateJSON(state: MainActivityState): String {
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
        val physcellid = state.Pci.toLongOrNull() ?: 0L

        val messageData = MessageData(
            state.JwtToken,
            state.Uuid,
            formattedTimestamp,
            state.Latitude.toDoubleOrNull() ?: 0.0,
            state.Longtitude.toDoubleOrNull() ?: 0.0,
            rsrp,
            rssi,
            rsrq,
            rssnr,
            cqi,
            bandwidth,
            cellid,
            physcellid
        )

        return Json.encodeToString(messageData)
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
                        val jsonString = generateJSON(state)
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

    @RequiresApi(Build.VERSION_CODES.M)
    fun getAppTrafficData(context: Context, days: Int): List<AppTrafficData> {
        val appTrafficDataList = mutableListOf<AppTrafficData>()
        val packageManager = context.packageManager
        val networkStatsManager =
            context.getSystemService(Context.NETWORK_STATS_SERVICE) as android.app.usage.NetworkStatsManager

        val currentTime = System.currentTimeMillis()
        val startTime = currentTime - TimeUnit.DAYS.toMillis(days.toLong())

        for (packageInfo in packageManager.getInstalledApplications(PackageManager.GET_META_DATA)) {
            val uid = packageInfo.uid
            val appName = packageManager.getApplicationLabel(packageInfo).toString()

            var mobileBytes = 0L
            var wifiBytes = 0L
            var totalDownlinkBytes = 0L
            var totalUplinkBytes = 0L

            try {
                val mobileStats = networkStatsManager.queryDetailsForUid(
                    ConnectivityManager.TYPE_MOBILE,
                    null,
                    startTime,
                    currentTime,
                    uid
                )
                var bucket = android.app.usage.NetworkStats.Bucket()
                while (mobileStats.hasNextBucket()) {
                    mobileStats.getNextBucket(bucket)
                    if (bucket.uid == uid) {
                        mobileBytes += bucket.rxBytes + bucket.txBytes
                        totalDownlinkBytes += bucket.rxBytes
                        totalUplinkBytes += bucket.txBytes
                    }
                }
                mobileStats.close()

                val wifiStats = networkStatsManager.queryDetailsForUid(
                    ConnectivityManager.TYPE_WIFI,
                    null,
                    startTime,
                    currentTime,
                    uid
                )
                bucket = android.app.usage.NetworkStats.Bucket()
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
                        totalDownlinkBytes,
                        totalUplinkBytes
                    )
                )

            } catch (e: Exception) {
                Log.e("AppTraffic", "Error getting traffic data for $appName: ${e.message}", e)
            }
        }

        return appTrafficDataList
    }


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

        // Переменные состояния для отображения графиков
        var showRSRPChart by mutableStateOf(false)
        var showRSSIChart by mutableStateOf(false)
        var showRSRQChart by mutableStateOf(false)

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

}