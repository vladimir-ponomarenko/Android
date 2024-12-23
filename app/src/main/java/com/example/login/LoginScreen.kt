package com.example.login

import android.content.Context
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.Button
import androidx.compose.material.Checkbox
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
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch
import androidx.compose.ui.res.stringResource
import androidx.compose.foundation.Image
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.layout.ContentScale
import androidx.compose.foundation.layout.*
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.sp
import androidx.compose.ui.text.TextStyle
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.ButtonDefaults
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.background



@RequiresApi(Build.VERSION_CODES.O)
@Composable
fun LoginScreen(
    state: MainActivity.MainActivityState,
    onLoginSuccess: () -> Unit,
    onCellInfoDataClick: () -> Unit
) {
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
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        if (showRegistration) {
            // Показываем форму регистрации
            RegistrationForm(
                email = email,
                onEmailChange = { email = it.replace(" ", "") },
                password = password,
                onPasswordChange = { password = it.replace(" ", "") },
                onRegisterClick = {
                    coroutineScope.launch {
                        MainActivity.networkManager.registerUser(email, password) { response ->
                            if (response != null) {
                                // Обновляем данные и показываем сообщение об успешной регистрации
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
                                // Показываем сообщение об ошибке
                                showErrorMessage = true
                            }
                        }
                    }
                }
            )
        } else {
            // Показываем форму входа
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
            Text(text = stringResource(id = R.string.registration_success, jwtToken), color = Color(0xCC34204C))
        }
        if (showErrorMessage) {
            Text(text = stringResource(id = R.string.registration_failure), color = Color(0xCC34204C))
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

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(top = 142.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // Логотип
        Image(
            painter = painterResource(id = R.drawable.logo_reg),
            contentDescription = stringResource(id = R.string.app_logo_desc),
            modifier = Modifier
                .height(100.dp)
                .fillMaxWidth(),
            contentScale = ContentScale.Fit
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Заголовок
        Text(
            text = stringResource(id = R.string.registration_title),
            style = TextStyle(
                color = Color(0xCC34204C),
                fontSize = 18.sp,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Center
            )
        )

        Spacer(modifier = Modifier.height(24.dp))

        // Надпись для поля Email
        Text(
            text = "Почта",
            style = TextStyle(
                color = Color(0xFF9E9E9E),
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium,
                textAlign = TextAlign.Start
            ),
            modifier = Modifier.fillMaxWidth(0.8f) // Ширина для выравнивания с полем ввода
        )

        // Поле ввода Email
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
                textStyle = TextStyle(color = Color(0xFF828282)), // Цвет текста
                decorationBox = { innerTextField ->
                    Column {
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(start = 16.dp, end = 16.dp, top = 8.dp, bottom = 8.dp)
                        ) {
                            innerTextField() // Само поле ввода
                        }
                        // Линия под полем с нужным размером и цветом
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .height(1.5.dp) // Толщина линии
                                .background(Color(0x4D9E9E9E)) // Цвет линии с 30% прозрачностью
                        )
                    }
                }
            )
        }

        Spacer(modifier = Modifier.height(20.dp))

        // Надпись для поля пароля
        Text(
            text = "Пароль",
            style = TextStyle(
                color = Color(0xFF9E9E9E),
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium,
                textAlign = TextAlign.Start
            ),
            modifier = Modifier.fillMaxWidth(0.8f) // Ширина для выравнивания с полем ввода
        )

        // Поле ввода пароля
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
                textStyle = TextStyle(color = Color(0xFF828282)), // Цвет текста
                decorationBox = { innerTextField ->
                    Column {
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(start = 16.dp, end = 16.dp, top = 8.dp, bottom = 8.dp)
                        ) {
                            innerTextField() // Само поле ввода
                        }
                        // Линия под полем с нужным размером и цветом
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .height(1.5.dp) // Толщина линии
                                .background(Color(0x4D9E9E9E)) // Цвет линии с 30% прозрачностью
                        )
                    }
                }
            )
        }
        Spacer(modifier = Modifier.height(20.dp))

        // Кнопка регистрации
        Button(
            onClick = onRegisterClick,
            colors = ButtonDefaults.buttonColors(backgroundColor = Color(0xCC132C86)), // Синий цвет
            shape = RoundedCornerShape(15.dp) // Закругленные края
        ) {
            Text(
                text = stringResource(id = R.string.register),
                color = Color.White // Белый текст на синей кнопке
            )
        }

        // Отступ 15 между кнопкой и текстом
        Spacer(modifier = Modifier.height(15.dp))

        // Текст "или"
        Text(
            text = "или",
            style = TextStyle(
                color = Color(0xFF34204C),
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium,
                textAlign = TextAlign.Center
            )
        )

        Spacer(modifier = Modifier.height(15.dp))

        Text(
            text = "перейдите на сайт",
            style = TextStyle(
                color = Color(0xFF34204C),
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
    onShowRegistrationClick: () -> Unit // This is the key new parameter
) {
    val focusManager = LocalFocusManager.current

    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        // Email Input
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = email,
                onValueChange = onEmailChange,
                label = { Text(text = stringResource(id = R.string.email_label)) },
                keyboardOptions = KeyboardOptions(
                    keyboardType = KeyboardType.Email,
                    imeAction = ImeAction.Next
                ),
                keyboardActions = KeyboardActions(
                    onNext = { focusManager.moveFocus(androidx.compose.ui.focus.FocusDirection.Down) }
                )
            )
        }

        Spacer(modifier = Modifier.height(8.dp))

        // Password Input
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = password,
                onValueChange = onPasswordChange,
                label = { Text(text = stringResource(id = R.string.password_label)) },
                visualTransformation = PasswordVisualTransformation(),
                keyboardOptions = KeyboardOptions(
                    keyboardType = KeyboardType.Password,
                    imeAction = ImeAction.Next
                ),
                keyboardActions = KeyboardActions(
                    onNext = { focusManager.moveFocus(androidx.compose.ui.focus.FocusDirection.Down) }
                )
            )
        }

        Spacer(modifier = Modifier.height(8.dp))

        // UUID Input
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = uuid,
                onValueChange = onUuidChange,
                label = { Text(text = stringResource(id = R.string.uuid_label)) },
                keyboardOptions = KeyboardOptions(
                    keyboardType = KeyboardType.Text,
                    imeAction = ImeAction.Next
                ),
                keyboardActions = KeyboardActions(
                    onNext = { focusManager.moveFocus(androidx.compose.ui.focus.FocusDirection.Down) }
                )
            )
        }

        Spacer(modifier = Modifier.height(8.dp))

        // JWT Token Input
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = jwtToken,
                onValueChange = onJwtTokenChange,
                label = { Text(text = stringResource(id = R.string.jwt_token_label)) },
                keyboardOptions = KeyboardOptions(
                    keyboardType = KeyboardType.Text,
                    imeAction = ImeAction.Done
                ),
                keyboardActions = KeyboardActions(
                    onDone = {
                        focusManager.clearFocus()
                        onCellInfoDataClick()
                    }
                )
            )
        }

        Spacer(modifier = Modifier.height(8.dp))

        // Remember Me Checkbox
        Row(verticalAlignment = Alignment.CenterVertically) {
            Checkbox(
                checked = rememberMe,
                onCheckedChange = onRememberMeChange
            )
            Text(text = stringResource(id = R.string.remember_me))
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Send Data Button
        Button(
            onClick = onCellInfoDataClick,
            colors = ButtonDefaults.buttonColors(backgroundColor = Color(0xCC132C86)), // Blue color
            shape = RoundedCornerShape(15.dp) // Rounded corners
        ) {
            Text(
                text = stringResource(id = R.string.send_cell_info),
                color = Color.White // White text on blue button
            )
        }

        Spacer(modifier = Modifier.height(8.dp))

        // Show Registration Button
        Button(
            onClick = onShowRegistrationClick, // Trigger registration form
            colors = ButtonDefaults.buttonColors(backgroundColor = Color(0xCC132C86)), // Blue color
            shape = RoundedCornerShape(15.dp) // Rounded corners
        ) {
            Text(
                text = stringResource(id = R.string.register),
                color = Color.White // White text on blue button
            )
        }
    }
}