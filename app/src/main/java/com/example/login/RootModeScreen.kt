package com.example.login

import android.os.Build
import android.util.Log
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
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Security
import androidx.compose.material3.IconButton
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

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
    val rootManager = remember { RootManager(context) }

    var isRootModeEnabled by remember { mutableStateOf(rootManager.isRootModeEnabled()) }
    val isDarkTheme = isSystemInDarkTheme()

    LaunchedEffect(rootManager.isRootModeEnabled()) {
        val actualState = rootManager.isRootModeEnabled()
        if (isRootModeEnabled != actualState) {
            Log.d("RootModeScreen", "Syncing UI state ($isRootModeEnabled -> $actualState)")
            isRootModeEnabled = actualState
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

            }
        }
    }
}

