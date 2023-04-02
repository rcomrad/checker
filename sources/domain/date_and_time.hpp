#ifndef DATE_AND_TIME_HPP
#define DATE_AND_TIME_HPP

#include <string>

namespace dom
{
class DateAndTime
{
public:
    static std::string getCurentTime();
    static int64_t getCurentTimeNum();
};
} // namespace dom

//--------------------------------------------------------------------------------

#endif // !DATE_AND_TIME_HPP
