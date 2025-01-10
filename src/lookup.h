#ifndef LOOKUP_H
#define LOOKUP_H
#include <array>
#include <tuple>
#include <cstddef>
#include <string>
#include "units.h"

enum class For {PREFILL, DECODE};

//Abstract class for a function returning the prefill/decode time
class LookupFunction {
	public:
		virtual simtime_ps_t get_time_for(For t, int n) const = 0;
		virtual ~LookupFunction() = default;
};

class LookupTable : public LookupFunction {
		using lookup_el_t = std::tuple<int, double, double>;
	public:
		LookupTable(const std::string& filename);	
		virtual simtime_ps_t get_time_for(For t, int n) const override;
	private:
		std::array<lookup_el_t, 200> m_table;
		int m_num_entries = 0;	
};
#endif
