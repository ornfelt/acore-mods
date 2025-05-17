#ifndef MOD_OLLAMA_CHAT_RANDOM_H
#define MOD_OLLAMA_CHAT_RANDOM_H

#include "ScriptMgr.h"

class OllamaBotRandomChatter : public WorldScript
{
public:
    OllamaBotRandomChatter();
    void OnUpdate(uint32 diff) override;

private:
    void HandleRandomChatter();
};

#endif // MOD_OLLAMA_CHAT_RANDOM_H
