package com.example.login

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.util.Log
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.isSystemInDarkTheme
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
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Button
import androidx.compose.material.ButtonDefaults
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.FolderOpen
import androidx.compose.material.icons.filled.Security
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.launch

@Composable
fun RootModeTopPanel(onNavigateBack: () -> Unit) {
    val isDarkTheme = isSystemInDarkTheme()
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(56.dp)
            .background(if (isDarkTheme) Color(0xFF2C2C2E) else Color(0xFFF8F8F8))
            .border(
                width = 2.dp,
                color = if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E),
                shape = RoundedCornerShape(0.dp)
            )
    ) {
        Row(
            modifier = Modifier.fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            IconButton(onClick = onNavigateBack) {
                Image(
                    painter = painterResource(id = if (isDarkTheme) R.drawable.transition_dark else R.drawable.transition_light),
                    contentDescription = stringResource(R.string.settings_title),
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            Text(
                text = stringResource(R.string.root_mode_title),
                color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold
            )
        }
    }
}

@RequiresApi(Build.VERSION_CODES.O)
@Composable
fun RootModeScreen(
    state: MainActivity.MainActivityState,
    onNavigateBack: () -> Unit
) {
    val context = LocalContext.current
    val prefs = remember { context.getSharedPreferences("RootModePrefs", Context.MODE_PRIVATE) }

    var selectedDirectoryUri by remember {
        mutableStateOf<Uri?>(
            prefs.getString("selected_log_dir_uri", null)?.let { Uri.parse(it) }
        )
    }


    val coroutineScope = rememberCoroutineScope()



    val rootManager = remember(context, selectedDirectoryUri) {
        Log.d("RootModeScreen", "Re-creating RootManager with URI: $selectedDirectoryUri")
        RootManager(context) { selectedDirectoryUri }
    }

    var isRootModeEnabled by remember { mutableStateOf(rootManager.isRootModeEnabled()) }
    val isDarkTheme = isSystemInDarkTheme()


    LaunchedEffect(key1 = rootManager) {
        Log.d("RootModeScreen", "LaunchedEffect for rootManager: Updating isRootModeEnabled from RootManager")
        isRootModeEnabled = rootManager.isRootModeEnabled()
    }


    LaunchedEffect(Unit) {
        while(true) {
            kotlinx.coroutines.delay(1000)
            val actualState = rootManager.isRootModeEnabled()
            if (isRootModeEnabled != actualState) {
                Log.d("RootModeScreen", "Background Sync: UI state ($isRootModeEnabled -> $actualState)")
                isRootModeEnabled = actualState
            }
        }
    }


    val directoryPickerLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.OpenDocumentTree()
    ) { uri: Uri? ->
        if (uri != null) {
            val contentResolver = context.contentResolver
            val takeFlags: Int = Intent.FLAG_GRANT_READ_URI_PERMISSION or
                    Intent.FLAG_GRANT_WRITE_URI_PERMISSION
            try {
                contentResolver.takePersistableUriPermission(uri, takeFlags)
                selectedDirectoryUri = uri
                prefs.edit().putString("selected_log_dir_uri", uri.toString()).apply()
                Log.i("RootModeScreen", "Directory selected: $uri")


                if (isRootModeEnabled) {
                    Log.d("RootModeScreen", "Root mode is enabled, restarting to apply new log path.")

                    coroutineScope.launch {
                        rootManager.setRootModeEnabled(false)
                        kotlinx.coroutines.delay(500)
                        rootManager.setRootModeEnabled(true)

                        isRootModeEnabled = rootManager.isRootModeEnabled()
                    }
                }
            } catch (e: SecurityException) {
                Log.e("RootModeScreen", "Failed to take persistable URI permission", e)
            }
        } else {
            Log.w("RootModeScreen", "Directory selection cancelled.")
        }
    }

    Surface(
        modifier = Modifier.fillMaxSize(),
        color = if (isDarkTheme) Color(0xFF1C1C1E) else Color(0xFFFFFFFF)
    ) {
        Column(modifier = Modifier.fillMaxSize()) {
            RootModeTopPanel(onNavigateBack = onNavigateBack)
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(16.dp)
            ) {
                SettingItemWithSwitch(
                    label = stringResource(R.string.enable_root_mode),
                    isChecked = isRootModeEnabled,
                    onCheckedChange = { enabled ->
                        Log.d("RootModeScreen", "Switch toggled to $enabled")

                        rootManager.setRootModeEnabled(enabled)

                        isRootModeEnabled = rootManager.isRootModeEnabled()
                    },
                    iconVector = Icons.Filled.Security,
                    iconTint = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                )
                Spacer(modifier = Modifier.height(16.dp))
                Text(
                    text = stringResource(R.string.root_features_description),
                    color = if (isDarkTheme) Color(0x99FFFFFF) else Color(0xCC34204C),
                    fontSize = 14.sp
                )
                Spacer(modifier = Modifier.height(24.dp))

                Text(
                    text = stringResource(R.string.log_directory_path_label),
                    color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                    fontWeight = FontWeight.Medium
                )
                Spacer(modifier = Modifier.height(4.dp))
                Text(
                    text = selectedDirectoryUri?.path ?: stringResource(R.string.default_log_path_internal_app),
                    color = if (isDarkTheme) Color(0x99FFFFFF) else Color(0x9934204C),
                    fontSize = 12.sp,
                    modifier = Modifier.padding(bottom = 8.dp)
                )
                Button(
                    onClick = {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                            try {
                                directoryPickerLauncher.launch(null)
                            } catch (e: Exception) {
                                Log.e("RootModeScreen", "Error launching directory picker", e)

                            }
                        } else {
                            Log.w("RootModeScreen", "SAF directory picker not available on this API level.")
                        }
                    },
                    modifier = Modifier.fillMaxWidth(),
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = if (isDarkTheme) Color(0xFF3C3C3E) else Color(0xFFE0E0E0)
                    )
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Icon(
                            imageVector = Icons.Filled.FolderOpen,
                            contentDescription = stringResource(R.string.select_directory_button),
                            tint = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Text(
                            text = stringResource(R.string.select_directory_button),
                            color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                        )
                    }
                }
            }
        }
    }
}

@RequiresApi(Build.VERSION_CODES.O) 
@Preview(showBackground = true, name = "Light Mode")
@Preview(showBackground = true, name = "Dark Mode", uiMode = android.content.res.Configuration.UI_MODE_NIGHT_YES)
@Composable
fun PreviewRootModeScreen() {
    val mockContext = LocalContext.current
    val mockMainActivityState = remember { MainActivity.MainActivityState(mockContext) }
    Surface {
        RootModeScreen(
            state = mockMainActivityState, 
            onNavigateBack = { Log.d("Preview", "Navigate Back clicked") }
        )
    }
}