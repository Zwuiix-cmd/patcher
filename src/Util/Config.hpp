//
// Created by slq on 10/31/2025.
//

#pragma once
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <string_view>

namespace cfg {
    inline std::filesystem::path roaming_appdata() {
        if (const char* app = std::getenv("APPDATA"); app && *app)
            return std::filesystem::path(app);

        PWSTR p = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &p))) {
            std::filesystem::path out = p;
            CoTaskMemFree(p);
            return out;
        }
        return std::filesystem::path(".");
    }

    inline std::filesystem::path bedrock_mods_config_path(std::string_view filename = "patcherconf.json") {
        return roaming_appdata() / "Minecraft Bedrock" / "mods" / std::string(filename);
    }

    inline std::filesystem::path path(const std::filesystem::path& custom = {}) {
        if (!custom.empty()) return custom;

        if (const char* ov = std::getenv("PATCHER_CONFIG"); ov && *ov) {
            return std::filesystem::path(ov);
        }

        const char* app = std::getenv("APPDATA");
        std::filesystem::path base = app ? app : ".";
        return base / "Patcher" / "patcherconf.json";
    }

    inline void ensure_dir(const std::filesystem::path& file) {
        std::filesystem::create_directories(file.parent_path());
    }

    inline nlohmann::json load_or_create(const std::filesystem::path& file,
                                         const nlohmann::json& defaults = nlohmann::json::object()) {
        ensure_dir(file);
        if (std::filesystem::exists(file)) {
            std::ifstream is(file);
            nlohmann::json j;
            try { is >> j; } catch (...) { j = nlohmann::json::object(); }
            return j.is_object() ? j : nlohmann::json::object();
        } else {
            nlohmann::json j = defaults.is_object() ? defaults : nlohmann::json::object();
            std::ofstream os(file);
            os << std::setw(2) << j;
            return j;
        }
    }

    inline void save(const std::filesystem::path& file, const nlohmann::json& j) {
        ensure_dir(file);
        std::ofstream os(file);
        os << std::setw(2) << j;
    }

    inline bool is_enabled(const nlohmann::json& root, std::string_view module) {
        auto it = root.find(std::string(module));
        if (it == root.end() || !it->is_object()) return false;
        return it->value("enabled", false);
    }

    template<typename T>
    inline T get_value(const nlohmann::json& root, std::string_view module,
                       std::string_view key, const T& def) {
        auto it = root.find(std::string(module));
        if (it == root.end() || !it->is_object()) return def;
        return it->value(std::string(key), def);
    }

    inline void ensure_module(nlohmann::json& root, std::string_view module,
                              const nlohmann::json& module_defaults = nlohmann::json::object()) {
        std::string name(module);
        if (!root.contains(name) || !root[name].is_object()) {
            root[name] = module_defaults.is_object() ? module_defaults : nlohmann::json::object();
        }
    }

} // namespace cfg
