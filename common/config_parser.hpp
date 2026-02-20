#ifndef FSAE_CONFIG_PARSER_HPP
#define FSAE_CONFIG_PARSER_HPP

#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include "config_types.hpp"

/*

JSON structure:

{
  "frames": {
    "0x100": {
      "name": "BMS",
      "signals": [
        {
          "name": "state_of_charge",
          "start_byte": 0,
          "length": 2,
          "type": "uint16",
          "scale": 0.1,
          "offset": 0
        },
        {
          "name": "tolage",
          "start_byte": 2,
          "length": 2,
          "type": "int16",
          "scale": 0.01,
          "offset": -40
        }
      ]
    }
  }
}

*/



inline constexpr const char* DEFAULT_CONFIG_PATH = "/tmp/display.json";

inline FrameMap load_can_config(const std::string& path) {
    FrameMap result;

    std::ifstream f(path);
    if (!f.is_open()) return result;

    auto j = nlohmann::json::parse(f);

    for (auto& [id_str, frame] : j["frames"].items()) {

      std::printf("Processing frame with ID: %s\n", id_str.c_str());
        // uint32_t id = std::stoul(id_str, nullptr, 16);
        // for (auto& sig : frame["signals"]) {
        //     ChannelConfig cfg;
        //     cfg.can_id = id;
        //     cfg.start_byte = sig["start_byte"];
        //     cfg.length = sig["length"];
        //     cfg.type = sig["type"];
        //     cfg.scale = sig["scale"];
        //     cfg.offset = sig["offset"];
        //     result[id].push_back(cfg);
        // }
      }
    return result;
}

inline std::vector<WidgetConfig> load_display_config(const std::string& path) {
    std::vector<WidgetConfig> result;
    // TODO
    return result;
}

#endif
