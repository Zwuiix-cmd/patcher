//
// Created by slq on 10/31/2025.
//


#pragma once
#include <MinHook.h>
#include <type_traits>

class detour {
public:
    template <typename T, typename C>
    detour(T target, C callback) {
        mTarget   = cast(target);
        mCallback = cast(callback);
        MH_CreateHook(mTarget, mCallback, &mTrampoline);
    }

    ~detour()                { MH_RemoveHook(mTarget); }
    void enable() const      { MH_EnableHook(mTarget); }
    void disable() const     { MH_DisableHook(mTarget); }

    template<typename Fn> Fn getOriginal() const {
        static_assert(std::is_pointer_v<Fn> && std::is_function_v<std::remove_pointer_t<Fn>>,
                      "Fn must be a plain function pointer type (not a member function pointer).");
        return reinterpret_cast<Fn>(mTrampoline);
    }

private:
    void* mTarget   = nullptr;
    void* mCallback = nullptr;
    void* mTrampoline = nullptr;

    template<typename T>
    static void* cast(T ptr) {
        if constexpr (std::is_pointer_v<T>) return const_cast<void*>(static_cast<const void*>(ptr));
        else                                return reinterpret_cast<void*>(ptr);
    }
};