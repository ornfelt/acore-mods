#include "mod-ollama-chat_config.h"
#include "mod-ollama-chat_handler.h"
#include "mod-ollama-chat_random.h"
#include "Log.h"

void Addmod_ollama_chatScripts()
{
    new OllamaChatConfigWorldScript();
    LOG_INFO("server.loading", "Registering mod-ollama-chat scripts.");
    new PlayerBotChatHandler();
    new OllamaBotRandomChatter();
}
