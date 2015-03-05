/**
 * @file DotLock.h
 *
 * @brief implement dot locking.
 *
 * @author Manuel Schiller
 * @date 2009-08-06
 */
#ifndef DOTLOCK_H
#define DOTLOCK_H

#include <string>
#include <exception>

/** @brief class acquiring a dotlock while in existence
 *
 * @author Manuel Schiller
 * @date 2009-08-06
 * 	initial version
 * @date 2013-03-14
 * 	updates to be safe for use from multiple hosts
 *
 * If you need to serialise several processes wanting to write to the same
 * file, you can use the following code:
 * @code
 * {
 *     DotLock lock("/path/to/file.you.want.to.modify");
 *     // by the time the DotLock constructor is done, we hold the lock
 *     //
 *     // modify that file in whatever way you see fit...
 *     modify("/path/to/file.you.want.to.modify");
 * } // lock's destructor is called here, releasing the lock
 * @endcode
 */
class DotLock
{
    public:
	/** @brief constructor
	 *
	 * @param filename	lock file name base
	 * @param timeout	if >0, timeout after timeout seconds
	 */
	DotLock(const char* filename, int timeout = 0);
	/** @brief constructor
	 *
	 * @param filename	lock file name base
	 * @param timeout	if >0, timeout after timeout seconds
	 */
	DotLock(const std::string& filename, int timeout = 0);
	/// destructor
	virtual ~DotLock();
    private:
	/// name of lock file
	std::string m_dotlockfilename;
	/// initialise
	void init(int timeout);
	/// routine to acquire lock (returns errno, doesn't leak resources)
	int getlock(const char* fname, int timeout) const;
	/// routine to release lock (returns errno, doesn't leak resources)
	static int releaselock(const char* fname);
	/// routine for safe reading/writing (returns errno, doesn't leak)
	static int xfer(const char* fname, bool read, std::size_t sz, char* buf);
	/// fully qualified host name (no leaks, if NULL, errno says why)
	static char* gethostname();
	/// xor val into arr
	static void xor_in(unsigned char (&arr)[8], unsigned long long val);

    public:
	/// throw a DotLockExpection in case of trouble during locking
	class DotLockException : public std::exception
        {
	    private:
		/// size of static buffer for error message from strerror_r
		static const int s_sz = 256;
		/// pointer to error from getaddrinfo
		const char* m_gaierr;
		/// need static buffer for error message from strerror_r
		char m_buf[s_sz];
	    public:
		/// constructor
		DotLockException(int err, bool isgetaddrinfoerr = false);
		/// routine returning a C std::string describing what went wrong
		const char* what() const throw ();
	};
};

#endif // DOTLOCK_H

// vim:tw=78:sw=4:ft=cpp
