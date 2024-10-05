#pragma once
#include <unordered_map>
struct Wrap
{
};

template <typename In, typename Out>
class MemoTable
{
private:
    using Functor = Out (*)(MemoTable&, const In&);
    Functor m_f;

    std::unordered_map<In, Out> m_cache;

public:
    MemoTable(Functor f)
        : m_f(f)
    {
    }

    Out operator()(const In& input)
    {
        auto it = m_cache.find(input);
        if (it != m_cache.end()) {
            return it->second;
}

        auto ret = m_f(input);
        m_cache.emplace(input, ret);

        return ret;
    }
};