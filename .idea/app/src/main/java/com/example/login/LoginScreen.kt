package com.example.login

import android.content.Context
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
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
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.Button
import androidx.compose.material.ButtonDefaults
import androidx.compose.material.Checkbox
import androidx.compose.material.IconButton
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.launch
import androidx.compose.material.TextFieldDefaults
import androidx.compose.foundation.isSystemInDarkTheme

// Предварительный просмотр UI
@RequiresApi(Build.VERSION_CODES.O)
@Preview(showBackground = true)
@Composable
fun PreviewLoginScreen() {
    val state = MainActivity.MainActivityState(LocalContext.current)
    LoginScreen(
        state = state,
        onLoginSuccess = {},
        onCellInfoDataClick = {},
        onBackClick = {}
    )
}

@RequiresApi(Build.VERSION_CODES.O)
@Composable
fun LoginScreen(
    state: MainActivity.MainActivityState,
    onLoginSuccess: () -> Unit,
    onCellInfoDataClick: () -> Unit,
    onBackClick: () -> Unit
) {
    val isDarkTheme = isSystemInDarkTheme()

    val context = LocalContext.current
    var email by remember { mutableStateOf(state.Email) }
    var password by remember { mutableStateOf(state.Password) }
    var jwtToken by remember { mutableStateOf(state.JwtToken) }
    var uuid by remember { mutableStateOf(state.Uuid) }

    val sharedPreferences = LocalContext.current.getSharedPreferences(
        MainActivity.SHARED_PREFS_NAME,
        Context.MODE_PRIVATE
    )
    val savedRememberMe = sharedPreferences.getBoolean(MainActivity.REMEMBER_ME_KEY, false)

    var rememberMe by rememberSaveable { mutableStateOf(savedRememberMe) }

    var showRegistration by remember { mutableStateOf(false) }
    var showSuccessMessage by remember { mutableStateOf(false) }
    var showErrorMessage by remember { mutableStateOf(false) }
    val coroutineScope = rememberCoroutineScope()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(if (isDarkTheme) Color(0xFF1C1C1E) else Color(0xFFF5F5F5)),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Top
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.End
        ) {
            IconButton(onClick = onBackClick) {
                Image(
                    painter = painterResource(id = if (isDarkTheme) R.drawable.transition_dark_r else R.drawable.transition_light_r),
                    contentDescription = stringResource(R.string.settings_title),
                    modifier = Modifier.size(24.dp)
                )
            }
        }

        if (showRegistration) {
            RegistrationForm(
                email = email,
                onEmailChange = { email = it.replace(" ", "") },
                password = password,
                onPasswordChange = { password = it.replace(" ", "") },
                onRegisterClick = {
                    coroutineScope.launch {
                        MainActivity.networkManager.registerUser(email, password) { response ->
                            if (response != null) {
                                jwtToken = response.jwt
                                uuid = response.uuid
                                state.JwtToken = response.jwt
                                state.Uuid = response.uuid
                                state.Email = email
                                state.Password = password
                                state.RememberMe = rememberMe
                                state.saveLoginData()
                                showSuccessMessage = true
                                showRegistration = false
                            } else {
                                showErrorMessage = true
                            }
                        }
                    }
                }
            )
        } else {
            LoginForm(
                email = email,
                onEmailChange = { email = it.replace(" ", "") },
                password = password,
                onPasswordChange = { password = it.replace(" ", "") },
                jwtToken = jwtToken,
                onJwtTokenChange = { jwtToken = it.replace(" ", "") },
                uuid = uuid,
                onUuidChange = { uuid = it.replace(" ", "") },
                rememberMe = rememberMe,
                onRememberMeChange = {
                    rememberMe = it
                    with(sharedPreferences.edit()) {
                        putBoolean(MainActivity.REMEMBER_ME_KEY, it)
                        apply()
                    }
                },
                onCellInfoDataClick = onCellInfoDataClick,
                onShowRegistrationClick = { showRegistration = true }
            )
        }
        if (showSuccessMessage) {
            Text(
                text = stringResource(id = R.string.registration_success, jwtToken),
                color = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xFF34204C)
            )
        }
        if (showErrorMessage) {
            Text(
                text = stringResource(id = R.string.registration_failure),
                color = Color.Red // Red error color for both themes
            )
        }
    }
}

@Composable
fun RegistrationForm(
    email: String,
    onEmailChange: (String) -> Unit,
    password: String,
    onPasswordChange: (String) -> Unit,
    onRegisterClick: () -> Unit,
) {
    val focusManager = LocalFocusManager.current
    val isDarkTheme = isSystemInDarkTheme()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(top = 100.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Image(
            painter = painterResource(id = if (isDarkTheme) R.drawable.lg else R.drawable.logo_reg),
            contentDescription = stringResource(id = R.string.app_logo_desc),
            modifier = Modifier
                .height(100.dp)
                .fillMaxWidth(),
            contentScale = ContentScale.Fit
        )

        Spacer(modifier = Modifier.height(16.dp))

        Text(
            text = stringResource(id = R.string.registration_title),
            style = TextStyle(
                color = if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xCC34204C),
                fontSize = 18.sp,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Center
            )
        )

        Spacer(modifier = Modifier.height(26.dp))

        Text(
            text = stringResource(id = R.string.email_label),
            style = TextStyle(
                color = if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xFF929292),
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium,
                textAlign = TextAlign.Start
            ),
            modifier = Modifier.fillMaxWidth(0.8f)
        )

        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            BasicTextField(
                value = email,
                onValueChange = onEmailChange,
                keyboardOptions = KeyboardOptions(
                    keyboardType = KeyboardType.Email,
                    imeAction = ImeAction.Next
                ),
                keyboardActions = KeyboardActions(
                    onNext = { focusManager.moveFocus(androidx.compose.ui.focus.FocusDirection.Down) }
                ),
                textStyle = TextStyle(color = if (isDarkTheme) Color(0xFFFFFFFF) else Color(0xFF787878)
                ),
                decorationBox = { innerTextField ->
                    Column {
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(start = 16.dp, end = 16.dp, top = 8.dp, bottom = 8.dp)
                        ) {
                            innerTextField()
                        }
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .height(1.5.dp)
                                .background(if (isDarkTheme) Color(0xB3FFFFFF) else Color(0x809E9E9E))
                        )
                    }
                }
            )
        }

        Spacer(modifier = Modifier.height(20.dp))

        Text(
            text = stringResource(id = R.string.password_label),
            style = TextStyle(
                color = if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xFF929292),
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium,
                textAlign = TextAlign.Start
            ),
            modifier = Modifier.fillMaxWidth(0.8f)
        )

        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            BasicTextField(
                value = password,
                onValueChange = onPasswordChange,
                visualTransformation = PasswordVisualTransformation(),
                keyboardOptions = KeyboardOptions(
                    keyboardType = KeyboardType.Password,
                    imeAction = ImeAction.Done
                ),
                keyboardActions = KeyboardActions(
                    onDone = {
                        focusManager.clearFocus()
                        if (email.isNotBlank() && password.isNotBlank()) {
                            onRegisterClick()
                        }
                    }
                ),
                textStyle = TextStyle(color = if (isDarkTheme) Color(0xFFFFFFFF) else Color(0xFF787878)),
                decorationBox = { innerTextField ->
                    Column {
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(start = 16.dp, end = 16.dp, top = 8.dp, bottom = 8.dp)
                        ) {
                            innerTextField()
                        }
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .height(1.5.dp)
                                .background(if (isDarkTheme) Color(0xB3FFFFFF) else Color(0x809E9E9E))
                        )
                    }
                }
            )
        }

        Spacer(modifier = Modifier.height(20.dp))

        Button(
            onClick = onRegisterClick,
            colors = ButtonDefaults.buttonColors(backgroundColor = if (isDarkTheme) Color(0xCC567BFF) else Color(0xFF132C86)),
            shape = RoundedCornerShape(15.dp)
        ) {
            Text(
                text = stringResource(id = R.string.register),
                color = Color.White
            )
        }

        Spacer(modifier = Modifier.height(15.dp))

        Text(
            text = stringResource(id = R.string.or),
            style = TextStyle(
                color = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xFF34204C),
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium,
                textAlign = TextAlign.Center
            )
        )

        Spacer(modifier = Modifier.height(15.dp))

        Text(
            text = stringResource(id = R.string.go_to),
            style = TextStyle(
                color = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xFF34204C),
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium,
            )
        )
    }
}

@Composable
fun LoginForm(
    email: String,
    onEmailChange: (String) -> Unit,
    password: String,
    onPasswordChange: (String) -> Unit,
    jwtToken: String,
    onJwtTokenChange: (String) -> Unit,
    uuid: String,
    onUuidChange: (String) -> Unit,
    rememberMe: Boolean,
    onRememberMeChange: (Boolean) -> Unit,
    onCellInfoDataClick: () -> Unit,
    onShowRegistrationClick: () -> Unit
) {
    val focusManager = LocalFocusManager.current
    val isDarkTheme = isSystemInDarkTheme()

    val headerTextStyle = TextStyle(
        color =  if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xFF929292),
        fontSize = 16.sp,
        fontWeight = FontWeight.Medium,
        textAlign = TextAlign.Start
    )

    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(top = 10.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Image(
                painter = painterResource(id = if (isDarkTheme) R.drawable.lg else R.drawable.logo_reg),
                contentDescription = stringResource(id = R.string.app_logo_desc),
                modifier = Modifier
                    .height(100.dp)
                    .fillMaxWidth(),
                contentScale = ContentScale.Fit
            )

            Spacer(modifier = Modifier.height(16.dp))

            Text(
                text = stringResource(id = R.string.registration_title),
                style = TextStyle(
                    color = if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xCC34204C),
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold,
                    textAlign = TextAlign.Center
                )
            )

            Spacer(modifier = Modifier.height(24.dp))

            Text(
                text = stringResource(id = R.string.email_label),
                style = headerTextStyle,
                modifier = Modifier.fillMaxWidth(0.8f)
            )

            Column(modifier = Modifier.fillMaxWidth(0.8f)) {
                BasicTextField(
                    value = email,
                    onValueChange = onEmailChange,
                    keyboardOptions = KeyboardOptions(
                        keyboardType = KeyboardType.Email,
                        imeAction = ImeAction.Next
                    ),
                    keyboardActions = KeyboardActions(
                        onNext = { focusManager.moveFocus(androidx.compose.ui.focus.FocusDirection.Down) }
                    ),
                    textStyle = TextStyle(color = if (isDarkTheme) Color(0xFFFFFFFF) else Color(0xFF787878)),
                    decorationBox = { innerTextField ->
                        Column {
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(start = 16.dp, end = 16.dp, top = 8.dp, bottom = 8.dp)
                            ) {
                                innerTextField()
                            }
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .height(1.5.dp)
                                    .background(if (isDarkTheme) Color(0xB3FFFFFF) else Color(0x809E9E9E))
                            )
                        }
                    }
                )
            }

            Spacer(modifier = Modifier.height(20.dp))

            Text(
                text = stringResource(id = R.string.password_label),
                style = headerTextStyle,
                modifier = Modifier.fillMaxWidth(0.8f)
            )

            Column(modifier = Modifier.fillMaxWidth(0.8f)) {
                BasicTextField(
                    value = password,
                    onValueChange = onPasswordChange,
                    visualTransformation = PasswordVisualTransformation(),
                    keyboardOptions = KeyboardOptions(
                        keyboardType = KeyboardType.Password,
                        imeAction = ImeAction.Done
                    ),
                    keyboardActions = KeyboardActions(
                        onDone = {
                            focusManager.clearFocus()
                            if (email.isNotBlank() && password.isNotBlank()) {
                                onCellInfoDataClick()
                            }
                        }
                    ),
                    textStyle = TextStyle(color = if (isDarkTheme) Color(0xFFFFFFFF) else Color(0xFF787878)),
                    decorationBox = { innerTextField ->
                        Column {
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(start = 16.dp, end = 16.dp, top = 8.dp, bottom = 8.dp)
                            ) {
                                innerTextField()
                            }
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .height(1.5.dp)
                                    .background(if (isDarkTheme) Color(0xB3FFFFFF) else Color(0x809E9E9E))
                            )
                        }
                    }
                )
            }

            Spacer(modifier = Modifier.height(8.dp))

            Column(modifier = Modifier.fillMaxWidth(0.8f)) {
                OutlinedTextField(
                    value = uuid,
                    onValueChange = onUuidChange,
                    label = {
                        Text(
                        text = stringResource(id = R.string.uuid_label),
                        style = headerTextStyle
                    ) },
                    keyboardOptions = KeyboardOptions(
                        keyboardType = KeyboardType.Text,
                        imeAction = ImeAction.Next
                    ),
                    keyboardActions = KeyboardActions(
                        onNext = { focusManager.moveFocus(androidx.compose.ui.focus.FocusDirection.Down) }
                    ),
                    colors = TextFieldDefaults.outlinedTextFieldColors(
                        textColor = if (isDarkTheme) Color(0xFFFFFFFF) else Color(0xFF787878),
                        focusedBorderColor = if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xFF9E9E9E),
                        unfocusedBorderColor = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xFF9E9E9E)
                    )
                )
            }

            Spacer(modifier = Modifier.height(8.dp))

            Column(modifier = Modifier.fillMaxWidth(0.8f)) {
                OutlinedTextField(
                    value = jwtToken,
                    onValueChange = onJwtTokenChange,
                    label = {
                        Text(
                        text = stringResource(id = R.string.jwt_token_label),
                        style = headerTextStyle
                    ) },
                    keyboardOptions = KeyboardOptions(
                        keyboardType = KeyboardType.Text,
                        imeAction = ImeAction.Done
                    ),
                    keyboardActions = KeyboardActions(
                        onDone = {
                            focusManager.clearFocus()
                            onCellInfoDataClick()
                        }
                    ),
                    colors = TextFieldDefaults.outlinedTextFieldColors(
                        textColor = if (isDarkTheme) Color(0xFFFFFFFF) else Color(0xFF787878),
                        focusedBorderColor = if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xFF34204C),
                        unfocusedBorderColor = if (isDarkTheme) Color(0xB3FFFFFF) else Color(0xFF9E9E9E)
                    )
                )
            }

            Spacer(modifier = Modifier.height(8.dp))

            Row(verticalAlignment = Alignment.CenterVertically) {
                Checkbox(
                    checked = rememberMe,
                    onCheckedChange = onRememberMeChange
                )
                Text(
                    text = stringResource(id = R.string.remember_me),
                    color = if (isDarkTheme) Color(0xD9FFFFFF) else Color(0xFF34204C)
                )
            }

            Spacer(modifier = Modifier.height(16.dp))

            Button(
                onClick = onCellInfoDataClick,
                colors = ButtonDefaults.buttonColors(backgroundColor = if (isDarkTheme) Color(0xCC567BFF) else Color(0xFF132C86)),
                shape = RoundedCornerShape(15.dp)
            ) {
                Text(
                    text = stringResource(id = R.string.send_cell_info),
                    color = Color.White
                )
            }

            Spacer(modifier = Modifier.height(8.dp))

            Button(
                onClick = onShowRegistrationClick,
                colors = ButtonDefaults.buttonColors(backgroundColor = if (isDarkTheme) Color(0xCC567BFF) else Color(0xFF132C86)),
                shape = RoundedCornerShape(15.dp)
            ) {
                Text(
                    text = stringResource(id = R.string.register),
                    color = Color.White
                )
            }
        }
    }
}
