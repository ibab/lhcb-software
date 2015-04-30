'''
Module for electron ID studies, including one
Tag-And-Probe line for lifetime biased Jpsi->ee

To include it:

from StrippingSelections.StrippingElectronID import ElectronIDConf
StrippingElectronIDConf =  ElectronIDConf( name = 'Jpsi2eeForElectronID', config = ElectronIDConf.config_default )
stream.appendLines( [
    StrippingElectronIDConf.line
    ])
'''

__author__=['Jibo He']
__date__ = '03/10/2010'
__version__= '$Revision: 1.2 $'


__all__ = (
    'ElectronIDConf',
    'makeEE',
    'default_config'
    )


config_default =  {
        'JpsiLinePrescale'            :   0.5   ,
        'JpsiLineHltFilter'           : None    ,

        'Both_PT'                 :  500.   ,  # MeV
        'Both_P'                  : 3000.   ,  # MeV
        'Both_TRCHI2DOF'          :    5.   ,
        'Both_MIPCHI2'            :    9.   ,
        
        'Tag_PT'                  : 1500.   ,  # MeV
        'Tag_P'                   : 6000.   ,  # MeV
        'Tag_PIDe'                :    5.   ,
        'Tag_MIPCHI2'             :    9.   ,

        'Probe_PT'                :  500.   ,  # MeV
        'Probe_P'                 : 3000.   ,  # MeV
        'Probe_MIPCHI2'           :    9.   ,
        
        'eeCombMinMass'           : 2100.   ,  # MeV         
        'eeCombMaxMass'           : 4300.   ,  # MeV   
        'eeVCHI2PDOF'             :    9.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV

        'JpsiLineCut'             : "(PT>2.*GeV) & (BPVDLS>50) ",      
        
        'Bu2JpsieeKLine_Prescale'  :  1,
        'Bu2JpsieeKLine_HltFilter' : None,
        'Bu2JpsieeKLine_KaonCut'   : "(TRCHI2DOF<4) & (PT>1.0*GeV) & (PIDK >0) & (BPVIPCHI2()>9)",
        'Bu2JpsieeKLine_JpsiCut'   : "(BPVDLS>5)",
        'Bu2JpsieeKLine_BuComCut'  : "in_range(4.1*GeV,AM,6.1*GeV)",
        'Bu2JpsieeKLine_BuMomCut'  : "in_range(4.2*GeV,M, 6.0*GeV) & (VFASPF(VCHI2PDOF)<9)"
        }

default_config = {
    'NAME'        : 'ElectronIDCalib',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'ElectronIDConf',
    'CONFIG'      : config_default,
    'STREAMS'     :  [ 'PID' ]
    }



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class ElectronIDConf(LineBuilder):
    
    __configuration_keys__ = (
        'JpsiLinePrescale',
        'JpsiLineHltFilter',
        
        'Both_PT',
        'Both_P',
        'Both_TRCHI2DOF',
        'Both_MIPCHI2',
       
        'Tag_PT',
        'Tag_P',
        'Tag_PIDe',
        'Tag_MIPCHI2',

        'Probe_PT',
        'Probe_P',
        'Probe_MIPCHI2',
        
        'eeCombMinMass',
        'eeCombMaxMass',
        'eeVCHI2PDOF',
        'eeMinMass',
        'eeMaxMass',

        'JpsiLineCut',
        
        'Bu2JpsieeKLine_Prescale',
        'Bu2JpsieeKLine_HltFilter',
        'Bu2JpsieeKLine_KaonCut',
        'Bu2JpsieeKLine_JpsiCut',
        'Bu2JpsieeKLine_BuComCut',
        'Bu2JpsieeKLine_BuMomCut'
        )
    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        
        self.name = name
        self.config = config
        
        self.JpsiList= makeEE( 'eeFor'+self.name,                             
                                    #
                                    Both_PT = config['Both_PT'],
                                    Both_P = config['Both_P'],
                                    Both_TRCHI2DOF = config['Both_TRCHI2DOF'],
                                    Both_MIPCHI2 = config['Both_MIPCHI2'],
                                    #
                                    Tag_PT = config['Tag_PT'],
                                    Tag_P = config['Tag_P'],
                                    Tag_PIDe = config['Tag_PIDe'],
                                    Tag_MIPCHI2 = config['Tag_MIPCHI2'],
                                    #
                                    Probe_PT = config['Probe_PT'],
                                    Probe_P = config['Probe_P'],
                                    Probe_MIPCHI2 = config['Probe_MIPCHI2'],
                                    #
                                    eeCombMinMass = config['eeCombMinMass'],
                                    eeCombMaxMass = config['eeCombMaxMass'],
                                    eeVCHI2PDOF = config['eeVCHI2PDOF'],
                                    eeMinMass = config['eeMinMass'],
                                    eeMaxMass = config['eeMaxMass']
                                    )
        
        self.makeJpsi2eeLine()
        self.makeBu2JpsieeKLine() 
        

    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                          Algorithm = filter,
                          RequiredSelections = [ InputList ] )
    
    def createCombinationSel( self, OutputList,
                              DecayDescriptor,
                              DaughterLists,
                              DaughterCuts = {} ,
                              PreVertexCuts = "ALL",
                              PostVertexCuts = "ALL" ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = True)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
    
        
    
    def makeJpsi2eeLine( self ):
        
        Jpsi2eeForElectronID = self.createSubSel(  OutputList = self.JpsiList.name() + "ForJpsi2eeLine",
                                                   InputList  = self.JpsiList,
                                                   Cuts = self.config['JpsiLineCut'] )
        
        
        Jpsi2eeForElectronIDLine = StrippingLine( self.name + "JpsiLine",
                                                  HLT       = self.config['JpsiLineHltFilter'], 
                                                  algos = [ Jpsi2eeForElectronID ],
                                                  prescale = self.config['JpsiLinePrescale']
                                                  )
        
        self.registerLine( Jpsi2eeForElectronIDLine )
        
            
    def makeBu2JpsieeKLine( self ):
        
        from StandardParticles import StdTightKaons
        
        Bu2JpsieeK = self.createCombinationSel(
            OutputList = "Bu2JpsieeKFor" + self.name,
            DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
            DaughterLists = [ self.JpsiList, StdTightKaons ],
                DaughterCuts = { "J/psi(1S)" :  self.config['Bu2JpsieeKLine_JpsiCut'],
                                 "K+"        :  self.config['Bu2JpsieeKLine_KaonCut']},
                PreVertexCuts = self.config['Bu2JpsieeKLine_BuComCut'],
                PostVertexCuts = self.config['Bu2JpsieeKLine_BuMomCut'] )
        
        Bu2JpsieeKLine = StrippingLine( self.name + "Bu2JpsiKLine",
                                        HLT       = self.config['Bu2JpsieeKLine_HltFilter'], 
                                        algos = [ Bu2JpsieeK ] ,
                                        prescale = self.config["Bu2JpsieeKLine_Prescale"]
                                        )
        self.registerLine( Bu2JpsieeKLine )
        
            
def makeEE( name,
            #
            Both_PT,
            Both_P,
            Both_TRCHI2DOF,
            Both_MIPCHI2,
            #
            Tag_PT,
            Tag_P,
            Tag_PIDe,
            Tag_MIPCHI2,
            #
            Probe_PT,
            Probe_P,
            Probe_MIPCHI2,
            #
            eeCombMinMass,
            eeCombMaxMass,
            eeVCHI2PDOF,
            eeMinMass,
            eeMaxMass
            ):
    
    from StandardParticles import StdNoPIDsElectrons as NoPIDsElectronsForElectronID
    
    InAccCuts = "(0.5<PPINFO(LHCb.ProtoParticle.InAccEcal,-1))"
    
    BothCuts = "(PT> %(Both_PT)s *MeV)"\
               " & (TRCHI2DOF < %(Both_TRCHI2DOF)s)"\
               " & (MINTREE('e+'==ABSID,BPVIPCHI2())> %(Both_MIPCHI2)s )" % locals()
    
    EEComCut = "(in_range(%(eeCombMinMass)s *MeV, AM, %(eeCombMaxMass)s *MeV))" % locals()
    
    EEMomCut = "(VFASPF(VCHI2)< %(eeVCHI2PDOF)s)"\
               " & (in_range(%(eeMinMass)s *MeV, MM, %(eeMaxMass)s *MeV))" % locals()
    
    Tag1Cuts = "(CHILDCUT((PT>%(Tag_PT)s*MeV),1)) & (CHILDCUT((PIDe>%(Tag_PIDe)s),1)) & (CHILDCUT((P>%(Tag_P)s*MeV),1))"\
               " & (CHILDCUT((BPVIPCHI2()> %(Tag_MIPCHI2)s),1))" % locals()
    Tag2Cuts = "(CHILDCUT((PT>%(Tag_PT)s*MeV),2)) & (CHILDCUT((PIDe>%(Tag_PIDe)s),2)) & (CHILDCUT((P>%(Tag_P)s*MeV),2))"\
               " & (CHILDCUT((BPVIPCHI2()> %(Tag_MIPCHI2)s),2))" % locals()
        
    Probe1Cuts = "(CHILDCUT((PT>%(Probe_PT)s*MeV),1)) & (CHILDCUT((P>%(Probe_P)s*MeV),1))"\
                 " & (CHILDCUT((BPVIPCHI2()> %(Probe_MIPCHI2)s),1))" % locals()
    Probe2Cuts = "(CHILDCUT((PT>%(Probe_PT)s*MeV),2)) & (CHILDCUT((P>%(Probe_P)s*MeV),2))"\
                 " & (CHILDCUT((BPVIPCHI2()> %(Probe_MIPCHI2)s),2))" % locals()
    
    Tag1Probe2Cuts = Tag1Cuts + " & " + Probe2Cuts
    Tag2Probe1Cuts = Tag2Cuts + " & " + Probe1Cuts 
    
    _EE = CombineParticles( DecayDescriptor = "J/psi(1S) -> e+ e-",
                            DaughtersCuts = {"e+" : InAccCuts + "&" + BothCuts },
                            CombinationCut = EEComCut,
                            MotherCut = EEMomCut + " & ( ( " + Tag1Probe2Cuts + " ) | (" + Tag2Probe1Cuts + " ) ) "
                            )
    
    return Selection( name,
                      Algorithm = _EE,
                      RequiredSelections = [ NoPIDsElectronsForElectronID ]
                      )    

