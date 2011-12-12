from LbRelease.CMT.Version import LHCb2CMT, CMT2LHCb

from LbUtils.CMT import Project
from LbUtils.CMT import CMTWhich
from LbPackage import LbPackage
from LbConfiguration.Project import getProject
from LbConfiguration.Version import sortStrings

import logging

import os



class LbProject(Project):
    def __init__(self, projectpath):
        super(LbProject,self).__init__(projectpath)
        self._pkgclass = LbPackage
        self._conf = None
        self._release_notes = None
        self._release_date = None
    def configuration(self):
        if self._conf is None :
            self._conf = getProject(self.name())
        return self._conf
    def container(self):
        log = logging.getLogger()
        if self._container is None :
            self._container = super(LbProject, self).container()
            if self._container == "" :
                log.debug("Native container not found. Using configuration.")
                self._container = self.configuration().SteeringPackage()
        return self._container
    def CMTVersion(self):
        return super(LbProject, self).version()
    def version(self):
        return CMT2LHCb(self.CMTName().upper(), self.CMTVersion())[1]
    def CMTName(self):
        return super(LbProject, self).name()
    def name(self):
        return CMT2LHCb(self.CMTName(),self.CMTVersion())[0]
    def releaseNotes(self):
        if self._release_notes is None :
            self._release_notes = self.containerPackage().releaseNotes()
        return self._release_notes
    def releaseDate(self):
        if self._release_date is None :
            self._release_date = self.containerPackage().releaseDate()
        return self._release_date

def LbCMTWhich(project, package=None, version=None, all_occurences=False,
               casesense=True, with_user_area=True,
               project_class=LbProject, package_class=LbPackage):
    log = logging.getLogger()

    result = None

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

    if with_user_area and os.environ.has_key("User_release_area") :
        log.debug("adding the User release area %s to CMTPROJECTPATH" % os.environ["User_release_area"])
        os.environ["CMTPROJECTPATH"] = os.pathsep.join([os.environ["User_release_area"], os.environ["CMTPROJECTPATH"]])

    log.debug("CMTPROJECTPATH used: %s" % os.environ["CMTPROJECTPATH"])

    if with_user_area and os.environ.has_key("User_release_area") :
        if version :
            proj = "%s_%s" % (project, version)
            if os.path.exists(os.path.join(os.environ["User_release_area"], proj)) :
                result = CMTWhich(proj, package, None, all_occurences, casesense,
                                  project_class=project_class)
        else :
            proj = "%s_%s" % (project, package)
            if os.path.exists(os.path.join(os.environ["User_release_area"], proj)) :
                result = CMTWhich(proj, None, None, all_occurences, casesense,
                                  project_class=project_class)

        if not result and not version :
            prjlist = []
            for l in os.listdir(os.environ["User_release_area"]) :
                if l.startswith("%s_" % project) :
                    prjlist.append(l)
            prjlist = sortStrings(prjlist, versiontype=None, reverse=True)
            prjlist.insert(0, "%s_HEAD" % project)
            for p in prjlist :
                if os.path.exists(os.path.join(os.environ["User_release_area"], p)) :
                    result = CMTWhich(p, package , None, all_occurences, casesense,
                                      project_class=project_class)
                    if result :
                        break

    if not result :
        result = CMTWhich(project, package, version, all_occurences, casesense,
                          project_class=project_class)

    if not result :
        if version :
            proj, ver = LHCb2CMT(project, version)
            result = CMTWhich(proj, package, ver, all_occurences, casesense,
                              project_class=project_class)
        else :
            proj, pack = LHCb2CMT(project, package)
            result = CMTWhich(proj, pack, version, all_occurences, casesense,
                              project_class=project_class)

    if not result :
        result = CMTWhich(proj, package, version, all_occurences, casesense,
                          project_class=project_class)

    return result

