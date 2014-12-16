/** @file TimeStamp.h
 *
 * @brief class representing a time stamp
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 */
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <string>

#include <Rtypes.h>

/** @brief a time stamp
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 *
 * A class representing a time stamp with second granularity.
 */
class TimeStamp
{
    public:
	/// constructor (initialised to now)
	TimeStamp();
	/// constructor
	TimeStamp(ULong64_t timestamp);
	/// constructor
	TimeStamp(const std::string& timestamp);
	/// constructor
	TimeStamp(const char* timestamp);

	/// return time stamp
	ULong64_t value() const;

	/// return as string
	std::string toString() const;
	/// return as string
	operator const std::string() const;

	// comparison operators
	/// returns if two time stamps are equal
	bool operator==(const TimeStamp& other) const;
	/// returns if two time stamps are not equal
	bool operator!=(const TimeStamp& other) const;
	/// return if one time stamp is smaller than the other
	bool operator<(const TimeStamp& other) const;
	/// return if one time stamp is larger than the other
	bool operator>(const TimeStamp& other) const;
	/// return if one time stamp is smaller or equal than the other
	bool operator<=(const TimeStamp& other) const;
	/// return if one time stamp is larger or equal than the other
	bool operator>=(const TimeStamp& other) const;

	/// increment time stamp by nsec seconds
	TimeStamp& operator+=(Long64_t nsec);
	/// decrement time stamp by nsec seconds
	TimeStamp& operator-=(Long64_t nsec);
	/// add nsec seconds to time stamp
	TimeStamp operator+(Long64_t nsec) const;
	/// subtract nsec seconds from time stamp
	TimeStamp operator-(Long64_t nsec) const;

	/// difference between two time stamps in seconds
	Long64_t operator-(const TimeStamp& other) const;

    private:
	ULong64_t m_time; ///< time stamp in seconds since the epoch

	static ULong64_t fromStr(const char* str);

    public:
        /// throw a TimeStampExpection in case of trouble
        class TimeStampException : public std::exception
        {
            private:
                /// pointer to string describing the error
                const char* m_str;
            public:
                /// constructor
                TimeStampException(const char* str);
                /// routine returning a C string describing what went wrong
                const char* what() const throw ();
        };
};

#endif // TIMESTAMP_H

// vim: tw=78:sw=4:ft=cpp
