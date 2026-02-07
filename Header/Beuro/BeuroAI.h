#pragma once
#include <dpp/dpp.h>

dpp::task<void> Beuro_Response(std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro);
void writeBeuro_ChatHistory(std::string beuro_chat, std::string user, std::string user_message);