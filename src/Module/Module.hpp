//
// Created by slq on 10/31/2025.
//

#pragma once
#include <string>
#include <utility>
#include <nlohmann/json.hpp>
#include <libhat/Signature.hpp>
#include <libhat/Scanner.hpp>
#include "src/Util/Mem.hpp"
#include <src/Util/Console.hpp>
#include "src/Util/Detour.hpp"
#include "src/Util/SigScan.hpp"

class Module {
private:
    std::string name;
    std::string description;

    bool enabled = false;
public:
    Module(std::string name, std::string description) : name(std::move(name)), description(std::move(description)) {}
    virtual ~Module() = default;

    [[nodiscard]] std::string getName() const {
        return name;
    }

    [[nodiscard]] std::string getDescription() const {
        return description;
    }

    bool isEnabled() const {
        return enabled;
    }

    void setEnabled(const bool v = true) {
        enabled = v;
    }

    virtual void onEnable(nlohmann::json& config) {};
    virtual void onDisable() {};
};
