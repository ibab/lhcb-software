"""
Module which implements simple file-based locking mechanism

@author: Karol Kruzelecki
"""
import os, time, md5


def _hash(value):
    """ returns md5 sum of the given value """
    return md5.new(value).hexdigest()

class Lock(object):
    """ Lock(command, uniqId[, commonId]) -> (not important)

        Object of the class is created to run a <command> and to be sure that nobody else runs the same <command> from the same CWD at the same time.
        Constructor of the class has 2 obligatory parameters:
        - <command> to run
        - <uniqId> of the launcher
        and one optional parameter:
        - <commonId> which is a key to make other processes with the same <commonId> parameter wait until the first one finishes.
        If <commonId> is not given, it is created on the base of <command>
    """
    def __init__(self, command, uniqId, commonId=None):
        """ object constructor. Parameters described in more details in the __class__.__doc__. """
        if commonId == None: self._hashCmd = _hash(command) # check if it's None - '' is different than None so '== None' is necessary
        else: self._hashCmd = _hash(commonId)
        self._command = command
        self._uniqId = uniqId
        self._hashUniqId = _hash(uniqId)
        self._lockFileName = '.lock_' + self._hashCmd
        while True:
            c = self._checkLock()
            if c == self._hashUniqId:
                os.system(self._command)
                self._removeLock()
                break
            elif c == None:
                self._createLock()
                continue
            time.sleep(5)
    def _checkLock(self):
        """ checks if lockfile for the given commands exists, and if so returns the hash value of the launcher unique id """
        if not os.path.exists(self._lockFileName): return None
        else:
            lock = file(self._lockFileName, 'r')
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
        lock = file(self._lockFileName, 'w')
        lock.writelines([self._command, '\n', self._uniqId, '\n', _hash(self._uniqId)])
        lock.close()
    def _removeLock(self):
        """ removes the lockfile if it exists """
        try:
            os.remove(self._lockFileName)
        except OSError:
            pass
