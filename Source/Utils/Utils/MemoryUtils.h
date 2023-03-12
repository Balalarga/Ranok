#pragma once
#include <memory>

/// Declare templated func for creating unique_ptr items in container by push_back
#define DECLARE_VECTOR_UNIQUE_ADDER(funcName, container) \
template<class ItemType, class ...TArgs> \
ItemType& funcName(TArgs&& ...args) \
{ \
    auto item = new ItemType(std::forward<TArgs>(args)...); \
    container.push_back(std::unique_ptr<ItemType>(item)); \
    return *item; \
}
