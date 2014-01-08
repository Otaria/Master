/*
 * RestBrowser.cpp
 *
 *  Created on: Jan 6, 2014
 *      Author: denia
 */

#include "logging.h"
#include "RestBrowser.h"

#include <iostream>
#include <sstream>
#include <curl/curl.h>

namespace ydle {

RestBrowser::RestBrowser(std::string url) {
	this->url = url;
}

RestBrowser::~RestBrowser() {
	// TODO Auto-generated destructor stub
}
// VA_GET etc...
Json::Value RestBrowser::doGet(std::string url, std::string parameters) {
	CURL *curl;
	CURLcode res;
	Json::Reader reader;
	Json::Value response;

	curl = curl_easy_init();
	if(curl) {
		// Build get request
		std::stringstream request;
		std::string req_param;
		request << "http://" << this->url << url;
		YDLE_DEBUG << "Full uri: " << request.str();
		req_param = request.str();

		curl_easy_setopt(curl, CURLOPT_URL, req_param.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		// Define the callback function and the data pointer
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ydle::RestBrowser::responseToJsonObjectCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK){
			YDLE_WARN << "curl_easy_perform() failed: " << curl_easy_strerror(res);
		}
		curl_easy_cleanup(curl);
	}

	return response;
}

Json::Value RestBrowser::doPost() {
	return new Json::Value();
}

static size_t RestBrowser::responseToJsonObjectCallback( char *response, size_t size, size_t nmemb, void *userdata){
	size_t realsize = size * nmemb;

	Json::Value *result = userdata;

	Json::Reader reader;
	bool parsingSuccessful = reader.parse(response, *result);
	if ( !parsingSuccessful )
	{
	    // report to the user the failure and their locations in the document.
	    YDLE_WARN  << "Failed to parse configuration\n"
	               << reader.getFormatedErrorMessages();
	}

	return realsize;
}

} /* namespace ydle */
