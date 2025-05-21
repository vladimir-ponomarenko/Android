package com.example.login

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.util.Log
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.Button
import androidx.compose.material.ButtonDefaults
import androidx.compose.material.Checkbox
import androidx.compose.material.CheckboxDefaults
import androidx.compose.material.Text
import androidx.compose.material.TextField
import androidx.compose.material.TextFieldDefaults
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.filled.ArrowDropUp
import androidx.compose.material.icons.filled.Clear
import androidx.compose.material.icons.filled.FolderOpen
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.filled.Security
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.rememberUpdatedState
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.launch

@Composable
fun RootModeTopPanel(onNavigateBack: () -> Unit) {
    val isDarkTheme = isSystemInDarkTheme()
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(56.dp)
            .background(if (isDarkTheme) Color(0xFF2C2C2E) else Color(0xFFF8F8F8))
            .border(
                width = 2.dp,
                color = if (isDarkTheme) Color(0x4D9E9E9E) else Color(0x809E9E9E),
                shape = RoundedCornerShape(0.dp)
            )
    ) {
        Row(
            modifier = Modifier.fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            IconButton(onClick = onNavigateBack) {
                Image(
                    painter = painterResource(id = if (isDarkTheme) R.drawable.transition_dark else R.drawable.transition_light),
                    contentDescription = stringResource(R.string.settings_title),
                    modifier = Modifier
                        .padding(start = 16.dp)
                        .size(24.dp)
                )
            }
            Spacer(modifier = Modifier.width(16.dp))
            Text(
                text = stringResource(R.string.root_mode_title),
                color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold
            )
        }
    }
}

data class MessageTypeItem(
    val name: String,
    val id: Int,
    var isSelected: Boolean,
    val uniqueKeyInPrefs: String
)

@RequiresApi(Build.VERSION_CODES.O)
@Composable
fun RootModeScreen(
    state: MainActivity.MainActivityState,
    onNavigateBack: () -> Unit
) {
    val context = LocalContext.current
    val dirPrefs = remember { context.getSharedPreferences("RootModePrefs", Context.MODE_PRIVATE) }
    val msgTypePrefs = remember { context.getSharedPreferences("RootMsgTypesPrefs", Context.MODE_PRIVATE) }

    var selectedDirectoryUri by remember {
        mutableStateOf<Uri?>(
            dirPrefs.getString("selected_log_dir_uri", null)?.let { Uri.parse(it) }
        )
    }

    val coroutineScope = rememberCoroutineScope()

    val rootManager = remember(context, selectedDirectoryUri) {
        Log.d("RootModeScreen", "Re-creating RootManager with URI: $selectedDirectoryUri")
        RootManager(context) { selectedDirectoryUri }
    }

    var isRootModeEnabled by remember { mutableStateOf(rootManager.isRootModeEnabled()) }
    val isDarkTheme = isSystemInDarkTheme()

    var allMessageTypes by remember { mutableStateOf<List<MessageTypeItem>>(emptyList()) }
    var messageTypesListExpanded by remember { mutableStateOf(false) }
    var searchText by remember { mutableStateOf("") }

    val focusManager = LocalFocusManager.current
    val focusRequester = remember { FocusRequester() }

    LaunchedEffect(key1 = rootManager) {
        Log.d("RootModeScreen", "LaunchedEffect for MessageTypes: Loading known message types.")
        val knownTypesFromDRC = rootManager.DRC.getKnownMessageTypes()
        val loadedTypes = knownTypesFromDRC.mapIndexedNotNull { index, item ->
            if (item is Array<*> && item.size >= 2 && item[0] is String && item[1] is Int) {
                val name = item[0] as String
                val id = item[1] as Int
                val prefsAndLazyColumnKey = "${id}_$name"
                val isSelected = msgTypePrefs.getBoolean(prefsAndLazyColumnKey, false)
                MessageTypeItem(name, id, isSelected, prefsAndLazyColumnKey)
            } else {
                Log.w("RootModeScreen", "Invalid item format in getKnownMessageTypes: $item at index $index")
                null
            }
        }.sortedBy { it.name }
        allMessageTypes = loadedTypes
        Log.d("RootModeScreen", "Loaded ${allMessageTypes.size} message types.")
    }

    val saveSelectedMessageTypes = remember {
        {
            Log.d("RootModeScreen", "Saving selected message types to SharedPreferences.")
            val editor = msgTypePrefs.edit()
            allMessageTypes.forEach { item ->
                editor.putBoolean(item.uniqueKeyInPrefs, item.isSelected)
            }
            editor.apply()
        }
    }

    LaunchedEffect(key1 = rootManager) {
        Log.d("RootModeScreen", "LaunchedEffect for rootManager state: Updating isRootModeEnabled from RootManager")
        isRootModeEnabled = rootManager.isRootModeEnabled()
    }

    LaunchedEffect(Unit) {
        while (true) {
            kotlinx.coroutines.delay(1000)
            val actualRootState = rootManager.isRootModeEnabled()
            if (isRootModeEnabled != actualRootState) {
                Log.d("RootModeScreen", "Background Sync: Root mode UI state ($isRootModeEnabled -> $actualRootState)")
                isRootModeEnabled = actualRootState
            }
        }
    }

    val directoryPickerLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.OpenDocumentTree()
    ) { uri: Uri? ->
        if (uri != null) {
            val contentResolver = context.contentResolver
            val takeFlags: Int = Intent.FLAG_GRANT_READ_URI_PERMISSION or
                    Intent.FLAG_GRANT_WRITE_URI_PERMISSION
            try {
                contentResolver.takePersistableUriPermission(uri, takeFlags)
                selectedDirectoryUri = uri
                dirPrefs.edit().putString("selected_log_dir_uri", uri.toString()).apply()
                Log.i("RootModeScreen", "Directory selected: $uri")

                if (isRootModeEnabled) {
                    Log.d("RootModeScreen", "Root mode is enabled, restarting to apply new log path.")
                    coroutineScope.launch {
                        rootManager.setRootModeEnabled(false)
                        kotlinx.coroutines.delay(500)
                        rootManager.setRootModeEnabled(true)
                        isRootModeEnabled = rootManager.isRootModeEnabled()
                    }
                }
            } catch (e: SecurityException) {
                Log.e("RootModeScreen", "Failed to take persistable URI permission", e)
            }
        } else {
            Log.w("RootModeScreen", "Directory selection cancelled.")
        }
    }

    val onRootModeToggle: (Boolean) -> Unit = remember(rootManager) {
        { enabled ->
            Log.d("RootModeScreen", "Switch toggled to $enabled")
            saveSelectedMessageTypes()
            rootManager.setRootModeEnabled(enabled)
            isRootModeEnabled = rootManager.isRootModeEnabled()
        }
    }


    val filteredMessageTypes = remember(allMessageTypes, searchText) {
        if (searchText.isBlank()) {
            allMessageTypes
        } else {
            allMessageTypes.filter {
                it.name.contains(searchText, ignoreCase = true)
            }
        }
    }

    Surface(
        modifier = Modifier.fillMaxSize(),
        color = if (isDarkTheme) Color(0xFF1C1C1E) else Color(0xFFFFFFFF)
    ) {
        Column(modifier = Modifier.fillMaxSize()) {
            RootModeTopPanel(onNavigateBack = onNavigateBack)
            LazyColumn(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(16.dp)
            ) {
                item {
                    SettingItemWithSwitch(
                        label = stringResource(R.string.enable_root_mode),
                        isChecked = isRootModeEnabled,
                        onCheckedChange = onRootModeToggle,
                        iconVector = Icons.Filled.Security,
                        iconTint = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                    )
                    Spacer(modifier = Modifier.height(16.dp))
                    Text(
                        text = stringResource(R.string.root_features_description),
                        color = if (isDarkTheme) Color(0x99FFFFFF) else Color(0xCC34204C),
                        fontSize = 14.sp
                    )
                    Spacer(modifier = Modifier.height(24.dp))
                }

                item {
                    Text(
                        text = stringResource(R.string.log_directory_path_label),
                        color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                        fontWeight = FontWeight.Medium
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        text = selectedDirectoryUri?.path ?: stringResource(R.string.default_log_path_internal_app),
                        color = if (isDarkTheme) Color(0x99FFFFFF) else Color(0x9934204C),
                        fontSize = 12.sp,
                        modifier = Modifier.padding(bottom = 8.dp)
                    )
                    Button(
                        onClick = {
                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                                try { directoryPickerLauncher.launch(null) }
                                catch (e: Exception) { Log.e("RootModeScreen", "Error launching directory picker", e) }
                            } else { Log.w("RootModeScreen", "SAF directory picker not available on this API level.") }
                        },
                        modifier = Modifier.fillMaxWidth(),
                        colors = ButtonDefaults.buttonColors(
                            backgroundColor = if (isDarkTheme) Color(0xFF3C3C3E) else Color(0xFFE0E0E0)
                        )
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Icon(
                                imageVector = Icons.Filled.FolderOpen,
                                contentDescription = stringResource(R.string.select_directory_button),
                                tint = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                            )
                            Spacer(modifier = Modifier.width(8.dp))
                            Text(
                                text = stringResource(R.string.select_directory_button),
                                color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                            )
                        }
                    }
                    Spacer(modifier = Modifier.height(24.dp))
                }

                item {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable { messageTypesListExpanded = !messageTypesListExpanded }
                            .padding(vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        Text(
                            text = stringResource(R.string.diag_message_types_title),
                            color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
                            fontWeight = FontWeight.Medium
                        )
                        Icon(
                            imageVector = if (messageTypesListExpanded) Icons.Filled.ArrowDropUp else Icons.Filled.ArrowDropDown,
                            contentDescription = if (messageTypesListExpanded) stringResource(R.string.collapse_list) else stringResource(R.string.expand_list),
                            tint = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                        )
                    }
                }

                item {
                    AnimatedVisibility(visible = messageTypesListExpanded) {
                        Column {
                            TextField(
                                value = searchText,
                                onValueChange = { searchText = it },
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(bottom = 8.dp)
                                    .focusRequester(focusRequester),
                                label = { Text(stringResource(R.string.search_message_types)) },
                                leadingIcon = {
                                    Icon(
                                        Icons.Filled.Search,
                                        contentDescription = stringResource(R.string.search_icon_desc)
                                    )
                                },
                                trailingIcon = {
                                    if (searchText.isNotEmpty()) {
                                        IconButton(onClick = { searchText = "" }) {
                                            Icon(
                                                Icons.Filled.Clear,
                                                contentDescription = stringResource(R.string.clear_search_desc)
                                            )
                                        }
                                    }
                                },
                                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Search),
                                keyboardActions = KeyboardActions(onSearch = {
                                    focusManager.clearFocus()
                                }),
                                singleLine = true,
                                colors = TextFieldDefaults.textFieldColors(
                                    textColor = if (isDarkTheme) Color.White else Color.Black,
                                    cursorColor = if (isDarkTheme) Color(0xFFBB86FC) else Color(0xFF6200EE),
                                    focusedIndicatorColor = if (isDarkTheme) Color(0xFFBB86FC) else Color(0xFF6200EE),
                                    unfocusedIndicatorColor = if (isDarkTheme) Color.Gray else Color.DarkGray,
                                    backgroundColor = if (isDarkTheme) Color(0xFF2C2C2E) else Color(0xFFF0F0F0),
                                    leadingIconColor = if (isDarkTheme) Color.LightGray else Color.Gray,
                                    trailingIconColor = if (isDarkTheme) Color.LightGray else Color.Gray,
                                    focusedLabelColor = if (isDarkTheme) Color(0xFFBB86FC) else Color(0xFF6200EE),
                                    unfocusedLabelColor = if (isDarkTheme) Color.Gray else Color.DarkGray
                                )
                            )

                            if (allMessageTypes.isNotEmpty()) {
                                Button(
                                    onClick = {
                                        val allCurrentlyDisplayedSelected = if (searchText.isBlank()) {
                                            allMessageTypes.all { it.isSelected }
                                        } else {
                                            filteredMessageTypes.isNotEmpty() && filteredMessageTypes.all { it.isSelected }
                                        }
                                        val newSelectionState = !allCurrentlyDisplayedSelected

                                        allMessageTypes = allMessageTypes.map { item ->
                                            if (searchText.isBlank() || item.name.contains(searchText, ignoreCase = true)) {
                                                item.copy(isSelected = newSelectionState)
                                            } else {
                                                item
                                            }
                                        }
                                        saveSelectedMessageTypes()
                                    },
                                    modifier = Modifier
                                        .fillMaxWidth()
                                        .padding(vertical = 8.dp),
                                    colors = ButtonDefaults.buttonColors(
                                        backgroundColor = if (isDarkTheme) Color(0xFF3C3C3E) else Color(0xFFE0E0E0)
                                    )
                                ) {
                                    val currentListToCheck = if (searchText.isBlank()) allMessageTypes else filteredMessageTypes
                                    Text(
                                        text = if (currentListToCheck.isNotEmpty() && currentListToCheck.all { it.isSelected }) stringResource(R.string.deselect_all_messages)
                                        else stringResource(R.string.select_all_messages),
                                        color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C)
                                    )
                                }
                            }

                            if (filteredMessageTypes.isNotEmpty()) {
                                Box(modifier = Modifier.heightIn(max = 300.dp)) {
                                    LazyColumn {
                                        items(items = filteredMessageTypes, key = { it.uniqueKeyInPrefs }) { item ->
                                            MessageTypeRow(
                                                item = item,
                                                onCheckedChange = { newItemState ->

                                                    allMessageTypes = allMessageTypes.map {
                                                        if (it.uniqueKeyInPrefs == newItemState.uniqueKeyInPrefs) newItemState else it
                                                    }
                                                    saveSelectedMessageTypes()
                                                },
                                                isDarkTheme = isDarkTheme
                                            )
                                        }
                                    }
                                }
                            } else if (allMessageTypes.isNotEmpty() && searchText.isNotEmpty()){
                                Text(
                                    text = stringResource(R.string.no_search_results),
                                    color = if (isDarkTheme) Color(0x99FFFFFF) else Color(0x9934204C),
                                    modifier = Modifier.padding(vertical = 8.dp).align(Alignment.CenterHorizontally)
                                )
                            } else if (allMessageTypes.isEmpty()) {
                                Text(
                                    text = stringResource(R.string.no_diag_message_types_available),
                                    color = if (isDarkTheme) Color(0x99FFFFFF) else Color(0x9934204C),
                                    modifier = Modifier.padding(vertical = 8.dp).align(Alignment.CenterHorizontally)
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun MessageTypeRow(
    item: MessageTypeItem,
    onCheckedChange: (MessageTypeItem) -> Unit,
    isDarkTheme: Boolean
) {
    val rememberedOnCheckedChange = rememberUpdatedState(onCheckedChange)

    Row(
        modifier = Modifier
            .fillMaxWidth()
            .clickable { rememberedOnCheckedChange.value(item.copy(isSelected = !item.isSelected)) }
            .padding(vertical = 4.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Checkbox(
            checked = item.isSelected,
            onCheckedChange = { isChecked ->
                rememberedOnCheckedChange.value(item.copy(isSelected = isChecked))
            },
            colors = CheckboxDefaults.colors(
                checkedColor = if (isDarkTheme) Color(0xFFBB86FC) else Color(0xFF6200EE),
                uncheckedColor = if (isDarkTheme) Color(0x99FFFFFF) else Color(0x9934204C),
                checkmarkColor = if (isDarkTheme) Color.Black else Color.White
            )
        )
        Spacer(modifier = Modifier.width(8.dp))
        Text(
            text = item.name,
            color = if (isDarkTheme) Color(0xCCFFFFFF) else Color(0xFF34204C),
            fontSize = 14.sp
        )
    }
}


@RequiresApi(Build.VERSION_CODES.O)
@Preview(showBackground = true, name = "Light Mode Root Screen")
@Preview(showBackground = true, name = "Dark Mode Root Screen", uiMode = android.content.res.Configuration.UI_MODE_NIGHT_YES)
@Composable
fun PreviewRootModeScreen() {
    val mockContext = LocalContext.current
    val mockMainActivityState = remember {
        MainActivity.MainActivityState(mockContext)
    }
    Surface {
        RootModeScreen(
            state = mockMainActivityState,
            onNavigateBack = { Log.d("Preview", "Navigate Back clicked") }
        )
    }
}