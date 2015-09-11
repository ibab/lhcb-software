from ROOT import *
from math import *
import os
import sys

def getLumi(inputFile_name):
    inputFile = TFile("%s"%(inputFile_name))
    inputTree = inputFile.Get("GetIntegratedLuminosity/LumiTuple")
    lumi = 0.0
    lumiErr = 0.0
    for event in inputTree:
        lumi += event.IntegratedLuminosity
        lumiErr += event.IntegratedLuminosityErr*event.IntegratedLuminosityErr

    return lumi, lumiErr


root = "/zfs_lhcb/users/sakar/realData/etacPhi/"
# fileNames = ["%s/2012/MagUp4000GeV_Reco14_s21_dv36r2_StrippingBs2EtacPhiBDTLine_2012_TrigLines.root"%root,
#              "%s/2012/MagDown4000GeV_Reco14_s21_dv36r2_StrippingBs2EtacPhiBDTLine_2012_TrigLines.root"%root,
#              "%s/2011/MagUp3500GeV_Reco14_s21r1_dv36r2_StrippingBs2EtacPhiBDTLine_2011_TrigLines.root"%root,
#              "%s/2011/MagDown3500GeV_Reco14_s21r1_dv36r2_StrippingBs2EtacPhiBDTLine_2011_TrigLines.root"%root]

fileNames = ["%s/2012/MagUpandDown4000GeV_Reco14_s21_dv36r2_StrippingBs2EtacPhiBDTLine_2012_TrigLines.root"%root,
             "%s/2011/MagUpandDown3500GeV_Reco14_s21r1_dv36r2_StrippingBs2EtacPhiBDTLine_2011_TrigLines.root"%root]

# root = "/zfs_lhcb/users/martin/Bs2EtacPhi/Tuples/merge/4h/"
# fileNames = ["%s/2012/Bs2etacphi_S21_20150511_4h_2012_Up.root"%root,
#              "%s/2012/Bs2etacphi_S21_20150511_4h_2012_Down.root"%root,
#              "%s/2011/Bs2etacphi_S21_20150511_4h_2011_Up.root"%root,
#              "%s/2011/Bs2etacphi_S21_20150511_4h_2011_Down.root"%root]

TotLumi = 0.0
TotLumiErr = 0.0
for fileName in fileNames:
    lumi, lumiErr = getLumi(fileName)
    TotLumi += lumi
    TotLumiErr += lumiErr
    print "Integrated luminosity is (%s +/- %s) in file %s"%(lumi,sqrt(lumiErr),fileName)

print "Total integrated luminosity is (%s +/- %s) for all files"%(TotLumi,sqrt(TotLumiErr))
