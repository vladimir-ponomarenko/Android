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
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
//noinspection UsingMaterialAndMaterial3Libraries
import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
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
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material.Scaffold
import androidx.compose.material.ScrollableTabRow
import androidx.compose.material.SnackbarDuration
import androidx.compose.material.Tab
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
import androidx.compose.ui.zIndex
import androidx.core.app.ActivityCompat
import androidx.lifecycle.lifecycleScope
import androidx.work.ExistingPeriodicWorkPolicy
import androidx.work.NetworkType
import androidx.work.PeriodicWorkRequestBuilder
import androidx.work.WorkManager
import androidx.work.workDataOf
import com.example.login.PermissionUtils.checkPermissions
import com.example.login.PermissionUtils.checkPermissionsForAndroid12
import com.example.login.PermissionUtils.checkPermissionsForAndroid13
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import com.google.android.gms.maps.model.LatLng
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import okhttp3.WebSocket
import java.util.concurrent.TimeUnit


@Suppress("NAME_SHADOWING")
class MainActivity : ComponentActivity(), ActivityCompat.OnRequestPermissionsResultCallback {

    companion object {
        private const val LOCATION_PERMISSION_REQUEST_CODE = 1001

        const val TAG = "com.example.login.MainActivity"
        const val ACTION_STOP_MAIN_ACTIVITY = "com.example.login.stop_main_activity"
        const val UPDATE_INTERVAL = 2000L
        private const val SERVER_URL = "http://109.172.114.128:10000"  // "http://78.24.222.170:8080" //"http://45.90.218.73:8080"

        internal const val SHARED_PREFS_NAME = "login_prefs"
        private const val EMAIL_KEY = "email"
        private const val PASSWORD_KEY = "password"
        private const val JWT_TOKEN_KEY = "jwt_token"
        private const val UUID_KEY = "uuid"
        internal const val REMEMBER_ME_KEY = "remember_me"

        lateinit var state: MainActivityState
        lateinit var networkManager: NetworkManager<Any?>
        private var webSocket: WebSocket? = null
        var instance: MainActivity? = null
    }
    init {
        instance = this
    }
    private var isDataCollectionEnabled by mutableStateOf(true)
    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private var webSocket: WebSocket? = null
    private var isWebSocketConnected by mutableStateOf(false)
    var isSendingData by mutableStateOf(false)

    private var sendingIndicatorJob: Job? = null

    fun showSendingIndicator() {
        sendingIndicatorJob?.cancel()
        sendingIndicatorJob = lifecycleScope.launch {
            isSendingData = true
            delay(1000)
            isSendingData = false
        }
    }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        state = MainActivityState(applicationContext)
        networkManager = NetworkManager(this, SERVER_URL, "/api/sockets/thermalmap")
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(this)
        DataManager.getLocation(this, state)
        startForegroundService()
        setContent {
            Box(modifier = Modifier.fillMaxSize()) {
                SendingIndicator(isSendingData)
                MainScreen(isSendingData)
            }
        }
        PermissionUtils.checkAndRequestPermissions(this)
        state.loadLoginData()
        if (intent?.action == ForegroundService.ACTION_STOP_SERVICE) {
            stopForegroundService()
            finish()
        }

        val constraints = androidx.work.Constraints.Builder()
            .setRequiredNetworkType(NetworkType.CONNECTED)
            .build()

        val data = workDataOf(
            "Email" to state.Email,
            "Password" to state.Password,
            "JwtToken" to state.JwtToken
        )

        val dailyWorkRequest = PeriodicWorkRequestBuilder<TrafficDataWorker>(
            1, TimeUnit.DAYS // Каждые 24 часа отправляем статистику о трафике
        )
            .setConstraints(constraints)
            .setInputData(data)
            .build()

// Отменяем существующую задачу (для дебага)
//        WorkManager.getInstance(this).cancelUniqueWork("TrafficDataWorker")
        WorkManager.getInstance(this).enqueueUniquePeriodicWork(
            "TrafficDataWorker",
            ExistingPeriodicWorkPolicy.KEEP,
            dailyWorkRequest
        )
//        val broadcastReceiver = object : BroadcastReceiver() {
//            override fun onReceive(context: Context?, intent: Intent?) {
//                if (intent?.action == ACTION_STOP_MAIN_ACTIVITY) {
//                    isDataCollectionEnabled = false
//                    finishAndRemoveTask()
//                }
//            }
//        }
//        val intentFilter = IntentFilter(ACTION_STOP_MAIN_ACTIVITY)
//
//        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//            registerReceiver(broadcastReceiver, intentFilter, RECEIVER_NOT_EXPORTED)
//        } else {
//            registerReceiver(broadcastReceiver, intentFilter)
//        }

        lifecycleScope.launch {
            while (true) {
                val currentTimestamp = System.currentTimeMillis()

                val lat = state.Latitude.toDoubleOrNull()
                val lng = state.Longtitude.toDoubleOrNull()
                if (lat != null && lng != null) {
                    val location = LatLng(lat, lng)

                    val colorForLocations = generateColorFromRSRP(state.Rsrp.replace(" dBm", "").toIntOrNull() ?: -140)
                    state.locations.add(Pair(LatLng(lat, lng), colorForLocations))

                    // LTE data
                    val lteCellInfo = state.messageToData2?.lte?.cellInfoList?.firstOrNull()
                    val rsrp = lteCellInfo?.rsrp?.toString()?.toIntOrNull()
                    if (rsrp != null && rsrp != 0 && rsrp != 2147483647) {
                        val color = generateColorFromRSRP(rsrp)
                        state.lteLocations.add(Pair(location, color))
                    }

                    // GSM data
                    val gsmCellInfo = state.messageToData2?.gsm?.cellInfoList?.firstOrNull()
                    val rssiGsm = gsmCellInfo?.rssi?.toString()?.toIntOrNull()
                    if (rssiGsm != null && rssiGsm != 0 && rssiGsm != 2147483647) {
                        val color = generateColorFromRSSIGsm(rssiGsm)
                        state.gsmLocations.add(Pair(location, color))
                    }

                    // WCDMA data
                    val wcdmaCellInfo = state.messageToData2?.wcdma?.cellInfoList?.firstOrNull()
                    val rscpWcdma = wcdmaCellInfo?.rscp?.toString()?.toIntOrNull()
                    if (rscpWcdma != null && rscpWcdma != 0 && rscpWcdma != 2147483647) {
                        val color = generateColorFromRSCPWcdma(rscpWcdma)
                        state.wcdmaLocations.add(Pair(location, color))
                    }

                    // CDMA data
                    val cdmaCellInfo = state.messageToData2?.cdma?.cellInfoList?.firstOrNull()
                    val rssiCdma = cdmaCellInfo?.rssi?.toString()?.toIntOrNull()
                    if (rssiCdma != null && rssiCdma != 0 && rssiCdma != 2147483647) {
                        val color = generateColorFromRSSICdma(rssiCdma)
                        state.cdmaLocations.add(Pair(location, color))
                    }

                    // NR data
                    val nrCellInfo = state.messageToData2?.nr?.cellInfoList?.firstOrNull()
                    val csiRsrpNr = nrCellInfo?.csiRsrp?.toString()?.toIntOrNull()
                    if (csiRsrpNr != null && csiRsrpNr != 0 && csiRsrpNr != 2147483647) {
                        val color = generateColorFromCSIRsrpNr(csiRsrpNr)
                        state.nrLocations.add(Pair(location, color))
                    }
                }

                // LTE data
                val lteCellInfo = state.messageToData2?.lte?.cellInfoList?.firstOrNull()
                lteCellInfo?.let {
                    addChartData(state.rsrpData, it.rsrp?.toString() ?: "0", currentTimestamp)
                    addChartData(state.rssiData, it.rssi?.toString() ?: "0", currentTimestamp)
                    addChartData(state.rsrqData, it.rsrq?.toString() ?: "0", currentTimestamp)
                    addChartData(state.asuLevelLTE, it.asuLevel?.toString() ?: "0", currentTimestamp)
                    addChartData(state.levelLTE, it.level?.toString() ?: "0", currentTimestamp)
                    addChartData(state.earfcnLTE, it.earfcn?.toString() ?: "0", currentTimestamp)

                    addChartDataForDetailedChart(state.rsrpDetailedData, it.rsrp?.toString() ?: "0", currentTimestamp, it.ci.toString())
                    addChartDataForDetailedChart(state.rssiDetailedData, it.rssi?.toString() ?: "0", currentTimestamp, it.ci.toString())
                    addChartDataForDetailedChart(state.rsrqDetailedData, it.rsrq?.toString() ?: "0", currentTimestamp, it.ci.toString())
                    addChartDataForDetailedChart(state.asuLevelLTEDetailed, it.asuLevel?.toString() ?: "0", currentTimestamp, it.ci.toString())
                    addChartDataForDetailedChart(state.levelLTEDetailed, it.level?.toString() ?: "0", currentTimestamp, it.ci.toString())
                    addChartDataForDetailedChart(state.earfcnLTEDetailed, it.earfcn?.toString() ?: "0", currentTimestamp, it.ci.toString())
                }

                // GSM data
                val gsmCellInfo = state.messageToData2?.gsm?.cellInfoList?.firstOrNull()
                gsmCellInfo?.let {
                    addChartData(state.rssiDataGsm, it.rssi?.toString() ?: "0", currentTimestamp)
                    addChartData(state.arfcnGsm, it.arfcn?.toString() ?: "0", currentTimestamp)

                    addChartDataForDetailedChart(state.rssiDataGsmDetailed, it.rssi?.toString() ?: "0", currentTimestamp, it.cid.toString())
                    addChartDataForDetailedChart(state.arfcnGsmDetailed, it.arfcn?.toString() ?: "0", currentTimestamp, it.cid.toString())
                }

                // WCDMA data
                val wcdmaCellInfo = state.messageToData2?.wcdma?.cellInfoList?.firstOrNull()
                wcdmaCellInfo?.let {
                    addChartData(state.rscpDataWcdma, it.rscp?.toString() ?: "0", currentTimestamp)
                    addChartData(state.levelWcdma, it.level?.toString() ?: "0", currentTimestamp)

                    addChartDataForDetailedChart(state.rscpDataWcdmaDetailed, it.rscp?.toString() ?: "0", currentTimestamp, it.cid.toString())
                    addChartDataForDetailedChart(state.levelWcdmaDetailed, it.level?.toString() ?: "0", currentTimestamp, it.cid.toString())
                }

                // CDMA data
                val cdmaCellInfo = state.messageToData2?.cdma?.cellInfoList?.firstOrNull()
                cdmaCellInfo?.let {
                    addChartData(state.rssiDataCdma, it.rssi?.toString() ?: "0", currentTimestamp)
                    addChartData(state.levelCdma, it.level?.toString() ?: "0", currentTimestamp)

                    addChartDataForDetailedChart(state.rssiDataCdmaDetailed, it.rssi?.toString() ?: "0", currentTimestamp, it.bsid.toString())
                    addChartDataForDetailedChart(state.levelCdmaDetailed, it.level?.toString() ?: "0", currentTimestamp, it.bsid.toString())
                }

                // NR data
                val nrCellInfo = state.messageToData2?.nr?.cellInfoList?.firstOrNull()
                nrCellInfo?.let {
                    addChartData(state.rssiDataNr, it.csiRsrp?.toString() ?: "0", currentTimestamp)
                    addChartData(state.asuLevelNr, it.asuLevel?.toString() ?: "0", currentTimestamp)

                    addChartDataForDetailedChart(state.rssiDataNrDetailed, it.csiRsrp?.toString() ?: "0", currentTimestamp, it.nci.toString())
                    addChartDataForDetailedChart(state.asuLevelNrDetailed, it.asuLevel?.toString() ?: "0", currentTimestamp, it.nci.toString())
                }

                delay(UPDATE_INTERVAL)
            }
        }
    }

    private var permissionsGranted by mutableStateOf(false)
    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun MainScreen(isSendingData: Boolean) {
        val context = LocalContext.current
        val scaffoldState = rememberScaffoldState()
        var showConnectionSnackbar by remember { mutableStateOf(false) }
        var isLoggedIn by remember { mutableStateOf(false) }

        permissionsGranted = if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) checkPermissions(context)
        else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            checkPermissionsForAndroid13(context)
        } else {
            checkPermissionsForAndroid12(context)
        }

        if (!permissionsGranted) {
            PermissionUtils.checkAndRequestPermissions(this@MainActivity)
        }

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

        Scaffold(
            scaffoldState = scaffoldState
        ) { innerPadding ->
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(innerPadding)
            ) {
                LaunchedEffect(key1 = permissionsGranted, key2 = isDataCollectionEnabled) {
                    if (permissionsGranted && isDataCollectionEnabled) {
                        while (true) {
                            DataManager.getLocation(this@MainActivity, state)
                            DataManager.getSignalStrength(state)
                            delay(UPDATE_INTERVAL)
                        }
                    }
                }

                if (permissionsGranted) {
                    MainContent(state, isLoggedIn) { isLoggedIn = true }
                } else {
                    Text("Waiting for permissions...")
                }

                LaunchedEffect(isSendingData) {
                    delay(500)
                }
                SendingIndicator(isSendingData)
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == UpdateManager.REQUEST_INSTALL_PACKAGES && resultCode == Activity.RESULT_OK) {
            if (state.DownloadLink.isNotBlank()) {
                UpdateManager.startDownloadAndInstall(this, state.DownloadLink)
            }
        }
    }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == PermissionUtils.REQUEST_CODE_PERMISSIONS) {
            val allPermissionsGranted = grantResults.all { it == PackageManager.PERMISSION_GRANTED }
            permissionsGranted = allPermissionsGranted

            if (allPermissionsGranted) {
                lifecycleScope.launch {
                    delay(100)
                    setContent {
                        MainScreen(isSendingData)
                    }
                }
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
                    checkPermissionsForAndroid12(context)
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
            permissionsGranted = if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.R) {
                checkPermissions(context)
            } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                checkPermissionsForAndroid13(context)
            } else {
                checkPermissionsForAndroid12(context)
            }
        }

        Box(modifier = Modifier.fillMaxSize()) {
            Scaffold(
                scaffoldState = scaffoldState
            ) { innerPadding ->
                if (permissionsGranted) {
                    LaunchedEffect(Unit, isDataCollectionEnabled) {
                        while (isActive && isDataCollectionEnabled) {
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
                        ScrollableTabRow(
                            selectedTabIndex = state.selectedTabIndex,
                            modifier = Modifier.fillMaxWidth(),
                            edgePadding = 0.dp
                        ) {
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
//                            Tab(
//                                selected = state.selectedTabIndex == 2,
//                                onClick = { state.selectedTabIndex = 2 },
//                                text = { Text("Графики") }
//                            )
                            Tab(
                                selected = state.selectedTabIndex == 2,
                                onClick = { state.selectedTabIndex = 2 },
                                text = { Text("Карта") }
                            )
                            Tab(
                                selected = state.selectedTabIndex == 3,
                                onClick = { state.selectedTabIndex = 3 },
                                text = { Text("Трафик") }
                            )
                            Tab(
                                selected = state.selectedTabIndex == 4,
                                onClick = { state.selectedTabIndex = 4 },
                                text = { Text("Обновление") }
                            )
                        }
                        when (state.selectedTabIndex) {
                            0 -> LoginScreen(
                                state,
                                onLoginSuccess = onLoginSuccess,
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
//                            2 -> RSRPGraph(state)
                            2 -> MapScreen(state)
                            3 -> TrafficScreen(state)
                            4 -> UpdateScreen(state.DownloadLink)
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
                    .zIndex(1f)
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
        var Altitude by mutableStateOf("")
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

        // Списки для хранения данных графиков
        val rsrpData = mutableStateListOf<Pair<Long, Float>>()
        val rssiData = mutableStateListOf<Pair<Long, Float>>()
        val rsrqData = mutableStateListOf<Pair<Long, Float>>()
        val asuLevelLTE = mutableStateListOf<Pair<Long, Float>>()
        val levelLTE = mutableStateListOf<Pair<Long, Float>>()
        val earfcnLTE = mutableStateListOf<Pair<Long, Float>>()

        var rssiDataGsm = mutableStateListOf<Pair<Long, Float>>()
        var arfcnGsm = mutableStateListOf<Pair<Long, Float>>()

        var rssiDataCdma = mutableStateListOf<Pair<Long, Float>>()
        var levelCdma = mutableStateListOf<Pair<Long, Float>>()

        var rssiDataNr = mutableStateListOf<Pair<Long, Float>>()
        var asuLevelNr = mutableStateListOf<Pair<Long, Float>>()

        var rscpDataWcdma = mutableStateListOf<Pair<Long, Float>>()
        var levelWcdma = mutableStateListOf<Pair<Long, Float>>()

        val rsrpDetailedData = mutableStateListOf<ChartDataPoint>()
        val rssiDetailedData = mutableStateListOf<ChartDataPoint>()
        val rsrqDetailedData = mutableStateListOf<ChartDataPoint>()
        val asuLevelLTEDetailed = mutableStateListOf<ChartDataPoint>()
        val levelLTEDetailed = mutableStateListOf<ChartDataPoint>()
        val earfcnLTEDetailed = mutableStateListOf<ChartDataPoint>()

        var rssiDataGsmDetailed = mutableStateListOf<ChartDataPoint>()
        var arfcnGsmDetailed = mutableStateListOf<ChartDataPoint>()

        var rssiDataCdmaDetailed = mutableStateListOf<ChartDataPoint>()
        var levelCdmaDetailed = mutableStateListOf<ChartDataPoint>()

        var rssiDataNrDetailed = mutableStateListOf<ChartDataPoint>()
        var asuLevelNrDetailed = mutableStateListOf<ChartDataPoint>()

        var rscpDataWcdmaDetailed = mutableStateListOf<ChartDataPoint>()
        var levelWcdmaDetailed = mutableStateListOf<ChartDataPoint>()


        //Для тепловой карты (точки)
        val locations = mutableStateListOf<Pair<LatLng, Color>>()

        var Email by mutableStateOf("")
        var Password by mutableStateOf("")
        var JwtToken by mutableStateOf("")
        var Uuid by mutableStateOf("")
        var RememberMe by mutableStateOf(false)

        // Выбранный тип сети для тепловой карты
        var selectedNetworkType by mutableStateOf("LTE")

        // Списки для хранения данных для каждого типа сети
        val lteLocations = mutableStateListOf<Pair<LatLng, Color>>()
        val gsmLocations = mutableStateListOf<Pair<LatLng, Color>>()
        val wcdmaLocations = mutableStateListOf<Pair<LatLng, Color>>()
        val cdmaLocations = mutableStateListOf<Pair<LatLng, Color>>()
        val nrLocations = mutableStateListOf<Pair<LatLng, Color>>()

        var isSendingCellInfoData by mutableStateOf(false)

        var isFullscreen by mutableStateOf(false)
        var DownloadLink by mutableStateOf("")

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
    @RequiresApi(Build.VERSION_CODES.O)
    fun startForegroundService() {
        val serviceIntent = Intent(this, ForegroundService::class.java)
        startForegroundService(serviceIntent)
    }

    fun stopForegroundService() {
        val serviceIntent = Intent(this, ForegroundService::class.java)
        stopService(serviceIntent)
    }
}