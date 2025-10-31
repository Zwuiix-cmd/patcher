//
// Created by slq on 10/31/2025.
//

#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <windows.h>
#include <psapi.h>
#include <libhat/Scanner.hpp>

#include "PE.hpp"

struct SigHit {
    const std::byte* ptr{};
    std::string module;
    std::string section;
};

inline const std::byte* scan_range(const std::byte* begin, const std::byte* end, hat::signature_view view) {
    if (!begin || !end || end <= begin) return nullptr;
    auto r = hat::find_pattern(begin, end, view);
    return r.get();
}

inline SigHit find_in_module(hat::signature_view view, HMODULE mod, const char* modName = nullptr) {
    SigHit hit{};
    if (!mod) return hit;

    auto secs = pe::enumerate_sections(mod);
    for (const auto& s : secs) {
        const auto* b = reinterpret_cast<const std::byte*>(s.vaBegin);
        const auto* e = reinterpret_cast<const std::byte*>(s.vaEnd);
        if (auto p = scan_range(b, e, view)) {
            hit.ptr = p;
            hit.module  = modName ? modName : "0x" + std::to_string(reinterpret_cast<std::uintptr_t>(mod));
            hit.section = s.name;
            return hit;
        }
    }
    return hit;
}

inline std::vector<HMODULE> enum_modules() {
    std::vector<HMODULE> mods;
    DWORD needed = 0;
    EnumProcessModulesEx(GetCurrentProcess(), nullptr, 0, &needed, LIST_MODULES_ALL);
    if (!needed) return mods;
    mods.resize(needed / sizeof(HMODULE));
    if (!EnumProcessModulesEx(GetCurrentProcess(), mods.data(), needed, &needed, LIST_MODULES_ALL)) {
        mods.clear();
    } else {
        mods.resize(needed / sizeof(HMODULE));
    }
    return mods;
}

inline SigHit find_in_main_module(hat::signature_view view) {
    return find_in_module(view, GetModuleHandleW(nullptr), "main");
}

inline SigHit find_in_all_modules(hat::signature_view view) {
    for (HMODULE m : enum_modules()) {
        char name[MAX_PATH]{};
        GetModuleBaseNameA(GetCurrentProcess(), m, name, MAX_PATH);
        if (auto hit = find_in_module(view, m, name); hit.ptr) return hit;
    }
    return {};
}

inline SigHit find_anywhere(hat::signature_view view) {
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    auto begin = reinterpret_cast<std::uintptr_t>(si.lpMinimumApplicationAddress);
    auto end   = reinterpret_cast<std::uintptr_t>(si.lpMaximumApplicationAddress);

    for (std::uintptr_t a = begin; a < end; ) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(a), &mbi, sizeof(mbi))) break;

        bool committed = (mbi.State & MEM_COMMIT) != 0;
        bool readable  = (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) != 0;
        bool guarded   = (mbi.Protect & PAGE_GUARD) != 0;

        if (committed && readable && !guarded) {
            const auto* b = reinterpret_cast<const std::byte*>(mbi.BaseAddress);
            const auto* e = b + mbi.RegionSize;
            if (auto p = scan_range(b, e, view)) {
                SigHit hit{};
                hit.ptr    = p;
                hit.module = "<committed region>";
                hit.section= "";
                return hit;
            }
        }
        a += mbi.RegionSize;
    }
    return {};
}

inline SigHit find_pattern_any_section(hat::signature_view view) {
    if (auto h = find_in_main_module(view); h.ptr) return h;
    if (auto h = find_in_all_modules(view); h.ptr) return h;
    return find_anywhere(view);
}

template <class CompiledSig>
inline SigHit find_pattern_any_section(const CompiledSig& sig) {
    return find_pattern_any_section(hat::signature_view{sig});
}
