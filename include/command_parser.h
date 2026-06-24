#ifndef CACHELITE_COMMAND_PARSER_H
#define CACHELITE_COMMAND_PARSER_H

#include <string>
#include <vector>

struct ParsedCommand {
    std::string name;
    std::vector<std::string> args;
    std::string error; // empty if valid
};

class CommandParser {
public:
    static ParsedCommand parse(const std::string& raw_input);
};

#endif // CACHELITE_COMMAND_PARSER_H
