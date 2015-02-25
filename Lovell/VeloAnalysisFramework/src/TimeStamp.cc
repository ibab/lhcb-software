/** @file TimeStamp.cc
 *
 * @brief class representing a time stamp
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 */
#include <cstring>
#include <ctime>

#include "TimeStamp.h"


TimeStamp::TimeStampException::TimeStampException(const char* str) :
    m_str(str)
{ }

const char* TimeStamp::TimeStampException::what() const throw()
{ return m_str; }

TimeStamp::TimeStamp() : m_time(static_cast<ULong64_t>(time(NULL)))
{ }

TimeStamp::TimeStamp(ULong64_t timestamp) : m_time(timestamp)
{ }

TimeStamp::TimeStamp(const std::string& timestamp) :
    m_time(fromStr(timestamp.c_str()))
{ }

TimeStamp::TimeStamp(const char* timestamp) : m_time(fromStr(timestamp))
{ }

ULong64_t TimeStamp::fromStr(const char* str)
{
    struct tm tm;
    std::memset(&tm, 0, sizeof(struct tm));
    // expected time format "Mon, 15 Apr 2013 15:20:14 +0200"
    const char *rstr = strptime(str, "%a, %d %b %Y %H:%M:%S %z", &tm);
    if (!rstr || rstr < str)
	throw TimeStampException("Unable to parse date string.");
    time_t t = mktime(&tm);
    if (static_cast<time_t>(-1) == t)
	throw TimeStampException("Unable to convert broken-down time to time_t");
    return ULong64_t(t);
}

ULong64_t TimeStamp::value() const
{ return m_time; }

std::string TimeStamp::toString() const
{
    static bool tzsetdone = false;
    if (!tzsetdone) {
	tzset();
	tzsetdone = true;
    }
    const time_t t = m_time;
    struct tm tm, *rtm;
    rtm = localtime_r(&t, &tm);
    if (rtm != &tm)
	throw TimeStampException("Unable to convert to broken-down time.");
    // format time into the format "Mon, 15 Apr 2013 15:20:14 +0200"
    char buf[64];
    size_t sz = strftime(buf, sizeof(buf) - 1, "%a, %d %b %Y %H:%M:%S %z", &tm);
    buf[sizeof(buf) - 1] = 0;
    if (sz >= sizeof(buf) - 1)
	throw TimeStampException("Buffer for formatted time string too small."); 
    return std::string(buf);
}

TimeStamp::operator const std::string() const
{ return toString(); }

bool TimeStamp::operator==(const TimeStamp& other) const
{ return m_time == other.m_time; }

bool TimeStamp::operator!=(const TimeStamp& other) const
{ return m_time != other.m_time; }

bool TimeStamp::operator<(const TimeStamp& other) const
{ return m_time < other.m_time; }

bool TimeStamp::operator>(const TimeStamp& other) const
{ return m_time > other.m_time; }

bool TimeStamp::operator<=(const TimeStamp& other) const
{ return m_time <= other.m_time; }

bool TimeStamp::operator>=(const TimeStamp& other) const
{ return m_time >= other.m_time; }

TimeStamp& TimeStamp::operator+=(Long64_t nsec)
{ m_time += nsec; return *this; }
TimeStamp& TimeStamp::operator-=(Long64_t nsec)
{ m_time -= nsec; return *this; }
TimeStamp TimeStamp::operator+(Long64_t nsec) const
{ return TimeStamp(m_time + nsec); }
TimeStamp TimeStamp::operator-(Long64_t nsec) const
{ return TimeStamp(m_time - nsec); }

Long64_t TimeStamp::operator-(const TimeStamp& other) const
{ return m_time - other.m_time; }

// vim: tw=78:sw=4:ft=cpp
