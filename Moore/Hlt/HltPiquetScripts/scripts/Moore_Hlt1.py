import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

## General Moore stuff, DB tags, etc.
Moore().OutputLevel = 3
Moore().EvtMax = 10000
Moore().DataType = '2015'
Moore().CondDBtag = 'cond-20150625'
Moore().DDDBtag = 'dddb-20150526'
Moore().Split = 'Hlt1'
Moore().RemoveInputHltRawBanks = True

from Configurables import EventSelector
EventSelector().PrintFreq = 100

## Enable histograms and some output
from Configurables import HltConf
HltConf().Verbose = True
HltConf().EnableMonitoring = True
HltConf().HistogrammingLevel = "All"

## EM Settings
Moore().ThresholdSettings = 'Physics_draftEM2015'

## CondDB for online
from Configurables import CondDB
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = True
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

import sys
try:
    import All
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import All

CondDB().RunChangeHandlerConditions = All.ConditionMap

from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseSetCurrent = True

## Input
base_dir = '/net/hlta0405/localdisk/Alignment/BWDivision'
import re
re_file = re.compile("Run_(\d+)_.*.mdf")
files = sorted([os.path.join(base_dir, f) for f in os.listdir(base_dir) if (int(re_file.match(f).group(1)) > 154795)])
Moore().inputFiles = files

# base_dir = '/localdisk/hlt1/data2015/0506NB_hlt_stripped'
# Moore().inputFiles = sorted([os.path.join(base_dir, f) for f in os.listdir(base_dir)])

## Write ROOT histogram file.
from Configurables import HistogramPersistencySvc, RootHistCnv__PersSvc
HistogramPersistencySvc().OutputFile = "MooreHistos.root"
RootHistCnv__PersSvc().OutputFile = "MooreHistos.root"

from GaudiPython import AppMgr
gaudi = AppMgr()

# gaudi.addAlgorithm(bs_algo)

gaudi.initialize()

TES = gaudi.evtSvc()

from collections import defaultdict
events = defaultdict(int)

# rf = gaudi.algorithm(richFilter.getFullName())
# from GaudiPython.Bindings import InterfaceCast, gbl
# rf = InterfaceCast(gbl.IAlgorithm)(rf.getInterface())

def cand_info(cand):
    tx = cand.numericalInfo()['3#Track.firstState.tx']
    ty = cand.numericalInfo()['4#Track.firstState.ty']
    qop = cand.numericalInfo()['5#Track.firstState.qOverP']
    chi2dof = cand.numericalInfo()[ "6#Track.chi2PerDoF" ]
    p = abs(1. / qop)

    nf = sqrt(tx * tx + ty * ty + 1);
    pT = sqrt(tx * tx + ty * ty) * p / nf;

    v = gbl.Gaudi.XYZVector(tx, ty, 1);
    eta = v.eta()
    phi = v.phi()

    return pT, p, chi2dof, eta, phi,

n = 0
while True:
    r = gaudi.run(1)
    if not TES['/Event']:
        break

    decReports = TES['Hlt1/DecReports']
    report = decReports.decReport("Hlt1CalibRICHMirrorRICH1Decision")
    if not report.decision():
        continue

    selReports = TES['Hlt1/SelReports']
    report = selReports.selReport("Hlt1CalibRICHMirrorRICH1Decision")
    if not report:
        continue

    for cand in report.substructure():
        print cand_info(cand.data())

    break
    # print rf.filterPassed(), '\n'
