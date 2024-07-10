package com.example.login

import android.util.Log
import androidx.compose.runtime.MutableState
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import java.io.IOException
import java.util.concurrent.TimeUnit

object SpeedTestManager {
    private const val TAG = "SpeedTestManager"
    private const val FAST_COM_URL = "https://fast.com"
    private var job: Job? = null

    var downloadSpeed = 0.0
    var uploadSpeed = 0.0
    var ping = 0L
    var testRunning = false

    fun startSpeedTest(scope: CoroutineScope, forceUpdate: MutableState<Boolean>, onTestCompleted: (TestResult) -> Unit) {
        if (testRunning) return

        testRunning = true
        downloadSpeed = 0.0
        uploadSpeed = 0.0
        ping = 0L

        job = scope.launch(Dispatchers.IO) {
            val startTime = System.currentTimeMillis()

            measureDownloadSpeed(forceUpdate)

            val endTime = System.currentTimeMillis()
            val duration = endTime - startTime
            Log.d(TAG, "Test Duration: $duration ms")

            testRunning = false
            val testResult = TestResult(downloadSpeed, uploadSpeed, ping, duration)
            withContext(Dispatchers.Main) {
                onTestCompleted(testResult)
            }
        }
    }

    fun stopSpeedTest() {
        job?.cancel()
        testRunning = false
    }

    private suspend fun measureDownloadSpeed(forceUpdate: MutableState<Boolean>) {
        var downloadedBytes = 0L
        val startTime = System.currentTimeMillis()
        var endTime = startTime
        try {
            val httpClient = OkHttpClient.Builder()
                .connectTimeout(10, TimeUnit.SECONDS)
                .readTimeout(30, TimeUnit.SECONDS)
                .build()

            val request = Request.Builder()
                .url(FAST_COM_URL)
                .get()
                .build()

            val response = httpClient.newCall(request).execute()
            if (!response.isSuccessful) {
                Log.e(TAG, "Error getting Fast.com download URL: ${response.code}")
                return
            }

            val downloadUrl = response.body?.string()?.substringAfter("href=\"")?.substringBefore("\"")
            Log.d(TAG, "Download URL: $downloadUrl")
            if (downloadUrl == null) {
                Log.e(TAG, "Failed to extract download URL from Fast.com response")
                return
            }

            val downloadRequest = Request.Builder()
                .url(downloadUrl)
                .get()
                .build()

            val downloadResponse = httpClient.newCall(downloadRequest).execute()
            if (!downloadResponse.isSuccessful) {
                Log.e(TAG, "Error downloading from Fast.com: ${downloadResponse.code}")
                return
            }

            val inputStream = downloadResponse.body?.byteStream()
            val buffer = ByteArray(1024)
            var bytesRead: Int

            while (inputStream?.read(buffer).also { bytesRead = it ?: -1 } != -1 && testRunning) {
                downloadedBytes += bytesRead
                val elapsedTime = System.currentTimeMillis() - startTime
                downloadSpeed = calculateSpeed(downloadedBytes, elapsedTime)
                Log.d(TAG, "Downloaded bytes: $downloadedBytes, Speed: $downloadSpeed Mbps")

                delay(100)
                withContext(Dispatchers.Main) {
                    forceUpdate.value = !forceUpdate.value
                }
            }

            endTime = System.currentTimeMillis()
        } catch (e: IOException) {
            Log.e(TAG, "Error measuring download speed: ${e.message}", e)
        } finally {
            Log.d(TAG, "Download time: ${endTime - startTime} ms")
        }
    }

    private fun calculateSpeed(bytes: Long, timeMillis: Long): Double {
        return if (timeMillis > 0) {
            (bytes * 8.0 / 1000 / 1000) / (timeMillis.toDouble() / 1000)
        } else {
            0.0
        }
    }
}

data class TestResult(
    val downloadSpeed: Double,
    val uploadSpeed: Double,
    val ping: Long,
    val duration: Long
)