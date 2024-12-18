#ifndef REQUEST_H
#define REQUEST_H

#include "units.h"
using request_id_t = int;

struct Request {
	Request(int input_len, int output_len) : id(++next_id), num_input_tokens(input_len), num_output_tokens(output_len) {}
	Request(const Request& that) = default;
	Request(Request&& that) =  default;
	request_id_t id;
	int num_input_tokens;
	int num_output_tokens;
	int num_tokens_generated = 0;
	simtime_ps_t arrival_time = 0;
	simtime_ps_t first_scheduled_time = 0;
	simtime_ps_t departure_time = 0;
private:
	static request_id_t next_id;
};

#endif
