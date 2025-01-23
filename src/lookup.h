#ifndef LOOKUP_H
#define LOOKUP_H
#include <array>
#include <tuple>
#include <cstddef>
#include <string>
#include "units.h"

enum class For {PREFILL, DECODE};
//Abstract class for a function returning the prefill/decode time for n tokens
class LookupFunction {
	public:
		virtual simtime_ps_t get_time_for(For t, int n) const = 0;
		virtual ~LookupFunction() = default;
};

class LookupTable : public LookupFunction {
		using lookup_el_t = std::tuple<int, double>;
		using entries_t = std::array<lookup_el_t, 512>;
	public:
		LookupTable(const std::string& dirname);	
		virtual simtime_ps_t get_time_for(For t, int n) const override;
	private:
		entries_t m_prefills, m_decodes;
		int m_num_entries_prefill = 0;
		int m_num_entries_decode = 0;
};
#endif
