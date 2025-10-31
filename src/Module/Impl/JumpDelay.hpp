//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class JumpDelay : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"C7 47 ? ? ? ? ? 48 8B 9C 24 ? ? ? ? 0F 28 74 24 ? 48 81 C4">();
    uintptr_t addr{};
    uint32_t oldVal{};
public:
    JumpDelay() : Module("JumpDelay", "Edits the jump delay.") {}

    void onEnable(nlohmann::json& config) override {
        uint32_t value = 0;
        if(config.contains("value")) {
            auto v = config["value"].get<uint32_t>();
            if(v > 10)
                Console::print("[JumpDelay] Invalid value in config, must be between 0 and 10. Using default 0.");
            else value = v;
        }

        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[JumpDelay] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr) + 3;
        {
            std::ostringstream oss;
            oss << "[JumpDelay] sig @ 0x"
                << std::uppercase << std::hex << std::setw(sizeof(uintptr_t)*2)
                << std::setfill('0') << addr;
            Console::print(oss.str());
        }

        mem::patch_value<uint32_t>(addr, value, &oldVal);
        config["value"] = value;
    }

    void onDisable() override {
        mem::patch_value<uint32_t>(addr, oldVal);
    }
};
