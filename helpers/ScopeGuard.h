#ifndef SCOPEGUARD_H
#define SCOPEGUARD_H

// Taken from "Declarative Control Flow" (CppCon 2015)
// adjusted to work with Visual C++ 2013

#include <utility>

#define CONCATENATE_IMPL(s1, s2) s1 ## s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

namespace detail {
enum class ScopeGuardOnExit
{
};

template<typename Fun> class ScopeGuard
{
public:
	ScopeGuard(Fun&& fn)
		: fn(std::move(fn))
	{
	}
	~ScopeGuard()
	{
		fn();
	}

private:
	Fun fn;
};

template<typename Fun> ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun&& fn)
{
	return ScopeGuard<Fun>(std::forward<Fun>(fn));
}

}

#define SCOPE_EXIT auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = ::detail::ScopeGuardOnExit() +[&]()

#endif
