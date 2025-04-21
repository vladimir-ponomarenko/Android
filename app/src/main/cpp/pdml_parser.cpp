#include "pdml_parser.h"
#include "pugixml/pugixml.hpp"
#include "utils.h"
#include <functional>
#include <android/log.h>
#include <string>

json parse_xml_node_recursive(pugi::xml_node xml_current_node) {

    std::string node_type_str = xml_current_node.name();


    if (node_type_str != "field" && node_type_str != "proto") {
        return nullptr;
    }


    if (node_type_str == "field") {
        const char* hide_attr = xml_current_node.attribute("hide").value();
        if (hide_attr && strcmp(hide_attr, "yes") == 0) {
            return nullptr;
        }
    }


    pugi::xml_node first_child = xml_current_node.first_child();
    bool has_children = first_child;


    bool is_list_container = false;
    if (has_children && node_type_str == "field") {
        pugi::xml_node first_field_child = xml_current_node.child("field");
        if (first_field_child) {
            const char* first_child_name = first_field_child.attribute("name").value();
            const char* first_child_showname = first_field_child.attribute("showname").value();
            if ((!first_child_name || strlen(first_child_name) == 0) &&
                first_child_showname && strncmp(first_child_showname, "Item ", 5) == 0)
            {
                is_list_container = true;
            }
        }

        const char* showname_val = xml_current_node.attribute("showname").value();
        if (!is_list_container && showname_val && strstr(showname_val, " items")) {

            is_list_container = true;

            if (strstr(showname_val, " 0 items")) {
                has_children = false;
            }
        }
    }


    json current_json_node;
    if (is_list_container) {
        current_json_node = json::array();
    } else if (has_children || node_type_str == "proto") {
        current_json_node = json::object();
    }



    if (has_children) {
        for (pugi::xml_node child_node = first_child; child_node; child_node = child_node.next_sibling()) {
            json child_json = parse_xml_node_recursive(child_node);

            if (!child_json.is_null()) {
                if (is_list_container) {

                    current_json_node.push_back(child_json);
                } else if (current_json_node.is_object()){

                    std::string child_key;
                    const char* child_name_cstr = child_node.attribute("name").value();
                    const char* child_showname_cstr = child_node.attribute("showname").value();

                    if (child_name_cstr && strlen(child_name_cstr) > 0) {
                        child_key = child_name_cstr;

                        size_t dot_pos = child_key.find('.');
                        if (dot_pos != std::string::npos) {

                        }
                    } else if (child_showname_cstr && strlen(child_showname_cstr) > 0) {

                        child_key = child_showname_cstr;
                        size_t colon_pos = child_key.find(':');
                        if (colon_pos != std::string::npos) {
                            child_key = child_key.substr(0, colon_pos);
                        }

                        while (!child_key.empty() && std::isspace(child_key.back())) {
                            child_key.pop_back();
                        }
                    } else {
                        child_key = "_unknown_child_" + std::to_string(current_json_node.size());
                    }


                    if (current_json_node.find(child_key) != current_json_node.end()) {
                        if (!current_json_node[child_key].is_array()) {
                            json existing_value = current_json_node[child_key];
                            current_json_node[child_key] = json::array({existing_value});
                        }
                        current_json_node[child_key].push_back(child_json);
                    } else {
                        current_json_node[child_key] = child_json;
                    }
                }
            }
        }
    }



    if (!is_list_container) {
        bool get_value_from_attributes = false;
        if (current_json_node.is_object() && current_json_node.empty()) {
            get_value_from_attributes = true;
        } else if (!has_children && node_type_str == "field") {
            get_value_from_attributes = true;
        }

        if (get_value_from_attributes) {
            const char* show_val = xml_current_node.attribute("show").value();
            const char* val_val = xml_current_node.attribute("value").value();
            json value_from_attr = nullptr;

            if (show_val && strlen(show_val) > 0) {
                std::string s_val = show_val;
                if (s_val == "true" || s_val == "True") value_from_attr = true;
                else if (s_val == "false" || s_val == "False") value_from_attr = false;
                else {
                    try {
                        size_t processed_chars = 0;
                        long long ll_val = std::stoll(s_val, &processed_chars);
                        if (processed_chars == s_val.length()) {
                            if (ll_val >= INT_MIN && ll_val <= INT_MAX) value_from_attr = static_cast<int>(ll_val);
                            else value_from_attr = ll_val;
                        } else {
                            double d_val = std::stod(s_val, &processed_chars);
                            if (processed_chars == s_val.length()) value_from_attr = d_val;
                            else value_from_attr = s_val;
                        }
                    } catch (...) { value_from_attr = s_val; }
                }
            } else if (val_val && strlen(val_val) > 0) {
                value_from_attr = val_val;
            }

            if (!value_from_attr.is_null()) {
                current_json_node = value_from_attr;
            }



            else if (!has_children && node_type_str == "field") {
                current_json_node = nullptr;
            }
        }
    }

    // Если после всего узел остался пустым объектом (например, протокол без полей),
    // то можно вернуть null, чтобы не засорять JSON.
    // if (current_json_node.is_object() && current_json_node.empty()) {
    //     return nullptr;
    // }

    return current_json_node;
}



json parse_pdml(const std::string& pdml_string, const std::string& target_protocol) {
    json result_json = json::object();
    pugi::xml_document doc;
    pugi::xml_parse_result parse_result = doc.load_string(pdml_string.c_str());

    if (!parse_result) {
        LOGD("PDML Parsing Failed: %s (Offset: %ld)", parse_result.description(), parse_result.offset);
        result_json["parsing_error"] = parse_result.description();
        return result_json;
    }

    pugi::xml_node packet_node = doc.child("packet");
    if (!packet_node) {
        result_json["parsing_error"] = "No <packet> root element found";
        return result_json;
    }

    pugi::xml_node target_start_node;

    for (pugi::xml_node proto_node = packet_node.child("proto");
         proto_node;
         proto_node = proto_node.next_sibling("proto"))
    {
        const char* proto_name = proto_node.attribute("name").value();
        if (!proto_name) continue;
        if (!target_protocol.empty()) {
            if (target_protocol == proto_name) { target_start_node = proto_node; break; }
            if (strcmp(proto_name, "fake-field-wrapper") == 0 && !target_start_node) { target_start_node = proto_node;}
        } else {
            if (strcmp(proto_name, "frame") != 0 && strcmp(proto_name, "user_dlt") != 0 && strcmp(proto_name, "aww") != 0) {
                target_start_node = proto_node; break;
            }
        }
    }

    if (target_start_node) {

        result_json = parse_xml_node_recursive(target_start_node);
    } else {
        LOGD("Target protocol node ('%s' or fallback) not found in PDML", target_protocol.c_str());
        result_json["parsing_error"] = "Target protocol node not found";
    }

    return result_json;
}