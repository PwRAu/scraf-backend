#include <curl/curl.h>
#include <mutex>
#include <string>
#include <array>

class ScrafCurl {
public:
	ScrafCurl();
	~ScrafCurl();

	void get(std::string_view url) noexcept;

	template<std::size_t count>
	void post(const std::string_view url, const std::array<std::string_view, count>& headers, const std::string_view request) noexcept {
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		struct curl_slist* curlHeaders {nullptr};
		for (const std::string_view header : headers) {
			curlHeaders = curl_slist_append(curlHeaders, header.data());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.data());
		curl_easy_perform(curl);
	}
	void post(std::string_view url, std::string_view header, std::string_view request) noexcept;

	template<std::size_t count>
	void put(const std::string_view url, const std::array<std::string_view, count>& headers, const std::string_view request) noexcept {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		struct curl_slist* curlHeaders {nullptr};
		for (const std::string_view header : headers) {
			curlHeaders = curl_slist_append(curlHeaders, header.data());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.data());
		curl_easy_perform(curl);
	}
	void put(std::string_view url, std::string_view header, std::string_view request) noexcept;

	template<std::size_t count>
	void patch(const std::string_view url, const std::array<std::string_view, count>& headers, const std::string_view request) noexcept {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		struct curl_slist* curlHeaders {nullptr};
		for (const std::string_view header : headers) {
			curlHeaders = curl_slist_append(curlHeaders, header.data());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.data());
		curl_easy_perform(curl);
	}
	void patch(std::string_view url, std::string_view header, std::string_view request) noexcept;

	void deletee(std::string_view url) noexcept;

	[[nodiscard]] long getResponseCode() const noexcept;

private:
	CURL* curl;
	std::string _responseBody;
	long _responseCode;
	static std::once_flag _isCurlInitialised;

private:
	static std::size_t writeData(char* incomingBuffer, std::size_t size, std::size_t count, std::string* data) noexcept;
};
