package com.example.login

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Devices
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun NavigationScreen(onNavigateTo: (Int) -> Unit) {
    val isDarkTheme = isSystemInDarkTheme()

    val exitIcon = if (isDarkTheme) R.drawable.exit_dark else R.drawable.exit_light
    val menuIcon = if (isDarkTheme) R.drawable.menu_dark1 else R.drawable.menu_light1
    val backgroundColor = if (isDarkTheme) Color(0xFF1C1C1E) else Color(0xFFFFFFFF)

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
        BoxWithConstraints(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
        ) {
            ContentWithAdaptiveLayout(
                maxWidth = maxWidth,
                maxHeight = maxHeight,
                isDarkTheme = isDarkTheme,
                onNavigateTo = onNavigateTo,
                menuIcon = menuIcon
            )
        }
    }
}

@Composable
fun ContentWithAdaptiveLayout(
    maxWidth: Dp,
    maxHeight: Dp,
    isDarkTheme: Boolean,
    onNavigateTo: (Int) -> Unit,
    menuIcon: Int
) {
    val isLandscape = maxWidth > maxHeight
    val logoSize = if (isLandscape) 80.dp else 110.dp
    val logoTopPadding = if (isLandscape) 0.dp else 16.dp
    val buttonsTopPadding = if (isLandscape) 0.dp else 32.dp

    if (isLandscape) {
        Row(
            modifier = Modifier
                .fillMaxSize()
                .padding(horizontal = 16.dp, vertical = 8.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceEvenly
        ) {
            Column(
                modifier = Modifier
                    .weight(0.35f)
                    .fillMaxHeight(),
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.Center
            ) {
                Image(
                    painter = painterResource(id = menuIcon),
                    contentDescription = "Логотип",
                    modifier = Modifier.size(logoSize)
                )
            }
            Box(
                modifier = Modifier
                    .weight(0.65f)
                    .fillMaxHeight()
                    .padding(start = 16.dp),
                contentAlignment = Alignment.Center
            ) {
                LandscapeButtonsGrid(
                    onNavigateTo = onNavigateTo,
                    isDarkTheme = isDarkTheme,
                    containerHeight = maxHeight
                )
            }
        }
    } else {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(horizontal = 16.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Spacer(modifier = Modifier.height(logoTopPadding))
            Image(
                painter = painterResource(id = menuIcon),
                contentDescription = "Логотип",
                modifier = Modifier.size(logoSize)
            )
            Spacer(modifier = Modifier.height(buttonsTopPadding))
            PortraitButtonsGrid(
                onNavigateTo = onNavigateTo,
                isDarkTheme = isDarkTheme,
                containerWidth = maxWidth
            )
        }
    }
}

@Composable
fun LandscapeButtonsGrid(
    onNavigateTo: (Int) -> Unit,
    isDarkTheme: Boolean,
    containerHeight: Dp
) {
    val buttonSize = (containerHeight * 0.45f).coerceAtMost(180.dp).coerceAtLeast(120.dp)
    val buttonSpacing = 16.dp

    Column(
        modifier = Modifier.fillMaxHeight(),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalAlignment = Alignment.CenterVertically
        ) {
            MenuButton(
                onClick = { onNavigateTo(2) },
                text = stringResource(id = R.string.map_button_text),
                iconResId = if (isDarkTheme) R.drawable.map1_icon_d else R.drawable.map1_icon_l,
                size = buttonSize
            )
            MenuButton(
                onClick = { onNavigateTo(3) },
                text = stringResource(id = R.string.traffic_button_text),
                iconResId = if (isDarkTheme) R.drawable.traffic_icon_d else R.drawable.traffic_icon_l,
                size = buttonSize
            )
        }

        Spacer(modifier = Modifier.height(buttonSpacing))

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalAlignment = Alignment.CenterVertically
        ) {
            MenuButton(
                onClick = { onNavigateTo(1) },
                text = stringResource(id = R.string.data_button_text),
                iconResId = if (isDarkTheme) R.drawable.data_icon_d else R.drawable.data_icon_l,
                size = buttonSize
            )
            MenuButton(
                onClick = { onNavigateTo(4) },
                text = stringResource(id = R.string.settings_button_text),
                iconResId = if (isDarkTheme) R.drawable.settings_icon_d else R.drawable.settings_icon_l,
                size = buttonSize
            )
        }
    }
}

@Composable
fun PortraitButtonsGrid(
    onNavigateTo: (Int) -> Unit,
    isDarkTheme: Boolean,
    containerWidth: Dp
) {
    val buttonSize = (containerWidth * 0.42f).coerceAtMost(200.dp).coerceAtLeast(140.dp)
    val buttonSpacing = 24.dp

    Column(
        modifier = Modifier.fillMaxWidth(),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly
        ) {
            MenuButton(
                onClick = { onNavigateTo(2) },
                text = stringResource(id = R.string.map_button_text),
                iconResId = if (isDarkTheme) R.drawable.map1_icon_d else R.drawable.map1_icon_l,
                size = buttonSize
            )
            MenuButton(
                onClick = { onNavigateTo(3) },
                text = stringResource(id = R.string.traffic_button_text),
                iconResId = if (isDarkTheme) R.drawable.traffic_icon_d else R.drawable.traffic_icon_l,
                size = buttonSize
            )
        }

        Spacer(modifier = Modifier.height(buttonSpacing))

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly
        ) {
            MenuButton(
                onClick = { onNavigateTo(1) },
                text = stringResource(id = R.string.data_button_text),
                iconResId = if (isDarkTheme) R.drawable.data_icon_d else R.drawable.data_icon_l,
                size = buttonSize
            )
            MenuButton(
                onClick = { onNavigateTo(4) },
                text = stringResource(id = R.string.settings_button_text),
                iconResId = if (isDarkTheme) R.drawable.settings_icon_d else R.drawable.settings_icon_l,
                size = buttonSize
            )
        }
    }
}

@Composable
fun MenuButton(
    onClick: () -> Unit,
    text: String,
    iconResId: Int,
    size: Dp
) {
    val isDarkTheme = isSystemInDarkTheme()
    val buttonColor = if (isDarkTheme) Color(0xCC567BFF) else Color(0xFF132C86)
    val borderColor = if (isDarkTheme) Color(0x4DF6F6F6) else Color(0xFF818EBA).copy(alpha = 0.3f)

    Box(
        modifier = Modifier
            .size(size)
            .padding(4.dp),
        contentAlignment = Alignment.Center
    ) {
        Button(
            onClick = onClick,
            modifier = Modifier.fillMaxSize(),
            shape = RoundedCornerShape(16.dp),
            colors = ButtonDefaults.buttonColors(containerColor = buttonColor),
            border = BorderStroke(2.dp, borderColor),
            contentPadding = PaddingValues(horizontal = 8.dp, vertical = 12.dp)
        ) {
            Column(
                modifier = Modifier.fillMaxSize(),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                val iconSize = when {
                    size < 140.dp -> 50.dp
                    size < 160.dp -> 60.dp
                    size < 180.dp -> 70.dp
                    else -> 80.dp
                }
                Image(
                    painter = painterResource(id = iconResId),
                    contentDescription = null,
                    modifier = Modifier.size(iconSize),
                    contentScale = ContentScale.Fit
                )

                Spacer(modifier = Modifier.height(8.dp))
                val fontSize = when {
                    size < 140.dp -> 14.sp
                    size < 160.dp -> 15.sp
                    size < 180.dp -> 16.sp
                    else -> 18.sp
                }
                Text(
                    text = text,
                    color = Color.White.copy(alpha = 0.85f),
                    fontSize = fontSize,
                    fontWeight = if (size < 140.dp) FontWeight.Normal else FontWeight.Medium,
                    textAlign = TextAlign.Center,
                    maxLines = 2,
                    lineHeight = when {
                        size < 140.dp -> 16.sp
                        size < 160.dp -> 18.sp
                        else -> 20.sp
                    }
                )
            }
        }
    }
}

@Preview(name = "Light Mode Portrait", showBackground = true, device = Devices.PIXEL_4)
@Preview(name = "Dark Mode Portrait", showBackground = true, device = Devices.PIXEL_4, uiMode = android.content.res.Configuration.UI_MODE_NIGHT_YES)
@Composable
fun NavigationScreenPreviewPortrait() {
    MaterialTheme {
        NavigationScreen(onNavigateTo = {})
    }
}

@Preview(name = "Light Mode Landscape Small", showBackground = true, device = "spec:width=640dp,height=360dp,orientation=landscape")
@Preview(name = "Light Mode Landscape Medium", showBackground = true, device = "spec:width=891dp,height=411dp,orientation=landscape")
@Preview(name = "Light Mode Landscape Large", showBackground = true, device = "spec:width=1280dp,height=800dp,orientation=landscape")
@Preview(name = "Dark Mode Landscape", showBackground = true, device = "spec:width=891dp,height=411dp,orientation=landscape", uiMode = android.content.res.Configuration.UI_MODE_NIGHT_YES)
@Composable
fun NavigationScreenPreviewLandscape() {
    MaterialTheme {
        NavigationScreen(onNavigateTo = {})
    }
}

@Preview(name = "Menu Button Small", showBackground = true)
@Preview(name = "Menu Button Medium", showBackground = true)
@Preview(name = "Menu Button Large", showBackground = true)
@Composable
fun MenuButtonPreviews() {
    MaterialTheme {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            MenuButton(
                onClick = {},
                text = "Карта",
                iconResId = R.drawable.map1_icon_l,
                size = 120.dp
            )
            MenuButton(
                onClick = {},
                text = "Трафик",
                iconResId = R.drawable.traffic_icon_l,
                size = 160.dp
            )
            MenuButton(
                onClick = {},
                text = "Настройки",
                iconResId = R.drawable.settings_icon_l,
                size = 200.dp
            )
        }
    }
}