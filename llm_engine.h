#ifndef LLMENGINE_H
#define LLMENGINE_H

#include "request.h"
#include "eventlist.h"
#include "scheduler.h"
#include "units.h"
#include <deque>
#include <list>
#include <memory>
#include <queue>
#include <type_traits>
#include <utility>


class LLMEngine : public EventSource {
	friend class AbstractScheduler;
	friend class Scheduler;
	/*Add your schduler as friend if
	 you need access to private memebers of engine*/
	public:
		using pending_q_t = std::deque<std::unique_ptr<Request>>;
		using running_q_t = std::list<std::unique_ptr<Request>>;
		using finish_q_t = std::vector<std::unique_ptr<Request>>;
		enum class State {
			IDLE,
			BUSY, /*Prefill or decode in progress*/
		}; 

		template<typename S>
		requires std::is_base_of_v<AbstractScheduler, std::remove_cvref_t<S>>
		LLMEngine(EventList&, S&& scheduler);

		void add_request(Request request);
		void add_request(std::unique_ptr<Request> request);
		virtual void process_event() override;
		finish_q_t m_finish_queue;
	private:
		std::unique_ptr<AbstractScheduler> m_scheduler;
		running_q_t m_running_queue;
		pending_q_t m_pending_queue;
		State m_state = State::IDLE;
};

template<typename S>
requires std::is_base_of_v<AbstractScheduler, std::remove_cvref_t<S>>
LLMEngine::LLMEngine(EventList& eventlist, S&& scheduler) : EventSource(eventlist) {
	this->m_scheduler = std::make_unique< std::remove_cvref_t<S> >(std::forward<S>(scheduler));
}

#endif
