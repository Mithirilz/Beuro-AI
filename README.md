# Beuro AI
Discord Bot Beuro.

## How the program works:
Ignoring most of main.cpp, the only thing that matters are the on_message create commands. From there, it routes the messages made to the AI. It can be any AI from Ollama but the model name must be changed in the json_Beuro located in BeuroAI.cpp Beuro_Response function to specific models name. 

You will also need to download docker and run the ChromaDB database on it, the commands to run it can be found in the [ChromaDB-cpp](https://github.com/BlackyDrum/chromadb-cpp) repository. Essentially, the same method is used here.

## Dependencies:

### D++
1. [D++](https://github.com/brainboxdotcc/DPP)
2. [OpenSSL](https://openssl.org/) for D++ HTTPS
3. [zlib](https://zlib.net/) D++ websocket compression
4. [libopus](https://www.opus-codec.org/) D++ audio encoding/decoding (Not yet used)
5. [MLS++](https://github.com/cisco/mlspp) for D++ voice support (Not yet used)

### RAG
1. [ChromaDB-cpp](https://github.com/BlackyDrum/chromadb-cpp)
2. [SQLite-cpp](https://github.com/SRombauts/SQLiteCpp)

### Json
1. [nlohmann-json](https://github.com/nlohmann/json)

### Environment variable
1. [dotenv](https://github.com/laserpants/dotenv-cpp)

## Remarks:
Feel free to Pull Request if you'd like, or submit an issue if there is something you'd like to suggest or point out regarding the program. Overall, this project is still in progress and would benefit from any recommendations given.

NOTE:
This bot is in no way meant to be a Neuro-sama copycat (Which is the AI the bot name is inspired by). The name Beuro is simply a play on the original name and if the Neuro-sama legal team would like it changed due to it being too similar to Neuro-sama's name, please feel free to submit an issue and it will be renamed.