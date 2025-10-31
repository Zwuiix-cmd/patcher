//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class CustomBlockShape : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"75 ? 48 8B 45 ? 48 8B 48 ? 48 81 C1">();
    uintptr_t addr{};
    std::vector<std::uint8_t> backup{};
public:
    CustomBlockShape() : Module("CustomBlockShape", "Allows custom block shapes to be used.") {}

    void onEnable(nlohmann::json& config) override {
        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[CustomBlockShape] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[CustomBlockShape] sig @ 0x"
                << std::uppercase << std::hex << std::setw(sizeof(uintptr_t)*2)
                << std::setfill('0') << addr;
            Console::print(oss.str());
        }

        mem::patch_bytes(addr, {0xEB}, &backup); // Change JNE to JMP
    }

    void onDisable() override {
        mem::patch_bytes(addr, backup.data(), backup.size());
    }
};
