import re

def isValidVersion(project, version):
    '''
    Check if the specified version number is a valid (reasonable) one for the
    specified project.
    '''
    # FIXME: for the moment we accept only some simple values, but we should look for aliases too
    return (version.lower() in ('latest', 'head')
            or re.match(r"^v[0-9]+r[0-9]+(p[0-9]+)?(g[0-9]+)?$", version)
            or (project == 'LCGCMT' and re.match(r"^[0-9]+([a-z]?|rc[0-9]+)$", version))
            )

def expandVersionAlias(project, version):
    '''
    Given a project and a version, check if the version is an alias for an
    explicit version (e.g. latest, reco19) and return the real version or the argument.
    '''
    # FIXME: for the moment there is no mechanism to store version aliases
    return version
