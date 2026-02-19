#include <dpp/dpp.h>
#include <chrono>
#include <dotenv.h>
#include "Beuro/BeuroAI.h"


dpp::job debugger(const dpp::slashcommand_t event);
dpp::task<void> owner_message_commands(const dpp::message_create_t& event, dpp::cluster& beuro, BeuroAI& beuro_exec);
dpp::task<void> general_message_commands(const dpp::message_create_t& event, dpp::cluster& beuro, BeuroAI& beuro_exec);
dpp::task<void> delay();

int main() {
    dotenv::init();
    const std::string BOT_TOKEN = dotenv::getenv("BOT_TOKEN", "None");
    const std::string FILEPATH = dotenv::getenv("FILEPATH", "None");
    const std::string PORT = dotenv::getenv("PORT", "None");
    
    if (BOT_TOKEN == "None" || FILEPATH == "None" || PORT == "None"){
        std::cout << "One of the Env's are not valid" << std::endl;
        return 0;
    }
    
    BeuroAI beuro_exec(FILEPATH, PORT);
    dpp::cluster beuro(BOT_TOKEN, dpp::i_default_intents | dpp::i_message_content);

    beuro.on_log(dpp::utility::cout_logger());
    
    beuro.on_slashcommand([](const dpp::slashcommand_t& event){
        if (event.command.get_command_name() =="neuro") {
            event.reply("https://cdn.discordapp.com/attachments/1072697081443131476/1439145564070740048/oo_ee_oo-1.mov?ex=692de380&is=692c9200&hm=c7eaedf245cf27b26b2e520a32b38d6d945c0e53f3ad1df93f66e7450977e89b&");
            debugger(event);
        }
        
        else if (event.command.get_command_name() == "ai_baby"){
            event.reply("https://images-ext-1.discordapp.net/external/eIJoIaL4bAbZi1LpCv7ZxwhyIYHDgut6OuDQ1lt6O0k/https/media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExaXc5cnRqa2YzcXM0N3l6amt2cjdqdDA4d3VuNGloMGg4bGg0a29tMSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/4tXCHa4BzNbeqsRNVv/giphy.gif?width=600&height=600");
            debugger(event);
        }

        else if (event.command.get_command_name() == "soyjack") {
            dpp::embed Soyjack;
            
            Soyjack
                .set_title("Soyjack: Nothing happened")
                .set_image("https://i.pinimg.com/736x/fe/c2/00/fec200c4814516fc64ec08da0fc30582.jpg")
                .set_colour(dpp::colors::cream)
                .set_footer(
                    dpp::embed_footer()
                    .set_text("requested by mr lacksgoodnamelmao (im calling by what beuro calls him)")
                );
            dpp::message msg (event.command.channel_id, Soyjack);

            event.reply(msg);
            debugger(event);
        }

        else if (event.command.get_command_name() == "hex") {
            dpp::embed Hex;

            Hex
                .set_title("This is Hex")
                .set_color(dpp::colors::black_cow)
                .set_image("https://cdn.discordapp.com/attachments/1394984787353141369/1435437218024914997/whythink.gif")
                .set_footer(
                    dpp::embed_footer()
                    .set_text("this was requested by the kat, meow meow")
                    .set_icon("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTUaXJ0Iwv7GayqtAnXh5M-1UT_QZs7jDRC_g&s")
                )
                .set_timestamp(time(0));

            dpp::message msg(event.command.channel_id, Hex);

            event.reply(msg);
            debugger(event);
        }

        else if (event.command.get_command_name() == "bot_inquiry") {
            dpp::embed Beuro;
            
            Beuro
                .set_title("Beuro")
                .set_description("Discord Bot inspired by Neuro-sama")
                .set_colour(dpp::colors::golden_yellow)
                .set_author(
                    "Mithirilz",
                    "https://discordapp.com/users/640069711341813763",
                    "https://cdn.discordapp.com/attachments/890000456502362202/1442416084526301255/Mith_Developer_pfp.webp?ex=69255a29&is=692408a9&hm=1c30d7aa9ca6e4cceb05d6a73a48e479bd588c4d1438ad6c03e57bb573cb5bf2&"
                )
                .set_image("https://i.pinimg.com/736x/19/52/d8/1952d89936f262e5e98eb401ad425dcf.jpg")
                .add_field(
                    "About the bot:",
                    "This bot serves as a Tester and is the earliest version of Euro Bot Series.\n \n Newer bots are planned for development and are built upon this bot."
                )
                .add_field(
                    "Developer:",
                    "Mithirilz, Meth",
                    true
                )
                .add_field(
                    "Contributors:",
                    "None",
                    true
                )
                .add_field(
                    "Bot Version:",
                    "0.1.3",
                    true
                );

            dpp::message msg(event.command.channel_id, Beuro);

            event.reply(msg);
            debugger(event);    
        }

        else if (event.command.get_command_name() == "future_devs") {
            dpp::embed Explanation;

            Explanation
                .set_color(dpp::colors::golden_yellow)
                .set_author(
                    "Mithirilz",
                    "https://discordapp.com/users/640069711341813763",
                    "https://cdn.discordapp.com/attachments/890000456502362202/1442416084526301255/Mith_Developer_pfp.webp?ex=69255a29&is=692408a9&hm=1c30d7aa9ca6e4cceb05d6a73a48e479bd588c4d1438ad6c03e57bb573cb5bf2&"
                )
                .set_title("Future Development Plans")
                .set_image("https://i.pinimg.com/1200x/2f/b7/f0/2fb7f0eeac48317e044bc1e6a77b7b9f.jpg")
                .set_thumbnail("https://i.pinimg.com/736x/67/5a/44/675a445db1cab22e1ba41ac62170117c.jpg")
                .add_field(
                "Explanation:",
                "If I'm being honest, the future for this bot isn't clear as I'm still learning quite a lot of stuff. That's not to say there ISN'T a goal though."
                )
                .add_field(
                "Goal:",
               "- Finetune said AI",
           false
                )
                .add_field(
            "Progress:",
            "- Beuro has been turned into a discord bot officially!\n- Beuro now has some weird slashcommands\n- Beuro can now join VC\n- Beuro's a chatbot now",
            false
                )
                .set_footer(
                    dpp::embed_footer()
                    .set_text("This embed was formatted by Mithirilz.")
                    .set_icon("https://i.pinimg.com/736x/f0/36/a3/f036a3372724af62c16cf70fd2d8c583.jpg")
                )
                .set_timestamp(time(0));

            dpp::message msg(event.command.channel_id, Explanation);
            event.reply(msg);
            debugger(event);    
        }

        else if(event.command.get_command_name() == "join"){
            dpp::guild* guild = dpp::find_guild(event.command.guild_id);

            if(!guild->connect_member_voice(*event.owner, event.command.get_issuing_user().id, false, true)){
                event.co_reply("Failed to join VC...");
            }

            else if(event.command.get_issuing_user().id == 640069711341813763){
                event.reply("Hey there Creator");
                debugger(event);
            }

            else{
                event.reply("Hey there random person");
                debugger(event);
            }
        }
    });
    

    beuro.on_ready([&beuro](const dpp::ready_t& event){
        if (dpp::run_once<struct register_bot_commands>()) {

            //For the Euro Dev Platform
            beuro.guild_bulk_command_create({
                dpp::slashcommand("join", "Beuro VC test", beuro.me.id),
                dpp::slashcommand("ai_baby", "golshi", beuro.me.id),
                dpp::slashcommand("neuro", "im thinking neuro", beuro.me.id),
                dpp::slashcommand("bot_inquiry", "What's the bot about?", beuro.me.id),
                dpp::slashcommand("future_devs", "What is the future for the Euro Series?", beuro.me.id)
            }, 1442447584596721705);

            //For World Descention
            beuro.guild_bulk_command_create({
                dpp::slashcommand("ai_baby", "golshi", beuro.me.id),
                dpp::slashcommand("hex", "Hex the Kat", beuro.me.id),
                dpp::slashcommand("join", "Beuro VC test", beuro.me.id),
                dpp::slashcommand("neuro", "im thinking neuro", beuro.me.id),
                dpp::slashcommand("bot_inquiry", "What's the bot about?", beuro.me.id),
                dpp::slashcommand("future_devs", "What is the future for the Euro Series?", beuro.me.id),
                dpp::slashcommand("soyjack", "Soyjack, nothing happened...", beuro.me.id)
            }, 1118551179518033931);

            //For Solo Dev Platform
            beuro.guild_bulk_command_create({
                dpp::slashcommand("join", "Beuro VC test", beuro.me.id),
                dpp::slashcommand("ai_baby", "golshi", beuro.me.id),
                dpp::slashcommand("hex", "Hex the Kat", beuro.me.id),
                dpp::slashcommand("neuro", "im thinking neuro", beuro.me.id),
                dpp::slashcommand("bot_inquiry", "What's the bot about?", beuro.me.id),
                dpp::slashcommand("future_devs", "What is the future for the Euro Series?", beuro.me.id),
                dpp::slashcommand("soyjack", "Soyjack, nothing happened...", beuro.me.id)
            }, 889999517758398494);
        }
    });

    beuro.on_message_create([&beuro, &beuro_exec](const dpp::message_create_t& event) -> dpp::task<void> {
        if (event.msg.author.id == 640069711341813763){
            co_await owner_message_commands(event, beuro, beuro_exec);
            co_return;
        }
        
        else{
            co_await general_message_commands(event, beuro, beuro_exec);
            co_return;
        }
    });

    beuro.on_voice_state_update([](const dpp::voice_state_update_t& event){
        dpp::snowflake Owners_ID = 640069711341813763;
        auto GuildVC = dpp::find_guild(event.state.guild_id);
        auto Mithirilz = GuildVC->voice_members.find(Owners_ID);

        if (Mithirilz == GuildVC->voice_members.end()){
            event.from()->disconnect_voice(event.state.guild_id);
            return;
        }
    });
        
    beuro.start(dpp::st_wait);
}

dpp::task<void> owner_message_commands(const dpp::message_create_t& event, dpp::cluster& beuro, BeuroAI& beuro_exec){
    if(event.msg.content.find("<@" + std::to_string(beuro.me.id) + ">") != std::string::npos || event.msg.is_dm()) {
        co_await beuro_exec.Beuro_Response(event.msg.content, event, beuro);
        co_return;
    }

    else if(event.msg.content.find("Beuro shutdown") != std::string::npos){
        co_await event.co_reply("Shutting down in 5 seconds...");
        co_await delay();
        beuro.shutdown();
    }
}

dpp::task<void> general_message_commands(const dpp::message_create_t& event, dpp::cluster& beuro, BeuroAI& beuro_exec){
    if(event.msg.content.find("<@" + std::to_string(beuro.me.id) + ">") != std::string::npos && !event.msg.is_dm()){
        co_await beuro_exec.Beuro_Response(event.msg.content, event, beuro);
        co_return;
    }

    else if(event.msg.content.find("Beuro shutdown") != std::string::npos || event.msg.content.find("beuro shutdown") != std::string::npos){
        co_await event.co_reply("Written by Mithirilz: \"sybau\"");
        co_return;
    }
}

dpp::task<void> delay(){
    std::this_thread::sleep_for(std::chrono::seconds(5));
    co_return;
}

dpp::job debugger(const dpp::slashcommand_t event){
    std::cout << event.command.get_issuing_user().username << " issued a command." << std::endl;
    co_return;
}