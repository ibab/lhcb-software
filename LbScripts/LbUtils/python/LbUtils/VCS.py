'''
Created on 26 sept. 2011

@author: hmdegaud
'''
from LbUtils.Processes import callCommand, RetryCommand

import re

#------------------------------------------------------------------------------
def splitlines(text):
    """
    Split a text in lines removing empty lines and lines starting with "#".
    """
    return [ l
             for l in [ l.strip() for l in text.splitlines() ]
             if l and l[0] != "#" ]



# constants for the regexp extraction of the items from the repository URL.
# They happen to be identical for both CVS and SVN.
VCS_PROTOCOL = 1
VCS_USER     = 2
VCS_HOST     = 3
VCS_PATH     = 4

## Small class for the generic manipulation of RevisionControlSystem URLs
class RepositoryInfo( object ):
    __protocols__ = []
    def __init__( self, protocol, host, path, user = None ):
        self.protocol = protocol
        self.host = host
        self.path = path
        self.user = user

    def _get_protocol( self ):
        return self._protocol
    def _set_protocol( self, protocol ):
        if protocol not in self.__protocols__:
            raise RuntimeError( "Invalid protocol '%s' (allowed: %r)" % ( protocol, self.__protocols__ ) )
        self._protocol = protocol
    ## Property for the validation of the protocol
    protocol = property( _get_protocol, _set_protocol )

    def _get_path( self ):
        return self._path
    def _set_path( self, path ):
        if not path.startswith( "/" ):
            self._path = "/" + path
        else:
            self._path = path
    ## Property for the validation of the path
    path = property( _get_path, _set_path )

#------------------------------------------------------------------------------

# command wrapper for CVS
cvs_command          = lambda *args, **kwargs: apply(callCommand, ("cvs",) + args, kwargs)
cvs_retry_command    = RetryCommand(cvs_command, lambda r: r[2] == 0)
cvs_repository_rexp  = re.compile(r":([pkg]server|ext):(?:([\w.]+)@)?([\w.]+):(/[\w./]+)*")

def isCVSRepository(url):
    iscvs = False
    if cvs_repository_rexp.match(url) :
        iscvs = True
    return iscvs

## CVS specific implementation of RepositoryInfo
class CVSReposInfo( RepositoryInfo ):
    __protocols__ = [ "gserver", "kserver", "ext", "pserver"]
    def __str__( self ):
        import os
        if self._protocol == "ext" and 'CVS_RSH' not in os.environ:
            raise RuntimeError( "Cannot use 'ext' protocol without the environment variable 'CVS_RSH' set (e.g. ssh or plink)" )
        # prepare url template
        if self.user:
            url = ":%(_protocol)s:%(user)s@%(host)s:%(_path)s"
        else:
            url = ":%(_protocol)s:%(host)s:%(_path)s"
        return url % self.__dict__
    def getModules(self):
        out, _err, _retcode = cvs_command("-d", self.__str__(), "checkout", "-s")
        return out

#------------------------------------------------------------------------------

# command wrapper for SVN
svn_command          = lambda *args, **kwargs: apply(callCommand, ("svn",) + args, kwargs)
svn_retry_command    = RetryCommand(svn_command, lambda r: r[2] == 0)
svn_repository_rexp  = re.compile(r"(svn(?:\+ssh)?|https?|file)://(?:([\w.]+)@)?([\w.]*)(/[\w./]+)*")

def isSVNRepository(url):
    issvn = False
    if svn_repository_rexp.match(url) :
        issvn = True
    return issvn

## Subversion specific implementation of RepositoryInfo
class SVNReposInfo( RepositoryInfo ):
    __protocols__ = ["svn", "svn+ssh", "file", "https", "http"]
    def __str__( self ):
        # prepare url template
        if self.user:
            url = "%(_protocol)s://%(user)s@%(host)s%(_path)s"
        else:
            url = "%(_protocol)s://%(host)s%(_path)s"
        return url % self.__dict__
    def getProperty(self, property, path=None):
        """
        get the property as text
        @param property: property name
        @type property: string
        @param path: relative path in the repository
        @type path: string
        """
        if path:
            if path[0] == "/": # we need relative path
                path = path[1:]
            path = "/".join([self.repository, path])
        else:
            path = self.__str__()
        out, _err, _retcode = svn_command("propget", property, path)
        return out

