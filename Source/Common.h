/*================================================================
Filename: Common.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include <memory>
#include <string>
#include <vector>

#define AGZ_NAMESPACE_BEGIN(Name) namespace Name {
#define AGZ_NAMESPACE_END(Name) }

#define AGZ_LOCAL_DEFINITION static
#define AGZ_LOCAL_DEFINITIONS_BEGIN namespace {
#define AGZ_LOCAL_DEFINITIONS_END }

AGZ_NAMESPACE_BEGIN(AGZ)

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T, typename...Args>
inline Ptr<T> MakePtr(Args&&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using WPtr = std::weak_ptr<T>;

bool ReadFile(const std::string &path, std::string &output);

std::string StrJoin(const std::vector<std::string> &strs, const std::string &join);

AGZ_NAMESPACE_END(AGZ)
