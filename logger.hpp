#ifndef ZIFMANN_LOGGER_HPP
#define ZIFMANN_LOGGER_HPP


#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <ctime>
#include <stdarg.h>
#include <iostream>

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define log_debug(message, ...) log(zifmann::logger::core::logger_level::Debug, message, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define log_error(message, ...) log(zifmann::logger::core::logger_level::Error, message, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define log_info(message, ...) log(zifmann::logger::core::logger_level::Info, message, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define log_warn(message, ...) log(zifmann::logger::core::logger_level::Warning, message, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define log_assert(condition, message, ...) assert(condition, message, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define dump(path) zifmann::logger::core::dump_logs(path)

#endif

namespace zifmann::logger::core {
	
	enum logger_level {
		Warning,
		Debug,
		Error,
		Info
	};

	static std::vector<std::string> logs;
	static std::string filepath;

	// @param level The logger level for the current log
	// @param format The format of the string to built from the given arguments
	// @param filename The name of the file from where the function is being called
	// @param line_number The line number of the line calling this function
	void log(const logger_level level, const std::string &format, const std::string &filename, const int line_number, ...);

	void assert(const bool expression, const std::string &format, const std::string &filename, const int line_number, ...);

	// @param filepath The full path of the file where to dump the logs
	void dump_logs(const std::string &filepath);

	#define BLK "\e[1;30m"
	#define RED "\e[1;31m"
	#define GRN "\e[1;32m"
	#define YEL "\e[1;33m"
	#define BLU "\e[1;34m"
	#define MAG "\e[1;35m"
	#define CYN "\e[1;36m"
	#define WHT "\e[1;37m"
	#define RESET "\e[0m"
	
	void init_logger(const std::string &path) {
		filepath = path;
	}

	void log(const logger_level level, const std::string &format, const std::string &filename, const int line_number, ...) {
		va_list args;
		va_start(args, line_number);
		std::stringstream string_builder;
		auto t = time(nullptr);
		tm *ltm = localtime(&t);
		switch (level) {
			case logger_level::Error:
				string_builder << RED << "Error**" << RESET;
				break;
			case logger_level::Debug:
				string_builder << WHT << "Debug**" << RESET;
				break;
			case logger_level::Warning:
				string_builder << YEL << "Warn**" << RESET;
				break;
			case logger_level::Info:
				string_builder << BLU << "Info**" << RESET;
				break;
		}
		string_builder << "\t (" << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << ") ";
		string_builder << "in " << filename << ":" << line_number << ": ";
		bool format_start = false;
		for (size_t i = 0; i < format.length(); i++) {
			if (format_start) {
				switch (format[i]) {
					case '%':
						string_builder << '%';
						break;
					case 'i': {
						int i = (int) va_arg(args, int);
						string_builder << i;
						break;
					}
					case 'f':
					case 'd': {
						double d = (double) va_arg(args, double);
						string_builder << d;
						break;
					}
					case 'b': {
						int b = (int) va_arg(args, int);
						string_builder << (b ? "true" : "false");
						break;
					}
					case 's': {
						const char *s = (const char*) va_arg(args, const char*);
						string_builder << s;
						break;
					}
					case 'c': {
						char c = (char) va_arg(args, int);
						string_builder << c;
						break;
					}
					default:
						log_error("Invalid format specified!");
				}
				format_start = false;
			} else {
				if (format[i] == '%') format_start = true;
				else string_builder << format[i];
			}
		}
		va_end(args);
		logs.push_back(string_builder.str());
        if (level == zifmann::logger::core::Error) std::cerr << string_builder.str() << "\n";
        else std::cout << string_builder.str() << "\n";
	}

	void assert(const bool expression, const std::string &format, 
				const std::string &filename, const int line_number, ...) {
		if (!expression) {
			va_list list;
			va_start(list, line_number);
			log(logger_level::Error, "Assertion Error: " + format, filename, line_number, list);
			va_end(list);
			exit(1);
		}
	}
	
	void dump_logs(const std::string &filepath) {
		std::stringstream path_formatter;
		auto t = time(nullptr);
		tm *ltm = localtime(&t);
		path_formatter << "log_" << ltm->tm_mday << "_" << ltm->tm_mon << "_" 
				<< (ltm->tm_year + 1900) << "_" << ltm->tm_hour << "_" 
				<< ltm->tm_min << "_" << ltm->tm_sec << ".txt";
		std::ofstream logfile(path_formatter.str(), std::ios::out);
		for (auto& str : logs) {
			logfile << str << "\n";
		}
		logfile.close();
	}


}

