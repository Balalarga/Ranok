#pragma once

template<class T>
struct CheckedResult
{
    CheckedResult(bool ok = false):
        ok(ok)
    {}

    CheckedResult(T result, bool ok = true):
        result(result),
        ok(ok)
    {}

    T result = 0;
    bool ok = false;

    operator T() const
    {
        return result;
    }
    operator bool() const
    {
        return ok;
    }
};
