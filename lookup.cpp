#include "lookup.h"
#include "units.h"
#include <cassert>
#include <fstream>
#include <sstream>

LookupTable::LookupTable(const std::string& filename) {
	std::ifstream file(filename);
	assert(file.good());
	std::string line;
	int k = 0;
	double pt, dt;
	while (std::getline(file, line)) {
		std::istringstream sstream(line);
		sstream >> k;
		sstream >> pt;
		sstream >> dt;
		m_table[m_num_entries++] = {k, pt, dt};
	}
	assert(m_num_entries);
}

auto LookupTable::get_time_for(LookupTable::For t, int n) -> simtime_ps_t {
	//Binary search code in case of LERP, not needed as we have all sample points
	/*
	int idx = 0;
	int lo = 0, hi = m_num_entries;
	int mid;
	while (lo < hi - 1) {
		mid = (lo + hi) / 2;
		auto [k, pt, dt] = m_table[mid];
		if (k < n) {
			lo = mid + 1;
		}
		else {
			hi = mid;
		}
	}
	auto [k, pt, dt] = m_table[lo];
	*/

	assert(n > 0);
	auto [_, pt, dt] = m_table[n-1];
	auto x = (t == LookupTable::For::PREFILL) ? pt : dt;
	return ps_from(TimeUnits::S, x);
}
