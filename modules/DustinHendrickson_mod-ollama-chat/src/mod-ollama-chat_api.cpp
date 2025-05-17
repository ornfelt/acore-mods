#include "mod-ollama-chat_api.h"
#include "Log.h"
#include "mod-ollama-chat_config.h"
#include <curl/curl.h>
#include <sstream>
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include "mod-ollama-chat_api.h"

// Callback for cURL write function.
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    std::string* responseBuffer = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    responseBuffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Function to perform the API call.
std::string QueryOllamaAPI(const std::string& prompt)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        LOG_INFO("server.loading", "Failed to initialize cURL.");
        return "Hmm... I'm lost in thought.";
    }

    std::string url   = g_OllamaUrl;
    std::string model = g_OllamaModel;

    nlohmann::json requestData = {
        {"model",  model},
        {"prompt", prompt}
    };
    std::string requestDataStr = requestData.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string responseBuffer;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestDataStr.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, long(requestDataStr.length()));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        LOG_INFO("server.loading",
                 "Failed to reach Ollama AI. cURL error: {}",
                 curl_easy_strerror(res));
        return "Failed to reach Ollama AI.";
    }

    std::stringstream ss(responseBuffer);
    std::string line;
    std::ostringstream extractedResponse;

    try
    {
        while (std::getline(ss, line))
        {
            nlohmann::json jsonResponse = nlohmann::json::parse(line);
            if (jsonResponse.contains("response"))
                extractedResponse << jsonResponse["response"].get<std::string>();
        }
    }
    catch (const std::exception& e)
    {
        LOG_INFO("server.loading",
                 "JSON Parsing Error: {}",
                 e.what());
        return "Error processing response.";
    }

    std::string botReply = extractedResponse.str();

    if (!botReply.empty() && botReply.front() == '"' && botReply.back() == '"')
        botReply = botReply.substr(1, botReply.size() - 2);

    if (botReply.empty())
    {
        LOG_INFO("server.loading", "No valid response extracted.");
        return "I'm having trouble understanding.";
    }

    LOG_INFO("server.loading", "Parsed bot response: {}", botReply);
    return botReply;
}

QueryManager g_queryManager;

// Interface function to submit a query.
std::future<std::string> SubmitQuery(const std::string& prompt)
{
    return g_queryManager.submitQuery(prompt);
}
