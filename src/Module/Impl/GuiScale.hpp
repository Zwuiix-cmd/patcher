//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class GuiScale : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"00 00 ? ? 00 00 A0 40 00 00 C0 40">();
    uintptr_t addr{};
    float oldValue{};
public:
    GuiScale() : Module("GuiScale", "Sets the GUI scale to a custom value.") {}

    void onEnable(nlohmann::json& config) override {
        float value = 2.0f;
        if(config.contains("value")) {
            auto v = config["value"].get<float>();

            if(v <= 0.0f || value > 4.0f)
                Console::print("[GuiScale] Invalid value in config, must be between 0.1 and 4.0. Using default 2.0.");
            else value = v;
        }

        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[GuiScale] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[GuiScale] sig @ 0x"
                << std::uppercase << std::hex << std::setw(sizeof(uintptr_t)*2)
                << std::setfill('0') << addr;
            Console::print(oss.str());
        }

        mem::patch_value<float>(addr, value, &oldValue);
        config["value"] = value;
    }

    void onDisable() override {

    }
};
