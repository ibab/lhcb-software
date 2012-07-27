'''
Module for construction of Bs-->K*0K*0 stripping selections and lines

Exported symbols (use python help!):
   - StrippingBs2KstKstConf
   - makeBs2KstKst
   - makeBs2KstKstSameCharge
   - makeKst2Kpi
   - makeJPsi2mumu
   - makeBd2JPsiKst
'''

__author__ = ['Paula Alvarez Cartelle']
__date__ = '22/07/2011'
__version__= '3.0'

__all__=('StrippingBs2KstKstConf',
         'makeBs2KstKst',
         'makeBs2KstKstSameCharge',
         'makeKst2Kpi',
         'makeJPsi2mumu')

__config_default__ = { 
     "KaonPT"		: 500.0 # MeV
     ,	"KaonIPCHI2"		: 9. 	# adimensional
     ,  "PionPT"                : 500.0 # MeV
     ,  "PionIPCHI2"            : 9.	# adimensional
     ,  "PionPIDK"              :10. #adimensional
     ,	"KstarVCHI2"		: 9.0   # adimensional
     ,	"KstarPT"		: 900.0 # MeV
     ,	"KaonPIDK"              : -5.0  # adimensional
     ,	"KstarMassWin"		: 150.0 # MeV
     ,	"BMassWin"		: 500.0 # MeV
     ,	"BVCHI2"		: 15.0	# adimensional
     ,	"BDOCA"                 : 0.3   # mm
     ,  "BIPCHI2"               : 25    # adimensional
     }

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from CommonParticles.Utils import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

default_name = "Bs2KstKst"

class StrippingBs2KstKstConf(LineBuilder):
     """
     Builder of:
     - Bs-> Kst0 anti-Kst0 stripping line (Bs2KstKst)
     - Bs-> Kst0 Kst0 stripping line (BsKstKstSameCharge)
     - Bd-> JPsi Kst0 stripping line (Bd2JPsiKstForBs2KstKst)
    

     Usage:
     >>> config = { .... }
     >>> bsConf = StrippingBs2KstKstConf('Bs2KstKstTest',config)
     >>> bsLines = bsConf.lines
     >>> for line in line :
     >>>  print line.name(), line.outputLocation()
     The lines can be used directly to build a StrippingStream object.


     Exports as instance data members:
     selKst2Kpi               : nominal Kst->Kpi Selection object
     selJPsi2mumu             : JPsi->mu mu Selection object
     selBs2KstKst             : nominal Bs -> Kst(K+pi-) anti-Kst(K-pi+) Selection object
     selBs2KstKstSameCharge   : Bs -> Kst(K+pi-) Kst(K+pi-) Selection object
     self.selBd2JPsiKst       : Bd -> JPsi(mu+ mu-) Kst(K+ pi-) Selection object
     Bs2KstKst_line           : StrippingLine made out of selBs2KstKst
     Bs2KstKstSameCharge_line : StrippingLine made out of selBs2KstKstSameCharge
     Bd2JPsiKstar_line        : StrippingLine made out of selBd2JPsiKst
     
     lines                    : List of lines, [Bs2KstKst_line,Bs2KstKstSameCharge_line,Bd2JPsiKstar_line]
    
     Exports as class data member:
     StrippingBs2KstKstConf.__configuration_keys__ : List of required configuration parameters.
     """
     __configuration_keys__ = (
          "KaonPT",
          "KaonIPCHI2",
          "PionPT",
          "PionIPCHI2",
          "PionPIDK",
          "KstarVCHI2",
          "KstarPT",
          "KaonPIDK",
          "KstarMassWin",
          "BMassWin",
          "BVCHI2",
          "BDOCA",
          "BIPCHI2")


    


     def __init__(self, name, config) :

          LineBuilder.__init__(self, name, config)

          KstKst_name = name+"Nominal"
          KstKstSameCharge_name = name+'SameCharge'
          BdJpsiKst_name = "Bd2JPsiKstFor"+name

          self.selJPsi2mumu = makeJPsi2mumu('Jpsi2MuMuFor'+name,
                                            MuonPT = config['KaonPT'],
                                            MuonIPCHI2 = config['KaonIPCHI2'],
                                            JPsiPT = config["KstarPT"],
                                            JPsiVCHI2 = config['KstarVCHI2'])


          self.selKst2Kpi = makeKst2Kpi('Kst2KpiFor'+name,
                                        KaonPT = config['KaonPT'],
                                        KaonIPCHI2 = config['KaonIPCHI2'],
                                        PionPT = config['PionPT'],
                                        PionIPCHI2 = config['PionIPCHI2'],
                                        KstarPT = config["KstarPT"],
                                        KaonPIDK = config['KaonPIDK'],
                                        KstarVCHI2 = config['KstarVCHI2'],
                                        KstarMassWin = config['KstarMassWin'],
                                        PionPIDK = config['PionPIDK'])

          self.selBs2KstKst = makeBs2KstKst(KstKst_name,
                                            Kstsel = self.selKst2Kpi,
                                            BMassWin = config['BMassWin'],
                                            BVCHI2 = config['BVCHI2'],
                                            BDOCA = config['BDOCA'],
                                            BIPCHI2 = config['BIPCHI2'])
          
          self.selBs2KstKstSameCharge = makeBs2KstKstSameCharge(KstKstSameCharge_name,
                                                                Kstsel = self.selKst2Kpi,
                                                                BMassWin = config['BMassWin'],
                                                                BVCHI2 = config['BVCHI2'],
                                                                BDOCA = config['BDOCA'],
                                                                BIPCHI2 = config['BIPCHI2'])

          self.selBd2JPsiKst = makeBd2JPsiKst(BdJpsiKst_name,
                                              JPsisel = self.selJPsi2mumu,
                                              Kstsel = self.selKst2Kpi,
                                              BMassWin = config['BMassWin'],
                                              BVCHI2 = config['BVCHI2'],
                                              BDOCA = config['BDOCA'],
                                              BIPCHI2 = config['BIPCHI2'])
          
        
          self.Bs2KstKst_line = StrippingLine(KstKst_name+"Line",
                                              prescale = 1,
                                              postscale = 1,
                                              algos = [ self.selBs2KstKst ]
                                              )
          
          self.Bs2KstKstSameCharge_line = StrippingLine(KstKstSameCharge_name+"Line",
                                                        prescale = 1,
                                                        postscale = 1,
                                                        algos = [ self.selBs2KstKstSameCharge ]
                                                        )

          self.Bd2JPsiKstar_line = StrippingLine(BdJpsiKst_name+"Line",
                                                 prescale = 1,
                                                 postscale = 1,
                                                 algos = [ self.selBd2JPsiKst ]
                                                 )

          self.registerLine(self.Bs2KstKst_line)
          self.registerLine(self.Bs2KstKstSameCharge_line)
          self.registerLine(self.Bd2JPsiKstar_line)




def makeJPsi2mumu(name,
                  MuonPT,
                  MuonIPCHI2,
                  JPsiPT,
                  JPsiVCHI2) :
    """
    Create and return a JPsi -> Mu Mu Selection object.
    Starts from DataOnDemand 'Phys/StdLooseMuons/Particles'.
    Arguments:
    name             : name of the Selection.
    MuonPT           : Minimum transverse momentum of Mu (MeV).
    MuonIPCHI2       : Minimum impact parameter chi2 of Mu.
    JPsiPT           : Minimum transverse momentum of JPsi (MeV).
    JPsiVCHI2        : Maximum JPsi vertex chi2 per degree of freedom.
    """

    MuCuts = "(TRCHI2DOF < 5 )"\
             "& (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s)"\
             "& (PT > %(MuonPT)s *MeV)" % locals()

    MotherCuts = "(VFASPF(VCHI2/VDOF)< %(JPsiVCHI2)s)"\
                 "& (PT > %(JPsiPT)s *MeV)"\
                 "& (ADMASS('J/psi(1S)') < 100*MeV )" % locals()
                 
    _JPsi = CombineParticles("_"+name)
    _JPsi.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
    _JPsi.DaughtersCuts = { "mu+" : MuCuts}

    _JPsi.CombinationCut = "(ADAMASS('J/psi(1S)')<100*MeV) "\
                                   "& (AMAXDOCA('')<0.3*mm)"

    _JPsi.MotherCut = MotherCuts

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = _JPsi,
                      RequiredSelections = [ _stdLooseMuons ])




        
def makeKst2Kpi(name,
                KaonPT,
                KaonIPCHI2,
                PionPT,
                PionIPCHI2,
                KstarPT,
                KaonPIDK,
                KstarVCHI2,
                KstarMassWin,
                PionPIDK):

    """
    Create and return a Kstar -> K+pi- Selection object.
    Starts from DataOnDemand 'Phys/StdVeryLooseDetachedKst2Kpi'.
    Arguments:
    name             : name of the Selection.
    KaonPT           : Minimum transverse momentum of K (MeV).
    KaonIPCHI2       : Minimum impact parameter chi2 of K.
    PionPT           : Minimum transverse momentum of pi (MeV).
    PionIPCHI2       : Minimum impact parameter chi2 of pi.
    PionPIDK         : Maximum PID_{K-pi} of pi.
    KstarPT          : Minimum transverse momentum of Kstar (MeV).
    KaonPIDK         : Minimum PID_{K-pi} of K.
    KstarVCHI2       : Maximum Kstar vertex chi2 per degree of freedom.
    KstarMassWin     : Kstar invariant mass window around PDG mass value (MeV).
    """


    KstarCuts = "(INTREE((ABSID=='K+') & (PT > %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KaonIPCHI2)s) & (PIDK > %(KaonPIDK)s) ))"\
        "& (INTREE((ABSID=='pi-') & (PT > %(PionPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(PionIPCHI2)s) & (PIDK < %(PionPIDK)s )  ))"\
        "& (ADMASS('K*(892)0') < %(KstarMassWin)s *MeV)"\
        "& (VFASPF(VCHI2/VDOF)< %(KstarVCHI2)s) & (PT > %(KstarPT)s *MeV)"% locals()
        

    _KstarFilter = FilterDesktop("_filterFor"+name)
    _KstarFilter.Code = KstarCuts
    _stdKst2Kpi = DataOnDemand(Location="Phys/StdVeryLooseDetachedKst2Kpi/Particles")


    return Selection (name,
                      Algorithm = _KstarFilter,
                      RequiredSelections = [_stdKst2Kpi])
        





def makeBs2KstKst(name,
                  Kstsel,
                  BMassWin,
                  BVCHI2,
                  BDOCA,
                  BIPCHI2):
       """
       Create and return a Bs -> Kstar (Kpi) anti-Kstar (Kpi) Selection object.
       Arguments:
       name        : name of the Selection.
       Kstsel      : Kst -> K+pi- Selection object.
       BMassWin    : Bs invariant mass window around PDG mass value (MeV).
       BVCHI2      : Maximum Bs vertex chi2 per degree of freedom.
       BDOCA       : Maximum Bs DOCA.
       """ 
       
       _motherCuts = " (VFASPF(VCHI2/VDOF) < %(BVCHI2)s) & (MIPCHI2DV(PRIMARY)< %(BIPCHI2)s)"% locals()
       _combinationCut = "(ADAMASS('B_s0') < %(BMassWin)s *MeV) & (AMAXDOCA('')< %(BDOCA)s *mm)" % locals() 

       _Bs = CombineParticles('_'+name)
       _Bs.DecayDescriptor = "B_s0 -> K*(892)0 K*(892)~0"
       _Bs.CombinationCut = _combinationCut
       _Bs.MotherCut = _motherCuts

       _Bs.ReFitPVs = True

       _Bs.addTool( OfflineVertexFitter )
#       _Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
       _Bs.ParticleCombiners.update( { "" : "OfflineVertexFitter"} ) # Fix for DaVinci v32r0 by A.Poluektov
       _Bs.OfflineVertexFitter.useResonanceVertex = False


       return Selection ( name,
                          Algorithm = _Bs,
                          RequiredSelections = [Kstsel])





def makeBs2KstKstSameCharge(name,
                            Kstsel,
                            BMassWin,
                            BVCHI2,
                            BDOCA,
                            BIPCHI2):
       """
       Create and return a Bs -> Kstar (Kpi) Kstar (Kpi) Selection object.
       Arguments:
       name        : name of the Selection.
       Kstsel      : Kst -> K+pi- Selection object.
       BMassWin    : Bs invariant mass window around PDG mass value (MeV).
       BVCHI2      : Maximum Bs vertex chi2 per degree of freedom.
       BDOCA       : Maximum Bs DOCA.
       """ 
       
       _motherCuts = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s) & (MIPCHI2DV(PRIMARY)< %(BIPCHI2)s)"% locals()
       _combinationCut = "(ADAMASS('B_s0') < %(BMassWin)s *MeV) & (AMAXDOCA('')< %(BDOCA)s *mm)" % locals() 

       _Bs = CombineParticles('_'+name)
       _Bs.DecayDescriptor = "[B_s0 -> K*(892)0 K*(892)0]cc"
       _Bs.CombinationCut = _combinationCut
       _Bs.MotherCut = _motherCuts

       _Bs.ReFitPVs = True

       _Bs.addTool( OfflineVertexFitter )
#       _Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
       _Bs.ParticleCombiners.update( { "" : "OfflineVertexFitter"} ) # Fix for DaVinci v32r0 by A.Poluektov
       _Bs.OfflineVertexFitter.useResonanceVertex = False


       return Selection ( name,
                          Algorithm = _Bs,
                          RequiredSelections = [Kstsel])




def makeBd2JPsiKst(name,
                   JPsisel,
                   Kstsel,
                   BMassWin,
                   BVCHI2,
                   BDOCA,
                   BIPCHI2):
       """
       Create and return a Bd -> JPsi(mumu) Kstar (Kpi) Selection object.
       Arguments:
       name        : name of the Selection.
       JPsisel     : JPsi -> mu+ mu- Selection object.
       Kstsel      : Kst -> K+pi- Selection object.
       BMassWin    : Bd invariant mass window around PDG mass value (MeV).
       BVCHI2      : Maximum Bd vertex chi2 per degree of freedom.
       BDOCA       : Maximum Bd DOCA.
       """ 
       
       _motherCuts = " (VFASPF(VCHI2/VDOF) < %(BVCHI2)s) & (MIPCHI2DV(PRIMARY)< %(BIPCHI2)s)"% locals()
       _combinationCut = "(ADAMASS('B_s0') < %(BMassWin)s *MeV) & (AMAXDOCA('')< %(BDOCA)s *mm)" % locals() 

       _Bd = CombineParticles('_'+name)
       _Bd.DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc"
       _Bd.CombinationCut = _combinationCut
       _Bd.MotherCut = _motherCuts

       _Bd.ReFitPVs = True

       _Bd.addTool( OfflineVertexFitter )
#       _Bd.VertexFitters.update( { "" : "OfflineVertexFitter"} )
       _Bd.ParticleCombiners.update( { "" : "OfflineVertexFitter"} ) # Fix for DaVinci v32r0 by A.Poluektov
       _Bd.OfflineVertexFitter.useResonanceVertex = False

       
       return Selection ( name,
                          Algorithm = _Bd,
                          RequiredSelections = [JPsisel,Kstsel])
