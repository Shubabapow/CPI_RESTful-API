#include "restclient-cpp/restclient.h"
#include <sstream>
#include <fstream>
#include <string>

class test {

	void main() {
		RestClient::Response r = RestClient::get("http://localhost.com:18080/hello/55");
	}

};