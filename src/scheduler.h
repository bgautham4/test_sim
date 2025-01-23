#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "request.h"
#include <vector>

class LLMEngine;

struct SchedulerOutputs {
	int prefill_count;
	std::vector<request_id_t> completed_group;
};

class AbstractScheduler {
	public:
		AbstractScheduler() = default;
		virtual ~AbstractScheduler() = default;
		virtual SchedulerOutputs schedule(LLMEngine&) const = 0;
		
};

class Scheduler : public AbstractScheduler {
	public:
		Scheduler() = default;
		Scheduler(int max_N, int prefill_k) : m_N(max_N), m_k(prefill_k) {}
		virtual SchedulerOutputs schedule(LLMEngine&) const override;
	private:
		int m_N = 100;
		int m_k = 1;
};
#endif
