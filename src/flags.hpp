#pragma once

#include "frontend/utils/results.hpp"
#include <cstdlib>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef ISA_LLVM_FLAGS_COMPILER
#define ISA_LLVM_FLAGS_COMPILER

enum class FlagType {
    Bool,
    String
};

enum class FlagID {
    RunInputFile,
    BuildInputFile,
    OutputFile,
    GenerateBytecode,
    EmittLexer,
    Help
};

struct Flag {
    FlagType type;
    FlagID id;
    std::string longName;
    std::optional<std::string> value;
    bool active = false;
};

static const char* flagDescriptions[] = {
    "--output, -o <file>        Specify the output file name.",
    "--running, run <file>      Run the specified input file.",
    "--building, build <file>   Build (compile) the specified input file.",
    "--bytecode, -b             Generate bytecode instead of native code.",
    "--lexercode, -lex          Emit only the lexer code.",
    "--helper, -h               Display this help message."
};

class FlagParser {
  std::stringstream err;
public:
    FlagParser() {
        registerFlag("-o", "--output", FlagType::String, FlagID::OutputFile);
        registerFlag("run", "--running", FlagType::String, FlagID::RunInputFile);
        registerFlag("build", "--building", FlagType::String, FlagID::BuildInputFile);
        registerFlag("-b", "--bytecode", FlagType::Bool, FlagID::GenerateBytecode);
        registerFlag("-lex", "--lexercode", FlagType::Bool, FlagID::EmittLexer);
        registerFlag("-h", "--helper", FlagType::Bool, FlagID::Help);
    }

    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);

            auto it = flagsByName.find(arg);
            if (it == flagsByName.end()) {
                err << "Unknown flags: " << arg << '\n';
                return false;
            }

            Flag& flag = flags[it->second];
            flag.active = true;

            if (flag.type == FlagType::String) {
                if (i + 1 >= argc) {
                    err << "Flag " << arg << " requires a value.\n";
                    return false;
                }
                flag.value = argv[++i];
            }
        }
        return true;
    }

    bool isActive(FlagID id) const {
        for (const auto& [_, flag] : flags) {
            if (flag.id == id)
                return flag.active;
        }
        return false;
    }

    Result<std::string> getValue(FlagID id) const {
        for (const auto& [_, flag] : flags) {
            if (flag.id == id && flag.value)
                return Result<std::string>::Ok(flag.value->c_str());
        }
        return Result<std::string>::Err("Invaled flags");
    }

    Result<std::string> getValue(std::vector<FlagID> id) const {
        for(auto& fl : id ){
            if(isActive(fl)) {
                for (const auto& [_, flag] : flags) {
                    if (flag.id == fl && flag.value)
                        return Result<std::string>::Ok(flag.value->c_str());
                }
            }
        }
        return Result<std::string>::Err("Invaled flags");
    }

    Result<std::string> getErr() const {
      return Result<std::string>::Err(err.str());
    }
private:
    void registerFlag(std::string shortName, std::string longName, FlagType type, FlagID id) {
        Flag flag{type, id, longName};
        flags[shortName] = flag;
        flagsByName[shortName] = shortName;
        flagsByName[longName] = shortName;
    }

    std::unordered_map<std::string, Flag> flags;
    std::unordered_map<std::string, std::string> flagsByName;
};

#endif // ISA_LLVM_FLAGS_COMPILER
