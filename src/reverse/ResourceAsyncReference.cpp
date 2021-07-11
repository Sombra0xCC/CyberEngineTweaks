#include <stdafx.h>

#include "ResourceAsyncReference.h"
#include "RTTILocator.h"
#include "Converter.h"

ResourceAsyncReference::ResourceAsyncReference(const TiltedPhoques::Locked<sol::state, std::recursive_mutex>& aView,
                                               RED4ext::IRTTIType* apType, RED4ext::ResourceAsyncReference<void>* apReference)
    : ClassType(aView, reinterpret_cast<RED4ext::CResourceAsyncReference*>(apType)->innerType)
    , m_reference(*apReference)
{
}

uint64_t ResourceAsyncReference::Hash(const std::string& aPath)
{
    // Should probably be moved to RED4ext.SDK after fixing RED4ext::RaRef
    // Needs normalization
    // 1) all lower case
    // 2) / becomes \
    // 3) /\/\\ becomes \

    return RED4ext::FNV1a(aPath.c_str());
}

RED4ext::ScriptInstance ResourceAsyncReference::GetHandle()
{
    return &m_reference;
}

uint64_t ResourceAsyncReference::GetHash() const
{
    return reinterpret_cast<uint64_t>(m_reference.ref);
}

// Manual uint64 to Lua conversion because sol + LuaJIT can't do it
sol::object ResourceAsyncReference::GetLuaHash()
{
    static RTTILocator s_uint64Type{RED4ext::FNV1a("Uint64")};

    auto lockedState = m_lua.Lock();

    RED4ext::CStackType stackType;
    stackType.type = s_uint64Type;
    stackType.value = GetHandle();

    return Converter::ToLua(stackType, lockedState);
}
