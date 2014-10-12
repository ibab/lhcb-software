#!/usr/bin/env python
"""
LbInstall specific config for LCG

"""
import os

class Config:

    def __init__(self):
        self.CONFIG_VERSION=1
    
    def initYUM(self, installArea):
        """ Initialize the YUM config """
        repourl = "http://service-spi.web.cern.ch/service-spi/external/rpms/lcg"
        if installArea.repourl != None:
            repourl =  installArea.repourl
        
        reposdpath = installArea.yumreposd
        yumrepolcg = os.path.join(reposdpath, "lcg.repo")
        extrasurl = "/".join([repourl, "extras"])
        rpmsurl = "/".join([repourl, "rpm"])
        lcgsurl = repourl

        if not os.path.exists(yumrepolcg):
            yplf = open(yumrepolcg, 'w')
            yplf.write(installArea._getYumRepo("lcg", lcgsurl))
            yplf.close()


    def getPrefix(self):
        """ Returns the prefix in this case """
        return "/opt/lhcb"

