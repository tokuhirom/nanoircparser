/* vim: set filetype=cpp: */
/**
 * This is yet another IRC protocol parser.
 * Copyright (C) 2012 Tokuhiro Matsuno.
 *
 * http://tokuhirom.mit-license.org/
 */

/**
 * SEE http://tools.ietf.org/html/rfc1459#page-8 for protocol details.
 */

#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NANOIRCPARSER_MESSAGE_IS_PARTIAL -2
#define NANOIRCPARSER_PARSE_FAILED -2

class NanoIRCParser {
public:
    struct Message {
        std::string prefix;
        std::string command;
        std::vector<std::string> params;
    };

    int parse(
        std::string src,
        Message &msg) {

        char *cursor = (char*)src.c_str();
        char *orig = cursor;
        char *marker = cursor;
        char *mark2 = cursor;
        
    /*!re2c
        re2c:define:YYCTYPE  = "char";
        re2c:define:YYCURSOR = cursor;
        re2c:define:YYMARKER = marker;
        re2c:yyfill:enable   = 0;
        re2c:yych:conversion = 1;
        re2c:indent:top      = 1;

        CRLF = "\015\012";
        SPACE = "\x20" ("\x20")*;
        NICK = [A-Za-z] [A-Za-z0-9\-\[\]\\\`\^\{\}]*;
        USER = [^\x20]+;
        IP = ([0-7]+ "." [0-7]+ "." [0-7]+ "." [0-7]+);
        HOST = IP | [0-9a-z-\.]+;
        ANY_CHARACTER = [^];
        LETTER_COMMAND = [A-Za-z]+;
        NUM_COMMAND = [0-9]{3};
        SERVERNAME = HOST;
        USERINFO = NICK ( "!" USER )? ( "@" HOST )?;
        PREFIX = USERINFO | SERVERNAME;
        COMMAND = LETTER_COMMAND | NUM_COMMAND;
        TRAILING = [^\000\015\012]*;
        MIDDLE = [^:] [^\x20\015\012]*;

        */

        /*
        * Note. fix HOST regexp to better one.
        */

#define NNMARK(x) do { \
        std::string text(mark2, cursor-mark2); \
        msg.x = text; \
    } while(0)

        mark2 = cursor;
    /*!re2c
        ":" {
            // ":" prefix SPACE
            goto prefix;
        }
        COMMAND {
            NNMARK(command);
            goto params_space;
        }
        ANY_CHARACTER { return return_error(src); }
        */

    command_only:
        mark2 = cursor;
    /*!re2c
        COMMAND {
            NNMARK(command);
            goto params_space;
        }
        ANY_CHARACTER { return return_error(src); }
        */

    prefix:
        // PREFIX = SERVERNAME | NICK ("!" USER)? ("@" HOST)?
        mark2 = cursor;
    /*!re2c
        PREFIX {
            NNMARK(prefix);
            goto prefix_space;
        }
        ANY_CHARACTER { return return_error(src); }
        */

#undef NNMARK

    prefix_space:
    /*!re2c
        SPACE {
            goto command_only;
        }
        ANY_CHARACTER { return return_error(src); }
        */

    params_space:
    /*!re2c
        SPACE {
            goto params;
        }
        ANY_CHARACTER { return return_error(src); }
        */

    after_params_middle:
    /*!re2c
        SPACE {
            goto params;
        }
        CRLF {
            goto success;
        }
        ANY_CHARACTER { return return_error(src); }
        */

    params:
        // <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
        mark2 = cursor;
    /*!re2c
        ":" TRAILING {
            std::string text(mark2+1, cursor-mark2-1);
            msg.params.push_back(text);
            goto crlf;
        }
        MIDDLE {
            std::string text(mark2, cursor-mark2);
            msg.params.push_back(text);
            goto after_params_middle;
        }
        ANY_CHARACTER { return return_error(src); }
        */

    crlf:
        mark2 = cursor;
    /*!re2c
        CRLF {
            goto success;
        }
        ANY_CHARACTER {
            return return_error(src);
        }
        */
    success:
        return cursor-orig;
    }

    int return_error(const std::string &src) {
        return memchr(src.c_str(), '\015', src.size()) == NULL ? NANOIRCPARSER_MESSAGE_IS_PARTIAL : NANOIRCPARSER_PARSE_FAILED;
    }
};

