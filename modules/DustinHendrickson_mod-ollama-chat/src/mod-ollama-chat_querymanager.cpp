#include "mod-ollama-chat_querymanager.h"
#include "mod-ollama-chat_config.h"  // For g_MaxConcurrentQueries
#include <thread>

// Constructor: initialize with the configuration value.
QueryManager::QueryManager()
    : maxConcurrentQueries(g_MaxConcurrentQueries), currentQueries(0)
{
}

// Set maximum concurrent queries (0 means no limit).
void QueryManager::setMaxConcurrentQueries(int maxQueries) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxConcurrentQueries = maxQueries;
}

// Submit a query and return a future for the result.
std::future<std::string> QueryManager::submitQuery(const std::string& prompt) {
    std::promise<std::string> promise;
    std::future<std::string> future = promise.get_future();

    bool shouldRunNow = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (maxConcurrentQueries == 0 || currentQueries < maxConcurrentQueries) {
            ++currentQueries;
            shouldRunNow = true;
        } else {
            taskQueue.push({ prompt, std::move(promise) });
        }
    }

    if (shouldRunNow) {
        std::thread(&QueryManager::processQuery, this, prompt, std::move(promise)).detach();
    }

    return future;
}

// Process the query by calling the API and then handling any queued tasks.
void QueryManager::processQuery(const std::string& prompt, std::promise<std::string> promise) {
    std::string result = QueryOllamaAPI(prompt);
    promise.set_value(result);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        --currentQueries;
        if (!taskQueue.empty() && (maxConcurrentQueries == 0 || currentQueries < maxConcurrentQueries)) {
            QueryTask task = std::move(taskQueue.front());
            taskQueue.pop();
            ++currentQueries;
            std::thread(&QueryManager::processQuery, this, task.prompt, std::move(task.promise)).detach();
        }
    }
}
