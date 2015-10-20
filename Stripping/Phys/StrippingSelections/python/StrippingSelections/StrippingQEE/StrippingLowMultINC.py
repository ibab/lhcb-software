"""
Stripping options for CEP in EM
Fully inclusive stream or of all Hlt Lines
A separate file is for exclusive selections
"""

########################################################################
__author__  = ['Paolo Gandini']
__date__    = '07/10/2015'
__version__ = '$Revision:1.0$'

__all__ = ('LowMultINCLines',
           'default_config')

# from Gaudi.Configuration import *
# from GaudiConfUtils.ConfigurableGenerators import CombineParticles
# from StandardParticles                     import StdNoPIDsPions, StdLooseProtons, StdLooseKaons

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder
########################################################################


#######################
# L0 Lines
#["DiEM,lowMult"]     
#["Photon,lowMult"]  
#["DiHadron,lowMult"]
#["DiMuon,lowMult"]  
#["Electron,lowMult"]
#["Muon,lowMult"]    
#fltrs = LoKi_Filters (
#        ODIN_Code  = ' in_range ( 5000 , ODIN_RUNNUM , 9000 ) '          ,
#              L0DU _Code  = ' L0_CHANNEL  ( ... )  '                           ,
#              HLT_Code   = ' HLT_PASS_RE ( 'Hlt1MBMicro.*Decision')  '         ,
#              STRIP_Code = ' HLT_PASS    ( 'StrippingBd2KstarGammaDecision') ' ,
#              VOID_Code  = ' CONTAINS    ('Rec/Vertex/Primary') == 1 '         ,
#              MC_Code    = ' count ( 'D0' == MCABSID ) > 0.5 '                 ,
#              GEN_Code   = ' count ( 'D0' ==  GABSID ) > 0.5 '                 ,
#             )                 
#"HLT_PASS_RE('Hlt1.*Decision')"
######################


default_config = {
    'NAME'        : 'LowMultINC',
    'BUILDERTYPE' : 'LowMultINCLines',
    'STREAMS'     : ['EW'],
    'WGs'         : ['QEE'],
    'CONFIG'      : {
        #
        'LowMultRequiredRawEvents' : ["Velo","Calo","HC","Trigger","Muon","Rich","Tracker"],  #Please add this!
        #
        'PrescaleL0DiHadron'  : 0,
        'DecisionL0DiHadron'  : None,#"L0_CHANNEL('DiHadron,lowMult')",
        'PrescaleL0DiEM'      : 0,
        'DecisionL0DiEM'      : None,#"L0_CHANNEL('DiEM,lowMult')",
        'PrescaleL0Photon'    : 0,
        'DecisionL0Photon'    : None,#"L0_CHANNEL('Photon,lowMult')",
        'PrescaleL0Muon'      : 0,
        'DecisionL0Muon'      : None,#"L0_CHANNEL('Muon,lowMult')",
        'PrescaleL0DiMuon'    : 0,
        'DecisionL0DiMuon'    : None,#"L0_CHANNEL('DiMuon,lowMult')",
        'PrescaleL0Electron'  : 0,
        'DecisionL0Electron'  : None,#"L0_CHANNEL('Electron,lowMult')",
        #
        #
        #
        #
        'PrescaleLowMultL2pPi'     : 1,
        'DecisionLowMultL2pPi'     : "HLT_PASS_RE('Hlt2LowMultL2pPiDecision') | HLT_PASS_RE('Hlt2LowMultL2pPiWSDecision')",
        'PrescaleLowMultD2KPi'     : 1,
        'DecisionLowMultD2KPi'     : "HLT_PASS_RE('Hlt2LowMultD2KPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiWSDecision')",
        'PrescaleLowMultD2KPiPi'   : 1,
        'DecisionLowMultD2KPiPi'   : "HLT_PASS_RE('Hlt2LowMultD2KPiPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiPiWSDecision')",
        'PrescaleLowMultD2KKPi'    : 1,
        'DecisionLowMultD2KKPi'    : "HLT_PASS_RE('Hlt2LowMultD2KKPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KKPiWSDecision')",
        'PrescaleLowMultD2K3Pi'    : 1,
        'DecisionLowMultD2K3Pi'    :"HLT_PASS_RE('Hlt2LowMultD2K3PiDecision') | HLT_PASS_RE('Hlt2LowMultD2K3PiWSDecision')",
        'PrescaleLowMultChiC2HH'   : 1,
        'DecisionLowMultChiC2HH'   :"HLT_PASS_RE('Hlt2LowMultChiC2HHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHWSDecision')",
        'PrescaleLowMultChiC2HHHH' : 1,
        'DecisionLowMultChiC2HHHH' :"HLT_PASS_RE('Hlt2LowMultChiC2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHHHWSDecision')",
        'PrescaleLowMultChiC2PP'   : 1,
        'DecisionLowMultChiC2PP'   :"HLT_PASS_RE('Hlt2LowMultChiC2PPDecision') | HLT_PASS_RE('Hlt2LowMultChiC2PPWSDecision')",
        #
        'PrescaleLowMultLMR2HH'           : 1,
        'DecisionLowMultLMR2HH'           :"HLT_PASS_RE('Hlt2LowMultLMR2HHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWSDecision')",
        'PrescaleLowMultLMR2HHHH'         : 1,
        'DecisionLowMultLMR2HHHH'         :"HLT_PASS_RE('Hlt2LowMultLMR2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWSDecision')",
        'PrescaleLowMultLMR2HH_mediumPS'  : 1,
        'DecisionLowMultLMR2HH_mediumPS'  :"HLT_PASS_RE('Hlt2LowMultLMR2HH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_mediumPSDecision')",
        'PrescaleLowMultLMR2HHHH_mediumPS': 1,
        'DecisionLowMultLMR2HHHH_mediumPS': "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_mediumPSDecision')",
        'PrescaleLowMultLMR2HH_heavyPS'   : 1,
        'DecisionLowMultLMR2HH_heavyPS'   : "HLT_PASS_RE('Hlt2LowMultLMR2HH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_heavyPSDecision')",
        'PrescaleLowMultLMR2HHHH_heavyPS' : 1,
        'DecisionLowMultLMR2HHHH_heavyPS' : "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_heavyPSDecision')",
        # Unbiased hadron line
        'PrescaleLowMultHadron_noTrFilt' : 1,
        'DecisionLowMultHadron_noTrFilt' : "HLT_PASS_RE('Hlt2LowMultHadron_noTrFiltDecision')",
        #
        #
        #
        'PrescaleLowMultDiMuon'             : 1,
        'DecisionLowMultDiMuon'             : "HLT_PASS('Hlt2LowMultDiMuonDecision')",
        'PrescaleLowMultDiMuon_PS'          : 1,
        'DecisionLowMultDiMuon_PS'          : "HLT_PASS('Hlt2LowMultDiMuon_PSDecision')",
        'PrescaleLowMultMuon'               : 1,
        'DecisionLowMultMuon'               : "HLT_PASS('Hlt2LowMultMuonDecision')",
        #
        'PrescaleLowMultDiElectron'         : 1,
        'DecisionLowMultDiElectron'         : "HLT_PASS_RE('Hlt2LowMultDiElectronDecision')",
        'PrescaleLowMultDiElectron_noTrFilt': 1,
        'DecisionLowMultDiElectron_noTrFilt': "HLT_PASS_RE('Hlt2LowMultDiElectron_noTrFiltDecision')",
        #
        'PrescaleLowMultDiPhoton'         : 1,
        'DecisionLowMultDiPhoton'         : "HLT_PASS_RE('Hlt2LowMultDiPhotonDecision')",
        'PrescaleLowMultDiPhoton_HighMass': 1,
        'DecisionLowMultDiPhoton_HighMass': "HLT_PASS_RE('Hlt2LowMultDiPhoton_HighMassDecision')",
        'PrescaleLowMultPi0'              : 1,
        'DecisionLowMultPi0'              : "HLT_PASS_RE('Hlt2LowMultPi0Decision')",
        #
        'PrescaleLowMultNonBeamBeamNoBias'         : 1,
        'DecisionLowMultNonBeamBeamNoBias'         : "HLT_PASS_RE('Hlt2NonBeamBeamNoBiasDecision')",
        'PrescaleLowMultTechnical'                 : 1,
        'DecisionLowMultTechnical'                 : "HLT_PASS_RE('Hlt2LowMultTechnical_MinBiasDecision')",
        #
        'PrescaleLowMultTMP1'                 : 0,
        'DecisionLowMultTMP1'                 : None,
        'PrescaleLowMultTMP2'                 : 0,
        'DecisionLowMultTMP2'                 : None,
    }
}




class LowMultINCLines( LineBuilder ) :
    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__( self,name,config ) :            

        LineBuilder.__init__(self, name, config)


#####################################################################################################################
        self.lineL0DiHadron = StrippingLine( "L0DiHadron"+"Line",
                                             prescale  = config['PrescaleL0DiHadron'],
                                             checkPV   = False,
                                             EnableFlavourTagging = False,
                                             L0DU      = config['DecisionL0DiHadron'],   
                                             RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineL0DiHadron)
        

        self.lineL0DiEM = StrippingLine( "L0DiEM"+"Line",
                                         prescale  = config['PrescaleL0DiEM'],
                                         checkPV   = False,
                                         EnableFlavourTagging = False,
                                         L0DU      = config['DecisionL0DiEM'],
                                         RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineL0DiEM)

                
        self.lineL0Photon = StrippingLine( "L0Photon"+"Line",
                                           prescale  = config['PrescaleL0Photon'],
                                           checkPV   = False,
                                           EnableFlavourTagging = False,
                                           L0DU      = config['DecisionL0Photon'],
                                           RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineL0Photon)


        self.lineL0DiMuon = StrippingLine( "L0DiMuon"+"Line",
                                           prescale  = config['PrescaleL0DiMuon'],
                                           checkPV   = False,
                                           EnableFlavourTagging = False,
                                           L0DU      = config['DecisionL0DiMuon'],
                                           RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineL0DiMuon)


        self.lineL0Muon = StrippingLine( "L0Muon"+"Line",
                                           prescale  = config['PrescaleL0Muon'],
                                           checkPV   = False,
                                           EnableFlavourTagging = False,
                                           L0DU      = config['DecisionL0Muon'],
                                           RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineL0Muon)

                
        self.lineL0Electron = StrippingLine( "L0Electron"+"Line",
                                         prescale  = config['PrescaleL0Electron'],
                                         checkPV   = False,
                                         EnableFlavourTagging = False,
                                         L0DU      = config['DecisionL0Electron'] ,
                                         RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineL0Electron)








                
#################################################
        self.lineLowMultL2pPi = StrippingLine( "LowMultL2pPi"+"Line",
                                               prescale  = config['PrescaleLowMultL2pPi'],
                                               checkPV   = False,
                                               EnableFlavourTagging = False,
                                               L0DU      = None,
                                               HLT2 = config['DecisionLowMultL2pPi'],
                                               RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultL2pPi)

        self.lineLowMultD2KPi = StrippingLine( "LowMultD2KPi"+"Line",
                                               prescale  = config['PrescaleLowMultD2KPi'],
                                               checkPV   = False,
                                               EnableFlavourTagging = False,
                                               L0DU      = None,
                                               HLT2 = config['DecisionLowMultD2KPi'],
                                               RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultD2KPi)

        self.lineLowMultD2KPiPi = StrippingLine( "LowMultD2KPiPi"+"Line",
                                               prescale  = config['PrescaleLowMultD2KPiPi'],
                                               checkPV   = False,
                                               EnableFlavourTagging = False,
                                               L0DU      = None,
                                               HLT2 = config['DecisionLowMultD2KPiPi'],
                                               RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultD2KPiPi)
                
        self.lineLowMultD2KKPi = StrippingLine( "LowMultD2KKPi"+"Line",
                                                 prescale  = config['PrescaleLowMultD2KKPi'],
                                                 checkPV   = False,
                                                 EnableFlavourTagging = False,
                                                 L0DU      = None,
                                                 HLT2 = config['DecisionLowMultD2KKPi'],
                                                 RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultD2KKPi)

        self.lineLowMultD2K3Pi = StrippingLine( "LowMultD2K3Pi"+"Line",
                                                prescale  = config['PrescaleLowMultD2K3Pi'],
                                                checkPV   = False,
                                                EnableFlavourTagging = False,
                                                L0DU      = None,
                                                HLT2 = config['DecisionLowMultD2K3Pi'],
                                                RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultD2K3Pi)

        self.lineLowMultChiC2HH = StrippingLine( "LowMultChiC2HH"+"Line",
                                                prescale  = config['PrescaleLowMultChiC2HH'],
                                                checkPV   = False,
                                                EnableFlavourTagging = False,
                                                L0DU      = None,
                                                HLT2 = config['DecisionLowMultChiC2HH'],
                                                RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultChiC2HH)

        self.lineLowMultChiC2HHHH = StrippingLine( "LowMultChiC2HHHH"+"Line",
                                                 prescale  = config['PrescaleLowMultChiC2HHHH'],
                                                 checkPV   = False,
                                                 EnableFlavourTagging = False,
                                                 L0DU      = None,
                                                 HLT2 = config['DecisionLowMultChiC2HHHH'],
                                                 RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultChiC2HHHH)

        self.lineLowMultChiC2PP = StrippingLine( "LowMultChiC2PP"+"Line",
                                                   prescale  = config['PrescaleLowMultChiC2PP'],
                                                   checkPV   = False,
                                                   EnableFlavourTagging = False,
                                                   L0DU      = None,
                                                   HLT2 = config['DecisionLowMultChiC2PP'],
                                                   RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultChiC2PP)

#################################################

        self.lineLowMultLMR2HH = StrippingLine( "LowMultLMR2HH"+"Line",
                                                 prescale  = config['PrescaleLowMultLMR2HH'],
                                                 checkPV   = False,
                                                 EnableFlavourTagging = False,
                                                 L0DU      = None,
                                                 HLT2 = config['DecisionLowMultLMR2HH'],
                                                 RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultLMR2HH)
        
        self.lineLowMultLMR2HHHH = StrippingLine( "LowMultLMR2HHHH"+"Line",
                                                  prescale  = config['PrescaleLowMultLMR2HHHH'],
                                                  checkPV   = False,
                                                  EnableFlavourTagging = False,
                                                  L0DU      = None,
                                                  HLT2 = config['DecisionLowMultLMR2HHHH'],
                                                  RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultLMR2HHHH)

#

        self.lineLowMultLMR2HH_mediumPS = StrippingLine( "LowMultLMR2HH_mediumPS"+"Line",
                                                prescale  = config['PrescaleLowMultLMR2HH_mediumPS'],
                                                checkPV   = False,
                                                EnableFlavourTagging = False,
                                                L0DU      = None,
                                                HLT2 = config['DecisionLowMultLMR2HH_mediumPS'],
                                                RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultLMR2HH_mediumPS)
        
        self.lineLowMultLMR2HH_mediumPSHH = StrippingLine( "LowMultLMR2HHHH_mediumPS"+"Line",
                                                  prescale  = config['PrescaleLowMultLMR2HHHH_mediumPS'],
                                                  checkPV   = False,
                                                  EnableFlavourTagging = False,
                                                  L0DU      = None,
                                                  HLT2 = config['DecisionLowMultLMR2HHHH_mediumPS'],
                                                  RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultLMR2HH_mediumPSHH)

#
        
        self.lineLowMultLMR2HH_heavyPS = StrippingLine( "LowMultLMR2HH_heavyPS"+"Line",
                                                         prescale  = config['PrescaleLowMultLMR2HH_heavyPS'],
                                                         checkPV   = False,
                                                         EnableFlavourTagging = False,
                                                         L0DU      = None,
                                                         HLT2 = config['DecisionLowMultLMR2HH_heavyPS'],
                                                         RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultLMR2HH_heavyPS)
        
        self.lineLowMultLMR2HH_heavyPSHH = StrippingLine( "LowMultLMR2HHHH_heavyPS"+"Line",
                                                           prescale  = config['PrescaleLowMultLMR2HHHH_heavyPS'],
                                                           checkPV   = False,
                                                           EnableFlavourTagging = False,
                                                           L0DU      = None,
                                                           HLT2 = config['DecisionLowMultLMR2HHHH_heavyPS'],
                                                           RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultLMR2HH_heavyPSHH)

#################################################
                        
        self.lineLowMultHadron_noTrFilt = StrippingLine( "LowMultHadron_noTrFilt"+"Line",
                                                 prescale  = config['PrescaleLowMultHadron_noTrFilt'],
                                                 checkPV   = False,
                                                 EnableFlavourTagging = False,
                                                 L0DU      = None,
                                                 HLT2 =config['DecisionLowMultHadron_noTrFilt'],
                                                 RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultHadron_noTrFilt)









#################################################
        self.lineLowMultDiMuon = StrippingLine( "LowMultDiMuon"+"Line",
                                                prescale  = config['PrescaleLowMultDiMuon'],
                                                checkPV   = False,
                                                EnableFlavourTagging = False,
                                                L0DU      = None,
                                                HLT2 = config['DecisionLowMultDiMuon'],
                                                RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultDiMuon)

        self.lineLowMultDiMuon_PS = StrippingLine( "LowMultDiMuon_PS"+"Line",
                                                prescale  = config['PrescaleLowMultDiMuon_PS'],
                                                checkPV   = False,
                                                EnableFlavourTagging = False,
                                                L0DU      = None,
                                                HLT2 = config['DecisionLowMultDiMuon_PS'],
                                                RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultDiMuon_PS)

        self.lineLowMultMuon = StrippingLine( "LowMultMuon"+"Line",
                                                   prescale  = config['PrescaleLowMultMuon'],
                                                   checkPV   = False,
                                                   EnableFlavourTagging = False,
                                                   L0DU      = None,
                                                   HLT2 = config['DecisionLowMultMuon'],
                                                   RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultMuon)
                                
#################################################

        self.lineLowMultDiElectron = StrippingLine( "LowMultDiElectron"+"Line",
                                              prescale  = config['PrescaleLowMultDiElectron'],
                                              checkPV   = False,
                                              EnableFlavourTagging = False,
                                              L0DU      = None,
                                              HLT2 = config['DecisionLowMultDiElectron'],
                                              RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultDiElectron)
                        
        self.lineLowMultDiElectron_noTrFilt = StrippingLine( "LowMultDiElectron_noTrFilt"+"Line",
                                                    prescale  = config['PrescaleLowMultDiElectron_noTrFilt'],
                                                    checkPV   = False,
                                                    EnableFlavourTagging = False,
                                                    L0DU      = None,
                                                    HLT2 = config['DecisionLowMultDiElectron_noTrFilt'],
                                                    RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultDiElectron_noTrFilt)


#################################################


        self.lineLowMultDiPhoton = StrippingLine( "LowMultDiPhoton"+"Line",
                                                             prescale  = config['PrescaleLowMultDiPhoton'],
                                                             checkPV   = False,
                                                             EnableFlavourTagging = False,
                                                             L0DU      = None,
                                                             HLT2 = config['DecisionLowMultDiPhoton'],
                                                             RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultDiPhoton)

        self.lineLowMultDiPhoton_HighMass = StrippingLine( "LowMultDiPhoton_HighMass"+"Line",
                                                  prescale  = config['PrescaleLowMultDiPhoton_HighMass'],
                                                  checkPV   = False,
                                                  EnableFlavourTagging = False,
                                                  L0DU      = None,
                                                  HLT2 = config['DecisionLowMultDiPhoton_HighMass'],
                                                  RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultDiPhoton_HighMass)

        self.lineLowMultPi0 = StrippingLine( "LowMultPi0"+"Line",
                                                           prescale  = config['PrescaleLowMultPi0'],
                                                           checkPV   = False,
                                                           EnableFlavourTagging = False,
                                                           L0DU      = None,
                                                           HLT2 = config['DecisionLowMultPi0'],
                                                           RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultPi0)
        

#####
#####
        self.lineLowMultNonBeamBeamNoBias = StrippingLine( "LowMultNonBeamBeamNoBias"+"Line",
                                                   prescale  = config['PrescaleLowMultNonBeamBeamNoBias'],
                                                   checkPV   = False,
                                                   EnableFlavourTagging = False,
                                                   L0DU      = None,
                                                   HLT2 = config['DecisionLowMultNonBeamBeamNoBias'],
                                                   RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultNonBeamBeamNoBias)
                        

        self.lineLowMultTechnical = StrippingLine( "LowMultTechnical"+"Line",
                                             prescale  = config['PrescaleLowMultTechnical'],
                                             checkPV   = False,
                                             EnableFlavourTagging = False,
                                             L0DU      = None,
                                             HLT2 = config['DecisionLowMultTechnical'],
                                             RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultTechnical)
        
#####
#####    
        self.lineLowMultTMP1 = StrippingLine( "LowMultTMP1"+"Line",
                                              prescale  = config['PrescaleLowMultTMP1'],
                                              checkPV   = False,
                                              EnableFlavourTagging = False,
                                              L0DU      = None,
                                              HLT2 = config['DecisionLowMultTMP1'],
                                              RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultTMP1)

        
        self.lineLowMultTMP2 = StrippingLine( "LowMultTMP2"+"Line",
                                              prescale  = config['PrescaleLowMultTMP2'],
                                              checkPV   = False,
                                              EnableFlavourTagging = False,
                                              L0DU      = None,
                                              HLT2 = config['DecisionLowMultTMP2'],
                                              RequiredRawEvents = config["LowMultRequiredRawEvents"])
        self.registerLine(self.lineLowMultTMP2)
                
