#include "llm_engine.h"
#include "eventlist.h"
#include "request.h"
#include "scheduler.h"
#include "lookup.h"
#include <memory>
#include <cassert>

extern LookupTable table;

auto LLMEngine::add_request(std::unique_ptr<Request> request) -> void {
	m_pending_queue.emplace_back(std::move(request));
	m_pending_queue.back()->arrival_time = m_eventlist_ref.now();
	if (m_state == State::IDLE) {
		m_eventlist_ref.event_in(*this, 1);
	}
}

auto LLMEngine::add_request(Request request) -> void {
	add_request(std::make_unique<Request>(request));
}

auto LLMEngine::process_event() -> void {
	m_state = State::BUSY;
	auto [prefill_count, completed_reqs] = m_scheduler->schedule(*this);
	assert(prefill_count >= 0);
	auto num_prefills = prefill_count;
	//First remove completed request.
	for (const auto& id : completed_reqs) {
		auto it = std::ranges::find_if(m_running_queue, [id=id](const auto& req){return req->id == id;});
		//Assert that request is present in queue
		assert(it != m_running_queue.end());
		auto req = std::move(*it);
		req->departure_time = m_eventlist_ref.now();
		m_finish_queue.emplace_back(std::move(req));
		m_running_queue.erase(it);
	}

	while (prefill_count--) {
		auto req = std::move(m_pending_queue.front());
		req->first_scheduled_time = m_eventlist_ref.now();
		req->num_tokens_generated++;
		m_running_queue.emplace_back(std::move(req));	
		m_pending_queue.pop_front();
	}
	if (num_prefills) {
		//Scheduled prefill
		m_eventlist_ref.event_in(
			*this,
			m_lookupfunc->get_time_for(For::PREFILL, num_prefills)
		);
		return;
	}
	
	//Scheduled decodes
	if (m_running_queue.empty()) {
		m_state = State::IDLE;
		return; // No decodes
	}
	
	auto num_decodes = static_cast<int>(m_running_queue.size());
	for (auto &req : m_running_queue) {
		req->num_tokens_generated++;
	}
	m_eventlist_ref.event_in(
		*this,
		m_lookupfunc->get_time_for(For::DECODE, num_decodes)
	);	
}
