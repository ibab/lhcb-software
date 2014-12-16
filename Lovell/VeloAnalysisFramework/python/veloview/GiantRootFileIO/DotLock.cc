/**
 * @file DotLock.cc
 *
 * @brief implement dot locking.
 *
 * @author Manuel Schiller
 * @date 2009-08-06
 */
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "DotLock.h"

DotLock::DotLockException::DotLockException(int err, bool isgetaddrinfoerr) :
    m_gaierr(0)
{
    m_buf[0] = 0;
    if (!isgetaddrinfoerr) {
	/* FIXME: extremely ugly: apparently, there's no easy way to
	 * tell/force libc to give us either the GNU or POSIX version of
	 * strerror_r, so we have to hope for things to go well */
	strerror_r(err, m_buf, s_sz);
	m_buf[s_sz - 1] = 0; /* enforce zero-termination */
    } else {
	m_gaierr = gai_strerror(err);
    }
}

const char* DotLock::DotLockException::what() const throw()
{ return m_gaierr ? m_gaierr : m_buf; }

DotLock::DotLock(const char* filename, int timeout) :
    m_dotlockfilename(filename)
{ init(timeout); }

DotLock::DotLock(const std::string& filename, int timeout) :
    m_dotlockfilename(filename)
{ init(timeout); }

void DotLock::init(int timeout)
{
    /* protect against empty filename */
    if (m_dotlockfilename.empty()) throw DotLockException(EINVAL);
    m_dotlockfilename += ".lock";
    int retVal = getlock(m_dotlockfilename.c_str(), timeout);
    if (0 != retVal) {
	/* FIXME: perror in here is temporary to facilitate debugging when
	 * exceptions don't make it */
	errno = retVal;
	perror("Ran into trouble while acquiring lock");
	throw DotLockException(retVal);
    }
}

DotLock::~DotLock()
{
    int retVal = releaselock(m_dotlockfilename.c_str());
    if (0 != retVal) {
	/* must not throw in destructor! print to stderr instead... */
	errno = retVal;
	perror("Ran into trouble while releasing lock");
    }
}

int DotLock::releaselock(const char* fname)
{
    if (0 != unlink(fname)) {
	int err = errno;
	if (0 == err) {
	    /* something went wrong, but we got errno == 0, so complain to
	     * stderr... */
	    ::write(2, "operation failed but errno is zero!\n", 36);
	    fsync(2);
	    err = -1;
	}
	return err;
    }
    return 0;
}

int DotLock::xfer(const char* fname, bool read, std::size_t sz, char* buf)
{
    int fd = -1, err = 0;
    std::size_t xferred = 0;
    /* ok, open the file */
    do {
	if (read)
	    fd = open(fname, O_RDONLY, S_IRUSR|S_IWUSR);
	else
	    fd = open(fname, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
	if (-1 != fd) break;
	if (EINTR == errno) continue;
	goto error;
    } while (1);
    /* transfer sz bytes */
    do {
	int xf;
        if (read)
	    xf = ::read(fd, buf + xferred, sz - xferred);
	else
	    xf = ::write(fd, buf + xferred, sz - xferred);
	if (xf + xferred != sz) {
	    if (EINTR != errno) goto error;
	}
	xferred += xf;
    } while (xferred != sz);
    if (!read) {
	/* sync */
	do {
	    if (0 == fsync(fd)) break;
	    if (EINTR == errno) continue;
	    goto error;
	} while (1);
    }
    /* close the file */
    do {
	if (0 != close(fd)) {
	    if (EINTR == errno) continue;
	    goto error;
	}
	break;
    } while (1);
    return 0;
error:
    err = errno;
    if (-1 == fd) close(fd);
    if (0 == err) {
	/* something went wrong, but we got errno == 0, so complain to
	 * stderr... */
	::write(2, "operation failed but errno is zero!\n", 36);
	fsync(2);
	err = -1;
    }
    return err;
}

char* DotLock::gethostname()
{
    struct addrinfo hints, *info = 0;
    char *hname = 0, *hostname = 0;
    long len = sysconf(_SC_HOST_NAME_MAX);
    int errnosav = 0, rc = 0;
    if (-1 == len) len = 256;
    if (!(hname = reinterpret_cast<char*>(malloc(1 + len)))) return hostname;
    if ((rc = ::gethostname(hname, len))) goto done;
    hname[len] = 0; /* make sure hostname is zero-terminated */
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* either IPV4 or IPV6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    if ((rc = getaddrinfo(hname, "http", &hints, &info))) {
	/* we have to throw explicitly here, since getaddrinfo uses
	 * gai_strerror */
	errnosav = errno;
	free(hname);
	errno = errnosav;
	throw DotLockException(rc, true);
    }
    for (struct addrinfo *p = info; true; p = p->ai_next) {
	/* skip entries beginning with "localhost" - they don't contain useful
	 * any information, fall back to the result of gethostname if there is
	 * no sufficiently informative answer from getaddrinfo */
	if (p && 0 == strncmp(p->ai_canonname, "localhost", 9)) continue;
	char *hnsrc = p ? p->ai_canonname : hname;
	hostname = reinterpret_cast<char*>(
		malloc(sizeof(char) * (std::strlen(hnsrc) + 1)));
	if (hostname) std::strcpy(hostname, hnsrc);
	break;
    }
done:
    errnosav = errno;
    if (info) freeaddrinfo(info);
    free(hname);
    errno = errnosav;
    return hostname;
}

void DotLock::xor_in(unsigned char (&arr)[8], unsigned long long val)
{
    arr[0] ^= static_cast<unsigned char>(val & 0xff); val >>= 8;
    arr[1] ^= static_cast<unsigned char>(val & 0xff); val >>= 8;
    arr[2] ^= static_cast<unsigned char>(val & 0xff); val >>= 8;
    arr[3] ^= static_cast<unsigned char>(val & 0xff); val >>= 8;
    arr[4] ^= static_cast<unsigned char>(val & 0xff); val >>= 8;
    arr[5] ^= static_cast<unsigned char>(val & 0xff); val >>= 8;
    arr[6] ^= static_cast<unsigned char>(val & 0xff); val >>= 8;
    arr[7] ^= static_cast<unsigned char>(val & 0xff);
}

int DotLock::getlock(const char *fname, int timeout) const
{
    size_t ftmpsz = 0;
    int havelock = 0, retVal = 0, pid, uid, to = timeout;
    char *hostname = 0, *ftmpname = 0, *ftmpname2 = 0;
    /* if DotLock::gethostname throws, that's ok, as we have not allocated any
     * resources yet, so nothing leaks... */
    if (!(hostname = DotLock::gethostname())) {
	retVal = errno;
	goto error;
    }
    ftmpsz = std::strlen(fname) + std::strlen(hostname) + 11 + 18 + 16;
    if (!(ftmpname = (char*) malloc(ftmpsz))) {
	retVal = errno;
	goto error;
    }
    *ftmpname = 0;
    if (!(ftmpname2 = (char*) malloc(ftmpsz))) {
	retVal = errno;
	goto error;
    }
    /* ok, try to acquire lock */
    do {
	{
	    struct timeval tv;
	    unsigned long long ptr = (unsigned long long) this;
	    unsigned char dummy[8]; /* use as unique identifier */
	    /* read 8 random bytes into dummy */
	    retVal = xfer("/dev/urandom", true, 8,
		    reinterpret_cast<char*>(dummy));
	    if (0 != retVal) goto error;
	    /* mix uid, pid, time of day and address of *this into dummy to
	     * avoid problems with concurrent reads from /dev/urandom */
	    xor_in(dummy, ptr);
	    ptr = pid = getpid();
	    xor_in(dummy, ptr);
	    ptr = uid = getuid();
	    xor_in(dummy, ptr);
	    gettimeofday(&tv, NULL);
	    ptr = tv.tv_sec;
	    xor_in(dummy, ptr);
	    ptr = tv.tv_usec;
	    xor_in(dummy, ptr);
	    /* construct a unique file name from hostname, uid, pid, random
	     * bytes */
	    snprintf(ftmpname, ftmpsz,
		    "%s.uid%09d.pid%09d.%s.%02x%02x%02x%02x%02x%02x%02x%02x",
		    fname, uid, pid, hostname,
		    ((unsigned) dummy[0]) & 255, ((unsigned) dummy[1]) & 255,
		    ((unsigned) dummy[2]) & 255, ((unsigned) dummy[3]) & 255,
		    ((unsigned) dummy[3]) & 255, ((unsigned) dummy[5]) & 255,
		    ((unsigned) dummy[4]) & 255, ((unsigned) dummy[7]) & 255);
	}
	/* write to ftmpname */
	retVal = xfer(ftmpname, false, std::strlen(ftmpname), ftmpname);
	if (0 != retVal) goto error;
	/* sleep as long as we can stat the lock file */
	do {
	    struct stat flags;
	    if (0 != stat(fname, &flags)) {
		if (ENOENT == errno) break;
		if (EINTR == errno) continue;
		retVal = errno;
		goto error;
	    }
	    if (0 < timeout) {
		if (to > 0) {
		    --to;
		} else {
		    // timeout - indicate that in retVal
		    retVal = EBUSY;
	            goto error;	    
		}
	    }
	    sleep(1);
	} while(1);
	/* try to link ftmpname to fname (i.e. acquire the lock) */
	do {
	    if (0 != (retVal = link(ftmpname, fname))) {
		if (EEXIST == errno) break;
		if (EINTR == errno) continue;
		retVal = errno;
		goto error;
	    } else {
		break;
	    }
	} while(1);
	/* failed to acquire lock... */
	if (0 != retVal && EEXIST == errno) continue;
	/* ok, open fname and read back into ftmpname2 */
	retVal = xfer(ftmpname, true, std::strlen(ftmpname), ftmpname2);
	if (0 != retVal) goto error;
	/* make sure that it's still "our" file */
	if (0 != memcmp(ftmpname, ftmpname2, std::strlen(ftmpname))) {
	    /* lost arbitration */
	    continue;
	}
	/* ok, acquired lock successfully */
	havelock = 1;
    } while (!havelock);
    /* unlink ftmpname */
    do {
	if (0 != unlink(ftmpname)) {
	    if (EINTR == errno) continue;
	    retVal = errno;
	    goto error;
	} else {
	    break;
	}
    } while (1);
    free(ftmpname);
    free(ftmpname2);
    free(hostname);
    return 0;

error:
    /* some kind of serious error, clean up, restore errno from retVal */
    if (hostname) free(hostname);
    if (ftmpname2) free(ftmpname2);
    if (ftmpname) {
	if (0 != *ftmpname) unlink(ftmpname);
	free(ftmpname);
    }
    if (0 == retVal) {
	/* something went wrong, but we got errno == 0, so complain to
	 * stderr... */
	::write(2, "operation failed but errno is zero!\n", 36);
	fsync(2);
	retVal = -1;
    }
    errno = retVal;
    return retVal;
}

#ifdef TEST_DOTLOCK
int main()
{
    DotLock dl("myfile");
    return 0;
}
#endif /* TEST_DOTLOCK */

/* vim:tw=78:sw=4:ft=cpp */
