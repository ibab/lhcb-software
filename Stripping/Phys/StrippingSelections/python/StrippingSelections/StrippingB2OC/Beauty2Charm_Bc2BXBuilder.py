from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, MergedSelection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *

class Bc2BXBuilder(object):
     
    def __init__(self,b,pions,kaons,config):
   
        self.config = config
        self.b      = b
        self.pions   = [pions]
        self.kaons   = [kaons]
        self.lines  = []

        b2dhd2hh = [filterSelection('B2D0HD02HH',"(MM > 5150*MeV) & (MM < 5400*MeV)",self.b.b2dhd2hh)]
        b2dhd2hhhh = [filterSelection('B2D0HD02HHHH',"(MM > 5150*MeV) & (MM < 5400*MeV)",self.b.b2dhd2hhhh)]
        b2dhhd2hhhcfpid = [filterSelection('B2DHHD2HHHCFPID',"(MM > 5150*MeV) & (MM < 5400*MeV)",self.b.b2dhhd2hhhcfpid)]
        b2dhhhd2hhpid = [filterSelection('B2D0HHHD02HHPID',"(MM > 5150*MeV) & (MM < 5400*MeV)",self.b.b2dhhhd2hhpid)]
        b2dd = [filterSelection('B2D0D',"(MM > 5150*MeV) & (MM < 5400*MeV)",self.b.b2dd)]
        bmodes = [MergedSelection('AllB', RequiredSelections = b2dhd2hh + b2dhd2hhhh + b2dhhd2hhhcfpid + b2dhhhd2hhpid + b2dd)]

        self._makeBc2BHH("B2D0HD02HH",b2dhd2hh,self.config)
        #self._makeBc2BHH("B02D0HHD02HH",b02dhhd2hh,self.config)
        #self._makeBc2BHH("B02DHD2HHH",b02dhd2hhh,self.config)      
        self._makeBc2BHH("B2D0HD02HHHH",b2dhd2hhhh,self.config)
        self._makeBc2BHH("B2DHHD2HHHCFPID",b2dhhd2hhhcfpid,self.config)
        self._makeBc2BHH("B2D0HHHD02HHPID",b2dhhhd2hhpid,self.config)
        self._makeBc2BHH("B2D0D",b2dd,self.config)

        self._makeUpsilon2BB(bmodes)

    def _makeBc2BHH(self,bname, b2dx,config):
        decays = {'Bc2BPiPi':   ["[B_c+ -> B+ pi+ pi-]cc"],
                  'Bc2BPiPiWS': ["[B_c+ -> B+ pi+ pi+]cc"],
                  'Bc2BPiK':    ["[B_c+ -> B+ pi+ K-]cc"],
                  'Bc2BPiKWS':  ["[B_c+ -> B+ pi+ K+]cc"],
                  'Bc2BKPi':    ["[B_c+ -> B+ K+  pi-]cc"],
                  'Bc2BKK':     ["[B_c+ -> B+ K+ K-]cc"],
                  'Bc2BKKWS':   ["[B_c+ -> B+ K+ K+]cc"]
                   }
        inputs = {'Bc2BPiPi': b2dx+self.pions,
                  'Bc2BPiPiWS': b2dx+self.pions,
                  'Bc2BPiK':  b2dx+self.pions+self.kaons,
                  'Bc2BPiKWS':  b2dx+self.pions+self.kaons,
                  'Bc2BKPi':   b2dx+self.pions+self.kaons,
                  'Bc2BKK':     b2dx+self.kaons,
                  'Bc2BKKWS':   b2dx+self.kaons}
         
        bc2bhh = makeBc2BXSels(decays,bname,inputs,config)
        self.lines.append(ProtoLine(bc2bhh,1.0))   

    def _makeUpsilon2BB(self,bmodes):
        comboCuts = "( (ACHILD(PT,1) +ACHILD(PT,2)) > 500*MeV) & (AM > 10000*MeV) & (AM < 12000*MeV)"
        momCuts = "(VFASPF(VCHI2/VDOF) < 20) & ( (CHILD(VFASPF(VZ),1) - VFASPF(VZ)) > -1.5*mm) & ((CHILD(VFASPF(VZ),2) - VFASPF(VZ)) > -1.5*mm)"
        u2bb = CombineParticles(DecayDescriptor= "Upsilon(4S) -> B+ B-",
                                CombinationCut=comboCuts, 
                                MotherCut=momCuts)
        u2bbws = CombineParticles(DecayDescriptors=["[Upsilon(4S) -> B+ B+]cc"],
                                  CombinationCut=comboCuts,
                                  MotherCut=momCuts)
        u2bbsel = [Selection('Upsilon2BB'+'Beauty2Charm',Algorithm=u2bb,RequiredSelections=bmodes)] 
        u2bbselws = [Selection('Upsilon2BBWS'+'Beauty2Charm',Algorithm=u2bbws,RequiredSelections=bmodes)]                   
        self.lines.append(ProtoLine(u2bbsel,1.0))
        self.lines.append(ProtoLine(u2bbselws,1.0))
