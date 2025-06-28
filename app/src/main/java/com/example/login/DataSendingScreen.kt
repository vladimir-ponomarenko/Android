package com.example.login

import android.content.Context
import android.os.Build
import android.util.Log
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.layout.Arrangement
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
import androidx.compose.material3.IconButton
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
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
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext


// Предварительный просмотр UI
@Preview(showBackground = true)
@Composable
fun PreviewDataSendingScreen() {
    val context = LocalContext.current
    val state = remember {
        MainActivity.MainActivityState(context).apply {
            JwtToken = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwiaWF0IjoxNTE2MjM5MDIyfQ.SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c"
            Uuid = "a0e1c4d8-3f2g-5h6i-7j8k-9l0m1n2o3p4q"
        }
    }
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
        DataSendingScreen(
            state = state,
            onNavigateTo = {},
            onCellInfoDataClick = {}
        )
    }
}

@RequiresApi(Build.VERSION_CODES.O)
@Composable
fun DataSendingScreen(
    state: MainActivity.MainActivityState,
    onNavigateTo: (Int) -> Unit,
    onCellInfoDataClick: () -> Unit
) {
    val jwtToken = state.JwtToken
    val uuid = state.Uuid
    val context = LocalContext.current
    val coroutineScope = rememberCoroutineScope()
    val (pendingCount, successCount) = DataManager.getFileCounters(context)

    val sharedPreferences = context.getSharedPreferences("app_prefs", Context.MODE_PRIVATE)
    var isRecording by remember {
        mutableStateOf(sharedPreferences.getBoolean("is_recording", true))
    }

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
            IconButton(onClick = { onNavigateTo(4) }) {
                Image(
                    painter = painterResource(id = if (isDarkTheme) R.drawable.transition_dark else R.drawable.transition_light),
                    contentDescription = stringResource(R.string.settings_title),
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            androidx.compose.material.Text(
                text = stringResource(R.string.sending_collected_data),
                color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold,
            )
            Spacer(modifier = Modifier.weight(1f))
        }
    }

    Surface(
        modifier = Modifier.fillMaxSize(),
        color = if (isDarkTheme) Color(0xFF1C1C1E) else Color(0xFFFFFFFF)
    )
    {
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 8.dp)
                    .background(if (isDarkTheme) Color(0xFF2C2C2E) else Color(0xFFF8F8F8))
                    .border(
                        width = 1.5.dp,
                        color = if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E),
                        shape = RoundedCornerShape(8.dp)
                    )
                    .padding(16.dp)
            ) {
                    Column {
                        Text(
                            text = stringResource(id = R.string.uuid_label),
                            color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                            fontSize = 15.sp,
                            fontWeight = FontWeight.Bold,
                            modifier = Modifier.padding(bottom = 4.dp)
                        )
                        Text(
                            text = uuid ?: stringResource(id = R.string.not_specified),
                            color = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xCC34204C),
                            fontSize = 14.sp,
                            fontWeight = FontWeight.Normal
                        )
                    }
                }

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 8.dp)
                    .background(if (isDarkTheme) Color(0xFF2C2C2E) else Color(0xFFF8F8F8))
                    .border(
                        width = 1.5.dp,
                        color = if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E),
                        shape = RoundedCornerShape(8.dp)
                    )
                        .padding(16.dp)
                ) {
                    Column {
                        Text(
                            text = stringResource(id = R.string.jwt_token_label),
                            color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                            fontSize = 15.sp,
                            fontWeight = FontWeight.Bold,
                            modifier = Modifier.padding(bottom = 4.dp)
                        )
                        Text(
                            text = jwtToken ?: stringResource(id = R.string.not_specified),
                            color = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xCC34204C),
                            fontSize = 14.sp,
                            fontWeight = FontWeight.Normal,
                            textAlign = TextAlign.Start,
                            maxLines = 1,
                            overflow = androidx.compose.ui.text.style.TextOverflow.Ellipsis
                        )
                    }
                }

                Button(
                    onClick = {
                        coroutineScope.launch {
                            val triggered = DataManager.triggerFileUpload(context)
                            withContext(Dispatchers.Main) {
                                if (triggered) {
                                    Toast.makeText(
                                        context,
                                        "Отправка данных инициирована",
                                        Toast.LENGTH_SHORT
                                    ).show()
                                } else {
                                    Toast.makeText(
                                        context,
                                        "Нет данных для отправки",
                                        Toast.LENGTH_SHORT
                                    ).show()
                                }
                            }
                        }
                    },
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = if (isDarkTheme) Color(0xCC567BFF) else Color(0xFF132C86),
                        contentColor = Color.White
                    ),
                    shape = RoundedCornerShape(10.dp),
                    elevation = ButtonDefaults.elevation(0.dp),
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 16.dp)
                        .height(50.dp)
                ) {
                    Text(
                        text = stringResource(id = R.string.send_cell_info),
                        color = Color.White,
                        fontSize = 18.sp,
                        fontWeight = FontWeight.Bold
                    )
                }

                Button(
                    onClick = {
                        coroutineScope.launch {
                            val deletedCount = DataManager.deleteAllLogFiles(context)
                            withContext(Dispatchers.Main) {
                                val message = if (deletedCount > 0) {
                                    "Удалено старых файлов: $deletedCount"
                                } else {
                                    "Нет старых файлов для удаления"
                                }
                                Toast.makeText(context, message, Toast.LENGTH_SHORT).show()
                            }
                        }
                    },
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = if (isDarkTheme) Color(0xFF2C2C2E) else Color(0x26567BFF),
                        contentColor = if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E)
                    ),
                    shape = RoundedCornerShape(10.dp),
                    elevation = ButtonDefaults.elevation(0.dp),
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 16.dp)
                        .height(50.dp)
                ) {
                    Text(
                        text = stringResource(id = R.string.delete_file),
                        color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                        fontSize = 18.sp,
                        fontWeight = FontWeight.Normal
                    )
                }

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                            .padding(16.dp),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Column {
                            Text(
                                text = stringResource(id = R.string.resume_recording),
                                color = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xFF34204C),
                                fontSize = 16.sp,
                                fontWeight = FontWeight.Medium
                            )
                            Text(
                                text = stringResource(id = R.string.resume_recording_description),
                                color = if (isDarkTheme) Color(0x80FFFFFF) else Color(0x9934204C),
                                fontSize = 14.sp,
                                fontWeight = FontWeight.Normal
                            )
                        }
                    Switch(
                        checked = isRecording,
                        onCheckedChange = {
                            isRecording = it
                            with(sharedPreferences.edit()) {
                                putBoolean("is_recording", isRecording)
                                apply()
                            }
                            if (isRecording) {
                                Log.d(MainActivity.TAG, "Resumed recording")
                                Toast.makeText(
                                    context,
                                    context.getString(R.string.resumed_recording),
                                    Toast.LENGTH_SHORT
                                ).show()
                            } else {
                                Log.d(MainActivity.TAG, "Stopped recording")
                                Toast.makeText(
                                    context,
                                    context.getString(R.string.stopped_recording),
                                    Toast.LENGTH_SHORT
                                ).show()
                            }
                        },
                        modifier = Modifier
                            .size(width = 51.dp, height = 31.dp)
                            .padding(2.dp),
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color.White,
                            checkedTrackColor = if (isDarkTheme) Color(0xCC567BFF) else Color(
                    0xFF132C86
                            ),
                            uncheckedThumbColor = Color.White,
                            uncheckedTrackColor = if (isDarkTheme) Color(0xFF2C2C2E) else Color(
                                0xFFf0f2f4
                            )
                        ),
                        thumbContent = {}
                    )
                }

            Column(
                modifier = Modifier
                    .align(Alignment.End)
                    .padding(16.dp)
            ) {
                Spacer(modifier = Modifier.weight(1f))
                Text(
                    text = stringResource(id = R.string.failed_to_send, pendingCount),
                    color = if (isDarkTheme) Color(0x80FFFFFF) else Color(0xCC9E9E9E),
                    fontSize = 14.sp
                )
                Text(
                    text = stringResource(id = R.string.successfully_sent, successCount),
                    color = if (isDarkTheme) Color(0x80FFFFFF) else Color(0xCC9E9E9E),
                    fontSize = 14.sp
                )
            }
        }
    }
}