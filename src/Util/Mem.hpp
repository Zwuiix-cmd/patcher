//
// Created by slq on 10/31/2025.
//

#pragma once
#include <windows.h>
#include <cstdint>
#include <type_traits>
#include <cstring>
#include <vector>
#include <array>

namespace mem {
    template <class T>
    inline std::enable_if_t<std::is_trivially_copyable_v<T>, bool>
    patch_value(void* addr, const T& value, T* oldValue = nullptr) noexcept {
        if (!addr) return false;
        DWORD oldProt{};
        if (!::VirtualProtect(addr, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProt)) return false;
        if (oldValue) std::memcpy(oldValue, addr, sizeof(T));
        std::memcpy(addr, &value, sizeof(T));
        ::FlushInstructionCache(::GetCurrentProcess(), addr, sizeof(T));
        DWORD _;
        ::VirtualProtect(addr, sizeof(T), oldProt, &_);
        return true;
    }

    inline bool patch_byte(void* addr, std::uint8_t value, std::uint8_t* oldValue = nullptr) noexcept {
        return patch_value<std::uint8_t>(addr, value, oldValue);
    }

    inline bool patch_bytes(void* addr, const std::uint8_t* data, std::size_t len,
                            std::vector<std::uint8_t>* oldOut = nullptr) noexcept {
        if (!addr || !data || len == 0) return false;
        DWORD oldProt{};
        if (!::VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &oldProt)) return false;
        if (oldOut) {
            oldOut->resize(len);
            std::memcpy(oldOut->data(), addr, len);
        }
        std::memcpy(addr, data, len);
        ::FlushInstructionCache(::GetCurrentProcess(), addr, len);
        DWORD _;
        ::VirtualProtect(addr, len, oldProt, &_);
        return true;
    }

    inline bool patch_bytes(void* addr, std::initializer_list<std::uint8_t> bytes,
                            std::vector<std::uint8_t>* oldOut = nullptr) noexcept {
        return patch_bytes(addr, bytes.begin(), bytes.size(), oldOut);
    }

    inline bool nop(void* addr, std::size_t count, std::vector<std::uint8_t>* oldOut = nullptr) noexcept {
        if (!addr || count == 0) return false;
        std::vector<std::uint8_t> nops(count, 0x90);
        return patch_bytes(addr, nops.data(), nops.size(), oldOut);
    }

    inline bool nop_call5(void* addr, std::array<std::uint8_t, 5>* oldOut = nullptr,
                          bool verifyOpcode = true) noexcept {
        if (!addr) return false;

        if (verifyOpcode) {
            std::uint8_t op{};
            std::memcpy(&op, addr, 1);
            if (op != 0xE8) return false;
        }

        DWORD oldProt{};
        if (!::VirtualProtect(addr, 5, PAGE_EXECUTE_READWRITE, &oldProt)) return false;

        if (oldOut) {
            std::memcpy(oldOut->data(), addr, 5);
        }

        static const std::uint8_t fiveNops[5] = {0x90, 0x90, 0x90, 0x90, 0x90};
        std::memcpy(addr, fiveNops, 5);

        ::FlushInstructionCache(::GetCurrentProcess(), addr, 5);
        DWORD _;
        ::VirtualProtect(addr, 5, oldProt, &_);
        return true;
    }

    template <class T>
    inline std::enable_if_t<std::is_trivially_copyable_v<T>, bool>
    patch_value(std::uintptr_t address, const T& value, T* oldValue = nullptr) noexcept {
        return patch_value(reinterpret_cast<void*>(address), value, oldValue);
    }

    inline bool patch_byte(std::uintptr_t address, std::uint8_t value,
                           std::uint8_t* oldValue = nullptr) noexcept {
        return patch_byte(reinterpret_cast<void*>(address), value, oldValue);
    }

    inline bool patch_bytes(std::uintptr_t address, const std::uint8_t* data, std::size_t len,
                            std::vector<std::uint8_t>* oldOut = nullptr) noexcept {
        return patch_bytes(reinterpret_cast<void*>(address), data, len, oldOut);
    }

    inline bool patch_bytes(std::uintptr_t address, std::initializer_list<std::uint8_t> bytes,
                            std::vector<std::uint8_t>* oldOut = nullptr) noexcept {
        return patch_bytes(reinterpret_cast<void*>(address), bytes, oldOut);
    }

    inline bool nop(std::uintptr_t address, std::size_t count,
                    std::vector<std::uint8_t>* oldOut = nullptr) noexcept {
        return nop(reinterpret_cast<void*>(address), count, oldOut);
    }

    inline bool nop_call5(std::uintptr_t address, std::array<std::uint8_t, 5>* oldOut = nullptr,
                          bool verifyOpcode = true) noexcept {
        return nop_call5(reinterpret_cast<void*>(address), oldOut, verifyOpcode);
    }

} // namespace mem
