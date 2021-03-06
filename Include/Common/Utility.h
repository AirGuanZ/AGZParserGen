/*================================================================
Filename: Utility.h
Date: 2018.4.15
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <memory>
#include <deque>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#define NS_BEGIN(Name) namespace Name {
#define NS_END(Name) }

NS_BEGIN(AGZ)

template<typename T>
using Vec = std::vector<T>;

template<typename K, typename V, typename Compare = std::less<K>>
using Map = std::map<K, V, Compare>;

template<typename K, typename V>
using MulMap = std::multimap<K, V>;

template<typename T>
using Deq = std::deque<T>;

template<typename E>
using Set = std::set<E>;

template<typename K, typename V,
         typename H = std::hash<K>,
         typename E = std::equal_to<K>>
using UrdMap = std::unordered_map<K, V, H, E>;

template<typename K, typename V>
using UrdMulMap = std::unordered_multimap<K, V>;

template<typename T>
using Stack = std::stack<T>;

using String = std::string;

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T, typename...Args>
inline Ptr<T> MakePtr(Args&&...args);

template<typename T>
using WPtr = std::weak_ptr<T>;

inline bool ReadTxt(const String &filename, String &output);

inline String StrJoin(const Vec<String> &strs, const String &join);

template<typename T>
inline bool WriteBinary(std::ofstream &fout, const T &data);

inline bool WriteString(std::ofstream &fout, const String &str);

template<typename T>
inline bool ReadBinary(std::ifstream &fin, T &data);

inline bool ReadString(std::ifstream &fin, String &str);

NS_END(AGZ)

#include <Common/Utility.inl>
