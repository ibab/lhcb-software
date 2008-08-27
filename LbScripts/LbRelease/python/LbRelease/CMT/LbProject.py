from LbUtils.CMT import Project
from LbPackage import LbPackage
from LbConfiguration.Project import getProject


class LbProject(Project):
    def __init__(self, projectpath):
        super(LbProject,self).__init__(projectpath)
        self._pkgclass = LbPackage
        self._conf = None
    def configuration(self):
        if self._conf is None :
            self._conf = getProject(self.name())
        return self._conf
    def container(self):
        if self._container is None :
            self._container = super(LbProject, self).container()
            if self._container == "" : 
                self._container = self.configuration().SteeringPackage()
        return self._container