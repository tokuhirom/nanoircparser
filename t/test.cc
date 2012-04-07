#include "nanotap.h"
#include "../nanoircparser.hpp"

int main() {
    {
        NanoIRCParser parser;
        NanoIRCParser::Message msg;
        is(parser.parse(std::string(":"), msg), NANOIRCPARSER_MESSAGE_IS_PARTIAL);
    }

    {
        NanoIRCParser parser;
        NanoIRCParser::Message msg;
        is(parser.parse(std::string("e\015\012"), msg), NANOIRCPARSER_PARSE_FAILED);
    }

    {
        NanoIRCParser parser;
        NanoIRCParser::Message msg;
        std::string src("PRIVMSG #test hello?\015\012");
        is(parser.parse(src, msg), (int)src.size());
        is(msg.command, "PRIVMSG");
        is(msg.prefix, "");
        is(msg.params.size(), (size_t)2);
        is(msg.params.at(0), std::string("#test"));
        is(msg.params.at(1), std::string("hello?"));
    }

    {
        // with trash
        NanoIRCParser parser;
        NanoIRCParser::Message msg;
        std::string src("PRIVMSG #test hello?\015\012XXX");
        is(parser.parse(src, msg), (int)src.size()-3);
        is(msg.command, "PRIVMSG");
        is(msg.params.size(), (size_t)2);
        is(msg.params.at(0), std::string("#test"));
        is(msg.params.at(1), std::string("hello?"));
    }

    {
        NanoIRCParser parser;
        NanoIRCParser::Message msg;
        std::string src(":cameron.freenode.net NOTICE * :*** Looking up your hostname...\015\012");
        is(parser.parse(src, msg), (int)src.size());
        is(msg.prefix, "cameron.freenode.net");
        is(msg.command, "NOTICE");
        is(msg.params.size(), (size_t)2);
        is(msg.params.at(0), std::string("*"));
        is(msg.params.at(1), std::string("*** Looking up your hostname..."));
    }

    {
        NanoIRCParser parser;
        NanoIRCParser::Message msg;
        std::string src(":tkhrm!~tk@www4649ub.example.jp JOIN #hoge\015\012");
        is(parser.parse(src, msg), (int)src.size());
        is(msg.prefix, "tkhrm!~tk@www4649ub.example.jp");
        is(msg.command, "JOIN");
        is(msg.params.size(), (size_t)1);
        is(msg.params.at(0), std::string("#hoge"));
    }

    {
        NanoIRCParser parser;
        NanoIRCParser::Message msg;
        std::string src(":zelazny.freenode.net 376 tkhrm :End of /MOTD command.\015\012");
        is(parser.parse(src, msg), (int)src.size());
        is(msg.prefix, "zelazny.freenode.net");
        is(msg.command, "376");
        is(msg.params.size(), (size_t)2);
        is(msg.params.at(0), std::string("tkhrm"));
        is(msg.params.at(1), std::string("End of /MOTD command."));
    }

    done_testing();
    return 0;
}

