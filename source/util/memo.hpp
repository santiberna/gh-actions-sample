#pragma once
#include <unordered_map>
struct wrap {};


template<typename In, typename Out>
class MemoTable
{
private:

    using Functor = Out(*)(MemoTable&, const In&);
    Functor f;

    std::unordered_map<In, Out> cache;

public:

    MemoTable(Functor f) : f(f) {}

    Out operator()(const In& input)
    {
        auto it = cache.find(input);
        if (it != cache.end()) return it->second;

        auto ret = f(input);
        cache.emplace(input, ret);

        return ret;
    }

};