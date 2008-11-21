"""
Module which implements simple file-based locking mechanism

@author: Karol Kruzelecki
"""
import os, time, md5


def _hash(value):
    """ returns md5 sum of the given value """
    return md5.new(value).hexdigest()

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
    def __init__(self, command, uniqId, commonId=None, environment=None, lockPath=None):
        """ object constructor. Parameters described in more details in the __class__.__doc__. """
        if commonId == None: self._hashCmd = _hash(command) # check if it's None - '' is different than None so '== None' is necessary
        else: self._hashCmd = _hash(commonId)
        self._command = command
        self._uniqId = uniqId
        self._hashUniqId = _hash(uniqId)
        self._lockFileName = '.lock_' + self._hashCmd
        self._env = environment
        if lockPath is None: self._dir = os.getcwd()
        else: self._dir = lockPath
        while True:
            c = self._checkLock()
            if c == self._hashUniqId:
                if environment is not None:
                    self._prevEnv = os.environ.copy()
                    for x in self._env.keys():
                        os.environ[x] = self._env[x]
                    self.exitCode = os.system(self._command)
                    for x in self._env.keys():
                        if x in self._prevEnv: os.environ[x] = self._prevEnv[x]
                        else: del os.environ[x]
                else:
                    self.exitCode = os.system(self._command)
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
