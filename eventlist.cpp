#include "eventlist.h"
#include "units.h"
#include <cassert>
#include <utility>

auto EventList::get_instance(simtime_ps_t end_time) -> EventList& {
	static EventList eventlist{end_time};
	return eventlist;
}

inline auto EventList::now() -> simtime_ps_t {
	return m_curr_time;
}

auto EventList::event_at(EventSource& event_source, simtime_ps_t when) -> void {
	assert(when >= now());	
	m_pending_events.emplace(
		when,
		&event_source
	);
}

auto EventList::event_in(EventSource& event_source, simtime_ps_t time_from_now) -> void {
	event_at(event_source, now() +  time_from_now);
}

auto EventList::is_done() -> bool {
	return now() >= m_end_time || m_pending_events.empty(); 
}

auto EventList::step() -> void {
	auto it = m_pending_events.begin();
	auto [time, ev_src] = *it;
	assert(time >= now());
	m_curr_time = time;
	ev_src->process_event();
	m_pending_events.erase(it);
}

