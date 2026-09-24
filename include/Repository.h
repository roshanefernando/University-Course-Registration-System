#ifndef REPOSITORY_H
#define REPOSITORY_H
// MEMBER 3 -- the group-written generic template (Section 4 requirement).
// Header-only: a class template's definitions must be visible at every
// point it gets instantiated.
//
// GENERICITY CONTRACT: Repository<T> works for any T that has a free
// function getRecordId(const T&) -> std::string, findable via ADL. An
// overload is provided below for Course* (returns ->getCode()), which is
// the concrete instantiation this project actually uses.
#include <vector>
#include <string>
#include <algorithm>

template <typename T>
class Repository {
private:
    std::vector<T> items;
public:
    Repository() = default;

    void add(const T& item) {
        items.push_back(item);
    }

    bool remove(const std::string& id) {
        auto it = std::find_if(items.begin(), items.end(),
            [&](const T& item) { return getRecordId(item) == id; });
        if (it == items.end()) return false;
        items.erase(it);
        return true;
    }

    T* find(const std::string& id) {
        for (auto& item : items) {
            if (getRecordId(item) == id) return &item;
        }
        return nullptr;
    }

    const std::vector<T>& getAll() const { return items; }
    void clear() { items.clear(); }
};

#endif
