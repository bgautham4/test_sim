#include "request_generator.h"
#include "request.h"
#include "units.h"
#include "llm_engine.h"
#include <fstream>
#include <random>
#include <sstream>


EmpericalRequestGenerator::EmpericalRequestGenerator(EventList& eventlist, LLMEngine& engine, const std::string& file, double rate, int max_num_reqs) : EventSource(eventlist),
	m_engine_ref(engine),
	m_rate(rate),
	m_max_num_req(max_num_reqs),
	m_PRNG(std::random_device{}())
	{
	std::ifstream dat_file(file);
	std::string line;
	int prompt_len = 0;
	int output_len = 0;
	while (std::getline(dat_file, line)) {
		std::istringstream str_stream(line);
		str_stream >> prompt_len;
		str_stream >> output_len;
		m_file_entries[m_num_entries++] = {prompt_len, output_len};	
	}
	m_udrv = std::uniform_int_distribution(0, m_num_entries - 1);
}

auto EmpericalRequestGenerator::process_event() -> void {
	if (m_max_num_req-- <= 0) {
		return;
	}
	auto index = m_udrv(m_PRNG);
	auto [prompt_len, output_len] = m_file_entries[index];
	m_engine_ref.add_request(Request(prompt_len, output_len));
	//TODO: Use m_rate, instead of fixed rate.
	m_eventlist_ref.event_in(*this, ps_from(TimeUnits::MS, 0.5));
}
