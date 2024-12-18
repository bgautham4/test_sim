#include "request_generator.h"
#include "request.h"
#include "units.h"
#include "llm_engine.h"
#include <algorithm>
#include <random>

int RequestGenerator::req_count = 0;
std::mt19937 RequestGenerator::PRNG(std::random_device{}());

auto RequestGenerator::process_event() -> void {
	auto rv = std::geometric_distribution<int>(0.01);
	auto raw_sample = rv(PRNG);
	auto fixed_sample = std::min(std::max(raw_sample, 1), 2048 - 100);
	auto req = Request(100,fixed_sample);
	m_engine_ref.add_request(req);
	if (++req_count >= m_max_req) {
		return;
	}
	m_eventlist_ref.event_in(*this, ps_from(TimeUnits::MS, 0.5));
}
