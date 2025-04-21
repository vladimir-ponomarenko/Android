//
// Created by Vladimir Ponomarenko on 19.04.2025.
//

#pragma once

#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * @brief Парсит строку PDML (XML) от Wireshark и извлекает данные полей в JSON.
 *
 * @param pdml_string Строка, содержащая PDML вывод.
 * @param target_protocol Имя основного протокола для извлечения (например, "lte-rrc", "nas-eps").
 *                        Если пустая строка, парсит все протоколы.
 * @return json Объект nlohmann::json с извлеченными полями.
 *              В случае ошибки парсинга содержит поле "parsing_error".
 */
json parse_pdml(const std::string& pdml_string, const std::string& target_protocol = "");
