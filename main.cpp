#include "eventlist.h"
#include "llm_engine.h"
#include "request_generator.h"
#include "scheduler.h"
#include "units.h"
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <iostream>

double to_seconds(simtime_ps_t time);

auto main (int argc, char* argv[]) -> int {
	if (argc != 2) {
		std::cout << "Argument count error! Requires 2 but got " << argc << '\n';
		std::exit(1);
	}
	auto k = std::atoi(argv[1]);
	auto& eventlist = EventList::get_instance(INT64_MAX);
	auto engine = LLMEngine(eventlist,
			 Scheduler(100, k)
			 );
	//Add requests
	auto req_generator = RequestGenerator(eventlist, engine, 1, 100000);
	eventlist.event_in(req_generator, 1);
	//Start sim
	while (!eventlist.is_done()) {
		eventlist.step();
	}
	//Calculate metrics
	simtime_ps_t end_time = eventlist.now();	
	simtime_ps_t mean_completion_time = 0;
	int total_tokens = 0;
	for (const auto& req : engine.m_finish_queue) {
		mean_completion_time += req->departure_time - req->first_scheduled_time;
		total_tokens += req->num_input_tokens + req->num_output_tokens;
	}
	std::cout << k << " " << engine.m_finish_queue.size() / to_seconds(end_time) << '\n';
}

auto to_seconds(simtime_ps_t time) -> double {
	return time*1e-12;
}
