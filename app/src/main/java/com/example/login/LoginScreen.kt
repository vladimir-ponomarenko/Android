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
            Text("Registration successful! Your JWT token is: $jwtToken")
        }
        if (showErrorMessage) {
            Text("Registration failed. Please try again.")
        }
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

    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = email,
                onValueChange = onEmailChange,
                label = { Text("Email") },
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
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = password,
                onValueChange = onPasswordChange,
                label = { Text("Password") },
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
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = uuid,
                onValueChange = onUuidChange,
                label = { Text("UUID") },
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
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = jwtToken,
                onValueChange = onJwtTokenChange,
                label = { Text("JWT Token") },
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
        Row(verticalAlignment = Alignment.CenterVertically) {
            Checkbox(
                checked = rememberMe,
                onCheckedChange = onRememberMeChange
            )
            Text("Remember me")
        }
        Spacer(modifier = Modifier.height(16.dp))
        Button(onClick = onCellInfoDataClick) {
            Text("Send CellInfoData")
        }
        Spacer(modifier = Modifier.height(8.dp))
        Button(onClick = onShowRegistrationClick) {
            Text("Register")
        }
    }
}

@Composable
fun RegistrationForm(
    email: String,
    onEmailChange: (String) -> Unit,
    password: String,
    onPasswordChange: (String) -> Unit,
    onRegisterClick: () -> Unit
) {
    val focusManager = LocalFocusManager.current

    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = email,
                onValueChange = onEmailChange,
                label = { Text("Email") },
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
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = password,
                onValueChange = onPasswordChange,
                label = { Text("Password") },
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
                )
            )
        }
        Spacer(modifier = Modifier.height(16.dp))
        Button(onClick = onRegisterClick) {
            Text("Register")
        }
    }
}