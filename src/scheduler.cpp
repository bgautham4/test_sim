#include "scheduler.h"
#include "llm_engine.h"
#include <algorithm>
#include <cassert>
#include <ranges>
#include <vector>

auto Scheduler::schedule(LLMEngine& engine) const -> SchedulerOutputs {
	auto& running_q =  engine.m_running_queue;
	auto& pending_q = engine.m_pending_queue;
	/*
	Find completed request and then schedule prefills and
	decodes as below:
	If size of running queue is not full and has
	vaccancy greater than k, then:
	Schedule prefills, 
	else:
	continue with decodes in running queue.
	*/
	auto get_finished = std::views::filter(
		[](const auto& req) {
			return req->num_tokens_generated == req->num_output_tokens;
		}
	);
	auto get_ids = std::views::transform(
		[](const auto& req) {
			return req->id;
		}
	);
	auto finished_ids = std::ranges::to<std::vector>(
		running_q | get_finished | get_ids
	);	
	if (running_q.size() - finished_ids.size() > m_N - m_k) {
		return {
			0,
			finished_ids
		};
	}
	int vaccany = m_N - (running_q.size() - finished_ids.size());
	assert(vaccany >= 0);
	return {
		std::min(vaccany, static_cast<int>(pending_q.size())),
		finished_ids
	};
}
