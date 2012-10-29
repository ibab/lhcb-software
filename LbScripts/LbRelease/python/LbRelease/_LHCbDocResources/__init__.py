"""
Module to store resources (e.g. files) used by LHCbDoc.
"""
try:
    import pkg_resources
except ImportError:
    from LbUtils._helpers import pkg_resources


def _versionTuple(version):
    '''
    Helper function to translate a version string like 'x.y.z' to a tuple (x,y,z).

    >>> _versionTuple('1.2.3')
    (1, 2, 3)
    '''
    return tuple(map(int, version.split('.')))

def _getVersion(resource_name):
    '''
    Extract the resource version from a resource name of the format 'myResourceName.txt-X.Y.Z'.

    >>> _getVersion('layout.xml-1.8.2')
    (1, 8, 2)
    >>> _getVersion('layout.xml')
    None
    '''
    try:
        return _versionTuple(resource_name.rsplit('-', 1)[-1])
    except ValueError:
        # we get here if the string does not end with a dot-separated list of numbers
        pass

def getString(name, version):
    '''
    Return the content of the resource identified by 'name' and compatible with
    the specified version of Doxygen.

    The possible values of 'name' are: ['layout.xml', 'stylesheet.css', 'class.php'].

    Resources are looked for in the module directory as files with names like
    'resource.name-1.2.3' or 'resource.name'. The string returned corresponds to
    the content of the file with the highest version number smaller or equal to
    the requested version, or without version number if a compatible one was not
    found. If there is no resource file matching the rules, an exception is raised.
    '''
    # list of resources with their versions:
    #  'file.txt-1.2.3' -> ((1, 2, 3), 'file.txt-1.2.3')
    #  'file.txt'       -> (None, 'file.txt')
    resources = sorted([(_getVersion(r), r)
                        for r in pkg_resources.resource_listdir(__name__, '.')
                        if r.startswith(name)], reverse=True)

    vers = _versionTuple(version)

    for v, r in resources:
        if v <= vers: # note that None is smaller than any tuple
            return pkg_resources.resource_string(__name__, r)
    raise ValueError('Cannot find resource %r for version %s' % (r, vers))
