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

using String = std::string;

AGZ_NAMESPACE_END(AGZ)
