<p align="center">
  <img src="./icon.png" alt="Ollama Chat Module" title="Ollama Chat Module Icon">
</p>

# AzerothCore + Playerbots Module: mod-ollama-chat

> [!CAUTION]
> This module is very early stages and can bog down your server due to the nature of running local LLM. Please proceed with this in mind.

## Overview

***mod-ollama-chat*** is an AzerothCore module that enhances the Player Bots module by integrating external language model (LLM) support via the Ollama API. This module enables player bots to generate dynamic, in-character chat responses using advanced natural language processing locally on your computer (or remotely hosted). Bots are enriched with personality traits, random chatter triggers, and context-aware replies that mimic the language and lore of World of Warcraft.

## Features

- **Ollama LLM Integration:**  
  Bots generate chat responses by querying an external Ollama API endpoint. This enables natural and contextually appropriate in-game dialogue.

- **Player Bot Personalities:**  
  When enabled, each bot is assigned a personality type (e.g., Gamer, Roleplayer, Trickster) that modifies its chat style. Personalities influence prompt generation and result in varied, immersive responses.

- **Context-Aware Prompt Generation:**  
  The module gathers extensive context about both the bot and the interacting player—including class, race, role, faction, guild, and more—to generate prompts for the LLM. A comprehensive WoW cheat sheet is appended to every prompt to ensure the LLM replies with accurate lore, terminology, and in-character language spanning Vanilla WoW, The Burning Crusade, and Wrath of the Lich King.

- **Random Chatter:**  
  Bots can periodically initiate random, environment-based chat when a real player is nearby. This feature adds an extra layer of immersion to the game world.

- **Blacklist for Playerbot Commands:**  
  A configurable blacklist prevents bots from responding to chat messages that start with common playerbot command prefixes, ensuring that administrative commands are not inadvertently processed. Additional commands can be appended via the configuration.

- **Asynchronous Response Handling:**  
  Chat responses are generated on separate threads to avoid blocking the main server loop, ensuring smooth server performance.

## Installation

> [!IMPORTANT]
> Prerequisite dependencies have only been verified to work on macOS Monterey 12.7.6.  
> If you encounter any issues or resolve problems related to dependencies on your OS, please open an issue describing the problem, the solution, and your OS version. I will update the README accordingly.

1. **Prerequisites:**
   - Ensure you have liyunfan1223's AzerothCore (https://github.com/liyunfan1223/azerothcore-wotlk) installation with the Player Bots (https://github.com/liyunfan1223/mod-playerbots) module enabled.
   - The module depends on:
     - cURL (https://curl.se/libcurl/)
     - fmtlib (https://github.com/fmtlib/fmt)
     - nlohmann/json (https://github.com/nlohmann/json)
     - Ollama LLM support – set up a local instance of the Ollama API server with the model of your choice. More details at https://ollama.com

2. **Clone the Module:**
   ```bash
   cd /path/to/azerothcore/modules
   git clone https://github.com/DustinHendrickson/mod-ollama-chat.git
   ```

3. **Recompile AzerothCore:**
   ```bash
   cd /path/to/azerothcore
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

4. **Configuration:**
   Copy the default configuration file to your server configuration directory and change to match your setup (if not already done):
   ```bash
   cp /path/to/azerothcore/modules/mod-ollama-chat/mod-ollama-chat.conf.dist /path/to/azerothcore/etc/config/mod-ollama-chat.conf
   ```

5. **Restart the Server:**
   ```bash
   ./worldserver
   ```

## Configuration Options

All configuration options for mod-ollama-chat are defined in `mod-ollama-chat.conf`. Key settings include:

- **OllamaChat.SayDistance:**  
  Maximum distance (in game units) a bot must be within to reply on a Say message.  
  Default: `30.0`

- **OllamaChat.YellDistance:**  
  Maximum distance for Yell messages.  
  Default: `100.0`

- **OllamaChat.GeneralDistance:**  
  Maximum distance for custom chat channels.  
  Default: `600.0`

- **OllamaChat.PlayerReplyChance:**  
  Percentage chance that a bot replies when a real player speaks.  
  Default: `90`

- **OllamaChat.BotReplyChance:**  
  Percentage chance that a bot replies when another bot speaks.  
  Default: `10`

- **OllamaChat.MaxBotsToPick:**  
  Maximum number of bots randomly chosen to reply when no bot is directly mentioned.  
  Default: `2`

- **OllamaChat.Url:**  
  URL of the Ollama API endpoint.  
  Default: `http://localhost:11434/api/generate`

- **OllamaChat.Model:**  
  The model identifier for the Ollama API query.  
  Default: `llama3.2:1b`

- **OllamaChat.EnableRandomChatter:**  
  Enable or disable random chatter from bots.  
  Default: `1` (true)

- **OllamaChat.MinRandomInterval:**  
  Minimum interval (in seconds) between random bot chat messages.  
  Default: `45`

- **OllamaChat.MaxRandomInterval:**  
  Maximum interval (in seconds) between random bot chat messages.  
  Default: `180`

- **OllamaChat.EnableRPPersonalities:**  
  Enable distinct roleplay personalities for bots.  
  Default: `0` (false)

- **OllamaChat.RandomChatterRealPlayerDistance:**  
  Maximum distance (in game units) a real player must be within to trigger random chatter.  
  Default: `40.0`

- **OllamaChat.RandomChatterBotCommentChance:**  
  Percentage chance that a bot adds a random comment when random chatter is triggered.  
  Default: `25`

- **OllamaChat.BlacklistCommands:**  
  Comma-separated list of command prefixes that should be ignored by bots.  
  Default: *(empty)*

- **OllamaChat.MaxConcurrentQueries:**  
  Maximum number of concurrent API queries allowed. Use `0` for no limit.  
  Default: `0`

## How It Works

1. **Chat Handling:**  
   When a player sends a chat message, the module determines the channel type and checks if the message starts with any blacklisted command prefix. If it does, the bot will not respond.

2. **Prompt Generation:**  
   For eligible messages, the module gathers detailed context about both the bot and the player (including class, race, faction, guild, and more) and appends a comprehensive WoW cheat sheet to the prompt. The cheat sheet covers lore, terminology, key locations, and communication style from Vanilla WoW, The Burning Crusade, and Wrath of the Lich King.

3. **LLM Query:**  
   The generated prompt is sent to the Ollama API asynchronously. The API’s response is then routed back through the appropriate chat channel by the bot.

4. **Personality & Random Chatter:**  
   Bots have assigned personality types that affect their responses. Additionally, bots may randomly generate environment-based chatter when real players are nearby.

## Debugging

For detailed logs of bot responses, prompt generation, and LLM interactions, enable debug mode via your server logs or module-specific settings.

## Troubleshooting

It's advised to turn off the normal Player Bots chat in your `playerbots.conf` by setting  `AiPlayerbot.EnableBroadcasts = 0`

## License

This module is released under the GNU GPL v3 license, consistent with AzerothCore's licensing.

## Contribution

Developed by Dustin Hendrickson

Pull requests, bug reports, and feature suggestions are welcome. Please adhere to AzerothCore's coding standards and guidelines when submitting contributions.
