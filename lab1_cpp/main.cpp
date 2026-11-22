#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stack>
#include <string>

enum class LogLevel {
    TRACE,
    INFO,
    WARN,
    ERROR
};

class Logger {
private:
    const std::string name;

    static LogLevel level;
    static std::string format;

    static std::string levelToString(LogLevel lvl) {
        switch (lvl) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
        }
        return "UNKNOWN";
    }

    static std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        char buf[16];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now));
        return buf;
    }

    static std::string getRandomMessage(LogLevel lvl) {
        // map
        static const std::string phrases[4][3] = {
            {"Inspecting token", "Scanning character", "Pushing to stack"},
            {"Operation completed", "Everything looks good", "Here is your result"},
            {"Suspicious situation", "Potential imbalance", "Unexpected token order"},
            {"Operation failed", "Invalid input detected", "Validation error"}
        };
        return phrases[static_cast<int>(lvl)][std::rand() % 3];
    }

public:
    explicit Logger(const std::string& loggerName) : name(loggerName) {}

    static void setLevel(LogLevel newLevel) {
        level = newLevel;
    }

    static void setFormat(const std::string& newFormat) {
        format = newFormat;
    }

    void log(LogLevel msgLevel, const std::string& message) const {
        if (msgLevel < level) {
            return;
        }

        for (std::size_t i = 0; i < format.size(); ++i) {
            if (format[i] == '%' && i + 1 < format.size()) {
                switch (format[++i]) {
                    case 't': std::cout << getCurrentTime(); break;
                    case 'l': std::cout << levelToString(msgLevel); break;
                    case 'n': std::cout << name; break;
                    case 'm': std::cout << message; break;
                    case 'r': std::cout << getRandomMessage(msgLevel); break;
                    default:  std::cout << '%' << format[i]; break;
                }
            } else {
                std::cout << format[i];
            }
        }
        std::cout << '\n';
    }

    void trace(const std::string& message) const { log(LogLevel::TRACE, message); }
    void info(const std::string& message)  const { log(LogLevel::INFO, message); }
    void warn(const std::string& message)  const { log(LogLevel::WARN, message); }
    void error(const std::string& message) const { log(LogLevel::ERROR, message); }
};
LogLevel Logger::level = LogLevel::INFO;
std::string Logger::format = "[%t] [%l] %n: %m";

class ParenthesesValidator {
private:
    const Logger logger{"Parentheses"};

public:
    bool isValid(const std::string& input) const {
        std::stack<std::size_t> openIndices;

        for (std::size_t i = 0; i < input.size(); ++i) {
            if (input[i] == '(') {
                openIndices.push(i);
                logger.trace("Found '(' at index " + std::to_string(i));
            } else if (input[i] == ')') {
                if (openIndices.empty()) {
                    logger.warn("Closing ')' at index " + std::to_string(i) + " has no matching '('");
                    logger.error("Invalid input \"" + input + "\": unmatched ')' at index " + std::to_string(i));
                    logger.info("Validation result for \"" + input + "\": false");
                    return false;
                }

                std::size_t openIdx = openIndices.top();
                openIndices.pop();
                logger.info("Matched '(' at index " + std::to_string(openIdx) + " with ')' at index " + std::to_string(i));
            }
        }

        if (!openIndices.empty()) {
            logger.warn("Reached end of input with " + std::to_string(openIndices.size()) + " unclosed '('");
            logger.error("Invalid input \"" + input + "\": missing closing parenthesis");
            logger.info("Validation result for \"" + input + "\": false");
            return false;
        }

        logger.info("Validation result for \"" + input + "\": true");
        return true;
    }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    ParenthesesValidator validator;

    std::cout << "--- 1. Required Test Cases (Level: TRACE) ---\n";
    Logger::setLevel(LogLevel::TRACE);

    const std::string testCases[] = {"()", ")(()))", "(", "(())((()())())"};
    for (const std::string& test : testCases) {
        std::cout << "\nInput: \"" << test << "\"\n";
        validator.isValid(test);
    }

    std::cout << "\n--- 2. Level Filtering Test (Level: INFO) ---\n";
    Logger::setLevel(LogLevel::INFO);
    Logger::setFormat("[%t]: %m");
    validator.isValid("(())");

    std::cout << "\n--- 3. Extra Credit Format (%r) ---\n";
    Logger::setFormat("[%t] [%l] %n: %m (%r)");
    validator.isValid("()");

    return 0;
}
