package com.example.login

import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.Context
import android.os.Build
import android.util.Log
import androidx.core.app.NotificationCompat
import androidx.work.CoroutineWorker
import androidx.work.WorkerParameters
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.util.Calendar
import java.util.concurrent.TimeUnit

class TrafficDataWorker(appContext: Context, workerParams: WorkerParameters) :
    CoroutineWorker(appContext, workerParams) {

    private val notificationManager =
        appContext.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager

    override suspend fun doWork(): Result = withContext(Dispatchers.IO) {
        try {
            val email = inputData.getString("Email") ?: ""
            val password = inputData.getString("Password") ?: ""

            val authResponse = MainActivity.networkManager.authenticateForTraffic(email, password).await()

            if (authResponse != null) {
                val calendar = Calendar.getInstance()
                calendar.add(Calendar.DAY_OF_YEAR, -1)
                val topApps = getAppTrafficDataForDays(
                    applicationContext,
                    listOf(
                        TimeUnit.MILLISECONDS.toDays(
                            System.currentTimeMillis() - calendar.timeInMillis
                        ).toInt()
                    )
                )
                    .sortedByDescending { it.totalBytes }
                    .take(10)

                MainActivity.networkManager.sendTrafficDataToServer(authResponse.jwt, topApps)

                showTrafficSentNotification()

                Result.success()
            } else {
                Log.e(MainActivity.TAG, "Authentication for traffic data failed")
                Result.failure()
            }
        } catch (e: Exception) {
            Log.e(MainActivity.TAG, "Error sending traffic data: ${e.message}", e)
            Result.retry()
        }
    }

    private fun showTrafficSentNotification() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channelId = "traffic_data_channel"
            val channelName = "Traffic Data"
            val importance = NotificationManager.IMPORTANCE_LOW
            val channel = NotificationChannel(channelId, channelName, importance)
            notificationManager.createNotificationChannel(channel)
        }

        val notificationBuilder = NotificationCompat.Builder(applicationContext, "traffic_data_channel")
            .setContentTitle("Данные о трафике")
            .setContentText("Данные о трафике отправлены на сервер")
            .setSmallIcon(R.drawable.ic_launcher_foreground)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .setAutoCancel(true)

        notificationManager.notify(TRAFFIC_NOTIFICATION_ID, notificationBuilder.build())
    }

    companion object {
        private const val TRAFFIC_NOTIFICATION_ID = 2
    }
}