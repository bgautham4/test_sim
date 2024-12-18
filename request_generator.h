#ifndef REQ_GEN_H
#define REQ_GEN_H

#include "eventlist.h"
#include <random>
class LLMEngine;

class RequestGenerator : public EventSource {
	public:
		RequestGenerator(EventList& eventlist, LLMEngine& engine, int rate, int max_num_reqs) : EventSource(eventlist), 
		m_engine_ref(engine),
		m_rate(rate),
		m_max_req(max_num_reqs) {}
		virtual void process_event() override;	
	private:
		LLMEngine& m_engine_ref;
		int m_rate, m_max_req;
		static int req_count;
		static std::mt19937 PRNG;
};
#endif
