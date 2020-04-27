//
// Created by Kelvin Macartney on 18/03/2020.
//

#ifndef VENUS_VENUSEXCEPTIONS_H
#define VENUS_VENUSEXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>
#include <sstream>
#include "Error/crashHandler.h"

namespace Venus {

    /**
	 * Base class for exceptions.
	 */
    class Exception : public std::exception {
    public:
        Exception(const char *type, std::string description, std::string source)
                : mTypeName(type), mDescription(std::move(description)), mSource(std::move(source)) {}

        Exception(const char *type, std::string description, std::string source, const char *file, long line)
                : mLine(line), mTypeName(type), mDescription(std::move(description)), mSource(std::move(source)),
                  mFile(file) {}

        Exception(const Exception &rhs)
                : mLine(rhs.mLine), mTypeName(rhs.mTypeName), mDescription(rhs.mDescription),
                  mSource(rhs.mSource), mFile(rhs.mFile) {}

        ~Exception() noexcept override = default;

        void operator=(const Exception &rhs) {
            mDescription = rhs.mDescription;
            mSource = rhs.mSource;
            mFile = rhs.mFile;
            mLine = rhs.mLine;
            mTypeName = rhs.mTypeName;

        }

        /**
         * Returns a string with the full description of the exception.
         *
         * @note
         * The description contains the error number, the description supplied by the thrower, what routine threw the
         * exception, and will also supply extra platform-specific information where applicable.
         */
        virtual const std::string &getFullDescription() const {
            if (mFullDesc.empty()) {
                std::stringstream desc;

                desc << "Venus::Engine EXCEPTION(" << mTypeName << "): "
                     << mDescription
                     << " in " << mSource;

                if (mLine > 0) {
                    desc << " at " << mFile << " (line " << mLine << ")";
                }

                mFullDesc = desc.str();
            }

            return mFullDesc;
        }

        /** Gets the source function that threw the exception. */
        virtual const std::string &getSource() const { return mSource; }

        /** Gets the source file name in which the exception was thrown. */
        virtual const std::string &getFile() const { return mFile; }

        /** Gets line number on which the exception was thrown. */
        virtual long getLine() const { return mLine; }

        /** Gets a short description about the exception. */
        virtual const std::string &getDescription(void) const { return mDescription; }

        /** Overridden std::exception::what. Returns the same value as getFullDescription(). */
        const char *what() const noexcept override { return getFullDescription().c_str(); }

    protected:
        long mLine = 0;
        std::string mTypeName;
        std::string mDescription;
        std::string mSource;
        std::string mFile;
        mutable std::string mFullDesc;
    };

    /**
     * Exception for signaling an internal error, normally something that shouldn't have happened or wasn't anticipated by
     * the programmers of that system.
     */
    class InternalErrorException : public Exception {
    public:
        InternalErrorException(const std::string &inDescription, const std::string &inSource, const char *inFile,
                               long inLine)
                : Exception("InternalErrorException", inDescription, inSource, inFile, inLine) {}
    };

    /**
     * Exception for signaling that the specific method has not been implemented
     */
    class NotImplementedException : public Exception {
    public:
        NotImplementedException(const std::string &inDescription, const std::string &inSource, const char *inFile,
                                long inLine)
                : Exception(" NotImplementedException", inDescription, inSource, inFile, inLine) {}
    };

    /**
     * The exception that is thrown when a method call is invalid for the object's current state.
     */
    class InvalidOperationException : public Exception {
    public:
        InvalidOperationException(const std::string &inDescription, const std::string &inSource, const char *inFile,
                                  long inLine)
                : Exception("InvalidOperationException", inDescription, inSource, inFile, inLine) {}
    };


#ifndef VENUS_EXCEPT
#define VENUS_EXCEPT(type, desc)                                                                  \
    {                                                                                             \
        static_assert((std::is_base_of<Venus::Exception, type>::value),                           \
            "Invalid exception type (" #type ") for VENUS_EXCEPT macro."                          \
            "Must derive from Engine::Exception.");                                               \
        Venus::CrashHandler::reportCrash(#type, desc, __PRETTY_FUNCTION__, __FILE__, __LINE__);   \
        abort();                                                                                  \
    }
#endif
}

#endif //VENUS_VENUSEXCEPTIONS_H
