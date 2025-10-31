//
// Created by slq on 10/31/2025.
//

#pragma once
#include <windows.h>
#include <winnt.h>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>

namespace pe {

    struct SectionInfo {
        std::string name;
        std::uintptr_t moduleBase{};
        std::uintptr_t vaBegin{};
        std::uintptr_t vaEnd{};
        DWORD characteristics{};
    };

    inline std::string secName(const IMAGE_SECTION_HEADER& s) {
        char buf[9]{};
        std::memcpy(buf, s.Name, 8);
        return std::string(buf);
    }

    inline std::vector<SectionInfo> enumerate_sections(HMODULE mod = GetModuleHandleA(nullptr)) {
        std::vector<SectionInfo> out;
        if (!mod) return out;

        auto base = reinterpret_cast<std::uintptr_t>(mod);
        auto dos  = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
        if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE) return out;

        auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);
        if (!nt || nt->Signature != IMAGE_NT_SIGNATURE) return out;

        auto sec = IMAGE_FIRST_SECTION(nt);
        for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
            SectionInfo si;
            si.name            = secName(*sec);
            si.moduleBase      = base;
            si.vaBegin         = base + sec->VirtualAddress;
            si.vaEnd           = si.vaBegin + sec->Misc.VirtualSize;
            si.characteristics = sec->Characteristics;
            out.push_back(si);
        }

        std::stable_sort(out.begin(), out.end(), [](const SectionInfo& a, const SectionInfo& b){
            bool ax = (a.characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
            bool bx = (b.characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
            if (ax != bx) return ax > bx;
            return a.name < b.name;
        });

        return out;
    }

} // namespace pe
