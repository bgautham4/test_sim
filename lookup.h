#ifndef LOOKUP_H
#define LOOKUP_H
#include <array>
#include <tuple>
#include <cstddef>
#include <string>
#include "units.h"
using lookup_el_t = std::tuple<int, double, double>;
constexpr int N = 200;
class LookupTable {
	public:
		enum class For{PREFILL, DECODE};
		LookupTable(const std::string& filename);	
		simtime_ps_t get_time_for(For t, int n);
	private:
		std::array<lookup_el_t, N> m_table;
		int m_num_entries = 0;	
};
#endif
