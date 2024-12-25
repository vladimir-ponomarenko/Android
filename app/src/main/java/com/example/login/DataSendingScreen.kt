package com.example.login

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
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.material3.Text
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.ui.res.stringResource
import androidx.compose.material.ButtonDefaults
import androidx.compose.material.Button
import androidx.compose.material3.IconButton
import androidx.compose.ui.Alignment
import androidx.compose.ui.res.painterResource


@Composable
fun DataSendingScreen(
    state: MainActivity.MainActivityState,
    onNavigateTo: (Int) -> Unit,
    onCellInfoDataClick: () -> Unit
) {
    val jwtToken = state.JwtToken
    val uuid = state.Uuid

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
            onClick = onCellInfoDataClick,
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
    }
}