"""
Module for handling of software repository informations.
"""
from LbUtils.VCS import SVNReposInfo, CVSReposInfo

__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

__all__ = ["repositories", "getRepositories"]



## List of known repositories
repositories = { "gaudi": { "ssh":       SVNReposInfo( "svn+ssh", "svn.cern.ch", "/reps/gaudi" ),
                            "anonymous": SVNReposInfo( "http", "svn.cern.ch", "/guest/gaudi" ) },
#                 "lhcb":  { "kerberos":  CVSReposInfo("gserver", "isscvs.cern.ch", "/local/reps/lhcb"),
#                            "kserver":   CVSReposInfo("kserver", "isscvs.cern.ch", "/local/reps/lhcb"),
#                            "ssh":       CVSReposInfo("ext", "isscvs.cern.ch", "/local/reps/lhcb"),
#                            "anonymous": CVSReposInfo("pserver", "isscvs.cern.ch", "/local/reps/lhcb", "anonymous") },
                 "lbsvn": { "ssh":       SVNReposInfo( "svn+ssh", "svn.cern.ch", "/reps/lhcb" ),
                            "anonymous": SVNReposInfo( "http", "svn.cern.ch", "/guest/lhcb" ) },
                 "dirac": { "ssh":       SVNReposInfo( "svn+ssh", "svn.cern.ch", "/reps/dirac" ),
                            "anonymous": SVNReposInfo( "http", "svn.cern.ch", "/guest/dirac" ) },
                 }

#only needed information for svn switch --relocate
repository_shortpaths = {
    "ssh" : SVNReposInfo( "svn+ssh", "svn.cern.ch", "/reps" ),
    "anonymous" : SVNReposInfo( "http", "svn.cern.ch", "/guest" ),
    }

# Define default repositories
for k in repositories:
    repositories[k]["default"] = repositories[k]["ssh"]

repository_shortpaths["default"]=repository_shortpaths["ssh"] #default protocol
repository_shortpaths["authenticated"]=repository_shortpaths["ssh"] #default authenticated protocol

def getRepositories( user_svn = None, user_cvs = None, protocol = "default" ):
    from urllib import splittype, splituser, splithost
    reps = {}
    for r in repositories :
        if repositories[r].has_key( protocol ) :
            reps[r] = repositories[r][protocol]
    if user_svn :
        i = 0
        for r in user_svn :
            # FIXME: need some error checking
            protocol, rest = splittype( r )
            rest, path = splithost( rest )
            user, host = splituser( rest )
            reps["user_svn_%d" % i] = SVNReposInfo( protocol, host, path, user )
            i += 1
    if user_cvs :
        i = 0
        for r in user_cvs :
            # FIXME: need some error checking
            dummy, protocol, rest, path = r.split( ":" )
            user, host = splituser( rest )
            reps["user_cvs_%d" % i] = CVSReposInfo( protocol, host, path, user )
            i += 1
    return reps

