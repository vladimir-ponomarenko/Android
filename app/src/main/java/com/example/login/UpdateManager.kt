package com.example.login

import android.app.Activity
import android.app.DownloadManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Context.RECEIVER_EXPORTED
import android.content.Intent
import android.content.IntentFilter
import android.net.Uri
import android.os.Build
import android.os.Environment
import android.provider.Settings
import androidx.core.content.FileProvider
import java.io.File


object UpdateManager {
    const val REQUEST_INSTALL_PACKAGES = 1234

    fun startDownloadAndInstall(context: Context, downloadUrl: String) {

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if (!context.packageManager.canRequestPackageInstalls()) {
                (context as Activity).startActivityForResult(
                    Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES).setData(
                        Uri.parse("package:${context.packageName}")
                    ), REQUEST_INSTALL_PACKAGES
                )

                return
            }
        }

        val fileName = "app-release.apk"
        val request = DownloadManager.Request(Uri.parse(downloadUrl))
            .setAllowedNetworkTypes(DownloadManager.Request.NETWORK_WIFI or DownloadManager.Request.NETWORK_MOBILE)
            .setTitle("Загрузка обновления")
            .setDescription("Загрузка новой версии приложения")
            .setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED)
            .setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, fileName)

        val downloadManager = context.getSystemService(Context.DOWNLOAD_SERVICE) as DownloadManager
        val downloadId = downloadManager.enqueue(request)

        val receiver = DownloadCompleteReceiver(context, downloadId, fileName)
        val intentFilter = getIntentFilter()

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            context.registerReceiver(receiver, intentFilter, RECEIVER_EXPORTED)
        } else {
            context.registerReceiver(receiver, intentFilter)
        }

    }

    fun installApk(context: Context, uri: Uri) {
        val installIntent = Intent(Intent.ACTION_VIEW)
        installIntent.setDataAndType(uri, "application/vnd.android.package-archive")
        installIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
        installIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)

        context.startActivity(installIntent)
    }



    private class DownloadCompleteReceiver(val context: Context, val downloadId: Long, val fileName: String) : BroadcastReceiver() {

        override fun onReceive(context: Context, intent: Intent) {
            val id = intent.getLongExtra(DownloadManager.EXTRA_DOWNLOAD_ID, -1)
            if (downloadId == id) {
                val downloadManager = context.getSystemService(Context.DOWNLOAD_SERVICE) as DownloadManager
                val uri = downloadManager.getUriForDownloadedFile(downloadId)

                if (uri != null) {
                    val contentUri = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                        val file = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), fileName)
                        FileProvider.getUriForFile(
                            context,
                            "${context.packageName}.provider",
                            file
                        )
                    } else {
                        uri
                    }
                    installApk(context, contentUri)
                }


                context.unregisterReceiver(this)
            }
        }
    }

    private fun getIntentFilter(): IntentFilter {
        return IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE)
    }
}