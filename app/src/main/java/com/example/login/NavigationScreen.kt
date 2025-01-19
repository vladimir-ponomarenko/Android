package com.example.login

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.foundation.BorderStroke
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.ui.res.stringResource
import androidx.compose.foundation.isSystemInDarkTheme


@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun NavigationScreen(onNavigateTo: (Int) -> Unit) {
    val isDarkTheme = isSystemInDarkTheme()

    val exitIcon = if (isDarkTheme) R.drawable.exit_dark else R.drawable.exit_light
    val menuIcon = if (isDarkTheme) R.drawable.menu_dark1 else R.drawable.menu_light1
    val backgroundColor = if (isDarkTheme) Color(0xFF1C1C1E) else Color(0xFFF8F8F8)

    Scaffold(
        containerColor = backgroundColor,
        topBar = {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(56.dp)
                    .background(if (isDarkTheme) Color(0xFF2C2C2E) else Color(0xFFF8F8F8))
                    .border(
                        width = 2.dp,
                        color = if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E),
                        shape = RoundedCornerShape(bottomStart = 0.dp, bottomEnd = 0.dp)
                    )
            ) {
                Row(
                    modifier = Modifier.fillMaxSize(),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    IconButton(onClick = { onNavigateTo(0) }) {
                        Image(
                            painter = painterResource(id = exitIcon),
                            contentDescription = "Выход",
                            modifier = Modifier
                                .padding(start = 16.dp)
                                .size(24.dp)
                        )
                    }
                    Spacer(modifier = Modifier.width(16.dp))
                    Text(
                        text = stringResource(id = R.string.menu_title),
                        color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                        fontSize = 20.sp,
                        fontWeight = FontWeight.SemiBold
                    )
                }
            }
        }
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
                .padding(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Spacer(modifier = Modifier.height(16.dp))
            Image(
                painter = painterResource(id = menuIcon),
                contentDescription = "Логотип",
                modifier = Modifier.size(110.dp)
            )

            Spacer(modifier = Modifier.height(32.dp))

            Column(
                modifier = Modifier.fillMaxWidth(),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceAround
                ) {
                    MenuButton(onClick = { onNavigateTo(2) }, text = stringResource(id = R.string.map_button_text), iconResId = if (isDarkTheme) R.drawable.map1_icon_d else R.drawable.map1_icon_l)
                    MenuButton(onClick = { onNavigateTo(3) }, text = stringResource(id = R.string.traffic_button_text), iconResId = if (isDarkTheme) R.drawable.traffic_icon_d else R.drawable.traffic_icon_l)
                }
                Spacer(modifier = Modifier.height(16.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceAround
                ) {
                    MenuButton(onClick = { onNavigateTo(1) }, text = stringResource(id = R.string.data_button_text), iconResId = if (isDarkTheme) R.drawable.data_icon_d else R.drawable.data_icon_l)
                    MenuButton(onClick = { onNavigateTo(4) }, text = stringResource(id = R.string.settings_button_text), iconResId = if (isDarkTheme) R.drawable.settings_icon_d else R.drawable.settings_icon_l)
                }
            }
        }
    }
}

@Composable
fun MenuButton(onClick: () -> Unit, text: String, iconResId: Int) {
    val isDarkTheme = isSystemInDarkTheme()

    val buttonColor = if (isDarkTheme) Color(0xCC567BFF) else Color(0xFF132C86)
    val borderColor = if (isDarkTheme) Color(0x4DF6F6F6) else Color(0xFF818EBA).copy(alpha = 0.3f)

    Button(
        onClick = onClick,
        modifier = Modifier.size(172.dp),
        shape = RoundedCornerShape(16.dp),
        colors = ButtonDefaults.buttonColors(containerColor = buttonColor),
        border = BorderStroke(2.dp, borderColor)
    ) {
        Column(
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Image(
                painter = painterResource(id = iconResId),
                contentDescription = null,
                modifier = Modifier.size(91.dp)
            )
            Spacer(modifier = Modifier.height(8.dp))
            Text(
                text = text,
                color = Color.White.copy(alpha = 0.85f),
                fontSize = 20.sp,
                textAlign = TextAlign.Center
            )
        }
    }
}