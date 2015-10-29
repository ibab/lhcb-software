import re
import logging

log = logging.getLogger(__name__)

# default version alias
DEFAULT_VERSION = 'prod'

def isValidVersion(project, version):
    '''
    Check if the specified version number is a valid (reasonable) one for the
    specified project.
    '''
    # FIXME: for the moment we accept only some simple values, but we should look for aliases too (LBCORE-938)
    return (version.lower() in ('prod', 'latest', 'head')
            or re.match(r'^v[0-9]+r[0-9]+(p[0-9]+)?(g[0-9]+)?$', version)
            or (project == 'LCGCMT' and re.match(r'^[0-9]+([a-z]?|rc[0-9]+)$', version))
            or (project == 'ROOT' and re.match(r'\d+\.\d+\.\d+', version))
            )

def expandVersionAlias(project, version, platform):
    '''
    Given a project and a version, check if the version is an alias for an
    explicit version (e.g. latest, reco19) and return the real version or the argument.
    '''
    log.debug('resolving version %r for %s %s', version, project, platform)
    result = version
    if version.lower() == 'latest':
        from lookup import listVersions
        for vers, _path in listVersions(project, platform):
            if vers.lower() != 'head':
                result = vers
                break
    else:
        # FIXME: for the moment there is no mechanism to store version aliases (LBCORE-938)
        pass
    log.debug('using %r', result)
    return result
