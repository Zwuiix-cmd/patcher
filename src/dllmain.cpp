//
// Created by slq on 10/31/2025.
//
#include "pch.hpp"

static const std::filesystem::path confPath = cfg::bedrock_mods_config_path();

BOOL WINAPI DllMain(HMODULE, DWORD reason, LPVOID)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        {
#if DEV_MODE
            Console::init(L"patcher.dll");
#endif
            Console::print("Patcher DLL injected successfully.");

            MH_Initialize();
            Console::print("MinHook initialized.");

            auto registry = new ModuleRegistry();
            auto modules = registry->getModules();
            Console::print("Loaded " + std::to_string(modules.size()) + " modules.");

            nlohmann::json defaults = nlohmann::json::object();
            for (auto* m : modules) {
                defaults[m->getName()] = { { "enabled", false } };
            }

            nlohmann::json conf = cfg::load_or_create(confPath, defaults);

            for (auto* m : modules) {
                cfg::ensure_module(conf, m->getName(), { { "enabled", false }, {"description", m->getDescription() } });
            }

            for (auto* module : modules) {
                const std::string& name = module->getName();
                auto& modConf = conf[name];

                if (!modConf.value("enabled", false)) {
                    continue;
                }

                module->setEnabled(true);
                module->onEnable(modConf);
            }

            cfg::save(confPath, conf);
            Console::print("Config loaded & saved at: " + confPath.string());

            break;
        }

        case DLL_PROCESS_DETACH:
        {
            Console::print("Patcher DLL detaching...");

            if (auto* registry = ModuleRegistry::getInstance()) {
                for (auto* module : registry->getModules()) {
                    if(module->isEnabled()) {
                        module->setEnabled(false);
                        module->onDisable();
                    }
                }
            }

            MH_Uninitialize();
            Console::print("MinHook uninitialized.");

#if DEV_MODE
            Console::shutdown();
#endif
            break;
        }

        default: break;
    }

    return TRUE;
}
