#include <iostream>

#include <httputil/parser/uri_parser.h>
#include <httputil/uri.h>

#define CSV_IO_NO_THREAD
#include "csv.h"

struct test_case {
	size_t processed;
	int error;
	int type;
	size_t scheme_sz;
	size_t user_sz;
	size_t host_sz;
	size_t port_sz;
	unsigned short port;
	size_t path_sz;
	size_t query_sz;
};

static bool match_test_data(int n, const test_case &e, const test_case &d)
{
	if (e.processed != d.processed) {
		printf("Test case %d: invalid processed %ld/%ld\n", n, d.processed, e.processed);
		return false;
	}

	if (e.error != d.error) {
		printf("Test case %d: invalid error %d/%d\n", n, d.error, e.error);
		return false;
	}

	if (e.error)
		return true;

	if (e.type != d.type) {
		printf("Test case %d: invalid type %d/%d\n", n, d.type, e.type);
		return false;
	}

	if (e.scheme_sz != d.scheme_sz) {
		printf("Test case %d: invalid scheme size %ld/%ld\n", n, d.scheme_sz, e.scheme_sz);
		return false;
	}

	if (e.user_sz != d.user_sz) {
		printf("Test case %d: invalid user size %ld/%ld\n", n, d.user_sz, e.user_sz);
		return false;
	}

	if (e.host_sz != d.host_sz) {
		printf("Test case %d: invalid host size %ld/%ld\n", n, d.host_sz, e.host_sz);
		return false;
	}

	if (e.port_sz != d.port_sz) {
		printf("Test case %d: invalid port size %ld/%ld\n", n, d.port_sz, e.port_sz);
		return false;
	}

	if (e.port != d.port) {
		printf("Test case %d: invalid port %d/%d\n", n, d.port, e.port);
		return false;
	}

	if (e.path_sz != d.path_sz) {
		printf("Test case %d: invalid path size %ld/%ld\n", n, d.path_sz, e.path_sz);
		return false;
	}

	if (e.query_sz != d.query_sz) {
		printf("Test case %d: invalid query size %ld/%ld\n", n, d.query_sz, e.query_sz);
		return false;
	}

	return true;
}

static test_case parse_test_uri(const std::string &uri)
{
	httputil::parser::uri_parser p;
	test_case c;

	c.processed = p.parse(uri.data(), uri.size() + 1);

	auto r = p.results();

	c.error = r.type == httputil::uri::uri_type::invalid;;
	c.type = static_cast<int>(r.type);
	c.scheme_sz = r.scheme.second;
	c.user_sz = r.user.second;
	c.host_sz = r.host.second;
	c.port_sz = r.port.second;
	c.path_sz = r.path.second;
	c.query_sz = r.query.second;
	c.port = r.port_num;

	return c;
}

int main(int argc, char *argv[])
{
	io::CSVReader<11> csv("test_uri_parser.csv");

	csv.read_header(io::ignore_extra_column,
			"uri", "processed", "error",
			"type", "scheme_sz", "user_sz",
			"host_sz", "port_sz", "port",
			"path_sz", "query_sz");

	std::string uri;
	test_case e;
	int n = 0;
	bool success = true;

	while(csv.read_row(uri, e.processed, e.error, e.type, e.scheme_sz,
			e.user_sz, e.host_sz, e.port_sz, e.port, e.path_sz, e.query_sz)) {

		uri = uri.substr(1, uri.size() -2);

		printf("Test case %d: uri=\"%s\"\n", n + 1, uri.c_str());

		test_case d = parse_test_uri(uri);

		if (!match_test_data(++n, e, d))
			success = false;
	}

	auto u = httputil::uri::parse("http://user:pass@host:123/path/to/entry?querystr");

	std::cout << "scheme=" << u.scheme() << std::endl;
	std::cout << "user=" << u.user() << std::endl;
	std::cout << "host=" << u.host() << std::endl;
	std::cout << "port=" << u.port() << std::endl;
	std::cout << "path=" << u.path() << std::endl;
	std::cout << "query=" << u.query() << std::endl;

	return success ? 0 : 1;
}
