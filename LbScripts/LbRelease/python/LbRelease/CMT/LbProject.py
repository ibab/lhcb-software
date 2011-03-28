from LbUtils.CMT import Project
from LbUtils.CMT import CMTWhich
from LbPackage import LbPackage
from LbConfiguration.Project import getProject
from LbConfiguration.Version import LHCb2CMT

import logging

import os



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


def LbCMTWhich(project, package=None, version=None, all_occurences=False,
               casesense=True, with_user_area=True):
    log = logging.getLogger()

    if os.environ.has_key("LHCBPROJECTPATH") :
        log.debug("Using LHCBPROJECTPATH (%s)" % os.environ["LHCBPROJECTPATH"])
        os.environ["CMTPROJECTPATH"] = os.environ["LHCBPROJECTPATH"]
    else :
        log.warning("No LHCBPROJECTPATH")
        if os.environ.has_key("CMTPROJECTPATH") :
            log.warning("Trying CMTPROJECTPATH instead")
        else :
            log.fatal("Not fallback CMTPROJECTPATH in the environment")
            return 1

    if version :
        proj, ver = LHCb2CMT(project, version)
        result = CMTWhich(proj, package, ver, all_occurences, casesense)
    else :
        proj, pack = LHCb2CMT(project, package)
        result = CMTWhich(proj, pack, version, all_occurences, casesense)

    if not result :
        result = CMTWhich(proj, package, version, all_occurences, casesense)

    if not result :
        result = CMTWhich(project, package, version, all_occurences, casesense)

    return result