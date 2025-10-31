//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class ItemUseDelay : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"FF 15 ? ? ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 32 DB">();
    uintptr_t addr{};
    std::vector<std::uint8_t> backup{};
public:
    ItemUseDelay() : Module("ItemUseDelay", "Removes 200ms delay after attack on using items.") {}

    void onEnable(nlohmann::json& config) override {
        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[ItemUseDelay] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[ItemUseDelay] sig @ 0x"
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
