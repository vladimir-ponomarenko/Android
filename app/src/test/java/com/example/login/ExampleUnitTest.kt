//package com.example.login
//
//import android.content.Context
//import android.os.Build
//import androidx.compose.runtime.Composable
//import androidx.compose.ui.graphics.Color
//import androidx.compose.ui.test.assertIsDisplayed
//import androidx.compose.ui.test.junit4.createAndroidComposeRule
//import androidx.compose.ui.test.onNodeWithText
//import androidx.test.core.app.ApplicationProvider
//import androidx.test.ext.junit.runners.AndroidJUnit4
//import androidx.test.filters.SdkSuppress
//import org.junit.Assert.assertEquals
//import org.junit.Rule
//import org.junit.Test
//import org.junit.runner.RunWith
//import org.robolectric.annotation.Config
//import java.time.Instant
//import java.time.format.DateTimeFormatter
//
//// Интерфейсы для зависимостей
//interface TrafficDataProvider {
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    fun getHourlyTrafficData(context: Context, packageName: String): List<Pair<Int, Long>>
//}
//
//interface AppInfoProvider {
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    fun getPackageNameForApp(context: Context, appName: String): String?
//}
//
//// Фейковые реализации зависимостей
//class FakeTrafficDataProvider : TrafficDataProvider {
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    override fun getHourlyTrafficData(context: Context, packageName: String): List<Pair<Int, Long>> {
//        // Возвращаем фейковые данные для тестов
//        return listOf(Pair(0, 1024L), Pair(1, 2048L), Pair(2, 0L))
//    }
//}
//
//class FakeAppInfoProvider : AppInfoProvider {
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    override fun getPackageNameForApp(context: Context, appName: String): String? {
//        // Возвращаем фейковое имя пакета для тестов
//        return "com.example.test.app"
//    }
//}
//
//// Запускаем тесты на API 29
//@Config(sdk = [Build.VERSION_CODES.Q])
//@RunWith(AndroidJUnit4::class)
//class ExampleInstrumentedTest {
//
//    @get:Rule
//    val composeTestRule = createAndroidComposeRule<MainActivity>() // Используем createAndroidComposeRule
//
//    private val context = ApplicationProvider.getApplicationContext<Context>()
//
//    // Юнит-тесты пользовательского интерфейса
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Test
//    fun testLoginScreenElements() {
//        composeTestRule.setContent {
//            MainActivity().LoginScreen(MainActivity.MainActivityState(context))
//        }
//
//        composeTestRule.onNodeWithText("Email").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Password").assertIsDisplayed()
//        composeTestRule.onNodeWithText("JWT Token").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Remember me").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Login").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Register").assertIsDisplayed()
//    }
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Test
//    fun testDataScreenElements() {
//        composeTestRule.setContent {
//            MainActivity().DataScreen(MainActivity.MainActivityState(context))
//        }
//
//        composeTestRule.onNodeWithText("RSRP value:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Rssi value:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Rsrq value:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Rssnr value:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Cqi value:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Bandwidth:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Cell ID:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("LAT:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("LON:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("MCC:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("MNC:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("LAC:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("TAC:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("PCI:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("EARFCN:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("CI:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Network Type:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Signal Strength:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Bit Error Rate:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Timing Advance:").assertIsDisplayed()
//    }
//
//    // Проверка функциональности кнопок и других элементов управления
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Test
//    fun testTrafficScreenElements() {
//        composeTestRule.setContent {
//            TestTrafficScreen() // Запускаем тестовый Composable
//        }
//
//        // Проверяем отображение элементов на экране "Traffic"
//        composeTestRule.onNodeWithText("Total Traffic:").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Days").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Grant Usage Stats Permission").assertIsDisplayed() // Кнопка для предоставления разрешения
//    }
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Test
//    fun testTrafficItemElements() {
//        val appData = MainActivity.AppTrafficData(
//            appName = "Test App",
//            totalBytes = 1024 * 1024, // 1 MB
//            mobileBytes = 512 * 1024, // 512 KB
//            wifiBytes = 512 * 1024, // 512 KB
//            rxBytes = 768 * 1024, // 768 KB
//            txBytes = 256 * 1024  // 256 KB
//        )
//
//        composeTestRule.setContent {
//            MainActivity().TrafficItem(appData) { } // Пустая лямбда для onShowChart
//        }
//
//        // Проверяем отображение элементов в TrafficItem
//        composeTestRule.onNodeWithText("Test App").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Total: 1024 Kb").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Mobile: 512 Kb").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Wi-Fi: 512 Kb").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Downlink: 768 Kb").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Uplink: 256 Kb").assertIsDisplayed()
//        composeTestRule.onNodeWithText("Show Chart").assertIsDisplayed()
//    }
//
//    // Юнит-тесты логики приложения
//
//    // Проверка правильности вычислений и логических операций
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Test
//    fun testGenerateColorFromRSRP() {
//        val mainActivity = MainActivity()
//        assertEquals(Color.Red, mainActivity.generateColorFromRSRP(-70))
//        assertEquals(Color.Blue, mainActivity.generateColorFromRSRP(-85))
//        assertEquals(Color.Cyan, mainActivity.generateColorFromRSRP(-100))
//    }
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Test
//    fun testGetTotalTrafficData() {
//        val mainActivity = MainActivity()
//        val totalTrafficData = mainActivity.getTotalTrafficData(context, 1)
//
//        // Проверяем, что значения не отрицательные
//        assert(totalTrafficData.totalBytes >= 0)
//        assert(totalTrafficData.mobileBytes >= 0)
//        assert(totalTrafficData.wifiBytes >= 0)
//    }
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Test
//    fun testGetHourlyTrafficData_CorrectHourCalculation() {
//        composeTestRule.setContent {
//            TestHourlyTrafficChart() // Запускаем тестовый Composable
//        }
//        composeTestRule.onNodeWithText("Hourly Traffic for Test App").assertIsDisplayed() // Проверить, что заголовок диаграммы отображается
//
//        composeTestRule.onNodeWithText("0:00").assertIsDisplayed()
//
//    }
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.O)
//    @Test
//    fun testGenerateJSON() {
//        val state = MainActivity.MainActivityState(context).apply {
//            JwtToken = "testJwt"
//            Uuid = "testUuid"
//            Latitude = "55.0"
//            Longtitude = "83.0"
//            Rsrp = "-75 dBm"
//            Rssi = "-60 dBm"
//            Rsrq = "-10 dB"
//            Rssnr = "15 dB"
//            Cqi = "10"
//            Bandwidth = "20"
//            Cellid = "12345"
//        }
//
//        val expectedJson = """
//            {"jwt":"testJwt","uuid":"testUuid","time":"${
//            String.format(
//                DateTimeFormatter.ISO_INSTANT.format(
//                    Instant.now()
//                )
//            )
//        }","latitude":55.0,"Longitude":83.0,"rsrp":-75,"rssi":-60,"rsrq":-10,"rssnr":15,"cqi":10,"bandwidth":20,"cellID":12345}
//        """.trimIndent()
//        val actualJson = MainActivity().generateJSON(state)
//
//        assertEquals(expectedJson, actualJson)
//    }
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Composable
//    fun TestTrafficScreen(trafficDataProvider: TrafficDataProvider = FakeTrafficDataProvider()) {
//        val state = MainActivity.MainActivityState(context)
//        MainActivity().TrafficScreen(state, trafficDataProvider) // Передаем фейковый провайдер
//    }
//
//    @SdkSuppress(minSdkVersion = Build.VERSION_CODES.M)
//    @Composable
//    fun TestHourlyTrafficChart(
//        appName: String = "Test App",
//        appInfoProvider: AppInfoProvider = FakeAppInfoProvider(),
//        trafficDataProvider: TrafficDataProvider = FakeTrafficDataProvider()
//    ) {
//        MainActivity().HourlyTrafficChart(appName, {}, context, appInfoProvider, trafficDataProvider)
//    }
//}