#ifndef LOGGER_H
#define LOGGER_H

#include <string>

void init_log_file(const std::string& filename);
void log_message(const std::string& message);
void close_log_file();

#endif // LOGGER_H



