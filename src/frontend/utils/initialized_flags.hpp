#pragma once 


#include "flags.hpp"
#include "frontend/utils/results.hpp"
#include <string>


static FlagParser flags;


static Result<std::string> inicialize_flags(int argc, char **argsv) {
    if (!flags.parse(argc, argsv)) {
        return flags.getErr();
    }
    return Result<std::string>::Ok("Initialized flags");
}