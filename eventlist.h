#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <map>
#include <memory>
#include "units.h"

class EventList;

/*
Abstract class for an entity which produces discrete time events
*/
class EventSource {
	public:
		EventSource(EventList& eventlist) : m_eventlist_ref(eventlist) {}
		virtual ~EventSource() = default;
		virtual void process_event() = 0;
		EventList& get_ref_to_eventlist() {return m_eventlist_ref;}
	protected:
		EventList& m_eventlist_ref;
};

class EventList {
	public:
		static EventList& get_instance(simtime_ps_t end_time);
		EventList(const EventList&) = delete;
		EventList& operator=(const EventList&) = delete;
		~EventList() = default;
		simtime_ps_t now();
		void event_at(EventSource& event_source, simtime_ps_t); /*Absolute time*/
		void event_in(EventSource& event_source, simtime_ps_t); /*Relative time*/
		bool is_done();
		void step();
	private:
		EventList(simtime_ps_t end_time) : m_curr_time(0),m_end_time(end_time) {}
		std::multimap<simtime_ps_t, EventSource*> m_pending_events;
		simtime_ps_t m_curr_time = 0;
		simtime_ps_t m_end_time = 0;
};
#endif
