#ifndef INCLUDED_ON_LEAVING_SCOPE_
#define INCLUDED_ON_LEAVING_SCOPE_
#include <utility> // std::move, std::forward


template<typename Func>
class On_Leaving_Scope
{
public:
    // Prevent copying
    On_Leaving_Scope(const On_Leaving_Scope&) = delete;
    On_Leaving_Scope& operator=(const On_Leaving_Scope&) = delete;

    // Allow moving
    On_Leaving_Scope(On_Leaving_Scope&& other) :
        m_func(std::move(other.m_func)),
        m_active(other.m_active)
    {
        other.m_active = false;
    }

    // Accept lvalue function objects
    On_Leaving_Scope(const Func& f) :
        m_func(f),
        m_active(true)
    {
    }

    // Accept rvalue function objects
    On_Leaving_Scope(Func&& f) :
        m_func(std::move(f)),
        m_active(true)
    {
    }

    // Only invoke function object if it
    // hasn't been moved elsewhere
    ~On_Leaving_Scope()
    {
        if (m_active)
            m_func();
    }

private:
    Func m_func;
    bool m_active;
};

template<typename Func>
On_Leaving_Scope<typename std::decay<Func>::type>
onLeavingScope(Func&& f)
{
    return On_Leaving_Scope<typename std::decay<Func>::type>(std::forward<Func>(f));
}

#endif