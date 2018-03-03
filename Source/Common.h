/*================================================================
Filename: Common.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <memory>
#include <string>

#define AGZ_NAMESPACE_BEGIN(Name) namespace Name {
#define AGZ_NAMESPACE_END(Name) }

AGZ_NAMESPACE_BEGIN(AGZ)

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T, typename...Args>
inline Ptr<T> MakePtr(Args&&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

using String = std::string;
using Char = String::value_type;

bool ReadFile(const String &path, String &output);

AGZ_NAMESPACE_END(AGZ)
