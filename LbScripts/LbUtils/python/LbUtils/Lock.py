"""
Module which implements simple file-based locking mechanism

@author: Karol Kruzelecki
"""
import os, time, sys

if sys.version_info[:3] >= (2,6,0) :
    from hashlib import md5 #@UnusedImport
else :
    from md5 import md5 #@Reimport



def _hash(value):
    """ returns md5 sum of the given value """
    return md5(value).hexdigest()

class Lock(object):
    """ Lock(command, uniqId[, commonId, [lockfilePath]]) -> (not important)

        Object of the class is created to run a <command> and to be sure that nobody else runs the same <command> from the same CWD (or any other, specified directory) at the same time.
        Constructor of the class has 2 obligatory parameters:
        - <command> to run
        - <uniqId> of the launcher
        and three optional parameters:
        - <commonId> which is a key to make other processes with the same <commonId> parameter wait until the first one finishes.
        If <commonId> is not given, it is created on the base of <command>
        - <environment> is a dictionary with a values to set into working environment before <command> launch
        - <lockPath> is a directory where the lockfiles should be stored
    """
    def __init__(self, command, uniqId, commonId=None, environment=None, lockPath=None, splitCommand=None):
        """ object constructor. Parameters described in more details in the __class__.__doc__. """
        if commonId == None: self._hashCmd = _hash(command) # check if it's None - '' is different than None so '== None' is necessary
        else: self._hashCmd = _hash(commonId)
        self._command = command
        self._uniqId = uniqId
        self._hashUniqId = _hash(uniqId)
        self._lockFileName = '.lock_' + self._hashCmd
        self._env = environment
        self._splitCommand = splitCommand
        if lockPath is None: self._dir = os.getcwd()
        else: self._dir = lockPath
        while True:
            c = self._checkLock()
            if c == self._hashUniqId:
                if environment is not None:
                    self._prevEnv = os.environ.copy()
                    for x in self._env.keys():
                        os.environ[x] = self._env[x]
                    self.exitCode = self._runCommand()
                    for x in self._env.keys():
                        if x in self._prevEnv: os.environ[x] = self._prevEnv[x]
                        else: del os.environ[x]
                else:
                    self.exitCode = self._runCommand()
                self._removeLock()
                break
            elif c == None:
                self._createLock()
                continue
            time.sleep(5)
    def _checkLock(self):
        """ checks if lockfile for the given commands exists, and if so returns the hash value of the launcher unique id """
        if not os.path.exists(os.path.join(self._dir, self._lockFileName)): return None
        else:
            lock = file(os.path.join(self._dir, self._lockFileName), 'r')
            lockData = lock.readlines()
            if len(lockData) < 3: # file currently in use by someone else, but not saved yet
                return 'in use'
            lockData = lockData[2]
            lock.close()
            return lockData
    def _createLock(self):
        """ creates the lockfile in the current working directory

        The file contains the following in separate lines:
        - the command locking is being done
        - launcher unique id
        - result of hash function on the launcher unique id
        """
        lock = file(os.path.join(self._dir, self._lockFileName), 'w')
        lock.writelines([self._command, '\n', self._uniqId, '\n', _hash(self._uniqId)])
        lock.close()
    def _removeLock(self):
        """ removes the lockfile if it exists """
        try:
            os.remove(os.path.join(self._dir, self._lockFileName))
        except OSError:
            pass
    def _runCommand(self):
        if self._splitCommand is None:
            return os.system(self._command)
        else:
            cmds = self._command.split(self._splitCommand)
            for c in cmds:
                ret = os.system(c)
                if ret != 0:
                    return ret


##########################################################
# A set of object for providing simple, cross-platform file locking

class Lock2Error(IOError):
    """The generic error for locking - it is a subclass of ``IOError``."""

class Lock2(object):
    """A simple file lock, compatible with windows and Unixes."""

    def __init__(self, filename, timeout=5, step=0.1):
        """
        Create a ``Lock`` object on file ``filename``

        ``timeout`` is the time in seconds to wait before timing out, when
        attempting to acquire the lock.

        ``step`` is the number of seconds to wait in between each attempt to
        acquire the lock.

        """
        self.timeout = timeout
        self.step = step
        self.filename = os.path.realpath(filename)
        self.locked = False

    def lock(self, force=True):
        """
        Lock the file for access by creating a directory of the same name (plus
        a trailing underscore).

        The file is only locked if you use this class to acquire the lock
        before accessing.

        If ``force`` is ``True`` (the default), then on timeout we forcibly
        acquire the lock.

        If ``force`` is ``False``, then on timeout a ``Lock2Error`` is raised.
        """
        if self.locked:
            raise Lock2Error('%s is already locked' % self.filename)
        t = 0
        name = self._mungedname()
        while t < self.timeout:
            t += self.step
            try:
                if os.path.isdir(name):
                    raise os.error
                else:
                    os.mkdir(name)
            except os.error:
                time.sleep(self.step)
            else:
                self.locked = True
                return
        if force:
            self.locked = True
        else:
            raise Lock2Error('Failed to acquire lock on %s' % self.filename)

    def unlock(self, ignore=True):
        """
        Release the lock.

        If ``ignore`` is ``True`` and removing the lock directory fails, then
        the error is surpressed. (This may happen if the lock was acquired
        via a timeout.)
        """
        if not self.locked:
            raise Lock2Error('%s is not locked' % self.filename)
        self.locked = False
        try:
            os.rmdir(self._mungedname())
        except os.error:
            if not ignore:
                raise Lock2Error('unlocking appeared to fail - %s' %
                    self.filename)

    def _mungedname(self):
        """
        Override this in a subclass if you want to change the way ``Lock2``
        creates the directory name.
        """
        return self.filename + '_'

    def __del__(self):
        """Auto unlock when object is deleted."""
        if self.locked:
            self.unlock()

class Lock2File(Lock2):
    """
    A file like object with an exclusive lock, whilst it is open.

    The lock is provided by the ``Lock2`` class, which creates a directory
    with the same name as the file (plus a trailing underscore), to indicate
    that the file is locked.

    This is simple and cross platform, with some limitations :

        * Unusual process termination could result in the directory
          being left.
        * The process acquiring the lock must have permission to create a
          directory in the same location as the file.
        * It only locks the file against other processes that attempt to
          acquire a lock using ``Lock2File`` or ``Lock2``.
    """

    def __init__(self, filename, mode='r', bufsize=-1, timeout=5, step=0.1,
        force=True):
        """
        Create a file like object that is locked (using the ``Lock2`` class)
        until it is closed.

        The file is only locked against another process that attempts to
        acquire a lock using ``Lock2`` (or ``Lock2File``).

        The lock is released automatically when the file is closed.

        The filename, mode and bufsize arguments have the same meaning as for
        the built in function ``open``.

        The timeout and step arguments have the same meaning as for a ``Lock2``
        object.

        The force argument has the same meaning as for the ``Lock2.lock`` method.

        A ``Lock2File`` object has all the normal ``file`` methods and
        attributes.
        """
        Lock2.__init__(self, filename, timeout, step)
        # may raise an error if lock is ``False``
        self.lock(force)
        # may also raise an error
        self._file = open(filename, mode, bufsize)

    def close(self, ignore=True):
        """
        close the file and release the lock.

        ignore has the same meaning as for ``Lock2.unlock``
        """
        self._file.flush()
        os.fsync(self._file.fileno())
        self._file.close()
        self.unlock(ignore)

    def __getattr__(self, name):
        """delegate appropriate method/attribute calls to the file."""
        return getattr(self._file, name)

    def __setattr__(self, name, value):
        """Only allow attribute setting that don't clash with the file."""
        if not '_file' in self.__dict__:
            Lock2.__setattr__(self, name, value)
        elif hasattr(self._file, name):
            return setattr(self._file, name, value)
        else:
            Lock2.__setattr__(self, name, value)

    def __del__(self):
        """Auto unlock (and close file) when object is deleted."""
        if self.locked:
            self.unlock()
            self._file.close()
