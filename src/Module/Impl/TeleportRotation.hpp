//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class TeleportRotation : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"E8 ? ? ? ? 48 8B 03 48 8D 94 24 ? ? ? ? 48 8B 4B">();
    uintptr_t addr{};
    std::array<uint8_t, 5> backup{};
public:
    TeleportRotation() : Module("TeleportRotation", "Removes rotation interpolation when teleporting.") {}

    void onEnable(nlohmann::json& config) override {
        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[TeleportRotation] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[TeleportRotation] sig @ 0x"
                << std::uppercase << std::hex << std::setw(sizeof(uintptr_t)*2)
                << std::setfill('0') << addr;
            Console::print(oss.str());
        }

        mem::nop_call5(addr, &backup);
    }

    void onDisable() override {
        mem::patch_bytes(addr, backup.data(), backup.size());
    }
};
