//
// Created by slq on 10/31/2025.
//

#pragma once
#include "Module.hpp"
#include "Impl/CustomBlockShape.hpp"
#include "Impl/Fullbright.hpp"
#include "Impl/GuiScale.hpp"
#include "Impl/ItemUseDelay.hpp"
#include "Impl/MinimalViewBobbing.hpp"
#include "Impl/NoHurtCam.hpp"
#include "Impl/JumpDelay.hpp"
#include "Impl/TeleportRotation.hpp"
#include "Impl/ThirdPersonNametag.hpp"

class ModuleRegistry {
private:
    inline static ModuleRegistry* instance{};
    std::vector<Module*> modules;
public:
    ModuleRegistry() {
        instance = this;

        _register(new Fullbright());
        _register(new GuiScale());
        _register(new ItemUseDelay());
        _register(new JumpDelay());
        _register(new MinimalViewBobbing());
        _register(new NoHurtCam());
        _register(new TeleportRotation());
        _register(new ThirdPersonNametag());

        _register(new CustomBlockShape());
    }

    inline static ModuleRegistry* getInstance() {
        return instance;
    }

    std::vector<Module*> getModules() {
        return modules;
    }

    Module* getModule(std::string name) {
        for (auto module : modules) {
            if(module->getName() == name) {
                return module;
            }
        }

        return nullptr;
    }

    void _register(Module* module) {
        modules.push_back(module);
    }
};
