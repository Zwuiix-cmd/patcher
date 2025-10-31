//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class MinimalViewBobbing : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"FF 15 ? ? ? ? 80 7C 24 ? ? 0F 84 ? ? ? ? F3 0F 10 4C 24 ? F3 0F 59 0D">();
    uintptr_t addr{};
    std::vector<std::uint8_t> backup{};
public:
    MinimalViewBobbing() : Module("MinimalViewBobbing", "Reduces view bobbing to a minimum.") {}

    void onEnable(nlohmann::json& config) override {
        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[MinimalViewBobbing] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[MinimalViewBobbing] sig @ 0x"
                << std::uppercase << std::hex << std::setw(sizeof(uintptr_t)*2)
                << std::setfill('0') << addr;
            Console::print(oss.str());
        }

        mem::nop(addr, 6, &backup);
    }

    void onDisable() override {
        mem::patch_bytes(addr, backup.data(), backup.size());
    }
};
