package com.example.login

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.core.app.NotificationCompat
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

class ForegroundService : Service() {

    private val serviceScope = CoroutineScope(Dispatchers.IO + Job())
    private val TAG = "ForegroundService"

    override fun onCreate() {
        super.onCreate()
        createNotificationChannel()
    }

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        val notification = createNotification().build()
        startForeground(NOTIFICATION_ID, notification)

        serviceScope.launch {
            while (true) {
                try {
                    val state = MainActivity.state
                    DataManager.getLocation(applicationContext, state)
                    DataManager.getSignalStrength(state)

                    Handler(Looper.getMainLooper()).post {
                        DataManager.getCellInfo(applicationContext, state)
                    }
                } catch (e: Exception) {
                    Log.e(TAG, "Error in ForegroundService: ", e)
                }
                delay(2000)
            }
        }

        return START_STICKY
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    override fun onDestroy() {
        super.onDestroy()
        serviceScope.cancel()
        Log.d(TAG, "ForegroundService stopped")
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                CHANNEL_ID,
                CHANNEL_NAME,
                NotificationManager.IMPORTANCE_DEFAULT
            )
            val manager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            manager.createNotificationChannel(channel)
        }
    }

    private fun createNotification(): NotificationCompat.Builder {
        val notificationIntent = Intent(this, MainActivity::class.java)
        val pendingIntent = PendingIntent.getActivity(
            this, 0, notificationIntent,
            PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
        )

        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("Thermalmap")
            .setContentText("Собирает данные...")
            .setSmallIcon(R.drawable.ic_launcher_foreground)
            .setContentIntent(pendingIntent)
    }

    companion object {
        private const val CHANNEL_ID = "ForegroundServiceChannel"
        private const val CHANNEL_NAME = "Foreground Service"
        private const val NOTIFICATION_ID = 1
    }
}