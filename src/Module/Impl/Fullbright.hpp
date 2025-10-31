//
// Created by slq on 10/31/2025.
//

#pragma once
#include "src/Module/Module.hpp"

class Fullbright : public Module {
private:
    static constexpr auto sig = hat::compile_signature<"48 83 EC ? 48 8B 01 48 8D 54 24 ? 41 B8 ? ? ? ? 48 8B 40 ? FF 15 ? ? ? ? 48 8B 10 48 8B 42 ? 48 8B 88 ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 83 C4 ? C3 F3 0F 10 42 ? 48 83 C4 ? C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 ? 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 48 8B 01 8B FA 41 B8 ? ? ? ? 48 8D 54 24 ? 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 48 8B 18 48 8B 43 ? 48 83 B8 ? ? ? ? ? 75 ? 39 7B ? 74 ? 8B D7 48 8B CB E8 ? ? ? ? 48 8B 4B ? 48 8B D3 89 43 ? E8 ? ? ? ? 48 8B 43 ? 48 8B 88 ? ? ? ? 48 85 C9 74 ? C6 44 24 ? ? 48 8D 54 24 ? 48 8B 01 48 8B 40 ? FF 15 ? ? ? ? 48 8B 4C 24 ? 48 33 CC E8 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24">();
    uintptr_t addr{};
    std::unique_ptr<detour> d{};
public:
    Fullbright() : Module("Fullbright", "Removes darkness from the game.") {}

    void onEnable(nlohmann::json& config) override {
        SigHit hit = find_pattern_any_section(sig);
        if (!hit.ptr) {
            Console::print("[Fullbright] Signature not found.");
            return;
        }

        addr = reinterpret_cast<uintptr_t>(hit.ptr);
        {
            std::ostringstream oss;
            oss << "[Fullbright] sig @ 0x"
                << std::uppercase << std::hex << std::setw(sizeof(uintptr_t)*2)
                << std::setfill('0') << addr;
            Console::print(oss.str());
        }

        d = std::make_unique<detour>(addr, &Fullbright::hook);
        d->enable();
    }

    void onDisable() override {
        d->disable();
        d.reset();
    }

    static float hook() {
        return 100;
    }
};
