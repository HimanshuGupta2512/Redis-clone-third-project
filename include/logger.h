#ifndef CACHELITE_LOGGER_H
#define CACHELITE_LOGGER_H

#include <string>
#include <mutex>
#include <iostream>

class Logger {
public:
    static void log(const std::string& message);
    static void error(const std::string& message);

private:
    static std::mutex log_mutex;
};

#endif // CACHELITE_LOGGER_H
