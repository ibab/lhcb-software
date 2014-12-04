#!/usr/bin/env python
"""
LbInstall specific config for ATLAS

"""
import os

class Config:

    def __init__(self):
        self.CONFIG_VERSION=1
    
    def initYUM(self, installArea):
        """ Initialize the YUM config """
        repourl = "http://lhcbproject.web.cern.ch/lhcbproject/dist/rpm"
        if installArea.repourl != None:
            repourl =  installArea.repourl
        
        reposdpath = installArea.yumreposd
        yumrepolhcb = os.path.join(reposdpath, "lhcb.repo")
        yumrepolcg = os.path.join(reposdpath, "lcg.repo")
        extrasurl = "/".join([repourl, "extras"])
        rpmsurl = "/".join([repourl, "rpm"])
        lhcbsurl = "/".join([repourl, "lhcb"])

        if not os.path.exists(yumrepolhcb):
            yplf = open(yumrepolhcb, 'w')
            yplf.write(installArea._getYumRepo("lhcbold", rpmsurl))
            yplf.write(installArea._getYumRepo("lhcb", lhcbsurl))
            yplf.close()

        if not os.path.exists(yumrepolcg):
            lcgsurl = "http://service-spi.web.cern.ch/service-spi/external/rpms/lcg"
            yplf = open(yumrepolcg, 'w')
            yplf.write(installArea._getYumRepo("lcg", lcgsurl))
            yplf.close()


        lbrepourl = "http://lhcbproject.web.cern.ch/lhcbproject/dist/rpm"
        reposdpath = installArea.yumreposd
        yumrepolhcbext = os.path.join(reposdpath, "lhcbext.repo")
        lhcbsurlext = "/".join([lbrepourl, "lcg"])

        if not os.path.exists(yumrepolhcbext):
            yplf = open(yumrepolhcbext, 'w')
            yplf.write(installArea._getYumRepo("lhcbext", lhcbsurlext))
            yplf.close()

    def getRelocateCommand(self, siteroot):
        """ Returns relocate command to be passed to RPM for the repositories """
        rpmcmd  = " --relocate %s=%s " % ('/opt/lcg/external', os.path.join(siteroot, 'lcg', 'external'))
        rpmcmd += " --relocate %s=%s " % ('/opt/lcg', os.path.join(siteroot, 'lcg', 'releases'))
        rpmcmd += " --relocate %s=%s " % ('/opt/LHCbSoft', siteroot)
        rpmcmd += " --badreloc "
        return rpmcmd

