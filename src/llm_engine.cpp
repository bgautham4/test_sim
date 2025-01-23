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
	//auto max_input_size = 0;
	auto num_prefill_tokens = 0;
	//First remove completed request.
	for (const auto& id : completed_reqs) {
		auto req = std::move(m_running_queue.at(id)); //Throw if not found, this should not happen
		req->departure_time = m_eventlist_ref.now();
		m_finish_queue.emplace_back(std::move(req));
		m_running_queue.erase(id);
	}
	//Move pending requests into running queue
	while (prefill_count--) {
		auto req = std::move(m_pending_queue.front());
		/*
		if (req->num_input_tokens > max_input_size) {
			max_input_size = req->num_input_tokens;
		}
		*/
		num_prefill_tokens += req->num_input_tokens;
		req->first_scheduled_time = m_eventlist_ref.now();
		req->num_tokens_generated++;
		m_running_queue.emplace(req->id,std::move(req));	
		m_pending_queue.pop_front();
	}
	if (num_prefills) {
		//Scheduled prefill
		m_eventlist_ref.event_in(
			*this,
			m_lookupfunc->get_time_for(For::PREFILL, /*num_prefills * max_input_size*/ num_prefill_tokens)
		);
		return;
	}
	
	//Scheduled decodes
	if (m_running_queue.empty()) {
		m_state = State::IDLE;
		return; // No decodes
	}
	
	auto num_decode_tokens = static_cast<int>(m_running_queue.size());
	for (auto &req : m_running_queue) {
		req.second->num_tokens_generated++;
	}
	m_eventlist_ref.event_in(
		*this,
		m_lookupfunc->get_time_for(For::DECODE, num_decode_tokens)
	);	
}
