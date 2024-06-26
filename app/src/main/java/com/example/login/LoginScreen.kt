package com.example.login

import android.app.Activity
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.material.Button
import androidx.compose.material.Checkbox
import androidx.compose.material.OutlinedTextField
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch

@RequiresApi(Build.VERSION_CODES.O)
@Composable
fun LoginScreen(
    state: MainActivity.MainActivityState,
    onLoginSuccess: () -> Unit
) {
    val context = LocalContext.current
    var email by remember { mutableStateOf(state.Email) }
    var password by remember { mutableStateOf(state.Password) }
    var jwtToken by remember { mutableStateOf(state.JwtToken) }
    var rememberMe by remember { mutableStateOf(state.RememberMe) }
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
                onEmailChange = { email = it },
                password = password,
                onPasswordChange = { password = it },
                onRegisterClick = {
                    coroutineScope.launch {
                        MainActivity.networkManager.registerUser(email, password) { response ->
                            if (response != null) {
                                jwtToken = response.jwt
                                state.JwtToken = response.jwt
                                state.Uuid = response.uuid
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
                onEmailChange = { email = it },
                password = password,
                onPasswordChange = { password = it },
                jwtToken = jwtToken,
                onJwtTokenChange = { jwtToken = it },
                rememberMe = rememberMe,
                onRememberMeChange = { rememberMe = it },
                onLoginClick = {
                    state.Email = email
                    state.Password = password
                    state.JwtToken = jwtToken
                    state.RememberMe = rememberMe
                    state.saveLoginData()
                    coroutineScope.launch {
                        MainActivity.networkManager.authenticateUser(email, password, jwtToken) { authResponse ->
                            if (authResponse != null) {
                                (context as? Activity)?.runOnUiThread {
                                    state.JwtToken = authResponse.jwt
                                    state.Uuid = authResponse.uuid
                                    state.saveLoginData()
                                    onLoginSuccess()
                                }
                                MainActivity().connectWebSocket(authResponse.jwt)
                            } else {
                                Log.e(MainActivity.TAG, "Authentication failed")
                            }
                        }
                    }
                },
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
    rememberMe: Boolean,
    onRememberMeChange: (Boolean) -> Unit,
    onLoginClick: () -> Unit,
    onShowRegistrationClick: () -> Unit
) {
    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = email,
                onValueChange = onEmailChange,
                label = { Text("Email") }
            )
        }
        Spacer(modifier = Modifier.height(8.dp))
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = password,
                onValueChange = onPasswordChange,
                label = { Text("Password") },
                visualTransformation = PasswordVisualTransformation()
            )
        }
        Spacer(modifier = Modifier.height(8.dp))
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = jwtToken,
                onValueChange = onJwtTokenChange,
                label = { Text("JWT Token") },
                enabled = true
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
        Button(onClick = onLoginClick) {
            Text("Login")
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
    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = email,
                onValueChange = onEmailChange,
                label = { Text("Email") }
            )
        }
        Spacer(modifier = Modifier.height(8.dp))
        Column(modifier = Modifier.fillMaxWidth(0.8f)) {
            OutlinedTextField(
                value = password,
                onValueChange = onPasswordChange,
                label = { Text("Password") },
                visualTransformation = PasswordVisualTransformation()
            )
        }
        Spacer(modifier = Modifier.height(16.dp))
        Button(onClick = onRegisterClick) {
            Text("Register")
        }
    }
}