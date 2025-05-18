package com.example.login



import android.content.Context
import android.net.Uri
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.core.content.edit
import com.example.login.MainActivity.Companion.TAG
import com.topjohnwu.superuser.Shell
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import kotlinx.serialization.json.Json
import kotlinx.serialization.json.JsonObject
import kotlinx.serialization.json.buildJsonObject
import kotlinx.serialization.json.jsonObject
import kotlinx.serialization.json.put
import java.io.BufferedWriter
import java.io.File
import java.io.FileWriter
import java.io.IOException
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.concurrent.ArrayBlockingQueue
import java.util.concurrent.CancellationException




data class LogEntry(
    val rawJson: String, val latitude: String, val longitude: String, val altitude: String, val timestamp: Long
)

class RootDataLogger {
    @Volatile private var writer: BufferedWriter? = null
    private var isFirstEntry = true
    private var logFile: File? = null
    private var documentFileUri: Uri? = null
    private var outputStream: java.io.OutputStream? = null

    private val TAG = "RootDataLogger"
    private val json = Json { ignoreUnknownKeys = true; prettyPrint = false }
    private val lock = Any()


    @Synchronized
    fun init(context: Context, selectedDirUri: Uri? = null): Boolean {
        if (writer != null || outputStream != null) {
            Log.w(TAG, "init: Already initialized.")
            return true
        }
        try {
            val timestamp = SimpleDateFormat("H-m-s_d.M.y", Locale.getDefault()).format(Date())
            val fileName = "Signal_data_diag_$timestamp.txt"

            if (selectedDirUri != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                Log.d(TAG, "Attempting to use SAF with URI: $selectedDirUri")
                try {
                    val docTree = androidx.documentfile.provider.DocumentFile.fromTreeUri(context, selectedDirUri)
                    if (docTree == null || !docTree.canWrite()) {
                        Log.e(TAG, "SAF: Cannot write to selected directory URI or URI is invalid: $selectedDirUri. Falling back.")
                        return initAppSpecificExternalStorage(context, fileName)
                    }
                    val newFile = docTree.createFile("text/plain", fileName)
                    if (newFile != null && newFile.canWrite()) {
                        documentFileUri = newFile.uri
                        outputStream = context.contentResolver.openOutputStream(documentFileUri!!)
                        writer = outputStream?.bufferedWriter()
                        if (writer == null) {
                            Log.e(TAG, "SAF: Failed to get writer from outputStream. Falling back.")
                            if (outputStream != null) try { outputStream?.close() } catch (e: IOException) {}
                            outputStream = null
                            documentFileUri = null
                            return initAppSpecificExternalStorage(context, fileName)
                        }
                        Log.i(TAG, "Logger initialized with SAF. Path: ${documentFileUri}")
                    } else {
                        Log.e(TAG, "SAF: Failed to create file in selected directory. Falling back. newFile: $newFile, canWrite: ${newFile?.canWrite()}")
                        return initAppSpecificExternalStorage(context, fileName)
                    }
                } catch (e: Exception) {
                    Log.e(TAG, "SAF: Exception during SAF file creation. Falling back.", e)
                    return initAppSpecificExternalStorage(context, fileName)
                }
            } else {

                Log.d(TAG, "SAF URI not provided or API level too low. Using app-specific external storage.")
                return initAppSpecificExternalStorage(context, fileName)
            }

            writer?.write("[")
            isFirstEntry = true
            Log.i(TAG, "Logger common initialization part finished.")
            return true
        } catch (e: Exception) {
            Log.e(TAG, "Error during logger init (outer catch)", e)
            closeInternal()
            return false
        }
    }


    private fun initAppSpecificExternalStorage(context: Context, fileName: String): Boolean {
        try {
            val externalFilesDir = context.getExternalFilesDir(null)
            if (externalFilesDir == null) {
                Log.e(TAG, "getExternalFilesDir(null) returned null! Cannot use app-specific external storage.")

                val internalFilesDir = context.filesDir
                val rootDataDir = File(internalFilesDir, "SignalDataRoot_Logs_Internal")
                if (!rootDataDir.exists() && !rootDataDir.mkdirs()) { Log.e(TAG, "Failed internal dir create"); return false }
                logFile = File(rootDataDir, fileName)
            } else {
                val rootDataDir = File(externalFilesDir, "SignalDataRoot_Logs")
                if (!rootDataDir.exists() && !rootDataDir.mkdirs()) { Log.e(TAG, "Failed external app-specific dir create"); return false }
                logFile = File(rootDataDir, fileName)
            }
            Log.i(TAG, "Initializing logger (app-specific storage). Path: ${logFile?.absolutePath}")
            writer = BufferedWriter(FileWriter(logFile, false))
            // writer?.write("[")
            // isFirstEntry = true
            return true
        } catch (e: Exception) {
            Log.e(TAG, "Error initializing logger with app-specific storage", e)
            return false
        }
    }


    fun log(entry: LogEntry) {

        val currentWriter = writer ?: return
        try {
            val originalJsonObject = try { json.parseToJsonElement(entry.rawJson).jsonObject }
            catch (e: Exception) { return }
            val modifiedJson = buildJsonObject {
                put("geo", buildJsonObject { put("latitude", entry.latitude); put("longitude", entry.longitude); put("altitude", entry.altitude) })
                put("diagTimestamp", entry.timestamp)
                originalJsonObject.entries.forEach { (key, value) -> if (key != "geo" && key != "diagTimestamp") { put(key, value) } }
            }
            val jsonString = json.encodeToString(JsonObject.serializer(), modifiedJson)

            synchronized(lock) {
                if (writer != null) {
                    if (!isFirstEntry) { writer?.write(",") }
                    writer?.write(jsonString)
                    isFirstEntry = false
                    try { writer?.flush() } catch (flushEx: IOException) { Log.e(TAG, "Error flushing writer", flushEx) }
                }
            }
        } catch (e: Exception) { Log.e(TAG, "Error logging entry", e) }
    }


    fun close() {
        synchronized(lock) { closeInternal() }
    }

    private fun closeInternal() {
        if (writer != null) {
            val writerToClose = writer; writer = null
            try {
                Log.i(TAG, "Closing logger file: ${logFile?.name}")
                if (!isFirstEntry) { writerToClose?.write("]") } else { writerToClose?.write("]") }
                writerToClose?.newLine()
                writerToClose?.flush()
                writerToClose?.close()
                Log.i(TAG, "Logger closed.")
            } catch (e: IOException) { Log.e(TAG, "Error closing logger file", e) }
            finally { logFile = null; isFirstEntry = true }
        }
    }
}





class DiagDataPacket {
    private var epoch_msec: Long = 0
    private var timestamp: String = ""
    private var data_length: Int = 0
    private var msg_type: Int = 0
    internal lateinit var payload: ByteArray

    constructor(bb: ByteArray, data_bb: ByteArray) {
        try {
            if (bb.size >= 12) {
                var idx = 0
                val low_msg_type = bb[idx++]
                val high_msg_type = bb[idx++]
                msg_type = ((high_msg_type.toInt() and 0xFF) shl 8) or (low_msg_type.toInt() and 0xFF)
                idx += 2
                epoch_msec = ByteBuffer.wrap(bb, 4, 8).order(ByteOrder.LITTLE_ENDIAN).long
                timestamp = SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS", Locale.US).format(Date(epoch_msec))
                payload = data_bb
                data_length = payload.size
            } else { setDefaultsOnError() }
        } catch (e: Exception) { setDefaultsOnError() }
    }
    private fun setDefaultsOnError() { payload = ByteArray(0); timestamp = "INVALID"; epoch_msec = 0; msg_type = -1; data_length = -1 }
    fun getEpochMs(): Long = epoch_msec
    fun getTimestamp(): String = timestamp
    fun getDataLength(): Int = data_length
    fun getPayload(): ByteArray = payload
    fun getMsgType(): Int = msg_type
}






class DiagRevealerControl(
    private val fifo: ArrayBlockingQueue<DiagDataPacket>,
    private val diagConfigFilePath: String,
    private val context: Context,
    private val selectedDirUriProvider: () -> Uri?
) {

    @Volatile
    private var revealerIsRunning = false
    private val TAG = "DiagRevealerControl"


    @Volatile
    private var rootDataLogger: RootDataLogger? = null

    private var loggingScope: CoroutineScope? = null


    companion object {
        init {
            try { System.loadLibrary("diagRevealer"); Log.d(TAG, "Lib loaded.") }
            catch (e: UnsatisfiedLinkError) { Log.e(TAG, "Lib load failed", e) }
        }
    }


    fun logRevealer(header_buf: ByteArray, data_buf: ByteArray) {

        if (!revealerIsRunning) return


        if (header_buf.size < 12 || data_buf.isEmpty()) { return }
        val diagPkt = DiagDataPacket(header_buf, data_buf)
        if (diagPkt.getMsgType() == -1) return
        fifo.offer(diagPkt)


        val timestampMs = diagPkt.getEpochMs()
        val payloadToDecode = diagPkt.getPayload()
        if (payloadToDecode.isEmpty()) return


        val currentLogger = rootDataLogger ?: return

        val decodedPackets: Array<Any?> = try {
            decodePacket(payloadToDecode)
        } catch (e: Throwable) {
            Log.e(TAG, "!!! Error during SYNC decodePacket in logRevealer !!!", e)
            return
        }


        if (decodedPackets.isNotEmpty()) {

            val currentLat = MainActivity.state.Latitude
            val currentLon = MainActivity.state.Longtitude
            val currentAlt = MainActivity.state.Altitude


            loggingScope?.launch {
                try {

                    if (!revealerIsRunning || rootDataLogger == null) return@launch

                    decodedPackets.forEach { packetObj ->
                        if (!isActive) return@forEach
                        if (packetObj is String && packetObj.isNotEmpty()) {
                            val logEntry = LogEntry(packetObj, currentLat, currentLon, currentAlt, timestampMs)

                            currentLogger.log(logEntry)
                        }
                    }
                } catch (e: Exception) {
                    Log.e(TAG, "Error during logging coroutine execution", e)
                }
            }


            CoroutineScope(Dispatchers.Main).launch {
                try {
                    if (!revealerIsRunning) return@launch

                    if (decodedPackets.isNotEmpty()) {
                        for (packet in decodedPackets) {
                            if (!revealerIsRunning) break
                            Log.d(TAG, "Decoded Packet: ${packet?.toString()?.take(50000)}")
                        }
                    }
                } catch (e: Exception) { Log.e(TAG, "Error in Logcat logging", e) }
            }

        }
    }



    fun getKnownMessageTypes(): Array<Any?> { return try { getTypeNames() } catch (e: Throwable) { Log.e(TAG, "Error getTypeNames", e); emptyArray() } }
    fun decodePacket(bb: ByteArray): Array<Any?> { return try { processLogChunk(bb) } catch (e: Throwable) { Log.e(TAG, "Error processLogChunk", e); emptyArray() } }
    fun writeNewDiagCfg(msgTypes: Array<String>): Boolean { return try { val r = writeDiagCfg(msgTypes, diagConfigFilePath); r.isNotEmpty() && (r[0] as? Int == 0) } catch (e: Throwable) { Log.e(TAG, "Error writeDiagCfg", e); false } }


    @RequiresApi(Build.VERSION_CODES.O)
    fun runRevealer() {
        if (revealerIsRunning) { Log.w(TAG, "runRevealer: Already running."); return }
        Log.i(TAG, "runRevealer: Starting...")


        synchronized(this) {
            if (rootDataLogger == null) {
                Log.i(TAG, "   runRevealer: Initializing RootDataLogger...")
                rootDataLogger = RootDataLogger()

                val selectedUri = selectedDirUriProvider()
                Log.d(TAG, "   runRevealer: URI for logger init: $selectedUri")
                if (rootDataLogger?.init(context, selectedUri) == false) {
                    Log.e(TAG, "   RootDataLogger init FAILED!")
                    rootDataLogger = null
                } else {
                    Log.i(TAG, "   RootDataLogger initialized successfully with URI: $selectedUri")
                }
            } else {
                Log.d(TAG, "   runRevealer: RootDataLogger already exists.")
            }


            if (loggingScope == null || loggingScope?.isActive == false) {
                Log.i(TAG, "   runRevealer: Creating new loggingScope (IO + SupervisorJob).")
                loggingScope = CoroutineScope(Dispatchers.IO + SupervisorJob())
            }
        }


        _setRevealerRunState(true)

        CoroutineScope(Dispatchers.IO).launch {
            Log.d(TAG,"   runRevealer: Calling native readDiag...")
            var nativeCallSuccess = false
            val resp = try {
                val result = readDiag(diagConfigFilePath)
                nativeCallSuccess = true
                result
            } catch (e: Throwable) { Log.e(TAG, "Native readDiag failed", e); arrayOf(-99, e.message)}
            finally {
                Log.i(TAG,"   runRevealer: Native readDiag finished/failed. Resetting state.")
                _setRevealerRunState(false)
            }
            val exitCode = resp.getOrNull(0) as? Int ?: -99
            val exitMsg = resp.getOrNull(1)?.toString() ?: "No message"
            Log.i(TAG, "   runRevealer: DIAG thread exited code $exitCode, msg: $exitMsg")

            if (exitCode != 0 || !nativeCallSuccess) {
                Log.e(TAG, "DIAG thread ended abnormally (code: $exitCode, nativeSuccess: $nativeCallSuccess).")
                CoroutineScope(Dispatchers.Main).launch {

                    RootManager(context, selectedDirUriProvider)
                        .handleRootOperationFailure("DIAG thread ended code $exitCode")

                }
            }
        }
    }


    fun stopRevealer() {
        if (!revealerIsRunning) { Log.d(TAG,"stopRevealer: Not running."); return }
        Log.i(TAG,"stopRevealer: Stopping...")


        closeLogger()

        try { stopDiag() }
        catch (e: Throwable) { Log.e(TAG, "Error native stopDiag", e); _setRevealerRunState(false) }

    }


    @Synchronized
    private fun closeLogger() {

        if (loggingScope?.isActive == true) {
            Log.d(TAG, "Cancelling loggingScope...")
            loggingScope?.cancel(CancellationException("Logger closing"))
        }
        loggingScope = null


        val loggerToClose = rootDataLogger
        if (loggerToClose != null) {
            rootDataLogger = null
            Log.i(TAG, "Closing RootDataLogger synchronously...")
            try { loggerToClose.close() }
            catch (e: Exception) { Log.e(TAG, "Error closing logger", e) }
        } else {
            // Log.d(TAG, "closeLogger: Logger already null.")
        }
    }

    fun getRevealerRunStatus(): Boolean = revealerIsRunning


    private fun _setRevealerRunState(boo: Boolean) {
        if (revealerIsRunning != boo) {
            Log.i(TAG,"Setting revealer run state to: $boo")
            revealerIsRunning = boo
            if (!boo) {

                Log.d(TAG, "_setRevealerRunState(false): Closing logger.")
                closeLogger()
            }
        }
    }


    private external fun processLogChunk(bb: ByteArray): Array<Any?>
    private external fun getTypeNames(): Array<Any?>
    private external fun writeDiagCfg(msgTypes: Array<String>, fileName: String): Array<Any?>
    private external fun wsTest(testnum: Byte): String
    private external fun readDiag(diagConfigFilePath: String): Array<Any?>
    private external fun stopDiag()
}





class RootManager(private val context: Context,
                  private val selectedDirUriProvider: () -> Uri?
                  ) {

    companion object {
        private const val PREFS_NAME = "RootPrefs"
        private const val KEY_ROOT_MODE = "root_mode"
        private const val DEV_DIAG = "/dev/diag"
        private const val DIAG_CFG_FILE_NAME = "Diag.cfg"
        init { Shell.enableVerboseLogging = true; Shell.setDefaultBuilder( Shell.Builder.create().setFlags(Shell.FLAG_MOUNT_MASTER).setTimeout(60) ) }
    }

    private val prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
    private val logTag = "RootManager"

    private lateinit var diagLogBuffer: ArrayBlockingQueue<DiagDataPacket>
    internal lateinit var DRC: DiagRevealerControl

    init {
        diagLogBuffer = ArrayBlockingQueue<DiagDataPacket>(65535, true)

        DRC = DiagRevealerControl(diagLogBuffer, "${context.filesDir}/$DIAG_CFG_FILE_NAME", context, selectedDirUriProvider)
        Log.d(logTag, "RootManager initialized (Final Log Safety Version).")
    }

    fun isRootModeEnabled() = prefs.getBoolean(KEY_ROOT_MODE, false)


    @RequiresApi(Build.VERSION_CODES.O)
    fun setRootModeEnabled(enabled: Boolean) {
        val currentState = isRootModeEnabled()

        if (currentState == enabled && (!enabled || DRC.getRevealerRunStatus())) {
            Log.d(logTag, "setRootModeEnabled: No change needed for state $enabled")
            return
        }
        Log.i(logTag, "setRootModeEnabled: Setting root mode to $enabled")
        prefs.edit { putBoolean(KEY_ROOT_MODE, enabled) }
        if (enabled) {
            requestRootAccess()
        } else {
            DRC.stopRevealer()
        }
    }


    @RequiresApi(Build.VERSION_CODES.O)
    fun requestRootAccess() {
        Log.d(logTag, "requestRootAccess called (Final Log Safety Version)")
        if (!isRootModeEnabled()){ Log.w(logTag,"Not enabled"); return }
        if (DRC.getRevealerRunStatus()) { Log.w(logTag,"Already running"); return }
        if (!isDeviceArchitectureSupported()) { Log.e(logTag,"Arch not supported"); handleRootOperationFailure("unsupported architecture"); return }

        CoroutineScope(Dispatchers.IO).launch {
            Log.d(logTag, "Checking root access...")
            Shell.getShell { shell ->
                if (shell.isRoot) {
                    Log.d(logTag, "Root access granted.")
                    if (initDiag()) {
                        Log.i(logTag, "Diag init OK. Starting revealer...")
                        DRC.runRevealer()
                    } else{ Log.e(logTag,"Diag init FAILED."); handleRootOperationFailure("Diag init failed") }
                } else { Log.e(logTag,"Root access denied."); handleRootOperationFailure("Root denied") }
            }
        }
    }


    @RequiresApi(Build.VERSION_CODES.O)
    internal fun handleRootOperationFailure(reason: String) {
        CoroutineScope(Dispatchers.Main).launch {
            Log.w(logTag, "handleRootOperationFailure: $reason. Disabling root mode.")
            if (isRootModeEnabled()) { setRootModeEnabled(false) }
        }
    }
    private fun isDeviceArchitectureSupported(): Boolean {
        val supportedAbis = Build.SUPPORTED_ABIS
        Log.d(logTag, "Supported ABIs: ${supportedAbis.joinToString()}")
        return supportedAbis.contains("arm64-v8a") || supportedAbis.contains("armeabi-v7a") || supportedAbis.contains("x86_64")
    }

    private fun initDiag(): Boolean {
        if (!Shell.rootAccess()) {
            Log.e(logTag, "Unable to get root access. Root access is required.")
            return false
        }

        if (!updateSecurityPolicy()) {
            return false
        }


        val enabledMessageTypesList = DRC.getKnownMessageTypes()
            .map { (it as Array<*>)[0] as String }
            .toTypedArray()
        val writeConfigResult = DRC.writeNewDiagCfg(enabledMessageTypesList)
        if (!writeConfigResult) {
            Log.e(logTag, "Failed to write initial Diag.cfg")
            return false
        }
        return true
    }

    private fun updateSecurityPolicy(): Boolean {
        val cmds = listOf(
            "setenforce 0",
            "chmod 777 /dev/diag",
            "supolicy --live \"allow untrusted_app diag_device chr_file {read write open ioctl}\"",
            "supolicy --live \"allow untrusted_app system_data_file file {read write}\"",
            "supolicy --live \"allow untrusted_app app_data_file file {rename}\"",
            "supolicy --live \"allow init diag_device chr_file {getattr write ioctl}\"",
            "supolicy --live \"allow system_server diag_device chr_file {read write}\""
        )
        for (cmd in cmds) {
            val res = Shell.cmd(cmd).exec()
            if (!res.isSuccess) {
                Log.e(logTag, "Failed to execute SELinux policy command: $cmd. Error: ${res.err.joinToString("")}")
                return false
            }
        }
        Log.d(logTag, "SELinux policies updated successfully")
        return true
    }
}