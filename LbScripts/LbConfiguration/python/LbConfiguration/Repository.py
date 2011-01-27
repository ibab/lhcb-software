"""
Module for handling of software repository informations.
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

__all__ = ["RepositoryInfo", "SVNReposInfo", "CVSReposInfo", "repositories",
           "getRepositories"]

## Small class for the generic manipulation of RevisionControlSystem URLs
class RepositoryInfo(object):
    __protocols__ = []
    def __init__(self, protocol, host, path, user = None):
        self.protocol = protocol
        self.host = host
        self.path = path
        self.user = user

    def _get_protocol(self):
        return self._protocol
    def _set_protocol(self, protocol):
        if protocol not in self.__protocols__:
            raise RuntimeError("Invalid protocol '%s' (allowed: %r)" % (protocol, self.__protocols__))
        self._protocol = protocol
    ## Property for the validation of the protocol
    protocol = property(_get_protocol, _set_protocol)

    def _get_path(self):
        return self._path
    def _set_path(self, path):
        if not path.startswith("/"):
            self._path = "/" + path
        else:
            self._path = path
    ## Property for the validation of the path
    path = property(_get_path, _set_path)


## Subversion specific implementation of RepositoryInfo
class SVNReposInfo(RepositoryInfo):
    __protocols__ = ["svn", "svn+ssh", "file", "https", "http"]
    def __str__(self):
        # prepare url template
        if self.user:
            url = "%(_protocol)s://%(user)s@%(host)s%(_path)s"
        else:
            url = "%(_protocol)s://%(host)s%(_path)s"
        return url % self.__dict__

## CVS specific implementation of RepositoryInfo
class CVSReposInfo(RepositoryInfo):
    __protocols__ = [ "gserver", "kserver", "ext", "pserver"]
    def __str__(self):
        import os
        if self._protocol == "ext" and 'CVS_RSH' not in os.environ:
            raise RuntimeError("Cannot use 'ext' protocol without the environment variable 'CVS_RSH' set (e.g. ssh or plink)")
        # prepare url template
        if self.user:
            url = ":%(_protocol)s:%(user)s@%(host)s:%(_path)s"
        else:
            url = ":%(_protocol)s:%(host)s:%(_path)s"
        return url % self.__dict__

## List of known repositories
repositories = { "gaudi": { "ssh":       SVNReposInfo("svn+ssh", "svn.cern.ch", "/reps/gaudi"),
                            "anonymous": SVNReposInfo("http", "svnweb.cern.ch", "/guest/gaudi") },
#                 "lhcb":  { "kerberos":  CVSReposInfo("gserver", "isscvs.cern.ch", "/local/reps/lhcb"),
#                            "kserver":   CVSReposInfo("kserver", "isscvs.cern.ch", "/local/reps/lhcb"),
#                            "ssh":       CVSReposInfo("ext", "isscvs.cern.ch", "/local/reps/lhcb"),
#                            "anonymous": CVSReposInfo("pserver", "isscvs.cern.ch", "/local/reps/lhcb", "anonymous") },
                 "lbsvn": { "ssh":       SVNReposInfo("svn+ssh", "svn.cern.ch", "/reps/lhcb"),
                            "anonymous": SVNReposInfo("http", "svnweb.cern.ch", "/guest/lhcb") },
                 "dirac": { "ssh":       SVNReposInfo("svn+ssh", "svn.cern.ch", "/reps/dirac"),
                            "anonymous": SVNReposInfo("http", "svnweb.cern.ch", "/guest/dirac") },
                 }
# Define default repositories
for k in repositories:
    repositories[k]["default"] = repositories[k]["ssh"]


def getRepositories(user_svn=None, user_cvs=None, protocol="default"):
    from urllib import splittype, splituser, splithost
    reps = {}
    for r in repositories :
        if repositories[r].has_key(protocol) :
            reps[r] = repositories[r][protocol]
    if user_svn :
        i = 0
        for r in user_svn :
            # FIXME: need some error checking
            protocol, rest = splittype(r)
            rest, path = splithost(rest)
            user, host = splituser(rest)
            reps["user_svn_%d" % i] = SVNReposInfo(protocol, host, path, user)
            i += 1
    if user_cvs :
        i = 0
        for r in user_cvs :
            # FIXME: need some error checking
            dummy, protocol, rest, path = r.split(":")
            user, host = splituser(rest)
            reps["user_cvs_%d" % i] = CVSReposInfo(protocol, host, path, user)
            i += 1
    return reps

