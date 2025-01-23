#include "eventlist.h"
#include "llm_engine.h"
#include "lookup.h"
#include "request_generator.h"
#include "scheduler.h"
#include "units.h"
#include <array>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include <string>

double to_seconds(simtime_ps_t time);

auto main (int argc, char* argv[]) -> int {
	const option lopts[2] = {
		{"time-dat-dir", required_argument, nullptr, 0},
		{"prompt-dat-file", required_argument, nullptr, 0}
	};
	std::array<std::string, 2> loptargs; /*Store lopts requrired_argument values*/
	auto sched_N = 100;
	auto sched_k = 1;
	auto loptidx = 0;
	std::string time_dat_file;
	std::string prompt_dat_file;
	while (1) {
		auto ret = getopt_long(argc, argv, "N:k:", lopts, &loptidx);
		if (ret == -1) {
			break;
		}
		switch (ret) {
			break; case 0: /*Got long opt*/
				loptargs[loptidx] = optarg;
			break; case 'N':
				sched_N = std::atoi(optarg);
			break; case 'k':
				sched_k = std::atoi(optarg);
			break; default:
				std::cout << "Unrecognized option!\n";
				std::exit(-1);
		}
	}	
	auto& eventlist = EventList::get_instance(INT64_MAX);
	auto engine = LLMEngine(eventlist,
			 Scheduler(sched_N, sched_k),
			 LookupTable(loptargs[0]));
	//Add requests
	auto req_generator = EmpericalRequestGenerator(eventlist, engine, loptargs[1], 1, 3000);
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
	std::cout << sched_k << " " << engine.m_finish_queue.size() / to_seconds(end_time) << '\n';
}

auto to_seconds(simtime_ps_t time) -> double {
	return time*1e-12;
}
