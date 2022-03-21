#ifndef STRINGUTILITY_H
#define STRINGUTILITY_H

#include <string>


class StringUtility
{
public:

    static std::string Trim(const std::string& str,
                            const std::string& whitespace = " \t\n");


    static std::string Reduce(const std::string& str,
                              const std::string& fill = " ",
                              const std::string& whitespace = " \t\n");

private:
    StringUtility() = delete;
};

#endif // STRINGUTILITY_H
