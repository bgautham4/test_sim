#include "lookup.h"
#include "units.h"
#include <cassert>
//#include <cmath>
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

auto LookupTable::get_time_for(For t, int n) const -> simtime_ps_t {
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
	auto [k1, pt1, dt1] = m_table[lo];
	auto x1 = (t == For::PREFILL) ? pt1 : dt1;
	if (k1 >= n) {
		return ps_from(TimeUnits::S, x1);
	}
	assert(lo < m_num_entries - 1);
	auto [k2, pt2, dt2] = m_table[lo+1];
	assert(k2 > n);
	auto x2 = (t == For::PREFILL) ? pt2 : dt2;
	auto x = std::lerp(x1, x2, static_cast<double>(k2 - n)/(k2 - k1));
	return ps_from(TimeUnits::S, x);	
	*/
	assert(n > 0);
	auto [_, pt, dt] = m_table[n-1];
	auto x = (t == For::PREFILL) ? pt : dt;
	return ps_from(TimeUnits::S, x);
}
