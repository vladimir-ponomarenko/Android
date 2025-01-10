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
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Button
import androidx.compose.material.ButtonDefaults
import androidx.compose.material.Divider
import androidx.compose.material.IconButton
import androidx.compose.material.Scaffold
import androidx.compose.material.SnackbarDuration
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.rememberScaffoldState
import androidx.compose.material3.Icon
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
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
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
//        startForegroundService()
        permissionsGranted = checkAllPermissions(this)

        setContent {
            Box(modifier = Modifier.fillMaxSize()) {
                SendingIndicator(isSendingData)
                MainScreen(isSendingData, permissionsGranted)
            }
        }

        if (permissionsGranted) {
            startForegroundService()
        }
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
    fun MainScreen(isSendingData: Boolean, permissionsGranted: Boolean) {
        val context = LocalContext.current
        val scaffoldState = rememberScaffoldState()
        var showConnectionSnackbar by remember { mutableStateOf(false) }
        var selectedTabIndex by remember { mutableStateOf(5) }

        if (!permissionsGranted) {
            PermissionRequestButtons(context) { allGranted ->
                this@MainActivity.permissionsGranted = allGranted
                if (allGranted) {
                    selectedTabIndex = 5
                }
            }
        } else {
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

            Scaffold(scaffoldState = scaffoldState) { innerPadding ->
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
                    MainContent(
                        state,
                        selectedTabIndex = selectedTabIndex,
                        onTabSelected = { newTabIndex -> selectedTabIndex = newTabIndex }
                    )

                    LaunchedEffect(isSendingData) {
                        delay(500)
                    }
                    SendingIndicator(isSendingData)
                }
            }
        }
    }

    // Предварительный просмотр UI
    @Preview(showBackground = true)
    @Composable
    fun PreviewPermissionRequestButtons() {
        PermissionRequestButtons(context = androidx.compose.ui.platform.LocalContext.current) {

        }
    }
    @Composable
    fun PermissionRequestButtons(context: Context, onPermissionsResult: (Boolean) -> Unit) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFFF8FAFC)),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .background(Color(0xFF132C86))
            ) {
                Column {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(top = 1.dp, end = 16.dp),
                        horizontalArrangement = Arrangement.End
                    ) {
                        IconButton(onClick = {
                            (context as? Activity)?.finish()
                        }) {
                            Icon(Icons.Filled.Close, contentDescription = "Закрыть", tint = Color(0xFFF8FAFC))
                        }
                    }

                    Box(
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(220.dp)
                    ) {
                        Image(
                            painter = painterResource(id = R.drawable.start_image),
                            contentDescription = "Стартовое изображение",
                            modifier = Modifier.fillMaxSize(),
                            contentScale = ContentScale.Fit
                        )
                    }
                }
            }

            Text(
                text = stringResource(id = R.string.get_information),
                modifier = Modifier.padding(top = 14.dp, start = 16.dp, end = 16.dp),
                color = Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.Bold,
                lineHeight = 26.sp
            )

            Text(
                text = stringResource(id = R.string.grant_permissions),
                modifier = Modifier.padding(top = 8.dp, start = 16.dp, end = 16.dp),
                color = Color(0xCC34204C),
                fontSize = 14.sp,
                lineHeight = 20.sp
            )

            Spacer(modifier = Modifier.height(16.dp))

            PermissionItem(
                title = stringResource(id = R.string.location),
                description = stringResource(id = R.string.coordinates),
                onClick = {
                    ActivityCompat.requestPermissions(
                        context as Activity,
                        arrayOf(android.Manifest.permission.ACCESS_FINE_LOCATION),
                        LOCATION_PERMISSION_REQUEST_CODE
                    )
                }
            )

            Divider(color = Color.LightGray, thickness = 1.dp)

            PermissionItem(
                title = stringResource(id = R.string.phone_status),
                description = stringResource(id = R.string.network_data),
                onClick = {
                    ActivityCompat.requestPermissions(
                        context as Activity,
                        arrayOf(android.Manifest.permission.READ_PHONE_STATE),
                        LOCATION_PERMISSION_REQUEST_CODE
                    )
                }
            )

            Divider(color = Color.LightGray, thickness = 1.dp)

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                PermissionItem(
                    title = stringResource(id = R.string.background_location),
                    description = stringResource(id = R.string.work_background),
                    onClick = {
                        ActivityCompat.requestPermissions(
                            context as Activity,
                            arrayOf(android.Manifest.permission.ACCESS_BACKGROUND_LOCATION),
                            LOCATION_PERMISSION_REQUEST_CODE
                        )
                    }
                )
                Divider(color = Color.LightGray, thickness = 1.dp)
            }

            Spacer(modifier = Modifier.weight(1f))

            Button(
                onClick = { onPermissionsResult(MainActivity().checkAllPermissions(context)) },
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(6.dp)
                    .height(48.dp),
                colors = ButtonDefaults.buttonColors(backgroundColor = Color(0xFF132C86)),
                shape = RoundedCornerShape(12.dp)
            ) {
                Text(
                    text = stringResource(id = R.string.confirm_permissions), // Используем строку "Confirm permissions"
                    color = Color.White,
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Bold
                )
            }

            Spacer(modifier = Modifier.height(20.dp))
        }
    }

    @Composable
    fun PermissionItem(title: String, description: String, onClick: () -> Unit) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column {
                Text(
                    text = title,
                    color = Color(0xD934204C),
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Medium,
                    lineHeight = 24.sp
                )
                Text(
                    text = description,
                    color = Color(0x9934204C),
                    fontSize = 14.sp,
                    lineHeight = 20.sp
                )
            }
            Button(
                onClick = onClick,
                modifier = Modifier
                    .width(130.dp)
                    .height(36.dp),
                colors = ButtonDefaults.buttonColors(backgroundColor = Color(0xFFE7EDF3)),
                shape = RoundedCornerShape(8.dp)
            ) {
                Text(
                    text = stringResource(id = R.string.allow), // Используем строку "Allow" из ресурсов
                    color = Color(0xCC34204C),
                    fontSize = 14.sp
                )
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
        if (requestCode == LOCATION_PERMISSION_REQUEST_CODE) {
            val allPermissionsGranted = grantResults.isNotEmpty() && grantResults.all { it == PackageManager.PERMISSION_GRANTED }
            permissionsGranted = allPermissionsGranted

            setContent {
                MainScreen(isSendingData, permissionsGranted)
            }

            if (allPermissionsGranted) {
                Log.d(TAG, "All permissions granted")
                startForegroundService()
            } else {
                Log.d(TAG, "Permissions not granted")
            }
        }
    }

    private fun checkAllPermissions(context: Context): Boolean {
//        Если приложения, предназначенные для Android 14, используют службу переднего плана,
//        они должны объявить определенное разрешение на основе типа службы переднего плана,
//        которое представлено в Android 14.
//        https://developer.android.com/about/versions/14/changes/fgs-types-required?hl=ru
        val hasForegroundServiceLocationPermission =
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
                ActivityCompat.checkSelfPermission(
                    context,
                    android.Manifest.permission.FOREGROUND_SERVICE_LOCATION
                ) == PackageManager.PERMISSION_GRANTED
            } else {
                true // На версиях ниже Android 14 разрешение не требуется
            }

        val hasBackgroundLocationPermission =
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                ActivityCompat.checkSelfPermission(
                    context,
                    android.Manifest.permission.ACCESS_BACKGROUND_LOCATION
                ) == PackageManager.PERMISSION_GRANTED
            } else {
                true
            }

        return ActivityCompat.checkSelfPermission(
            context,
            android.Manifest.permission.ACCESS_FINE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED &&
                ActivityCompat.checkSelfPermission(
                    context,
                    android.Manifest.permission.READ_PHONE_STATE
                ) == PackageManager.PERMISSION_GRANTED &&
                hasBackgroundLocationPermission &&
                hasForegroundServiceLocationPermission
    }

    @RequiresApi(Build.VERSION_CODES.O)
    @Composable
    fun MainContent(
        state: MainActivity.MainActivityState,
        selectedTabIndex: Int,
        onTabSelected: (Int) -> Unit
    ) {
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
                        when (selectedTabIndex) {
                            0 -> LoginScreen(
                                state,
                                onLoginSuccess = { onTabSelected(5) },
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
                                },
                                onBackClick = { onTabSelected(5) }
                            )
                            1 -> DataScreen(state, onNavigateTo = onTabSelected)
//                         2 -> RSRPGraph(state)
                            2 -> MapScreen(state, onNavigateTo = onTabSelected)
                            3 -> TrafficScreen(state, onNavigateTo = onTabSelected)
                            4 -> SettingsScreen(state, onNavigateTo = { index, uuid, jwtToken -> onTabSelected(index)})
                            5 -> NavigationScreen(onNavigateTo = onTabSelected)
                            7 -> DataSendingScreen(state, onNavigateTo = onTabSelected, onCellInfoDataClick  = {})
                        }
                    }
                }
                else {
                    PermissionRequestButtons(context) { allGranted ->
                        permissionsGranted = allGranted
                    }
                }
            }
        }
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
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.FOREGROUND_SERVICE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
                startForegroundService(serviceIntent)
            } else {
                Log.e(TAG, "Foreground service location permission not granted")
            }
        } else {
            startForegroundService(serviceIntent)
        }
    }

    fun stopForegroundService() {
        val serviceIntent = Intent(this, ForegroundService::class.java)
        stopService(serviceIntent)
    }
}