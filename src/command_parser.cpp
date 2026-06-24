#include "command_parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

ParsedCommand CommandParser::parse(const std::string& raw_input) {
    ParsedCommand cmd;
    std::istringstream iss(raw_input);
    std::string token;

    if (iss >> token) {
        // Convert command name to uppercase
        std::transform(token.begin(), token.end(), token.begin(),
            [](unsigned char c){ return std::toupper(c); });
        cmd.name = token;

        while (iss >> token) {
            cmd.args.push_back(token);
        }
    } else {
        cmd.error = "ERR empty command";
    }

    return cmd;
}
