package com.example.login

import android.app.Activity
import android.util.Log
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
import java.util.concurrent.TimeUnit

class NetworkManager<Context>(private val context: Context, private val serverUrl: String, private val webSocketEndpoint: String) {

    companion object {
        private const val TAG = "NetworkManager"
    }

    private val httpClient = OkHttpClient.Builder()
        .pingInterval(5, TimeUnit.SECONDS)
        .build()

    fun registerUser(email: String, password: String, onComplete: (RegisterResponse?) -> Unit) {
        val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val request = Request.Builder()
            .url("$serverUrl/api/user/register")
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
                        return@use
                    }
                    val responseBody = response.body?.string()
                    try {
                        val registerResponse = Json.decodeFromString<RegisterResponse>(responseBody ?: "")
                        Log.d(TAG, "Register response: $registerResponse")
                        (context as? Activity)?.runOnUiThread {  // Используем context для runOnUiThread
                            onComplete(registerResponse)
                        }
                    } catch (e: Exception) {
                        Log.e(TAG, "Failed to parse register response: ${e.message}")
                        (context as? Activity)?.runOnUiThread {  // Используем context для runOnUiThread
                            onComplete(null)
                        }
                    }
                }
            }})
    }

    fun verifyToken(token: String, onComplete: (Boolean) -> Unit) {
        val request = Request.Builder()
            .url("$serverUrl/api/jwt/verify")
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
                        return@use
                    }
                    val responseBody = response.body?.string()
                    val jsonResponse = Json.decodeFromString<Map<String, String>>(responseBody ?: "")
                    val result = jsonResponse["result"]
                    onComplete(result == "valid")
                }
            }
        })
    }

    fun authenticateUser(email: String, password: String, token: String, onComplete: (AuthResponse?) -> Unit) {
        verifyToken(token) { isValid ->
            if (isValid) {
                val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
                val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

                val request = Request.Builder()
                    .url("$serverUrl/api/user/auth")
                    .header("Authorization", "Bearer $token")
                    .post(requestBody)
                    .build()

                httpClient.newCall(request).enqueue(object : Callback {
                    override fun onFailure(call: Call, e: IOException) {
                        Log.e(TAG, "Failed to authenticate user (1)", e)
                        onComplete(null)
                    }

                    override fun onResponse(call: Call, response: Response) {
                        response.use {
                            if (!response.isSuccessful) {
                                if (response.code == 401) {
                                    Log.e(TAG, "Failed to authenticate user: Unauthorized")
                                } else {
                                    Log.e(TAG, "Failed to authenticate user (2): ${response.code}")
                                }
                                onComplete(null)
                                return@use
                            }
                            val responseBody = response.body?.string()
                            try {
                                val authResponse = Json.decodeFromString<AuthResponse>(responseBody ?: "")
                                Log.d(TAG, "Auth response: $authResponse")
                                (context as? Activity)?.runOnUiThread {
                                    onComplete(authResponse)
                                }
                            } catch (e: Exception) {
                                Log.e(TAG, "Failed to parse auth response: ${e.message}")
                                (context as? Activity)?.runOnUiThread {
                                    onComplete(null)
                                }
                            }
                        }
                    }
                })
            } else {
                Log.e(TAG, "Token is invalid, cannot authenticate user")
                onComplete(null)
            }
        }
    }

    fun authenticateForTraffic(email: String, password: String, onComplete: (AuthResponse?) -> Unit) {
        val jsonBody = Json.encodeToString(mapOf("email" to email, "password" to password))
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val request = Request.Builder()
            .url("$serverUrl/api/user/auth")
            .post(requestBody)
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to authenticate for traffic data", e)
                onComplete(null)
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful) {
                        Log.e(TAG, "Failed to authenticate for traffic data: ${response.code}")
                        onComplete(null)
                        return@use
                    }

                    val responseBody = response.body?.string()
                    try {
                        val authResponse = Json.decodeFromString<AuthResponse>(responseBody ?: "")
                        Log.d(TAG, "Auth response: $authResponse")
                        onComplete(authResponse)
                    } catch (e: Exception) {
                        Log.e(TAG, "Failed to parse auth response: ${e.message}")
                        onComplete(null)
                    }
                }
            }
        })
    }

    fun sendTrafficDataToServer(jwt: String, trafficData: List<AppTrafficData>, onComplete: ((Boolean) -> Unit)? = null) {
        val dataToSend = mapOf(
            "jwt" to jwt,
            "uuid" to MainActivity.state.Uuid,
            "trafficData" to trafficData.map { appData ->
                appData.copy(
                    totalBytes = appData.totalBytes / 1024,
                    mobileBytes = appData.mobileBytes / 1024,
                    wifiBytes = appData.wifiBytes / 1024,
                    rxBytes = appData.rxBytes / 1024,
                    txBytes = appData.txBytes / 1024
                )
            }
        )
        val jsonBody = Json.encodeToString(dataToSend)
        val requestBody = jsonBody.toRequestBody("application/json".toMediaTypeOrNull())

        val request = Request.Builder()
            .url("$serverUrl/api/***") //  для отправки трафика
            .header("Authorization", "Bearer $jwt")
            .post(requestBody)
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                Log.e(TAG, "Failed to send traffic data to server", e)
                onComplete?.invoke(false)
            }

            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful) {
                        Log.e(TAG, "Failed to send traffic data to server: ${response.code}")
                        onComplete?.invoke(false)
                    } else {
                        Log.d(TAG, "Traffic data sent to server successfully")
                        onComplete?.invoke(true)
                    }
                }
            }
        })
    }

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