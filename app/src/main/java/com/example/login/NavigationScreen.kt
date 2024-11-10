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
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.foundation.BorderStroke
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material.Text
import androidx.compose.material.Scaffold


@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun NavigationScreen(onNavigateTo: (Int) -> Unit) {
    val istokWebFontFamily = FontFamily(Font(R.font.istokweb_regular))

    Scaffold(
        topBar = {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(50.dp)
                    .background(Color(0xFFF8F8F8))
                    .border(
                        width = 2.dp,
                        color = (Color(0x809E9E9E)),
                        shape = RoundedCornerShape(bottomStart = 0.dp, bottomEnd = 0.dp)
                    )
            ) {
                Row(
                    modifier = Modifier.fillMaxSize(),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    IconButton(onClick = {
                        onNavigateTo(0)
                    }) {
                        Image(
                            painter = painterResource(id = R.drawable.exit_light),
                            contentDescription = "Выход",
                            modifier = Modifier
                                .padding(start = 20.dp)
                                .size(24.dp)
                        )
                    }
                    Spacer(modifier = Modifier.width(16.dp))
                    Text(
                        text = "Меню",
                        color = Color(0xFF34204C),
                        fontSize = 20.sp,
                        fontFamily = istokWebFontFamily,
                        fontWeight = FontWeight.Bold
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
                painter = painterResource(id = R.drawable.menu_light1),
                contentDescription = "Логотип",
                modifier = Modifier.size(110.dp)
            )

            Spacer(modifier = Modifier.height(32.dp))

            // Кнопки в сетке
            Column(
                modifier = Modifier.fillMaxWidth(),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceAround
                ) {
                    MenuButton(onClick = { onNavigateTo(2) }, text = "Карта", iconResId = R.drawable.map1_icon_l)
                    MenuButton(onClick = { onNavigateTo(3) }, text = "Трафик", iconResId = R.drawable.traffic_icon_l)
                }
                Spacer(modifier = Modifier.height(16.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceAround
                ) {
                    MenuButton(onClick = { onNavigateTo(1) }, text = "Данные", iconResId = R.drawable.data_icon_l)
                    MenuButton(onClick = { onNavigateTo(4) }, text = "Настройки", iconResId = R.drawable.settings_icon_l)
                }
            }
        }
    }
}

@Composable
fun MenuButton(onClick: () -> Unit, text: String, iconResId: Int) {
    val istokWebFontFamily = FontFamily(Font(R.font.istokweb_regular)) // Создаем FontFamily из ресурса шрифта

    Button(
        onClick = onClick,
        modifier = Modifier.size(172.dp),
        shape = RoundedCornerShape(16.dp),
        colors = ButtonDefaults.buttonColors(containerColor = Color(0xFF132C86)),
        border = BorderStroke(2.dp, Color(0xFF818EBA))
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
                color = Color.White,
                fontSize = 20.sp,
                fontFamily = istokWebFontFamily,
                textAlign = TextAlign.Center
            )
        }
    }
}
