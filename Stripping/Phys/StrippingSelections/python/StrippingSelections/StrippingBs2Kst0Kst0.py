'''
Module for construction of Bs-->K*0K*0 stripping selections and lines

Exported symbols (use python help!):
   - StrippingBs2KstKstConf
   - makeBs2KstKst
   - makeBs2KstKstSameCharge
   - makeKst2Kpi
'''




__author__ = ['Paula Alvarez Cartelle','Brais Sanmartin Sedes']
__date__ = '27/08/2014'
__version__= '6.0'

__all__=('StrippingBs2KstKstConf',
         'makeBs2KstKst',
         'makeBs2KstKstSameCharge',
         'makeKst2Kpi',
         'default_config')


default_config = {
     "NAME"       :    "Bs2KstKst",
     'WGs'         : ['Charmless'],
     'BUILDERTYPE' : 'StrippingBs2KstKstConf',
     'CONFIG'      : {"KaonPT"       : 500.0,
                      "KaonIPCHI2"   : 9.,
                      "PionPT"       : 500.0,
                      "PionIPCHI2"   : 9.,
                      "PionPIDK"     : 10.,
                      "KstarVCHI2"   : 9.0,
                      "KstarPT"      : 900.0,
                      "KaonPIDK"     : -5.0,
                      "KstarMassWin" : 150.0,
                      "BMassWin"     : 500.0,
                      "BVCHI2"       : 15.0,
                      "BDOCA"        : 0.3,
                      "BIPCHI2"      : 25,
                      "MaxGHOSTPROB" : 0.8,
                      "BDIRA"        : 0.99
                                          },
     'STREAMS'     : { 'Bhadron' : ['StrippingBs2KstKstSameChargeLine'],
			'BhadronCompleteEvent' :  ['StrippingBs2KstKstNominalLine']
                       }
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
         

     Usage:
     >>> config = { .... }
     >>> bsConf = StrippingBs2KstKstConf('Bs2KstKstTest',config)
     >>> bsLines = bsConf.lines
     >>> for line in line :
     >>>  print line.name(), line.outputLocation()
     The lines can be used directly to build a StrippingStream object.


     Exports as instance data members:
     selKst2Kpi               : nominal Kst->Kpi Selection object
     selBs2KstKst             : nominal Bs -> Kst(K+pi-) anti-Kst(K-pi+) Selection object
     selBs2KstKstSameCharge   : Bs -> Kst(K+pi-) Kst(K+pi-) Selection object
     Bs2KstKst_line           : StrippingLine made out of selBs2KstKst
     Bs2KstKstSameCharge_line : StrippingLine made out of selBs2KstKstSameCharge
          
     lines                    : List of lines, [Bs2KstKst_line,Bs2KstKstSameCharge_line]
    
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
          "BIPCHI2",
          "MaxGHOSTPROB",
          "BDIRA")


    


     def __init__(self, name, config) :

          LineBuilder.__init__(self, name, config)

          KstKst_name = name+"Nominal"
          KstKstSameCharge_name = name+'SameCharge'


          self.selKst2Kpi = makeKst2Kpi('Kst2KpiFor'+name,
                                        KaonPT = config['KaonPT'],
                                        KaonIPCHI2 = config['KaonIPCHI2'],
                                        PionPT = config['PionPT'],
                                        PionIPCHI2 = config['PionIPCHI2'],
                                        KstarPT = config["KstarPT"],
                                        KaonPIDK = config['KaonPIDK'],
                                        KstarVCHI2 = config['KstarVCHI2'],
                                        KstarMassWin = config['KstarMassWin'],
                                        PionPIDK = config['PionPIDK'],
                                        MaxGHOSTPROB = config['MaxGHOSTPROB'])

          self.selBs2KstKst = makeBs2KstKst(KstKst_name,
                                            Kstsel = self.selKst2Kpi,
                                            BMassWin = config['BMassWin'],
                                            BVCHI2 = config['BVCHI2'],
                                            BDOCA = config['BDOCA'],
                                            BIPCHI2 = config['BIPCHI2'],
                                            BDIRA = config['BDIRA'])
          
          self.selBs2KstKstSameCharge = makeBs2KstKstSameCharge(KstKstSameCharge_name,
                                                                Kstsel = self.selKst2Kpi,
                                                                BMassWin = config['BMassWin'],
                                                                BVCHI2 = config['BVCHI2'],
                                                                BDOCA = config['BDOCA'],
                                                                BIPCHI2 = config['BIPCHI2'],
                                            BDIRA = config['BDIRA'])

          
        
          self.Bs2KstKst_line = StrippingLine(KstKst_name+"Line",
                                              prescale = 1,
                                              postscale = 1,
                                              algos = [ self.selBs2KstKst ],
                                              EnableFlavourTagging = True,
 					      RequiredRawEvents = ["Trigger","Muon","Calo","Rich","Velo","Tracker"])
                                               
          self.Bs2KstKstSameCharge_line = StrippingLine(KstKstSameCharge_name+"Line",
                                                        prescale = 1,
                                                        postscale = 1,
                                                        algos = [ self.selBs2KstKstSameCharge ],
                                                        EnableFlavourTagging = True,
							MDSTFlag = True)


          self.registerLine(self.Bs2KstKst_line)
          self.registerLine(self.Bs2KstKstSameCharge_line)






        
def makeKst2Kpi(name,
                KaonPT,
                KaonIPCHI2,
                PionPT,
                PionIPCHI2,
                KstarPT,
                KaonPIDK,
                KstarVCHI2,
                KstarMassWin,
                PionPIDK,
                MaxGHOSTPROB):

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
    MaxGHOSTPROB     : Maximum Ghost Probability
    """


    KstarCuts = "(INTREE((ABSID=='K+') & (PT > %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KaonIPCHI2)s) & (PIDK > %(KaonPIDK)s) & (TRGHOSTPROB< %(MaxGHOSTPROB)s) ))"\
        "& (INTREE((ABSID=='pi-') & (PT > %(PionPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(PionIPCHI2)s) & (PIDK < %(PionPIDK)s ) & (TRGHOSTPROB< %(MaxGHOSTPROB)s) ))"\
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
                  BIPCHI2,
                  BDIRA):
       """
       Create and return a Bs -> Kstar (Kpi) anti-Kstar (Kpi) Selection object.
       Arguments:
       name        : name of the Selection.
       Kstsel      : Kst -> K+pi- Selection object.
       BMassWin    : Bs invariant mass window around PDG mass value (MeV).
       BVCHI2      : Maximum Bs vertex chi2 per degree of freedom.
       BDOCA       : Maximum Bs DOCA.
       """ 
       
       _motherCuts = " (VFASPF(VCHI2/VDOF) < %(BVCHI2)s) & (MIPCHI2DV(PRIMARY)< %(BIPCHI2)s) & (BPVDIRA > %(BDIRA)s)"% locals()
       _combinationCut = "(ADAMASS('B_s0') < %(BMassWin)s *MeV) & (AMAXDOCA('')< %(BDOCA)s *mm)" % locals() 

       _Bs = CombineParticles('_'+name)
       _Bs.DecayDescriptor = "B_s0 -> K*(892)0 K*(892)~0"
       _Bs.CombinationCut = _combinationCut
       _Bs.MotherCut = _motherCuts

       _Bs.ReFitPVs = False

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
                            BIPCHI2,
                            BDIRA):
       """
       Create and return a Bs -> Kstar (Kpi) Kstar (Kpi) Selection object.
       Arguments:
       name        : name of the Selection.
       Kstsel      : Kst -> K+pi- Selection object.
       BMassWin    : Bs invariant mass window around PDG mass value (MeV).
       BVCHI2      : Maximum Bs vertex chi2 per degree of freedom.
       BDOCA       : Maximum Bs DOCA.
       """ 
       
       _motherCuts = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s) & (MIPCHI2DV(PRIMARY)< %(BIPCHI2)s) & (BPVDIRA > %(BDIRA)s)"% locals()
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





