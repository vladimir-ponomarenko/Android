package com.example.login

import android.content.Context
import android.util.Log
import androidx.core.content.edit
import com.topjohnwu.superuser.Shell
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import mad.location.manager.BuildConfig

class RootManager(private val context: Context) {

    companion object {
        private const val PREFS_NAME = "RootPrefs"
        private const val KEY_ROOT_MODE = "root_mode"

        init {
            Shell.enableVerboseLogging = BuildConfig.DEBUG
            Shell.setDefaultBuilder(
                Shell.Builder.create()
                    .setFlags(Shell.FLAG_MOUNT_MASTER)
                    //.setInitializers(ShellInit::class.java)
                    .setTimeout(10)
            )
        }
    }


    private val prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)

    fun isRootModeEnabled(): Boolean {
        return prefs.getBoolean(KEY_ROOT_MODE, false)
    }

    fun setRootModeEnabled(enabled: Boolean) {
        prefs.edit {
            putBoolean(KEY_ROOT_MODE, enabled)
        }
    }


    fun requestRootAccess() {
        CoroutineScope(Dispatchers.IO).launch {
            val result = Shell.cmd("echo 'Root access requested'").exec()
            if (result.isSuccess) {
                Log.d("RootManager", "Root access granted")
            } else {
                Log.e("RootManager", "Root access denied or error: ${result.out}")
                setRootModeEnabled(false)
            }
        }
    }


    fun checkRootAccess(): Boolean {
        return Shell.rootAccess()
    }
}
