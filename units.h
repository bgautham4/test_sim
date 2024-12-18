#ifndef UNITS_H
#define UNITS_H
#include <cassert>
#include <cmath>
#include <concepts>
#include <type_traits>
#include <cstdint>

using simtime_ps_t = int64_t;

enum class TimeUnits {
	S = 1,
	MS = 2,
	US = 3,
	PS = 4
};

template<typename T>
requires std::is_arithmetic_v<std::remove_reference_t<T>>
inline constexpr auto ps_from(TimeUnits units, T&& time) {
	assert(std::isfinite(time));
	simtime_ps_t t;
	switch(units) {
		break; case TimeUnits::S:
			t = static_cast<simtime_ps_t>(time*1000000000000);
		break; case TimeUnits::MS:
			t = static_cast<simtime_ps_t>(time*1000000000);
		break; case TimeUnits::US:
			t = static_cast<simtime_ps_t>(time*1000000);
		break; case TimeUnits::PS:
			t = static_cast<simtime_ps_t>(time);
	}
	assert(t >= 0);
	return t;
}

#endif
