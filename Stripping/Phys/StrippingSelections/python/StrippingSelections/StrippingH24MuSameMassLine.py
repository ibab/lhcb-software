'''
Module for construction of h-->MuMuMuMu stripping selection.
Smaller rate, aiming for full dst. Both MuMu are required to
be resonant with aproximately the same mass.

Exported symbols (use python help!):
   - H24MuSameMassLineConf

Based on Bsmumu stripping lines
'''

__author__ = ['Xabier Cid Vidal']
__date__ = '11/22/2013'

__all__ = ('H24MuSameMassLineConf',
           'config_default_SameMass'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import OfflineVertexFitter

    
#### This is the dictionary of all tunable cuts ########
config_default_SameMass = {
    'DefaultPostscale'       : 1,
    'PromptLinePrescale'     : 1,
    'SimpleLinePrescale'     : 0.1,
    'DetachedLinePrescale'   : 1,
    
    'MuTrackChi2DoF'         : 3,
    'MupTprompt'             : 400,  #MeV
    'MupTdetached'           : 200,  #MeV
    'MuGhostProb'            : 0.4,
    'MuMaxIPchi2'            : 2,
    'MuMinIPchi2'            : 1,
    'MuPIDdll'               : -3, # muon combDLL
    'MuNShared'              : 2, # muon NShared
    
    'A1maxMass'              : 3500, #MeV
    'A1Doca'                 : 0.2,   #mm
    'A1DocaTight'            : 0.1,   #mm
    'A1Vchi2'                : 5,
    'A1Vchi2Tight'           : 1,
    'A1Dira'                 : 0,
    'A1maxIPchi2'            : 16,
    'A1FDChi2'               : 4,

    'A1sDeltaMass'           : 200, #MeV
    'HmaxDOCA'               : 1, #mm
    'HmaxDOCATight'          : 0.25, #mm
    'HVchi2'                 : 10,
    'HVchi2Tight'            : 2,
    'HpT'                    : 1200, #MeV
 
}                


class H24MuSameMassLineConf(LineBuilder) :
    """
    Builder of:
     - H-> mumumumu stripping lines: prompt, detached and control,

    Usage:
    >>> config = { .... }
    >>> Conf = H24MuSameMassLinesConf('Test',config)
    >>> myLines = Conf.lines
    >>> for line in myLines:
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.


    Exports as instance data members:
    selPrompt     : nominal prompt H24mu stripping line
    selSimple     : nominal simple H24mu stripping line (no pT, IP cuts)
    selDetached   : nominal detached H24mu stripping line
    promptLine    : Stripping line made from selPrompt
    simpleLine    : Stripping line made from selSimple
    detachedLine  : Stripping line made from selDetached
    lines         : list of lines:  [ promptLine, detachedLine, simpleLine]
    
    """
 
    __configuration_keys__ = (
        'DefaultPostscale',
        'PromptLinePrescale',
        'SimpleLinePrescale',
        'DetachedLinePrescale',
        'MuTrackChi2DoF',
        'MuPIDdll',
        'MuNShared',
        
        'MupTprompt',
        'MupTdetached',
        'MuMaxIPchi2',
        'MuMinIPchi2',
        'MuGhostProb',
        'A1maxMass',
        'A1Doca',
        'A1Vchi2',
        'A1DocaTight',
        'A1Vchi2Tight',
        'A1Dira',
        'A1maxIPchi2',
        'A1FDChi2',
        'A1sDeltaMass',
        'HmaxDOCA',
        'HpT',
        'HVchi2',
        'HmaxDOCATight',
        'HVchi2Tight',

    )


    def __init__(self, 
                 name = 'H24mu',
                 config = None,
                 debug_cuts = 0):
                 

        LineBuilder.__init__(self, name, config)

        prompt_name=name+'Prompt'
        simple_name=name+'Simple'
        detached_name=name+'Detached'

        self.config_dict = config
        self.debug_cuts = debug_cuts
        self.selPrompt = self.makeDefault(prompt_name,type = 0)
        self.selSimple = self.makeDefault(simple_name,type = 1)
        self.selDetached = self.makeDefault(detached_name,type = 2)
        
        self.promptLine = StrippingLine(prompt_name+"Line",
                                        prescale = config['PromptLinePrescale'],
                                        postscale = config['DefaultPostscale'],
                                        algos = [ self.selPrompt ]
                                        )

        self.simpleLine = StrippingLine(simple_name+"Line",
                                        prescale = config['SimpleLinePrescale'],
                                        postscale = config['DefaultPostscale'],
                                        algos = [ self.selSimple ]
                                        )


        self.detachedLine = StrippingLine(detached_name+"Line",
                                          prescale = config['DetachedLinePrescale'],
                                          postscale = config['DefaultPostscale'],
                                          algos = [ self.selDetached ]
                                          )
        

        self.registerLine(self.promptLine)
        self.registerLine(self.simpleLine)
        self.registerLine(self.detachedLine)



    def makeA1(self,name,type) :
        """
        Prompt A1 selection
        Arguments:
        name        : name of the Selection.
        type        : 0 (prompt), 1 (simple), 2 (detached)
        """

        A1 = CombineParticles("Combine"+name)
        A1.DecayDescriptor = "KS0 -> mu+ mu-"
        # prompt
        if type==0:
            A1.DaughtersCuts = { "mu+" : "(TRCHI2DOF < %(MuTrackChi2DoF)s ) "\
                                 "& ( TRGHOSTPROB < %(MuGhostProb)s ) " \
                                 "& (PT > %(MupTprompt)s * MeV ) "\
                                 "& (MIPCHI2DV(PRIMARY)< %(MuMaxIPchi2)s )" %self.config_dict }
            
            A1.CombinationCut = "(AM < %(A1maxMass)s * MeV ) "\
                                 "& (AMAXDOCA('')<%(A1Doca)s * mm)" %self.config_dict
            
            A1.MotherCut = "(VFASPF(VCHI2)< %(A1Vchi2)s ) "\
                           "& (MM < %(A1maxMass)s * MeV)" %self.config_dict

        # simple: tighten DOCA and Vchi2, tighten muID cut
        elif type==1:
            A1.DaughtersCuts = { "mu+" : "(TRCHI2DOF < %(MuTrackChi2DoF)s ) "\
                                         "& ( TRGHOSTPROB < %(MuGhostProb)s ) " \
                                         "& (PIDmu > %(MuPIDdll)s )  "\
                                         "& (PPINFO(LHCb.ProtoParticle.MuonNShared,99999)<= %(MuNShared)s ) " %self.config_dict }
            A1.CombinationCut = "(AM < %(A1maxMass)s * MeV ) "\
                                 "& (AMAXDOCA('')<%(A1DocaTight)s * mm)" %self.config_dict
            
            A1.MotherCut = "(VFASPF(VCHI2)< %(A1Vchi2Tight)s ) "\
                           "& (MM < %(A1maxMass)s * MeV)" %self.config_dict


        #detached
        elif type==2:

            #A1.addTool( OfflineVertexFitter )
            #A1.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
            #A1.ReFitPVs = True

            A1.DaughtersCuts = { "mu+" : "(TRCHI2DOF < %(MuTrackChi2DoF)s ) "\
                                 "& (PT > %(MupTdetached)s * MeV ) "\
                                 "& ( TRGHOSTPROB < %(MuGhostProb)s ) " \
                                 "& (MIPCHI2DV(PRIMARY)> %(MuMinIPchi2)s )" %self.config_dict }
            
            A1.CombinationCut = "(AM < %(A1maxMass)s * MeV ) "\
                                 "& (AMAXDOCA('')<%(A1Doca)s * mm)" %self.config_dict
            
            A1.MotherCut = "(VFASPF(VCHI2)< %(A1Vchi2)s ) "\
                           "& (MM < %(A1maxMass)s * MeV)" \
                           "& (BPVDIRA > %(A1Dira)s )" \
                           "& (BPVIPCHI2() < %(A1maxIPchi2)s )" \
                           "& (BPVVDCHI2 > %(A1FDChi2)s )" %self.config_dict

            
        _stdAllLooseMuons = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")

        if self.debug_cuts:
            print "DEBUG - A1 cuts for type", type
            print A1.DaughtersCuts
            print A1.MotherCut
            print A1.CombinationCut


        
        return Selection ("Sel"+name,
                          Algorithm = A1,
                          RequiredSelections = [ _stdAllLooseMuons ])




    def makeDefault(self,name,type=0) :
        """
        H-->A0(mumu)A0(mumu) selection.
        Arguments:
        name        : name of the Selection.
        type        : 0 (prompt), 1 (simple), 2 (detached)
        """
        
        from Configurables import OfflineVertexFitter
        
        SelA1 = self.makeA1("A1"+name,type)
        
        H25 = CombineParticles("Combine_H25"+name)
        H25.DecayDescriptor = "H_10 -> KS0 KS0"
        H25.DaughtersCuts = {}

        # simple: do not cut in pT, cut tighter in DOCA, VCHI2
        if type==1:
            H25.CombinationCut = "(AMAXDOCA('')< %(HmaxDOCATight)s * mm )" %self.config_dict
            H25.MotherCut = "(VFASPF(VCHI2)< %(HVchi2Tight)s )" %self.config_dict
        

        # prompt or detached
        else:
            H25.CombinationCut = "(AMAXDOCA('')< %(HmaxDOCA)s * mm )" %self.config_dict
            H25.MotherCut = "(PT > %(HpT)s * MeV ) "\
                            "& (VFASPF(VCHI2)< %(HVchi2)s ) " %self.config_dict


        # in all cases, add mass in DeltaM between both A1s
        H25.Preambulo = ["M1 = ACHILDFUN(1,MM)",
                         "M2 = ACHILDFUN(2,MM)",                     
                         "DeltaM = abs(M1 - M2)"]
        H25.CombinationCut+= "& ( DeltaM < %(A1sDeltaMass)s * MeV )" %self.config_dict

        if self.debug_cuts:
            print "DEBUG - H cuts for type", type
            print H25.MotherCut
            print H25.CombinationCut
        
        return Selection( "SelH4mu"+name,
                          Algorithm = H25,
                          RequiredSelections=[SelA1] )
    



