//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class ThirdPersonNametag : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"0F 84 ? ? ? ? 49 8B 45 ? 49 8B CD 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 0F 85">();
    uintptr_t addr{};
    std::vector<std::uint8_t> backup{};
public:
    ThirdPersonNametag() : Module("ThirdPersonNametag", "Shows nametags in third person view.") {}

    void onEnable(nlohmann::json& config) override {
        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[ThirdPersonNametag] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[ThirdPersonNametag] sig @ 0x"
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
