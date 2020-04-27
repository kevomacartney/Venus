//
// Created by Kelvin Macartney on 22/03/2020.
//

#include "crashHandler.h"
#include <spdlog/spdlog.h>
#include <string>
#include <sstream>
#include <backward.hpp>
#include <cxxabi.h>
#include <csignal>

void
Venus::CrashHandler::reportCrash(const std::string &type, const std::string &description, const std::string &function,
                                 const std::string &file, uint32_t line) {
    std::string stack = getStackTrace(); // Get stack early before saturating it

    std::stringstream ss;
    ss << "Exception: " << type << "\n";
    ss << "File: " << file;
    ss << " Line: " << line << "\n";
    ss << "Description: " << description << "\n";
    ss << "\n";
    ss << stack;

    std::cerr << ss.str();

    std::raise(SIGINT); // give a debugger the opportunity yo attach

    // TODO save crash log with system details
}

std::string Venus::CrashHandler::getStackTrace() {
    backward::StackTrace st;
    st.load_here(200);

    backward::TraceResolver tr;
    tr.load_stacktrace(st);

    std::stringstream ss;
    for (size_t i = 0; i < st.size(); ++i) {
        backward::ResolvedTrace trace = tr.resolve(st[i]);
        ss << "#" << i
           << " " << trace.object_filename
           << " " << trace.object_function
           << " [" << trace.addr << "]"
           << std::endl;
    }

    return ss.str();
}
