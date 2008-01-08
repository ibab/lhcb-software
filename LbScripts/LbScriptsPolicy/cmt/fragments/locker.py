##########################################################
## stolen and (slighty) adapted from:
##  http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/65203
##

import os

if os.name == 'nt':
    import win32con
    import win32file
    import pywintypes
    LOCK_EX = win32con.LOCKFILE_EXCLUSIVE_LOCK
    LOCK_SH = 0 # the default
    LOCK_NB = win32con.LOCKFILE_FAIL_IMMEDIATELY
    # is there any reason not to reuse the following structure?
    __overlapped = pywintypes.OVERLAPPED()
    hexCst = -0x10000
elif os.name == 'posix':
    import fcntl
    LOCK_EX = fcntl.F_WRLCK
    LOCK_SH = fcntl.F_RDLCK
    LOCK_NB = fcntl.F_UNLCK
else:
    raise RuntimeError("Locker only defined for nt and posix platforms")

if os.name == 'nt':
    def lock(file):
        hfile = win32file._get_osfhandle(file.fileno())
        win32file.LockFileEx(hfile, LOCK_EX, 0, hexCst, __overlapped)

    def unlock(file):
        hfile = win32file._get_osfhandle(file.fileno())
        win32file.UnlockFileEx(hfile, 0, hexCst, __overlapped)

elif os.name =='posix':
    import socket,errno,time

    def _tmpFileName(fileName):
        return "%s.%s.%d" % ( fileName, socket.gethostname(), os.getpid() )
    def _lckFileName(fileName):
        return "%s.lock" % fileName

    def _linkCount( lockFileName ):
        try:
            return os.stat(lockFileName).st_nlink
        except OSError, e:
            if e.errno != errno.ENOENT:
                raise
            return -1
    def _read(fileName):
        try:
            fp = open(fileName)
            try:     readFileName = fp.read()
            finally: fp.close()
            return readFileName
        except EnvironmentError, e:
            if e.errno != errno.ENOENT:
                raise
            return None
            
    def _sleep(): time.sleep(8)

    def lock(file):
        fileName = file.name
        tmpFileName = _tmpFileName(fileName)
        fp = open( tmpFileName, "w" )
        fp.write( tmpFileName )
        fp.close()

        lockFileName = _lckFileName( fileName )
        while True:
            if _linkCount(lockFileName) != -1: _sleep()
            try:
                os.link( tmpFileName, lockFileName )
                # we acquired the lock
                return
            except OSError, e:
                if e.errno == errno.ENOENT:
                    pass
                elif e.errno != errno.EEXIST:
                    os.unlink(tmpFileName)
                    raise
                elif _linkCount( lockFileName ) != 2:
                    pass
                elif _read(lockFileName) == tmpFileName:
                    raise
                else:
                    # someone else owns that lock
                    pass
            ## didn't get the lock !!
            ## say something ?
            #print _id_()," failed to acquire the lock ..."
            _sleep()
            pass
        return
    
    def unlock(file):
        fileName = file.name
        tmpFileName  = _tmpFileName(fileName)
        lockFileName = _lckFileName( fileName )

        try:
            os.unlink( lockFileName )
        except OSError, e:
            if e.errno != errno.ENOENT:
                raise
        # remove the tmp file
        try:
            os.unlink( tmpFileName )
        except OSError, e:
            if e.errno != errno.ENOENT:
                raise
        return
    
		
