#include <scraf-backend/scraf_curl.hpp>

// Must define static class members in cpp units
std::once_flag ScrafCurl::_isCurlInitialised;

ScrafCurl::ScrafCurl() {
	std::call_once(_isCurlInitialised, []() {
		curl_global_init(CURL_GLOBAL_NOTHING);
	});
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_responseBody);
	curl_easy_setopt(curl, CURLOPT_TCP_FASTOPEN, 1L);
	curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);
	curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
}

ScrafCurl::~ScrafCurl() {
	curl_easy_cleanup(curl);
}

void ScrafCurl::get(const std::string_view url) noexcept {
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_perform(curl);
}

void ScrafCurl::post(const std::string_view url, const std::string_view header, const std::string_view request) noexcept {
	ScrafCurl::post(url, std::array{header}, request);
}

void ScrafCurl::put(const std::string_view url, const std::string_view header, const std::string_view request) noexcept {
	ScrafCurl::put(url, std::array{header}, request);
}

void ScrafCurl::patch(const std::string_view url, const std::string_view header, const std::string_view request) noexcept {
	ScrafCurl::patch(url, std::array{header}, request);
}

void ScrafCurl::deletee(const std::string_view url) noexcept {
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_perform(curl);
}

[[nodiscard]] long ScrafCurl::getResponseCode() const noexcept {
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &_responseCode);
	return _responseCode;
}

std::size_t ScrafCurl::writeData(char* /*incomingBuffer*/, std::size_t size, std::size_t count, std::string* /*data*/) noexcept {
	// data->append(incomingBuffer, size * count);
	return size * count;
}
