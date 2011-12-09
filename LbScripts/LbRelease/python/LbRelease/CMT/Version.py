from LbConfiguration.Project import getProject
import re

lhcb_version_style = re.compile(r'v([0-9]+)r([0-9]+)(?:p([0-9]+))?')
lcg_version_style = re.compile(r'([0-9]+)([a-z]?)')

class Version(object):
    def __init__(self, versname):
        self._name = versname
        self._major = 0
        self._minor = 0
        self._patch = 0
    def __repr__(self):
        return self._name
    def __eq__(self, other):
        return self._name == other

class LHCbVersion(Version):
    def __init__(self, versname):
        super(LHCbVersion, self).__init__(versname)
        self._style = lhcb_version_style
        m = self._style.match(self._name)
        if m:
            a,b,c = m.groups()
            self._major = int(a)
            self._minor = int(b)
            if c:
                self._patch = int(c)
            else:
                self._patch = 0


def CMT2LHCb(projectname, projectversion):
    newversion = projectversion.split("_")[1]
    newname = getProject(projectname).Name()
    return newname, newversion


def LHCb2CMT(projectname, projectversion):
    newname = projectname.upper()
    if projectversion :
        newversion = "_".join([projectname.upper(),projectversion])
    else :
        newversion = None
    return newname, newversion