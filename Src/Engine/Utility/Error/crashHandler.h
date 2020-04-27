//
// Created by Kelvin Macartney on 22/03/2020.
//

#ifndef VENUS_CRASHHANDLER_H
#define VENUS_CRASHHANDLER_H

#include <string>
#define MAX_STACKTRACE_DEPTH 200
#define MAX_STACKTRACE_NAME_BYTES 1024
namespace Venus {
    class CrashHandler {
    public:
        /**
         * Records a crash with a custom error message.
         *
         * @param[in]	type		Type of the crash that occurred.
         * @param[in]	description	More detailed description of the issue that caused the crash.
         * @param[in]	function	Optional name of the function where the error occurred.
         * @param[in]	file		Optional name of the source code file in which the code that crashed the program exists.
         * @param[in]	line		Optional source code line at which the crash was triggered at.
         */
        static void reportCrash(const std::string &type, const std::string &description, const std::string &function = "",
                         const std::string &file = "", uint32_t line = 0);

    private:
        static std::string getStackTrace();
    };
}


#endif //VENUS_CRASHHANDLER_H
