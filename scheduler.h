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
		virtual SchedulerOutputs schedule(LLMEngine&) = 0;
		
};

class Scheduler : public AbstractScheduler {
	public:
		Scheduler() = default;
		virtual ~Scheduler() override = default;
		virtual SchedulerOutputs schedule(LLMEngine&) override;
		void set_k(int k) {
			m_k = k;
		}
	private:
		int m_k = 11;
};
#endif
