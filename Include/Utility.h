/*================================================================
Filename: Utility.h
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#define NS_BEGIN(Name) namespace Name {
#define NS_END(Name) }

NS_BEGIN(AGZ)

template<typename T>
using Vec = std::vector<T>;

template<typename K, typename V>
using Map = std::map<K, V>;

template<typename E>
using Set = std::set<E>;

using String = std::string;

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T, typename...Args>
inline Ptr<T> MakePtr(Args&&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using WPtr = std::weak_ptr<T>;

bool ReadTxt(const String &filename, String &output);

String StrJoin(const Vec<String> &strs, const String &join);

NS_END(AGZ)
