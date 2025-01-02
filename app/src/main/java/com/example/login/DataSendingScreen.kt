package com.example.login

import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
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
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
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
import java.io.File

// Предварительный просмотр UI
@Preview(showBackground = true)
@Composable
fun PreviewDataSendingScreen() {
    val context = LocalContext.current
    val state = remember {
        MainActivity.MainActivityState(context).apply {
            JwtToken = "eKKF2QT4fwpMeJf36POk6yJV_adQssw"
            Uuid = "7a98e528-d0d8-4478-b7bc-2545d5cb90b5"
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

    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(56.dp)
            .background(Color.White)
            .border(width = 2.dp, color = Color(0x809E9E9E), shape = RoundedCornerShape(0.dp))
    ) {
        Row(
            modifier = Modifier.fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            IconButton(onClick = { onNavigateTo(4) }) {
                Image(
                    painter = painterResource(id = R.drawable.transition_light),
                    contentDescription = stringResource(R.string.settings_title),
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            androidx.compose.material.Text(
                text = stringResource(R.string.sending_collected_data),
                color = Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold
            )
            Spacer(modifier = Modifier.weight(1f))
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 8.dp)
                .background(Color(0xFFF8F8F8))
                .border(
                    width = 1.5.dp,
                    color = Color(0x809E9E9E),
                    shape = RoundedCornerShape(8.dp)
                )
                .padding(16.dp)
        ) {
            Text(
                text = stringResource(id = R.string.uuid_label) + ": ${uuid ?: stringResource(id = R.string.not_specified)}",
                color = Color(0xFF34204C),
                fontSize = 16.sp,
                fontWeight = FontWeight.Normal,
                modifier = Modifier.weight(1f)
            )
        }
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 8.dp)
                .background(Color(0xFFF8F8F8))
                .border(
                    width = 1.5.dp,
                    color = Color(0x809E9E9E),
                    shape = RoundedCornerShape(8.dp)
                )
                .padding(16.dp)
        ) {
            Text(
                text = stringResource(id = R.string.jwt_token_label) + ":\n${
                    jwtToken ?: stringResource(
                        id = R.string.not_specified
                    )
                }",
                color = Color(0xFF34204C),
                fontSize = 16.sp,
                fontWeight = FontWeight.Normal,
                modifier = Modifier.weight(1f)
            )
        }

        Button(
            onClick = {
                coroutineScope.launch {
                    val signalDataDir = File(context.getExternalFilesDir(null), "Signal_data")
                    val file = File(signalDataDir, DataManager.fileName)
                    DataManager.sendFileWithRetry(context, file)
                }
            },
            colors = ButtonDefaults.buttonColors(
                backgroundColor = Color(0xCC132C86),
                contentColor = Color.White
            ),
            shape = RoundedCornerShape(10.dp),
            elevation = ButtonDefaults.elevation(0.dp)
        ) {
            Text(
                text = stringResource(id = R.string.send_cell_info),
                color = Color.White
            )
        }

        Spacer(modifier = Modifier.height(8.dp))

        Button(
            onClick = {
                coroutineScope.launch {
                    val signalDataDir = File(context.getExternalFilesDir(null), "Signal_data")
                    val file = File(signalDataDir, DataManager.fileName)
                    if (file.exists()) {
                        file.delete()
                        Log.d(MainActivity.TAG, "File deleted: ${file.absolutePath}")
                    } else {
                        Log.d(MainActivity.TAG, "File does not exist: ${file.absolutePath}")
                    }
                }
            },
            colors = ButtonDefaults.buttonColors(
                backgroundColor = Color(0xCC132C86),
                contentColor = Color.White
            ),
            shape = RoundedCornerShape(10.dp),
            elevation = ButtonDefaults.elevation(0.dp)
        ) {
            Text(
                text = stringResource(id = R.string.delete_file),
                color = Color.White
            )
        }
    }
}