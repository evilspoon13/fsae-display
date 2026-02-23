#ifndef FSAE_CONFIG_PARSER_HPP
#define FSAE_CONFIG_PARSER_HPP

#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include "config_types.hpp"

inline constexpr const char* DEFAULT_CONFIG_PATH = "/tmp/display.json";

inline SignalType parse_signal_type(std::string_view s) {
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

inline FrameMap load_can_config(const std::string& path) {
    FrameMap result;

    std::ifstream f(path);
    if (!f.is_open()) return result;

    auto j = nlohmann::json::parse(f);

    for (const auto& [id_str, frame] : j["frames"].items()) {

      std::printf("Processing frame with ID: %s\n", id_str.c_str());
        uint32_t id = std::stoul(id_str, nullptr, 16);
        for (auto& sig : frame["signals"]) {
            ChannelConfig cfg;
            cfg.start_byte = sig["start_byte"];
            cfg.length =  sig["length"];
            cfg.type = parse_signal_type(sig["type"].get<std::string>());
            cfg.scale = sig["scale"];
            cfg.offset = sig["offset"];
            result[id].emplace_back(cfg);
        }
      }
    return result;
}

inline std::vector<WidgetConfig> load_display_config(const std::string& path) {
    std::vector<WidgetConfig> result;
    // TODO
    return result;
}

#endif
