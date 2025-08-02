package com.example.login

import android.app.Activity
import android.net.Uri
import android.util.Log
import com.google.gson.stream.JsonReader
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.async
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import kotlinx.serialization.json.buildJsonObject
import kotlinx.serialization.json.encodeToJsonElement
import kotlinx.serialization.json.put
import okhttp3.Call
import okhttp3.Callback
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import org.json.JSONArray
import org.json.JSONObject
import java.io.File
import java.io.IOException
import java.io.InputStreamReader
import java.util.concurrent.TimeUnit


class NetworkManager<Context>(private val context: Context, private val serverUrl: String, private val webSocketEndpoint: String) {
    internal var webSocket: WebSocket? = null
    private var isWebSocketConnected = false
    companion object {
        private const val TAG = "NetworkManager"
    }

    private val httpClient = OkHttpClient.Builder()
        .pingInterval(30, TimeUnit.SECONDS)
        .build()

    fun registerUser(email: String, password: String, onComplete: (RegisterResponse?) -> Unit) {
        val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val request = Request.Builder()
            .url("$serverUrl/api/v1/auth/signup")
            .post(requestBody)
            .addHeader("Accept", "application/json")
            .build()

        Log.d(TAG, "Sending register request to ${request.url}")

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to register user - Network Error", e)
                (context as? Activity)?.runOnUiThread {
                    onComplete(null)
                }
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    val responseBodyString = try {
                        it.body?.string()
                    } catch (e: Exception) {
                        Log.e(TAG, "Error reading response body", e)
                        null
                    }

                    Log.d(TAG, "Raw Register Response Received. Code: ${it.code}")
                    if (!responseBodyString.isNullOrBlank()) {
                        Log.d(TAG, "Raw Register Response Body: $responseBodyString")
                    } else {
                        Log.w(TAG, "Register Response Body is null or empty")
                    }

                    if (it.isSuccessful && !responseBodyString.isNullOrBlank()) {
                        try {
                            val json = Json {
                                ignoreUnknownKeys = true
                                isLenient = true
                            }

                            val registerResponse = json.decodeFromString<RegisterResponse>(responseBodyString)

                            Log.d(TAG, "Successfully parsed Register Response (fields might be null): $registerResponse")

                            (context as? Activity)?.runOnUiThread {
                                onComplete(registerResponse)
                            }
                        } catch (e: Exception) {
                            Log.e(TAG, "Failed to parse register response JSON.", e)
                            Log.e(TAG, "Response body that failed parsing: $responseBodyString")
                            (context as? Activity)?.runOnUiThread {
                                onComplete(null)
                            }
                        }
                    } else {
                        Log.e(TAG, "Failed to register user - Server Error or empty body. Code: ${it.code}, Body: $responseBodyString")
                        (context as? Activity)?.runOnUiThread {
                            onComplete(null)
                        }
                    }
                }
            }
        })
    }
    fun verifyToken(token: String, onComplete: (Boolean) -> Unit) {
        val request = Request.Builder()
            .url("$serverUrl/api/jwt/verify")
            .header("Authorization", "Bearer $token")
            .post("".toRequestBody(null))
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to verify token (Network Error)", e)
                (context as? Activity)?.runOnUiThread { onComplete(false) }
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    val responseBody = try { it.body?.string() } catch (e: Exception) { null }
                    if (!it.isSuccessful || responseBody.isNullOrBlank()) {
                        Log.e(TAG, "Failed to verify token - Server Error or empty body. Code: ${it.code}, Body: $responseBody")
                        (context as? Activity)?.runOnUiThread { onComplete(false) }
                        return@use
                    }
                    try {
                        val json = Json { ignoreUnknownKeys = true; isLenient = true }
                        val jsonResponse = json.decodeFromString<Map<String, String>>(responseBody)
                        val result = jsonResponse["result"]
                        Log.d(TAG, "Token verification result: $result")
                        (context as? Activity)?.runOnUiThread {
                            onComplete(result == "valid")
                        }
                    } catch (e: Exception) {
                        Log.e(TAG, "Failed to parse verify token response JSON.", e)
                        Log.e(TAG, "Verify token response body: $responseBody")
                        (context as? Activity)?.runOnUiThread {
                            onComplete(false)
                        }
                    }
                }
            }
        })
    }

    fun authenticateUser(email: String, password: String, token: String?, onComplete: (AuthResponse?) -> Unit) {
        Log.d(TAG, "Attempting authentication for email: $email. Token provided: ${!token.isNullOrBlank()}")

        val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val requestBuilder = Request.Builder()
            .url("$serverUrl/api/v1/auth/signin")
            .post(requestBody)
            .addHeader("Accept", "application/json")

        if (!token.isNullOrBlank()) {
            Log.d(TAG, "Adding Authorization header to signin request.")
            requestBuilder.header("Authorization", "Bearer $token")
        } else {
            Log.d(TAG, "Skipping Authorization header (token is null or blank).")
        }
        val request = requestBuilder.build()

        Log.d(TAG, "Sending signin request to ${request.url}")

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to authenticate user (Network Error)", e)
                (context as? Activity)?.runOnUiThread { onComplete(null) }
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    val responseBody = try { it.body?.string() } catch (e: Exception) { null }
                    if (!it.isSuccessful || responseBody.isNullOrBlank()) {
                        Log.e(TAG, "Failed to authenticate user - Server Error or empty body. Code: ${it.code}, Body: $responseBody")
                        if (it.code == 401) {
                            Log.e(TAG, "Authentication failed: Unauthorized (401). Check credentials or token.")
                        }
                        (context as? Activity)?.runOnUiThread { onComplete(null) }
                        return@use
                    }

                    try {
                        val json = Json { ignoreUnknownKeys = true }
                        val actualAuthResponse = json.decodeFromString<ActualAuthResponse>(responseBody)
                        Log.d(TAG, "Successfully parsed actual auth response: $actualAuthResponse")


                        val firstLog = actualAuthResponse.logs.firstOrNull()
                        if (firstLog != null) {
                            val userUuid = firstLog.user_uuid
                            val accessToken = firstLog.access_token

                            val compatibleAuthResponse = AuthResponse(
                                email = email,
                                jwt = accessToken,
                                uuid = userUuid
                            )

                            Log.d(TAG, "Authentication successful. Passing compatible AuthResponse to callback: $compatibleAuthResponse")
                            (context as? Activity)?.runOnUiThread {
                                onComplete(compatibleAuthResponse)
                            }
                        } else {
                            Log.e(TAG, "Authentication response parsed, but 'logs' array is empty. Cannot extract credentials.")
                            Log.e(TAG, "Auth response body: $responseBody")
                            (context as? Activity)?.runOnUiThread {
                                onComplete(null)
                            }
                        }
                    } catch (e: Exception) {
                        Log.e(TAG, "Failed to parse successful auth response: ${e.message}", e)
                        Log.e(TAG, "Auth response body: $responseBody")
                        (context as? Activity)?.runOnUiThread {
                            onComplete(null)
                        }
                    }
                }
            }
        })
    }

    fun authenticateForTraffic(email: String, password: String): Deferred<AuthResponse?> = CoroutineScope(
        Dispatchers.IO).async {
        var authResponse: AuthResponse? = null
        val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val request = Request.Builder()
            .url("$serverUrl/api/v1/auth/signin")
            .post(requestBody)
            .build()

        try {
            val response = httpClient.newCall(request).execute()

            response.use {
                if (response.isSuccessful) {
                    val responseBody = response.body?.string()
                    try {
                        authResponse = Json { ignoreUnknownKeys = true }.decodeFromString<AuthResponse>(responseBody ?: "")
                        Log.d(TAG, "Auth response: $authResponse")
                    } catch (e: Exception) {
                        Log.e(TAG, "Failed to parse auth response: ${e.message}")
                    }
                } else {
                    Log.e(TAG, "Failed to authenticate for traffic data: ${response.code}")
                }
            }
        } catch (e: IOException) {
            Log.e(TAG, "Failed to authenticate for traffic data", e)
        }

        authResponse
    }

    fun sendTrafficDataToServer(jwt: String, trafficData: List<AppTrafficData>) {
        val state = MainActivity.state

        if (state.Uuid.isNullOrEmpty() || jwt.isNullOrEmpty()) {
            Log.e(TAG, "Cannot send traffic data: UUID or JWT is missing.")
            return
        }

        val top10TrafficData = trafficData.sortedByDescending { it.totalBytes }.take(10)

        val modifiedJson = buildJsonObject {
            put("jwt", jwt)
            put("uuid", MainActivity.state.Uuid)
            put("trafficData", Json.encodeToJsonElement(top10TrafficData.map { appData ->
                buildJsonObject {
                    put("appName", appData.appName)
                    put("totalBytes", appData.totalBytes / 1024)
                    put("mobileBytes", appData.mobileBytes / 1024)
                    put("wifiBytes", appData.wifiBytes / 1024)
                    put("rxBytes", appData.rxBytes / 1024)
                    put("txBytes", appData.txBytes / 1024)
                }
            }))
        }
        val jsonBody = modifiedJson.toString()

        val endpoint = "/api/user/postapptrafic"

        if (webSocket == null) {
            Log.e(TAG, "WebSocket is not initialized, attempting to connect...")

            val request = Request.Builder()
                .url("$serverUrl$endpoint")
                .header("Authorization", "Bearer $jwt")
                .build()

            this.webSocket = httpClient.newWebSocket(request, object : WebSocketListener() {
                override fun onOpen(webSocket: WebSocket, response: Response) {
                    Log.d(TAG, "WebSocket connected successfully for Traffic Data")
                    webSocket.send(jsonBody)
                    Log.d(TAG, "Sent Traffic Data: $jsonBody")
                    (context as? MainActivity)?.showSendingIndicator()
                }

                override fun onMessage(webSocket: WebSocket, text: String) {
                    Log.d(TAG, "Received message from server (Traffic Data): $text")
                }

                override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
                    Log.d(TAG, "WebSocket connection closing (Traffic Data): $code $reason")
                    this@NetworkManager.webSocket = null
                    this@NetworkManager.isWebSocketConnected = false
                }

                override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                    Log.d(TAG, "WebSocket connection closed (Traffic Data): $code $reason")
                    this@NetworkManager.webSocket = null
                    this@NetworkManager.isWebSocketConnected = false
                }

                override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                    Log.e(TAG, "Failed to connect WebSocket for Traffic Data", t)
                }
            })
            this.isWebSocketConnected = this.webSocket != null
        } else if (this.isWebSocketConnected) {
            Log.d(TAG, "Sending Traffic Data through existing WebSocket connection")
            this.webSocket?.send(jsonBody)
            Log.d(TAG, "Sent Traffic Data: $jsonBody")
            (context as? MainActivity)?.showSendingIndicator()
        } else {
            Log.e(TAG, "WebSocket is not connected, cannot send Traffic Data")
        }
    }

    fun sendMessageToData2ToServer(messageToData2: MessageToData2, onComplete: ((Boolean) -> Unit)? = null) {
        val endpoint = "/ws/putdata"

        val modifiedJson = buildJsonObject {
            put("jwt", messageToData2.jwt)
            put("UUID", messageToData2.UUID)
            put("time", messageToData2.time)
            put("latitude", messageToData2.latitude)
            put("longitude", messageToData2.longitude)
            put("altitude", messageToData2.altitude)
            put("manufacturer", messageToData2.manufacturer)
            put("model", messageToData2.model)
            put("androidVersion", messageToData2.androidVersion)
            put("hardware", messageToData2.hardware)
            put("product", messageToData2.product)
            put("board", messageToData2.board)
            put("operator", messageToData2.operator)
            put("cdma", Json.encodeToJsonElement(messageToData2.cdma.cellInfoList))
            put("wcdma", Json.encodeToJsonElement(messageToData2.wcdma.cellInfoList))
            put("gsm", Json.encodeToJsonElement(messageToData2.gsm.cellInfoList))
            put("lte", Json.encodeToJsonElement(messageToData2.lte.cellInfoList))
            put("nr", Json.encodeToJsonElement(messageToData2.nr.cellInfoList))
        }

        val jsonBody = modifiedJson.toString()

        if (webSocket == null) {
            Log.e(TAG, "WebSocket is not initialized, attempting to connect...")

            val request = Request.Builder()
                .url("$serverUrl$endpoint")
                .header("Authorization", "Bearer ${messageToData2.jwt}")
                .build()

            this.webSocket = httpClient.newWebSocket(request, object : WebSocketListener() {
                override fun onOpen(webSocket: WebSocket, response: Response) {
                    Log.d(TAG, "WebSocket connected successfully for CellInfo")
                    this@NetworkManager.webSocket?.send(jsonBody)
                    Log.d(TAG, "Sent MessageToData2: $jsonBody")
                    onComplete?.invoke(true)
                    (context as? MainActivity)?.showSendingIndicator()
                }

                override fun onMessage(webSocket: WebSocket, text: String) {
                    Log.d(TAG, "Received message from server CellInfo: $text")
                }

                override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
                    Log.d(TAG, "WebSocket connection closing CellInfo: $code $reason")
                    this@NetworkManager.webSocket = null
                    this@NetworkManager.isWebSocketConnected = false
                }

                override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                    Log.d(TAG, "WebSocket connection closed (CellInfo): $code $reason")
                    this@NetworkManager.webSocket = null
                    this@NetworkManager.isWebSocketConnected = false
                }

                override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                    Log.e(TAG, "Failed to connect WebSocket for CellInfo", t)
                    onComplete?.invoke(false)
                }
            })
            this.isWebSocketConnected = this.webSocket != null
        } else if (this.isWebSocketConnected) {
            Log.d(TAG, "Sending CellInfo through existing WebSocket connection")
            this.webSocket?.send(jsonBody)
            Log.d(TAG, "Sent CellInfo: $jsonBody")
            onComplete?.invoke(true)
            (context as? MainActivity)?.showSendingIndicator()
        } else {
            Log.e(TAG, "WebSocket is not connected, cannot send CellInfo")
            onComplete?.invoke(false)
        }
    }

    fun sendRootLogToServerFromFile(context: android.content.Context, fileUri: Uri, onComplete: ((Boolean) -> Unit)? = null) {
        val endpoint = "v1/ws/phonedata"
        val maxChunkSizeBytes = 900 * 1024

        var isCompleted = false
        val safeOnComplete = { result: Boolean ->
            if (!isCompleted) {
                isCompleted = true
                onComplete?.invoke(result)
            }
        }

        val processAndSendFile: (WebSocket) -> Unit = { socket ->
            CoroutineScope(Dispatchers.IO).launch {
                try {
                    context.contentResolver.openInputStream(fileUri)?.use { inputStream ->
                        val reader = JsonReader(InputStreamReader(inputStream, "UTF-8")).apply {
                            isLenient = true
                        }
                        reader.use {
                            if (it.peek() != com.google.gson.stream.JsonToken.BEGIN_ARRAY) {
                                Log.e(TAG, "File does not start with a JSON array")
                                safeOnComplete(false)
                                return@launch
                            }

                            it.beginArray()
                            var currentChunkArray = JSONArray()
                            var currentChunkSize = 0

                            while (it.hasNext()) {
                                val jsonObject = it.readJsonObject()
                                val objectString = jsonObject.toString()
                                val objectSize = objectString.toByteArray(Charsets.UTF_8).size + 1

                                if (currentChunkSize > 0 && currentChunkSize + objectSize > maxChunkSizeBytes) {
                                    val chunkToSend = currentChunkArray.toString()
                                    Log.d(TAG, "Sending chunk of size ${chunkToSend.toByteArray(Charsets.UTF_8).size / 1024} KB")
                                    if (!socket.send(chunkToSend)) {
                                        Log.e(TAG, "Failed to send a chunk, aborting.")
                                        safeOnComplete(false)
                                        return@launch
                                    }
                                    currentChunkArray = JSONArray()
                                    currentChunkSize = 0

                                }

                                currentChunkArray.put(jsonObject)
                                currentChunkSize += objectSize
                            }

                            if (currentChunkArray.length() > 0) {
                                val finalChunkToSend = currentChunkArray.toString()
                                Log.d(TAG, "Sending final chunk of size ${finalChunkToSend.toByteArray(Charsets.UTF_8).size / 1024} KB")
                                if (!socket.send(finalChunkToSend)) {
                                    Log.e(TAG, "Failed to send the final chunk, aborting.")
                                    safeOnComplete(false)
                                    return@launch
                                }
                            }

                            Log.i(TAG, "Finished sending all chunks for file: $fileUri")
                            safeOnComplete(true)
                        }
                    } ?: run {
                        Log.e(TAG, "Failed to open input stream for URI: $fileUri")
                        safeOnComplete(false)
                    }
                } catch (e: Exception) {
                    Log.e(TAG, "Error processing or sending file URI: $fileUri", e)
                    safeOnComplete(false)
                }
            }
        }

        if (webSocket != null && isWebSocketConnected) {
            Log.d(TAG, "Using existing WebSocket connection to send RootLog.")
            processAndSendFile(webSocket!!)
            return
        }

        Log.d(TAG, "WebSocket not connected. Creating new connection for RootLog.")
        val request = Request.Builder()
            .url("ws://127.0.0.1:3000/api/$endpoint")
            .header("Authorization", "Bearer ${MainActivity.state.JwtToken}")
            .build()

        val webSocketListener = object : WebSocketListener() {
            override fun onOpen(webSocket: WebSocket, response: Response) {
                Log.d(TAG, "WebSocket connected successfully for RootLog. Starting file processing.")
                this@NetworkManager.webSocket = webSocket
                this@NetworkManager.isWebSocketConnected = true
                processAndSendFile(webSocket)
            }

            override fun onMessage(webSocket: WebSocket, text: String) { Log.d(TAG, "Received message from server RootLog: $text") }

            override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
                Log.d(TAG, "WebSocket connection closing RootLog: $code $reason")
                this@NetworkManager.isWebSocketConnected = false
                safeOnComplete(false)
            }

            override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                Log.d(TAG, "WebSocket connection closed (RootLog): $code $reason")
                this@NetworkManager.webSocket = null
                this@NetworkManager.isWebSocketConnected = false
            }

            override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                Log.e(TAG, "WebSocket connection failed for RootLog", t)
                this@NetworkManager.isWebSocketConnected = false
                safeOnComplete(false)
            }
        }

        httpClient.newWebSocket(request, webSocketListener)
    }

    private fun JsonReader.readJsonObject(): JSONObject {
        beginObject()
        val jsonObject = JSONObject()
        while (hasNext()) {
            val name = nextName()
            when (peek()) {
                com.google.gson.stream.JsonToken.BEGIN_OBJECT -> {
                    jsonObject.put(name, readJsonObject())
                }
                com.google.gson.stream.JsonToken.BEGIN_ARRAY -> {
                    jsonObject.put(name, readJsonArray())
                }
                com.google.gson.stream.JsonToken.STRING -> {
                    jsonObject.put(name, nextString())
                }
                com.google.gson.stream.JsonToken.NUMBER -> {
                    val value = nextString()
                    try {
                        jsonObject.put(name, value.toInt())
                    } catch (e: NumberFormatException) {
                        try {
                            jsonObject.put(name, value.toDouble())
                        } catch (e: NumberFormatException) {
                            jsonObject.put(name, value)
                        }
                    }
                }
                com.google.gson.stream.JsonToken.BOOLEAN -> {
                    jsonObject.put(name, nextBoolean())
                }
                com.google.gson.stream.JsonToken.NULL -> {
                    nextNull()
                    jsonObject.put(name, JSONObject.NULL)
                }
                else -> skipValue()
            }
        }
        endObject()
        return jsonObject
    }

    private fun JsonReader.readJsonArray(): JSONArray {
        beginArray()
        val jsonArray = JSONArray()
        while (hasNext()) {
            when (peek()) {
                com.google.gson.stream.JsonToken.BEGIN_OBJECT -> {
                    jsonArray.put(readJsonObject())
                }
                com.google.gson.stream.JsonToken.BEGIN_ARRAY -> {
                    jsonArray.put(readJsonArray())
                }
                com.google.gson.stream.JsonToken.STRING -> {
                    jsonArray.put(nextString())
                }
                com.google.gson.stream.JsonToken.NUMBER -> {
                    val value = nextString()
                    try {
                        jsonArray.put(value.toInt())
                    } catch (e: NumberFormatException) {
                        try {
                            jsonArray.put(value.toDouble())
                        } catch (e: NumberFormatException) {
                            jsonArray.put(value)
                        }
                    }
                }
                com.google.gson.stream.JsonToken.BOOLEAN -> {
                    jsonArray.put(nextBoolean())
                }
                com.google.gson.stream.JsonToken.NULL -> {
                    nextNull()
                    jsonArray.put(JSONObject.NULL)
                }
                else -> skipValue()
            }
        }
        endArray()
        return jsonArray
    }

    fun sendMessageToServerFromFile(filePath: String, onComplete: ((Boolean) -> Unit)? = null) {
        val endpoint = "v1/ws/phonedata"
        val jsonBody = try {
            val fileContent = File(filePath).readText()
            if (!fileContent.startsWith("[")) {
                throw IOException("File content is not a valid JSON array")
            }
            fileContent
        } catch (e: IOException) {
            Log.e(TAG, "Error reading or validating JSON from file: $filePath", e)
            onComplete?.invoke(false)
            return
        }

        var isCompleted = false
        val safeOnComplete = { result: Boolean ->
            if (!isCompleted) {
                isCompleted = true
                onComplete?.invoke(result)
            }
        }

        val finishAfterDelay: () -> Unit = {
            CoroutineScope(Dispatchers.IO).launch {
                delay(10000)
                safeOnComplete(true)
            }
        }

        if (webSocket == null || !isWebSocketConnected) {
            Log.e(TAG, "WebSocket is not initialized or not connected, attempting to connect...")
            val request = Request.Builder()
                .url("ws://109.172.114.128:3000/api/v1/ws/phonedata") // TODO Хардкод на время теста. Вынести в MainActivity
                .header("Authorization", "Bearer ${MainActivity.state.JwtToken}")
                .build()
            this.webSocket = httpClient.newWebSocket(request, object : WebSocketListener() {
                override fun onOpen(webSocket: WebSocket, response: Response) {
                    Log.d(TAG, "WebSocket connected successfully for CellInfo (from file)")
                    sendJsonBody(jsonBody) { success ->
                        if (success) {
                            finishAfterDelay()
                        } else {
                            safeOnComplete(false)
                        }
                    }
                }

                override fun onMessage(webSocket: WebSocket, text: String) {
                    Log.d(TAG, "Received message from server CellInfo: $text")
                }

                override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
                    Log.d(TAG, "WebSocket connection closing CellInfo: $code $reason")
                    this@NetworkManager.webSocket = null
                    this@NetworkManager.isWebSocketConnected = false
                }

                override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                    Log.d(TAG, "WebSocket connection closed (CellInfo): $code $reason")
                    this@NetworkManager.webSocket = null
                    this@NetworkManager.isWebSocketConnected = false
                }

                override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                    Log.e(TAG, "Failed to connect WebSocket for CellInfo", t)
                    safeOnComplete(false)
                }
            })
            this.isWebSocketConnected = this.webSocket != null
        } else {
            Log.d(TAG, "Sending CellInfo through existing WebSocket connection")
            sendJsonBody(jsonBody) { success ->
                if (success) {
                    finishAfterDelay()
                } else {
                    safeOnComplete(false)
                }
            }
        }
    }

    private fun sendJsonBody(jsonBody: String, onComplete: (Boolean) -> Unit) {
        try {
            this.webSocket?.send(jsonBody)
            Log.d(TAG, "Sent CellInfo (from networkmanager): $jsonBody")
            onComplete(true)
            (context as? MainActivity)?.showSendingIndicator()
        } catch (e: Exception) {
            Log.e(TAG, "Failed to send WebSocket message", e)
            onComplete(false)
        }
    }

    /* Способ отправки JSON на сервер с помощью multipart/form-data   */
//    fun sendMessageToServerFromFile(filePath: String, onComplete: ((Boolean) -> Unit)? = null) {
//        val endpoint = "/api/sockets/thermalmap"
//        val file = File(filePath)
//
//        // Проверка, существует ли файл
//        if (!file.exists()) {
//            Log.e(TAG, "File not found: $filePath")
//            onComplete?.invoke(false)
//            return
//        }
//
//        // Использование правильного MIME-типа для передачи файла
//        val requestBody = MultipartBody.Builder()
//            .setType(MultipartBody.FORM)
//            .addFormDataPart("file", file.name, file.asRequestBody("application/octet-stream".toMediaTypeOrNull()))  // Более общий тип
//            .build()
//
//        val request = Request.Builder()
//            .url("$serverUrl$endpoint")
//            .post(requestBody)
//            .build()
//
//        // Отправка запроса
//        httpClient.newCall(request).enqueue(object : Callback {
//            override fun onFailure(call: Call, e: IOException) {
//                // Логируем ошибку при отправке
//                Log.e(TAG, "Failed to send file to server", e)
//                // Убедитесь, что onComplete вызывается в случае ошибки
//                onComplete?.invoke(false)
//            }
//
//            override fun onResponse(call: Call, response: Response) {
//                // Проверка кода ответа
//                if (response.isSuccessful) {
//                    Log.d(TAG, "File sent to server successfully")
//                    onComplete?.invoke(true)
//                    (context as? MainActivity)?.showSendingIndicator()
//                } else {
//                    // Логируем ошибку сервера
//                    Log.e(TAG, "Failed to send file to server: ${response.code} ${response.message}")
//                    onComplete?.invoke(false)
//                }
//                // Закрытие ответа для освобождения ресурсов
//                response.close()
//            }
//        })
//    }

    fun connectWebSocket(
        jwt: String,
        onOpen: (WebSocket, Response) -> Unit = { _, _ -> },
        onMessage: (WebSocket, String) -> Unit = { _, _ -> },
        onClosed: (WebSocket, Int, String) -> Unit = { _, _, _ -> },
        onFailure: (WebSocket, Throwable, Response?) -> Unit = { _, _, _ -> }
    ): WebSocket? {
        val request = Request.Builder()
            .url("$serverUrl$webSocketEndpoint")
            .header("Authorization", "Bearer $jwt")
            .build()

        val webSocketListener = object : WebSocketListener() {
            override fun onOpen(webSocket: WebSocket, response: Response) {
                onOpen(webSocket, response)
            }

            override fun onMessage(webSocket: WebSocket, text: String) {
                onMessage(webSocket, text)
            }

            override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                onClosed(webSocket, code, reason)
            }

            override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                onFailure(webSocket, t, response)
            }
        }

        return httpClient.newWebSocket(request, webSocketListener)
    }

}
