# =============================================================================
## @file
#  Configuration of Hadron Lines
#  @author V. Gligorov for J. Albrecht (vladimir.gligorov@cern.ch) 
#  @date 2009-09-15
# =============================================================================
"""
 script to configure Hadron via T trigger lines, used for commisioning
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.20 $"
# =============================================================================

from Gaudi.Configuration import * 

from HltLine.HltLinesConfigurableUser import *

class Hlt1HadronViaTLinesConf(HltLinesConfigurableUser) :
    #--------------------------------
    #
    # (V. Gligorov 15/09/09)

    # steering variables
    #------------------------
    #   Single Hadron Line
    #     SingleHadronViaT_PtCut
    #   DiHadron Line
    #     HadViaTMain_IPCut
    #     HadViaTMain_PtCut
    #     HadViaTCompanion_IPCut
    #     HadViaTCompanion_PtCut
    #
    __slots__ = { 'L0Channel'                   : "Hadron" 
                , 'HadViaTSingle_IPCut'         : 0.1
                , 'HadViaTDi_IPCut'             : 0.1
                , 'HadViaT_ETCut'               : 2500.
                , 'HadViaTMain_PTCut'           : 2500.
                , 'VeloTMatchCut'               : 80
                , 'HadViaTCompanion_DOCACut'    : 0.2
                , 'HadViaTCompanion_DZCut'      : 0.
                , 'HadViaTCompanion_PTCut'      : 1000.
                , 'HadViaTCompanion_PointingCut': 0.4
                , 'SingleHadronViaT_PTCut'      : 5000.
                }
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Tool   as Tool
        from HltLine.HltLine import hlt1Lines  
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit
        from HltTracking.HltReco import Velo
        from HltTracking.HltPVs  import PV3D

        from Configurables import HltTrackUpgradeTool, PatForwardTool
        from Hlt1Lines.HltConfigurePR import ConfiguredPR

        ## alias to get the slot associated to a name
        _cut = lambda x: str(self.getProp(x))
        
        # confirmed track
        #------------------------
        def confirmation(type=""):
            prefix = 'HadronViaTConfirmation'+type
            if (self.getProp('L0Channel') == 'CALO') : 
	            candidates = 'AllHadron'
            else :
	            candidates = self.getProp('L0Channel')

            L0candidates = "Hlt1L0"+candidates+"Decision"
            
            IPCut = str(min( self.getProp(type+"HadViaTDi_IPCut"),self.getProp("HadViaTSingle_IPCut")))
            PTCut = _cut(type+"HadViaTMain_PTCut")
            VTMatchCut = _cut(type+"VeloTMatchCut")
            # get the L0 candidates (all or L0)
            from Hlt1Lines.HltL0Candidates import convertL0Candidates
            #conf = [convertL0Candidates(candidates)]
            from HltLine.HltDecodeRaw import DecodeIT,DecodeTT
            from Configurables import PatConfirmTool, PatSeedingTool,L0ConfirmWithT,HltTrackUpgradeTool

            l0 = bindMembers(prefix, [ convertL0Candidates(candidates)
                                     , Member('TF','L0HadronViaT'
                                                  , FilterDescriptor = ["L0ET,>,%s"%self.getProp("HadViaT_ETCut")]
                                             )
                                     ]) 
	
            #Define the tool which actually makes the forward tracks
            #from the L0 confirmed objects
            Hlt1HadronViaTTUTConf = Member ( 'TU', 'TConf', RecoName = 'THadronConf' 
                                             , tools = [Tool( type = HltTrackUpgradeTool
                                               , name = 'HltTrackUpgradeTool'
                                               , tools=[Tool( type = L0ConfirmWithT
                                                 , name='THadronConf'
                                                 , particleType = 1
                                                 , trackingTool='PatConfirmTool'
                                                 , tools = [ Tool( type = PatConfirmTool
                                                   , name = 'PatConfirmTool'
                                                   , nSigmaX = 4
                                                   , nSigmaY = 4
                                                   , nSigmaTx = 4
                                                   , nSigmaTy = 4
                                                   , restrictSearch = True
                                                   , debugMode = False 
                                                   , tools = [ConfiguredPR( "PatSeeding" )]
#                                                                  [ Tool( type = PatSeedingTool

#                                                    , name = 'PatSeedingTool'
#                                                    , zMagnet = 0
#                                                                    )]
                                                                )]
                                                              )]
                                                            )]
                                               
                                           )

            conf = l0.members()  + [ DecodeIT 
                    , Hlt1HadronViaTTUTConf
                    , Member ( 'TF' , 'TConf' , FilterDescriptor = ['ptAtOrigin,>,'+PTCut])
		            , Velo
                    , PV3D().ignoreOutputSelection()
                    , DecodeTT
                    , Member ( 'TF' , 'VeloRZTMatch' , FilterDescriptor = ['VeloTMatch_%TFTConf,<,'+VTMatchCut]) 
                    , Member ( 'TF' , 'Velo3DIP', FilterDescriptor = [ 'IP_PV3D,||>,'+IPCut])
                    , Member ( 'TM' , 'VeloT'
                               , InputSelection1 = '%TFVelo3DIP'
                               , InputSelection2 = '%TFTConf'
                               , MatchName = 'VeloT' , MaxQuality = 2.
                               )
                    ]
            return bindMembers(prefix,conf)
        
        # simple hadron cut
        #---------------
        def singlehadron():
            sh = [ Member ( 'TF' , 'SingleHadronViaTPT' , 
                            FilterDescriptor = ['ptAtOrigin,>,'+_cut('SingleHadronViaT_PTCut')]
                            )
                   , Member ( 'TF', 'SingleHadronViaTIP', OutputSelection = "%Decision" ,
                           FilterDescriptor = [ 'IP_PV3D,||>,'+_cut('HadViaTSingle_IPCut')]
                           ) 
                   ]
            return sh


        # companion track - dihadron part
        #------------------------------------
        def companion(type=""):
            OutputOfConfirmation = confirmation(type).outputSelection() 
            IP2Cut = _cut(type+"HadViaTDi_IPCut")
            comp = [ Velo 
                   , PV3D().ignoreOutputSelection()
                   , Member ( 'TF', '1UVelo'
                           , FilterDescriptor = ['MatchIDsFraction_%s,<,0.9' %OutputOfConfirmation]
                           )
                   , Member ( 'TF', 'Companion'
                           , FilterDescriptor = [ 'IP_PV3D,||>,%s'%IP2Cut]
                           )
                   ]
            return comp

        #
        # dihadron part
        #---------------------
        def dihadron(type=""):
            OutputOfConfirmation = confirmation(type).outputSelection()
            PT2Cut = _cut(type+"HadViaTCompanion_PTCut")
            IP3Cut = _cut(type+"HadViaTDi_IPCut")
            dih = [ PV3D().ignoreOutputSelection()
                , Member ( 'TF', 'DiHadronViaTIP', InputSelection = '%s' %OutputOfConfirmation
                           , FilterDescriptor = [ 'IP_PV3D,||>,'+IP3Cut]
                           )
                , Member ( 'VM2', 'UVelo'
                         , InputSelection1 = '%TFDiHadronViaTIP'
                         , InputSelection2 = '%TFCompanion'
                         , FilterDescriptor = [ 'DOCA,<,%s'%(self.getProp('HadViaTCompanion_DOCACut'))]
                           )
                , Member ( 'VF', 'UVelo'
                           , FilterDescriptor = [ 'VertexDz_PV3D,>,'+_cut('HadViaTCompanion_DZCut')]
                           )
                , Member ( 'VU', 'Forward'
                           , RecoName = 'Forward'
                           , tools = [ Tool( HltTrackUpgradeTool
                                             ,tools = [ConfiguredPR( "Forward" )] )]
                           )
                , Member ( 'VF', '1Forward',
                           FilterDescriptor = [ 'VertexMinPT,>,'+PT2Cut]
                           )
                , Member ( 'VF', 'DiHadron', OutputSelection = "%Decision", 
                           FilterDescriptor = [ 'VertexPointing_PV3D,<,'+_cut("HadViaTCompanion_PointingCut")]
                           )
                ]
            return dih
        
        from Hlt1Lines.HltL0Candidates import L0Channels
        if self.getProp('L0Channel') in L0Channels() :
            # Single Hadron Line
            #-----------------------------------
            Line ( 'SingleHadronViaT'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , L0DU  = "L0_CHANNEL('%(L0Channel)s')" % self.getProps()
                   , algos = [confirmation()]+singlehadron()
                   )

            # DiHadron Line
            #-----------------------------------
            Line ('DiHadronViaT'
                  , prescale = self.prescale
                  , postscale = self.postscale
                  , L0DU  = "L0_CHANNEL('%(L0Channel)s')" % self.getProps()
                  , algos =  [confirmation()]+companion()+dihadron()
                  )
