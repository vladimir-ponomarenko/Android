package com.example.login.ui.theme

import android.R.id.primary
import android.app.Activity
import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.dynamicDarkColorScheme
import androidx.compose.material3.dynamicLightColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.SideEffect
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalView
import androidx.core.view.WindowCompat


internal val DarkColorScheme = darkColorScheme(
    primary = Color(0xCC567BFF), // Цвет кнопок
    secondary = Color(0xFF567BFF), // Цвет кнопок (тот же, что и primary)
    tertiary = Color(0xFF018786), // Цвет для третичных элементов (по желанию, можно изменить)
    background = Color(0xFF1C1C1E), // Задний фон приложения
    surface = Color(0xFF2C2C2E), // Верхняя плашка приложения
    onPrimary = Color(0xFFFFFFFF).copy(alpha = 0.85f), // Цвет текста внутри кнопок
    onSecondary = Color(0xFFFFFFFF).copy(alpha = 0.85f), // Цвет текста внутри кнопок
    onBackground = Color(0xFFFFFFFF).copy(alpha = 0.7f), // Основной текст и интерактивные элементы
    onSurface = Color(0xFFFFFFFF).copy(alpha = 0.8f),
)

internal val LightColorScheme = lightColorScheme(
    primary = Color(0xFF6200EE), // Цвет кнопок для светлой темы
    secondary = Color(0xFF03DAC6), // Вторичный цвет кнопок
    tertiary = Color(0xFF018786), // Цвет для третичных элементов
    background = Color(0xFFFFFFFF), // Цвет фона для светлой темы
    surface = Color(0xFFFFFFFF), // Цвет поверхности для светлой темы
    onPrimary = Color(0xFFFFFFFF), // Цвет текста внутри кнопок
    onSecondary = Color(0xFF000000), // Цвет текста внутри вторичных кнопок
    onBackground = Color(0xFF000000), // Цвет текста на светлом фоне
    onSurface = Color(0xFF000000),
)

@Composable
fun LoginTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    dynamicColor: Boolean = true,
    content: @Composable () -> Unit
) {
    val colorScheme = when {
        dynamicColor && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> {
            val context = LocalContext.current
            if (darkTheme) dynamicDarkColorScheme(context) else dynamicLightColorScheme(context)
        }
        darkTheme -> DarkColorScheme
        else -> LightColorScheme
    }

    val view = LocalView.current
    if (!view.isInEditMode) {
        SideEffect {
            val window = (view.context as Activity).window
            window.statusBarColor = colorScheme.primary.toArgb()
            WindowCompat.getInsetsController(window, view).isAppearanceLightStatusBars = darkTheme
        }
    }

    MaterialTheme(
        colorScheme = colorScheme,
        typography = Typography,
        content = content
    )
}