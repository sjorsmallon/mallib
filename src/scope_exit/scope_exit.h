#ifndef INCLUDED_SCOPE_EXIT_
#define INCLUDED_SCOPE_EXIT_

template<typename Func>
class OnLeavingScope
{
public:
    // Prevent copying
    OnLeavingScope(const OnLeavingScope&) = delete;
    OnLeavingScope& operator=(const OnLeavingScope&) = delete;

    // Allow moving
    OnLeavingScope(OnLeavingScope&& other) :
        m_func(std::move(other.m_func)),
        m_active(other.m_active)
    {
        other.m_active = false;
    }

    // Accept lvalue function objects
    OnLeavingScope(const Func& f) :
        m_func(f),
        m_active(true)
    {
    }

    // Accept rvalue function objects
    OnLeavingScope(Func&& f) :
        m_func(std::move(f)),
        m_active(true)
    {
    }

    // Only invoke function object if it
    // hasn't been moved elsewhere
    ~OnLeavingScope()
    {
        if (m_active)
            m_func();
    }

private:
    Func m_func;
    bool m_active;
};

template<typename Func>
OnLeavingScope<typename std::decay<Func>::type>
onLeavingScope(Func&& f)
{
    return OnLeavingScope<typename std::decay<Func>::type>(std::forward<Func>(f));
}

#endif