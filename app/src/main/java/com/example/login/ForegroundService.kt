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
import com.example.login.MainActivity.Companion.ACTION_STOP_MAIN_ACTIVITY
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

class ForegroundService : Service() {

    private val serviceScope = CoroutineScope(Dispatchers.IO + Job())
    private var dataCollectionJob: Job? = null
    private val TAG = "ForegroundService"

    override fun onCreate() {
        super.onCreate()
        createNotificationChannel()
    }
    private var job: Job? = null
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        val stopIntent = Intent(this, ForegroundService::class.java).apply {
            action = ACTION_STOP_SERVICE
        }
        val stopPendingIntent = PendingIntent.getService(
            this,
            0,
            stopIntent,
            PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
        )

        if (intent?.action == ACTION_STOP_SERVICE) {
            stopForeground(true)
            stopSelf()

            val stopMainActivityIntent = Intent(ACTION_STOP_MAIN_ACTIVITY)
//            sendBroadcast(stopMainActivityIntent)
            serviceScope.cancel()
            System.exit(0)
            return START_NOT_STICKY
        }

        val notification = createNotification(stopPendingIntent).build()
        startForeground(NOTIFICATION_ID, notification)

        if (dataCollectionJob == null || !dataCollectionJob!!.isActive) {
            dataCollectionJob = serviceScope.launch {
                while (isActive) {
                    try {
                        val state = MainActivity.state
                        DataManager.getLocation(applicationContext, state)
                        DataManager.getSignalStrength(state)

                        updateNotification(state, stopPendingIntent)

                        Handler(Looper.getMainLooper()).post {
                            DataManager.getCellInfo(applicationContext, state)
                        }
                    } catch (e: Exception) {
                        Log.e(TAG, "Error in ForegroundService: ", e)
                    }
                    delay(2000)
                }
            }
        }
        job = CoroutineScope(Dispatchers.IO).launch {
            while (isActive) {
                val messageToData2 = MainActivity.state.messageToData2
                if (messageToData2 != null) {
                    DataManager.saveCellInfoToJsonFile(this@ForegroundService, messageToData2)
                }
                delay(MainActivity.UPDATE_INTERVAL)
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
        job?.cancel()
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

    private fun createNotification(stopPendingIntent: PendingIntent): NotificationCompat.Builder {
        val notificationIntent = Intent(this, MainActivity::class.java)
        val pendingIntent = PendingIntent.getActivity(
            this,
            0,
            notificationIntent,
            PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
        )

        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("Application")
            .setContentText("Приложение работает в фоновом режиме")
            .setSmallIcon(R.drawable.ic_launcher_foreground)
            .setContentIntent(pendingIntent)
            .addAction(
                R.drawable.ic_launcher_foreground,
                "Stop",
                stopPendingIntent
            )
            .setOnlyAlertOnce(true)
    }

    private fun updateNotification(state: MainActivity.MainActivityState, stopPendingIntent: PendingIntent) {
        val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager

        val notificationText = "RSRP: ${state.Rsrp}\n" +
                "Cell ID: ${state.Cellid}\nLat: ${state.Latitude}\nLon: ${state.Longtitude}"

        val notification = createNotification(stopPendingIntent)
            .setContentText(notificationText)
            .setStyle(NotificationCompat.BigTextStyle().bigText(notificationText))
            .build()

        notificationManager.notify(NOTIFICATION_ID, notification)
    }

    companion object {
        private const val CHANNEL_ID = "ForegroundServiceChannel"
        private const val CHANNEL_NAME = "Foreground Service"
        private const val NOTIFICATION_ID = 1
        const val ACTION_STOP_SERVICE = "com.example.login.stop_service"
    }
}