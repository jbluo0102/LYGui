#pragma once

#include <string>

using FString = std::string;

template<typename T>
using TUniquePtr = std::unique_ptr<T>;

template<typename T>
using TSharedPtr = std::shared_ptr<T>;

template<typename T>
using TVector = std::vector<T>;

template<typename T, size_t>
using TArray = std::array<T, size_t>

template<typename Key, typename Value>
using TUnOrderedMap = std::unordered_map<Key, Value>;

template<typename Key, typename Value>
using TMap = std::map<Key, Value>;

template<typename T>
using TSet = std::set<T>;

template<typename T1, typename T2>
using TPair = std::pair<T1, T2>;

template<typename T>
using TFuture = std::future<T>;

template<typename T>
using TQueue = std::queue<T>;

template<typename T>
using TList = std::list<T>;

template<typename T>
using TFunction = std::function<T>;