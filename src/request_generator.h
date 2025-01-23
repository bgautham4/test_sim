#ifndef REQ_GEN_H
#define REQ_GEN_H

#include "eventlist.h"
#include <random>
#include <string>
class LLMEngine;

/*Generate requests from emperical data
 * The file is follows the following format:

prompt_len output_len
...

*/
class EmpericalRequestGenerator : public EventSource {
	public:
		EmpericalRequestGenerator(EventList& eventlist, LLMEngine& engine, const std::string& file, double rate, int max_num_reqs);
		virtual void process_event() override;
	private:
		using entries_t = std::array<std::tuple<int,int>, 2048>; /*Increase as needed*/
		LLMEngine& m_engine_ref;
		double m_rate;
		int m_max_num_req;
		entries_t m_file_entries; 
		int m_num_entries;
		std::mt19937 m_PRNG;
		std::uniform_int_distribution<int> m_udrv;
};

#endif
