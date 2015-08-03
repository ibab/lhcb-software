##
##  File containing options to lower kinetic energies cuts
##  Options provided by M.Needham
##

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits

from Configurables import ( GiGa, GiGaRunActionSequence, TrCutsRunAction )
giga = GiGa()
giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
giga.RunSeq.addTool( TrCutsRunAction("TrCuts") , name = "TrCuts" )
giga.RunSeq.TrCuts.MuonTrCut     = 1.0*SystemOfUnits.MeV
giga.RunSeq.TrCuts.pKpiCut       = 1.0*SystemOfUnits.MeV
giga.RunSeq.TrCuts.NeutrinoTrCut = 0.0*SystemOfUnits.MeV
giga.RunSeq.TrCuts.NeutronTrCut  = 0.0*SystemOfUnits.MeV
giga.RunSeq.TrCuts.GammaTrCut    = 0.1*SystemOfUnits.MeV
giga.RunSeq.TrCuts.ElectronTrCut = 0.1*SystemOfUnits.MeV
giga.RunSeq.TrCuts.OtherTrCut    = 0.0*SystemOfUnits.MeV



