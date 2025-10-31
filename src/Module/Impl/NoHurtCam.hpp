//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class NoHurtCam : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"EB ? 0F 57 C0 F3 0F 11 0B">();
    uintptr_t addr{};
    std::vector<std::uint8_t> backup{};
public:
    NoHurtCam() : Module("NoHurtCam", "Disables the hurt camera effect when taking damage.") {}

    void onEnable(nlohmann::json& config) override {
        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[NoHurtCam] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[NoHurtCam] sig @ 0x"
                << std::uppercase << std::hex << std::setw(sizeof(uintptr_t)*2)
                << std::setfill('0') << addr;
            Console::print(oss.str());
        }

        mem::nop(addr, 2, &backup);
    }

    void onDisable() override {
        mem::patch_bytes(addr, backup.data(), backup.size());
    }
};
