#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include "config_parser.hpp"
#include "config_types.hpp"

static WidgetType parse_widget_type(std::string_view s) {
    if (s == "gauge")     return WidgetType::Gauge;
    if (s == "bar")       return WidgetType::Bar;
    if (s == "number")    return WidgetType::Number;
    if (s == "indicator") return WidgetType::Indicator;
    throw std::invalid_argument("unknown widget type: " + std::string(s));
}

static DataUnit parse_data_unit(std::string_view s) {
    if (s == "temperature") return DataUnit::Temperature;
    if (s == "pressure")    return DataUnit::Pressure;
    if (s == "rpm")         return DataUnit::RPM;
    throw std::invalid_argument("unknown data unit: " + std::string(s));
}

static SignalType parse_signal_type(std::string_view s) {
    if (s == "uint8")  return SignalType::UINT8;
    if (s == "int8")   return SignalType::INT8;
    if (s == "uint16") return SignalType::UINT16;
    if (s == "int16")  return SignalType::INT16;
    if (s == "uint32") return SignalType::UINT32;
    if (s == "int32")  return SignalType::INT32;
    if (s == "float")  return SignalType::FLOAT;
    if (s == "double") return SignalType::DOUBLE;
    throw std::invalid_argument("unknown signal type: " + std::string(s));
}

static nlohmann::json parse_json_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return nullptr;
    return nlohmann::json::parse(f);
}


FrameMap load_can_config(const std::string& path) {
    FrameMap result;
    auto j = parse_json_file(path);

    for (const auto& [id_str, frame] : j["frames"].items()) {
        uint32_t id = std::stoul(id_str, nullptr, 16);
        for (auto& sig : frame["signals"]) {
            ChannelConfig cfg;
            cfg.start_byte  = sig["start_byte"];
            cfg.length      =  sig["length"];
            cfg.type        = parse_signal_type(sig["type"].get<std::string>());
            cfg.scale       = sig["scale"];
            cfg.offset      = sig["offset"];

            result[id].emplace_back(cfg);
        }
      }
    return result;
}

DisplayConfig load_display_config(const std::string& path) {
    DisplayConfig result;
    auto j = parse_json_file(path);
    if (j.is_null()) return result;

    for (const auto& screen : j["screens"]) {
        ScreenConfig scr;
        scr.name = screen["name"];

        for (const auto& w : screen["widgets"]) {
            WidgetConfig cfg;
            cfg.type  = parse_widget_type(w["type"].get<std::string>());
            cfg.alarm = w["alarm"];

            const auto& pos = w["position"];
            cfg.position = { pos["x"], pos["y"], pos["width"], pos["height"] };

            const auto& d = w["data"];
            cfg.data.can_id              = d["can_id"];
            cfg.data.can_id_label        = d["can_id_label"];
            cfg.data.signal              = d["signal"];
            cfg.data.unit                = parse_data_unit(d["unit"].get<std::string>());
            cfg.data.min                 = d["min"];
            cfg.data.max                 = d["max"];
            cfg.data.caution_threshold   = d["caution_threshold"];
            cfg.data.critical_threshold  = d["critical_threshold"];

            scr.widgets.emplace_back(cfg);
        }

        result.screens.emplace_back(scr);
    }

    return result;
}