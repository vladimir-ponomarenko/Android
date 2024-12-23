package com.example.login

import android.app.Activity
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.border
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.foundation.Image
import androidx.compose.ui.res.painterResource
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.ui.res.stringResource
import androidx.compose.material3.AlertDialog
import android.content.Context
import android.content.res.Configuration
import java.util.Locale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.material.Text


@Composable
fun SettingsScreen(
    state: MainActivity.MainActivityState,
    onNavigateTo: (Int, String?, String?) -> Unit,
) {
    var isLanguageDialogVisible by remember { mutableStateOf(false) }
    var currentLanguage by remember { mutableStateOf("ru") }

    val context = LocalContext.current

    if (isLanguageDialogVisible) {
        LanguageSelectionDialog(
            currentLanguage = currentLanguage,
            onDismiss = { isLanguageDialogVisible = false },
            onLanguageSelected = { selectedLanguage ->
                currentLanguage = selectedLanguage
                isLanguageDialogVisible = false
                setLocale(context, selectedLanguage)
                (context as? Activity)?.recreate()
            }
        )
    }

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
            IconButton(onClick = { onNavigateTo(5, null, null) }) {
                Image(
                    painter = painterResource(id = R.drawable.transition_light),
                    contentDescription = stringResource(R.string.settings_title),
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            Text(
                text = stringResource(R.string.settings_title),
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
            .padding(horizontal = 16.dp, vertical = 8.dp)
    ) {

        Spacer(modifier = Modifier.height(8.dp))
        SettingItemWithArrow(
            label = stringResource(R.string.interface_language),
            onNavigateTo = { isLanguageDialogVisible = true },
            iconRes = R.drawable.language
        )
        SettingItemWithArrow(
            label = stringResource(R.string.appearance),
            onNavigateTo = { onNavigateTo(6, null, null) },
            iconRes = R.drawable.style
        )
        SettingItemWithArrow(
            label = stringResource(R.string.sending_collected_data),
            onNavigateTo = { onNavigateTo(7, state.Uuid, state.JwtToken) }, // Используем значения из state
            iconRes = R.drawable.s_data
        )
        Spacer(modifier = Modifier.weight(1f))
    }
}

fun setLocale(context: Context, languageCode: String) {
    val locale = Locale(languageCode)
    Locale.setDefault(locale)

    val config = Configuration(context.resources.configuration)
    config.setLocale(locale)

    context.resources.updateConfiguration(config, context.resources.displayMetrics)
}

@Composable
fun LanguageSelectionDialog(
    currentLanguage: String, // Текущий язык, например "ru" или "en"
    onDismiss: () -> Unit,
    onLanguageSelected: (String) -> Unit
) {
    AlertDialog(
        onDismissRequest = { onDismiss() },
        confirmButton = {},
        dismissButton = {},
        text = {
            Column {
                LanguageOption(
                    language = "ru",
                    label = "Русский",
                    isSelected = currentLanguage == "ru", // Проверка, выбран ли язык
                    onSelect = { onLanguageSelected("ru") }
                )
                LanguageOption(
                    language = "en",
                    label = "English",
                    isSelected = currentLanguage == "en", // Проверка, выбран ли язык
                    onSelect = { onLanguageSelected("en") }
                )
            }
        }
    )
}

@Composable
fun LanguageOption(
    language: String,
    label: String,
    isSelected: Boolean,
    onSelect: (String) -> Unit
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 8.dp)
            .clickable { onSelect(language) },
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(
            text = label,
            modifier = Modifier.weight(1f),
            fontSize = 16.sp,
            color = if (isSelected) Color(0xFF34204C) else Color.Gray // Более бледный цвет для невыбранных языков
        )

        // Круг, меняющий цвет в зависимости от выбранного языка
        Box(
            modifier = Modifier
                .size(24.dp)
                .background(
                    color = if (isSelected) Color(0xFF132C86) else Color(0xFFB0B0B0), // Цвет серого для невыбранного
                    shape = RoundedCornerShape(12.dp)
                )
        )
    }
}

@Composable
fun SettingItemWithArrow(label: String, onNavigateTo: () -> Unit, iconRes: Int) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 8.dp)
            .background(Color(0xFFF8F8F8))
            .border(width = 1.5.dp, color = Color(0x809E9E9E), shape = RoundedCornerShape(8.dp))
            .padding(16.dp)
            .clickable { onNavigateTo() },
        verticalAlignment = Alignment.CenterVertically
    ) {
        Image(
            painter = painterResource(id = iconRes),
            contentDescription = label,
            modifier = Modifier.size(24.dp)
        )
        Spacer(modifier = Modifier.width(16.dp))

        Text(
            text = label,
            color = Color(0xFF34204C),
            fontSize = 16.sp,
            fontWeight = FontWeight.Normal,
            modifier = Modifier.weight(1f)
        )

        Icon(
            painter = painterResource(id = R.drawable.arrow),
            contentDescription = "Перейти",
            tint = Color(0xFF34204C),
            modifier = Modifier.size(24.dp)
        )
    }
}