#include "cli.h"

// Splits a string by a delimiter
vector<string> getCommand() {
    string command;
    console.log("> ", {BLUE});
    getline(std::cin, command);

    return splitString(command, ' ');
}

ParsedCommand parseCommand(vector<string> &args) {
    ParsedCommand result;
    result.cmd = toLowerCase(args[0]);
    if (args[0] == "cf" || args[0] == "codeframe")
        result.cmd = toLowerCase(args[1]);

    args.erase(args.begin());

    if (args.empty()) {
        result.args.push_back("");
        return result;
    }

    for (const auto &arg : args) {
        result.args.push_back(arg);
    }

    return result;
};

// Help Command
void helpMenu(const vector<CommandDefinition> &commands, const optional<rgb> &primary, const optional<rgb> &secondary) {
    console.logHeader(" HELP ");
    console.logLn();

    for (size_t i = 0; i < commands.size(); i++) {
        const CommandDefinition current = commands[i];

        if (current.info.cmd.empty()) // Skip Over Empty Spots in The Array
            continue;

        console.log(current.info.cmd + " | ", {MAGENTA});
        string aliases;
        for (const auto &alias : current.info.aliases) {
            aliases += alias + " ";
        }
        console.logLn(aliases);
        console.log(current.info.name + ": " + current.info.description, {GRAY});

        console.logLn();
        console.logLn();
    }
}