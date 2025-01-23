#include "lookup.h"
#include "units.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

LookupTable::LookupTable(const std::string& dirname) {
	std::ifstream prefill_file(dirname + "prefill.txt"), decode_file(dirname + "decode.txt");
	assert(prefill_file.good() && decode_file.good());
	std::string line;
	int k = 0;
	double time;
	while (std::getline(prefill_file, line)) {
		std::istringstream sstream(line);
		sstream >> k;
		sstream >> time;
		m_prefills[m_num_entries_prefill++] = {k, time};
	}
	assert(m_num_entries_prefill);
	while (std::getline(decode_file, line)) {
		std::istringstream sstream(line);
		sstream >> k;
		sstream >> time;
		m_decodes[m_num_entries_decode++] = {k, time};
	}
	assert(m_num_entries_decode);
}

auto LookupTable::get_time_for(For t, int n) const -> simtime_ps_t {
	//Binary search for approx time and lerp
	auto lo = 1; 
	auto mid = 0;
	auto hi = (t==For::PREFILL) ? m_num_entries_prefill - 1 : m_num_entries_decode - 1;
	const auto& table = (t==For::PREFILL) ? m_prefills : m_decodes;
	auto [k0,t0] = table[0]; 
	if (n <= k0) { //Fast ret
		return ps_from(TimeUnits::S, t0);
	}
	while (lo < hi) {
		mid = (lo + hi) / 2;
		auto [k, _] = table[mid];
		if (n > k) {
			lo = mid + 1;
		}
		else {
			hi = mid;
		}
	}
	auto [k1, t1] = table[lo-1];
	auto [k2, t2] = table[lo];
	auto x = std::lerp(t1, t2, static_cast<double>(n - k1)/(k2 - k1));
	return ps_from(TimeUnits::S, x);
}

