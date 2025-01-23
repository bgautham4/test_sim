#include "eventlist.h"
#include "units.h"
#include <cassert>
#include <utility>

auto EventList::get_instance(simtime_ps_t end_time) -> EventList& {
	static EventList eventlist{end_time};
	return eventlist;
}

auto EventList::event_at(EventSource& event_source, simtime_ps_t when) -> void {
	assert(when >= now());	
	m_pending_events.emplace(
		when,
		&event_source
	);
}

auto EventList::step() -> void {
	auto it = m_pending_events.begin();
	auto [time, ev_src] = *it;
	assert(time >= now());
	m_curr_time = time;
	ev_src->process_event();
	m_pending_events.erase(it);
}

