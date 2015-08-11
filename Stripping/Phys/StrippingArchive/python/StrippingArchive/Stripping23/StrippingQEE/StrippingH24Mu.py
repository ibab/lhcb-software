'''
Module for construction of h-->MuMuMuMu stripping selection

Exported symbols (use python help!):
   - H24MuLineConf

Based on Bsmumu stripping lines
'''

__author__ = ['Xabier Cid Vidal']
__date__ = '11/22/2013'

__all__ = ('H24MuLineConf',
           'default_name',
           'default_config'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from Configurables import OfflineVertexFitter

default_name = 'H24Mu'

#### This is the dictionary of all tunable cuts ########
default_config={
    'NAME': default_name,
    'BUILDERTYPE'  : 'H24MuLineConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'Leptonic' ],
    'CONFIG':{'DefaultPostscale'       : 1,
              'PromptLinePrescale'     : 1,
              'SimpleLinePrescale'     : 1,
              'DetachedLinePrescale'   : 1,
              'LooseLinePrescale'      : 0.01,
              
              'MuTrackChi2DoF'         : 3,
              'MupTprompt'             : 375,  #MeV
              'MupTdetached'           : 250,  #MeV
              'MuGhostProb'            : 0.4,
              'MuMaxIPchi2'            : 3,
              'MuMinIPchi2'            : 1,
              'MuPIDdll'               : -3, # muon combDLL
              'MuNShared'              : 3, # muon NShared
              
              'A1maxMass'              : 2000, #MeV
              'A1Doca'                 : 0.2,   #mm
              'A1DocaTight'            : 0.1,   #mm
              'A1Vchi2'                : 7.5,
              'A1Vchi2Tight'           : 1,
              'A1Dira'                 : 0,
              'A1maxIPchi2'            : 25,
              'A1FDChi2'               : 4,
              
              'HmaxDOCA'               : 0.75, #mm
              'HmaxDOCATight'          : 0.25, #mm
              'HVchi2'                 : 10,
              'HVchi2Tight'            : 2,
              'HpT'                    : 1200, #MeV
              
              'MuTrackChi2DoF_loose'   : 10,
              'MupT_loose'             : 0,
              'MuMaxIPchi2_loose'      : 1000000,
              'A1maxMass_loose'        : 5000, #MeV
              'A1Doca_loose'           : 10, #mm
              'A1Vchi2_loose'          : 20,
              'HmaxDOCA_loose'         : 1000000, #mm
              'HpT_loose'              : 300, #MeV
              'HVchi2_loose'           : 50
              
              }
    }   


class H24MuLineConf(LineBuilder) :
    """
    Builder of:
     - H-> mumumumu stripping lines: prompt, detached and control,

    Usage:
    >>> config = { .... }
    >>> Conf = H24MuLinesConf('Test',config)
    >>> myLines = Conf.lines
    >>> for line in myLines:
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.


    Exports as instance data members:
    selPrompt     : nominal prompt H24mu stripping line
    selSimple     : nominal simple H24mu stripping line (no pT, IP cuts)
    selDetached   : nominal detached H24mu stripping line
    selLoose      : loose H24mu stripping line to understand systematics (prescaled)
    promptLine    : Stripping line made from selPrompt
    simpleLine    : Stripping line made from selSimple
    detachedLine  : Stripping line made from selDetached
    looseLine     : Stripping line made from selLoose
    lines         : list of lines:  [ promptLine, simpleLine, detachedLine, looseLine ]
    
    """
 
    __configuration_keys__ = (
        'DefaultPostscale',
        'PromptLinePrescale',
        'SimpleLinePrescale',
        'DetachedLinePrescale',
        'LooseLinePrescale',
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
        'HmaxDOCA',
        'HpT',
        'HVchi2',
        'HmaxDOCATight',
        'HVchi2Tight',
        'MuTrackChi2DoF_loose',
        'MupT_loose',
        'MuMaxIPchi2_loose',
        'A1maxMass_loose',
        'A1Doca_loose',
        'A1Vchi2_loose',
        'HmaxDOCA_loose',
        'HpT_loose',
        'HVchi2_loose'
    )


    def __init__(self, 
                 name = default_name,
                 config = None,
                 debug_cuts = 0):
                 

        LineBuilder.__init__(self, name, config)

        prompt_name=name+'Prompt'
        simple_name=name+'Simple'
        detached_name=name+'Detached'
        loose_name=name+'Loose'

        self.config_dict = config
        self.debug_cuts = debug_cuts
        self.selPrompt = self.makeDefault(prompt_name,type = 0)
        self.selSimple = self.makeDefault(simple_name,type = 1)
        self.selDetached = self.makeDefault(detached_name,type = 2)
        self.selLoose = self.makeDefault(loose_name,type = 3)

        ExtraInfoTools = [{'Type' : 'ConeVariables',
                           'ConeNumber' : 1,
                           'ConeAngle' : 1.0,
                           'Variables' : ['angle', 'mult','p','pt',
                                          'ptasy','pasy']},
                          {'Type' : 'ConeVariables', 
                           'ConeNumber' : 2, 
                           'ConeAngle' : 1.5, 
                           'Variables' : ['angle', 'mult','p','pt',
                                          'ptasy','pasy']},
                          {'Type' : 'ConeVariables',
                           'ConeNumber' : 3,
                           'ConeAngle' : 2.0,
                           'Variables' : ['angle', 'mult','p','pt',
                                          'ptasy','pasy']},
                          
                          {'Type' : 'VertexIsolation'}]

        ExtraInfoDaughters = {"prompt"  : [getattr(self,"A1"+prompt_name)],
                              "simple"  : [getattr(self,"A1"+simple_name)],
                              "detached": [getattr(self,"A1"+detached_name)],
                              "loose"   : [getattr(self,"A1"+loose_name)]}
        
        self.promptLine = StrippingLine(prompt_name+"Line",
                                        prescale = config['PromptLinePrescale'],
                                        postscale = config['DefaultPostscale'],
#                                        algos = [ self.selPrompt ],
                                        selection = self.selPrompt,
                                        ExtraInfoTools = ExtraInfoTools,
                                        ExtraInfoSelections = ExtraInfoDaughters["prompt"],
                                        MDSTFlag = True,
                                        RequiredRawEvents = ["Muon"]
                                        )

        self.simpleLine = StrippingLine(simple_name+"Line",
                                        prescale = config['SimpleLinePrescale'],
                                        postscale = config['DefaultPostscale'],
#                                        algos = [ self.selSimple ],
                                        selection = self.selSimple,
                                        ExtraInfoTools = ExtraInfoTools,
                                        ExtraInfoSelections = ExtraInfoDaughters["simple"],
                                        MDSTFlag = True,
                                        RequiredRawEvents = ["Muon"]
                                        )


        self.detachedLine = StrippingLine(detached_name+"Line",
                                          prescale = config['DetachedLinePrescale'],
                                          postscale = config['DefaultPostscale'],
#                                          algos = [ self.selDetached ],
                                          selection = self.selDetached,
                                          ExtraInfoTools = ExtraInfoTools,
                                          ExtraInfoSelections = ExtraInfoDaughters["detached"],
                                          MDSTFlag = True,
                                          RequiredRawEvents = ["Muon"]
                                          )
        
        ## no need for mdst or raw data in the loose line...
        self.looseLine = StrippingLine(loose_name+"Line",
                                       prescale = config['LooseLinePrescale'],
                                       postscale = config['DefaultPostscale'],
#                                       algos = [ self.selLoose ],
                                       selection = self.selLoose,
                                       ExtraInfoTools = ExtraInfoTools,
                                       ExtraInfoSelections = ExtraInfoDaughters["loose"],
                                       )

        self.registerLine(self.promptLine)
        self.registerLine(self.simpleLine)
        self.registerLine(self.detachedLine)
        #self.registerLine(self.looseLine)



    def makeA1(self,name,type) :
        """
        Prompt A1 selection
        Arguments:
        name        : name of the Selection.
        type        : 0 (prompt), 1 (simple), 2 (detached), 3 (loose)
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

        #loose
        else:
            
            A1.DaughtersCuts = { "mu+" : "(TRCHI2DOF < %(MuTrackChi2DoF_loose)s ) "\
                                 "& (PT > %(MupT_loose)s * MeV ) "\
                                 "& (MIPCHI2DV(PRIMARY)< %(MuMaxIPchi2_loose)s )" %self.config_dict }

            A1.CombinationCut = "(AM < %(A1maxMass_loose)s * MeV ) "\
                                 "& (AMAXDOCA('')<%(A1Doca_loose)s * mm)" %self.config_dict
            
            A1.MotherCut = "(VFASPF(VCHI2)< %(A1Vchi2_loose)s ) "\
                           "& (MM < %(A1maxMass_loose)s * MeV)" %self.config_dict

            
            
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
        type        : 0 (prompt), 1 (simple), 2 (detached), 3 (loose)
        """
        
        
        SelA1 = self.makeA1("A1"+name,type)
        setattr(self,"A1"+name,SelA1)
        
        H25 = CombineParticles("Combine_H25"+name)
        H25.DecayDescriptor = "H_10 -> KS0 KS0"
        H25.DaughtersCuts = {}

        # simple: do not cut in pT, cut tighter in DOCA, VCHI2
        if type==1:
            H25.CombinationCut = "(AMAXDOCA('')< %(HmaxDOCATight)s * mm )" %self.config_dict
            H25.MotherCut = "(VFASPF(VCHI2)< %(HVchi2Tight)s )" %self.config_dict
        
        # loose: loosen all cuts
        elif type==3:
            H25.CombinationCut = "(AMAXDOCA('')< %(HmaxDOCA_loose)s * mm )" %self.config_dict
            H25.MotherCut = "(PT > %(HpT_loose)s * MeV ) "\
                            "& (VFASPF(VCHI2)< %(HVchi2_loose)s ) " %self.config_dict

        # prompt or detached
        else:
            H25.CombinationCut = "(AMAXDOCA('')< %(HmaxDOCA)s * mm )" %self.config_dict
            H25.MotherCut = "(PT > %(HpT)s * MeV ) "\
                            "& (VFASPF(VCHI2)< %(HVchi2)s ) " %self.config_dict

        if self.debug_cuts:
            print "DEBUG - H cuts for type", type
            print H25.MotherCut
            print H25.CombinationCut
        
        return Selection( "SelH4mu"+name,
                          Algorithm = H25,
                          RequiredSelections=[SelA1] )
    



