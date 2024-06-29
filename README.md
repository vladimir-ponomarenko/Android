# Приложение сбора и отправки данных о сети

Это приложение для Android собирает подробные данные о мобильной сети, анализирует их и отправляет на сервер. Приложение отображает информацию о силе сигнала на карте, предоставляет детальную статистику использования трафика приложениями и строит почасовые диаграммы. Для работы приложению требуются разрешения на доступ к файлам, местоположению и чтению состояния телефона.

## Функциональность

##### Мониторинг параметров сети:
* Получение данных о местоположении устройства (широта и долгота).
* Сбор параметров сети, таких как RSRP, RSSI, RSRQ, RSSNR, CQI, полоса пропускания, идентификатор ячейки и другие (список получаемых параметров пополняется по мере разработки).
##### Визуализация параметров сети:
* Отображение на карте точек с цветовой индикацией силы сигнала (RSRP), позволяющей визуально оценить качество связи в разных локациях.
* Диаграммы с параметрами сети.
##### Анализ использования трафика:
* Подсчет общего объема трафика, использованного всеми приложениями.
* Отображение детальной статистики по использованию трафика каждым приложением с сортировкой по убыванию.
* Построение почасовых диаграмм за последние 24 часа, демонстрирующих динамику использования трафика каждым приложением.
##### Отправка данных на сервер:
* Возможность отправить собранные данные на сервер для дальнейшего исследования.
## Как запустить

1. Убедитесь, что ваше устройство работает под управлением операционной системы Android.
2. Скачайте и установите приложение на устройство.
3. Запустите приложение.
4. Предоставьте необходимые разрешения (доступ к файлам, местоположению и чтению состояния телефона).
5. После предоставления разрешений, приложение начнет сбор данных и автоматически будет отправлять их на сервер.

## Требования

- Операционная система Android.
- Разрешения на доступ к файлам, местоположению и чтению состояния телефона.

## Структура проекта

 * ***[MainActivity.kt](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/MainActivity.kt)***: Основной класс приложения, отвечающий за запуск приложения, обработку разрешений и управление основным экраном.
 *  ***[DataScreen.kt](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/DataScreen.kt)***: Вкладка с отображением собранных данных о сети.
 * ***[LoginScreen.kt:](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/LoginScreen.kt)*** Вкладка с аутентификацией пользователя на сервере.
 * ***[MapScreen.kt:](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/MapScreen.kt)*** Вкладка с отображением карты с точками, отражающими параметры сети.
 * ***[RSRPGraph.kt:](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/RSRPGraph.kt)*** Вкладка с отображением диаграмм собранных данных о сети.
 * ***[TrafficScreen.kt:](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/TrafficScreen.kt)*** Вкладка с отображением информации об использованном трафике установленными приложениями.
 * ***[DataClasses.kt:](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/DataClasses.kt)*** Файл с классами данных, используемыми для хранения и передачи информации
 * ***[DataManager.kt](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/DataManager.kt)***: Класс для получения данных о местоположении, параметрах сети и генерации JSON для отправки на сервер.
 * ***[NetworkManager.kt](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/NetworkManager.kt)***: Класс для взаимодействия с сервером, включая регистрацию пользователя, аутентификацию и отправку данных.
 * ***[TrafficManager.kt:](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/TrafficManager.kt)*** Класс, отвечающий за сбор и обработку данных об использованном трафике установленными приложениями.
 * ***[Utils.kt:](https://github.com/vladimir-ponomarenko/Android/blob/main/app/src/main/java/com/example/login/Utils.kt)*** Класс с вспомогательными функциями для запроса и  проверки разрешений.
 

> Требования для сборки проекта:
> Gradle 8.4;
 Android Studio Iguana;
 minSdk = 24;
 targetSdk = 34.

