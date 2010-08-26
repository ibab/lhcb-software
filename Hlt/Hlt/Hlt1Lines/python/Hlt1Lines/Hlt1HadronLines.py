# =============================================================================
## @file
#  Configuration of Hadron Lines
#  @author Vladimir Gligorov vladimir.gligorov@cern.ch
#  @date 2010-05-19
# =============================================================================
"""
 script to configure Hadron trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.32 $"
# =============================================================================

import Gaudi.Configuration 

def histosfilter(name,xlower=0.,xup=100.,nbins=100):
    """ return the dictonary with the booking of the histograms associated to a filter
    @param filter name i.e 'PT
    """
    histosfs = { name : ( name,xlower,xup,nbins),
                 name+"Best" : (name+"Best",xlower,xup,nbins) }
    return histosfs


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1HadronLinesConf(HltLinesConfigurableUser) :
    #--------------------------------
    #
    # V. Gligorov, based on code by J.A. Hernando 
    # External selection to be provided
    #    1) AllL0Hadrons (L0 Candidates above a given threshold i.e 2000 MeV)
    #    2) Velo, PV3D, Velo tracks
    # Replace opts-file to set the opts for the fast fit by configurables

    # Note: the 3D reconstruction is now done upfront by adding
    #
    # [Velo,PV3D().ignoreOutputSelection()]
    #
    # to the line. This replaces the old
    #
    # [RZVelo,PV2D().ignoreOutputSelection()]
    #     
    # It requires the import of Velo,PV3D instead of RZVelo,PV2D.    

    # steering variables
    #------------------------
    #   Single Hadron Line
    #     SingleHadron_PtCut
    #   DiHadron Line
    #     HadMain_IPCut
    #     HadMain_PtCut
    #     HadCompanion_IPCut
    #     HadCompanion_PtCut
    #
    __slots__ = { 'L0Channel'                   	: "Hadron" 
                , 'HadSingle_IPChi2Cut'         	: 25.
                , 'HadDi_VDChi2Cut'             	: 50.
                , 'HadMain_PTCut'               	: 2500.
                , 'HadMain_TrackFitChi2Cut'     	: 10.
                , 'HadCompanion_DOCACut'        	: 0.2
                , 'HadCompanion_DZCut'          	: 1.5
                , 'HadCompanion_PTCut'          	: 1000.
                , 'HadCompanion_PointingCut'    	: 0.4
                , 'SingleHadron_PTCut'          	: 5000.
                , 'HadDi_IP'                      : -1. #IP cut turned off for now
                # The lifetime unbiased slots 
                , 'HadLTUnbVertex_DOCACut'          :   0.4
                , 'HadLTUnbVertex_PCut'             : 10000.
                , 'HadLTUnbVertex_MaxPTCut'         :  1100.
                , 'HadLTUnbVertex_MinPTCut'         :  1000.
                , 'HadLTUnbVertex_MassMinCut'       :  5000.0
                , 'HadLTUnbVertex_MassMaxCut'       :  5800.0
                , 'HadLTUnbVertex_CosThetaStarCut'  :  0.9
                , 'HadMonCompRate'                  :  1
                , 'Postscale'               : { 'Hlt1HadronMonVeloReco' : 0.000001,
                                                'Hlt1HadronMonConf1'    : 0.000001,
                                                'Hlt1HadronMonConf2'    : 0.000001,
                                              }   
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
        from Configurables import HltTrackUpgradeTool, HltVeloTCaloMatch, PatForwardTool, HltGuidedForward
        from Hlt1Lines.HltConfigurePR import ConfiguredPR
        from Configurables import HltFilterFittedVertices
        
        # confirmed track
        #------------------------
        def confirmationl0part(type=""):
            prefix = 'HadronConfL0'+type
            from Hlt1Lines.HltL0Candidates import convertL0Candidates
            L0Channel = self.getProp("L0Channel")
            # get the L0 candidates (all or L0)
            # also, implement the global event cuts 
            from Hlt1Lines.Hlt1GECs import Hlt1GEC
            if type == "LTUnb" :
                from Configurables import LoKi__VoidFilter as VoidFilter
                from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
                modules =  CoreFactory('CoreFactory').Modules
                if 'LoKiTrigger.decorators' not in modules:
                    modules.append('LoKiTrigger.decorators')
                L0HadVoid = VoidFilter('L0HadVoid',Code = "(CONTAINS('Hlt/Track/Hlt1L0"+L0Channel+"Decision') > 1)")
                return bindMembers(prefix, [ Hlt1GEC(),convertL0Candidates(L0Channel),L0HadVoid] )
            return bindMembers(prefix, [ Hlt1GEC(),convertL0Candidates(L0Channel)] )

        def confirmationtrackmatch(type=""):
            prefix = 'HadronConfPreTrackMatch'+type
            OutputOfL0 = confirmationl0part(type).outputSelection()
            conf = [Velo,PV3D().ignoreOutputSelection(),
                    Member ( 'TM' , 'VeloCalo'
                            , InputSelection1 = 'Velo'
                            , InputSelection2 = '%s' %OutputOfL0
                            , MatchName = 'VeloCalo' , MaxQuality = 9., MaxQuality2 = 9.
                           )
                   ]

            return bindMembers(prefix, conf)

        def confirmationguidedforward(type=""):
            OutputOfVeloMatch = confirmationtrackmatch(type).outputSelection()
            
            prefix = 'HadronConfTrackMatch'+type
            from HltLine.HltDecodeRaw import DecodeIT
            conf =                  [ DecodeIT
                                    , Member ( 'TU', 'GuidedForward'
                                             , InputSelection = '%s' %OutputOfVeloMatch
                                             , RecoName = 'GuidedForward'
                                             , tools = [ Tool( HltTrackUpgradeTool
                                                               , tools = [ Tool( HltGuidedForward
                                                                                 ,tools = [ConfiguredPR( "Forward" )] )] )]
                                               , HistogramUpdatePeriod = 1
                                             )         
                                    ]
            return bindMembers(prefix, conf)
        
        # simple hadron cut
        #---------------
        def singlehadron(type=""):
            OutputOfConfirmation = confirmationguidedforward(type).outputSelection()
            sh = [ Member ( 'TF' , 'SingleHadronPT' ,
                            InputSelection = '%s' %OutputOfConfirmation,
                            FilterDescriptor = ['PT,>,%(SingleHadron_PTCut)s'%self.getProps()],
                            HistogramUpdatePeriod = 1,
                            HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                            )
                   ]
            return sh


        # companion track - dihadron part
        #------------------------------------
        def companion(type=""):
            prefix = "HadCompanion"+type

            comp = [ Velo , PV3D().ignoreOutputSelection()]

            if (type.find('Mon') > -1) :
                return bindMembers(prefix,comp)

            OutputOfConfirmation = confirmationguidedforward(type).outputSelection()
 
            comp += [ Member ( 'TF', '1UVelo'
                               , FilterDescriptor = ['MatchIDsFraction_%s,<,0.9' %OutputOfConfirmation ]
                               , HistogramUpdatePeriod = 1
                               , HistoDescriptor  = histosfilter('MatchIDsFraction_%s' %OutputOfConfirmation,0.,8000.,200)
                               )
                    ]

            return bindMembers(prefix,comp)

        #
        # dihadron part
        #---------------------
        def dihadron(type=""):
            prefix = "HadDi"
            
            OutputOfConfirmation = confirmationguidedforward(type).outputSelection()
            OutputOfCompanion    = companion(type).outputSelection() 
            
            from HltLine.HltDecodeRaw import DecodeIT
            dih = [ PV3D().ignoreOutputSelection()
                , Member ( 'TF' , 'ConfPT', InputSelection = '%s' %OutputOfConfirmation, 
                           FilterDescriptor = ['PT,>,%(HadCompanion_PTCut)s'%self.getProps()],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('PT',0.,8000.,200) 
                         ) 
                , Member ( 'VM2', 'UVelo'
                         , InputSelection1 = '%TFConfPT' 
                         , InputSelection2 = '%s' %OutputOfCompanion
                         , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('HadCompanion_DOCACut'))]
                         , HistoDescriptor  = histosfilter('DOCA_'+type,0.,1.,200)
                           )
                , Member ( 'VF', '1UVelo'
                           , FilterDescriptor = [ 'VertexDz_PV3D,>,%s'%self.getProp('HadCompanion_DZCut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('VertexDz_PV3D_'+type,1.,12.,200)                       
                           )
                , Member ( 'VF', '2UVelo'
                           , FilterDescriptor = [ 'VertexMinIP_PV3D,||>,%s'%self.getProp('HadDi_IP')] 
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('DiVtxIP',0.,400.,100)
                         )   
                , DecodeIT
                , Member ( 'VU', 'Forward'
                           , RecoName = 'Forward'
                           , tools = [ Tool( HltTrackUpgradeTool
                                             ,tools = [ConfiguredPR( "Forward" )] )]
                           )
                , Member ( 'VF', 'DiHadronPT1', 
                           FilterDescriptor = [ 'VertexMaxPT,>,%s'%self.getProp("HadMain_PTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT_'+type,0.,5000.,200)
                         )
                , Member ( 'VF', 'DiHadronPT2',
                           FilterDescriptor = [ 'VertexMinPT,>,%s'%self.getProp("HadCompanion_PTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT_'+type,0.,5000.,200)                       
                           )
                , Member ( 'VF', 'DiHadron',
                           FilterDescriptor = [ 'VertexPointing_PV3D,<,%s'%self.getProp("HadCompanion_PointingCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexPointing_PV3D_'+type,0.1,1.1,200)                       
                           )
                ]
            return bindMembers(prefix,dih)
        
        # afterburn of a line with tracks
        #--------------------------------
        def afterburn():
            after = [ PV3D().ignoreOutputSelection()
                , Member ( 'TU' , 'FitTrack' , RecoName = "FitTrack", callback = setupHltFastTrackFit )
                , Member ( 'TF' , '2FitTrack'
                           , FilterDescriptor = ["FitChi2OverNdf,<,%s"%self.getProp('HadMain_TrackFitChi2Cut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitChi2OverNdf',0.,100.,100)
                           )
                , Member ('HltFilterFittedTracks', 'FFT',
                          OutputSelection = "%Decision",
                          InputSelection1 = '%TF2FitTrack', 
                          InputSelection2 = 'PV3D',
                          MinIPCHI2 = self.getProp('HadSingle_IPChi2Cut')
                         )
                ]
            return after

        # afterburn of a line with vertices
        #-------------------
        def vafterburn(type=""):
            OutputOfDiHadron = dihadron(type).outputSelection()

            vafter =  [ PV3D().ignoreOutputSelection()
                , Member ( 'VU', 'FitTrack', InputSelection='%s' %OutputOfDiHadron
                           , RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                , Member ( 'VF', '2FitTrack'
                           , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,%s'%self.getProp('HadMain_TrackFitChi2Cut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('FitVertexMaxChi2OverNdf_'+type,0,50.,200)
                           )
                , Member ('HltFilterFittedVertices', 'FFV',
                          OutputSelection = "%Decision",
                          InputSelection1 = '%VF2FitTrack', 
                          InputSelection2 = 'PV3D',
                          MinVDCHI2 = self.getProp('HadDi_VDChi2Cut')
                         )
                ]
            return vafter
           
        def monveloreco():
            prefix = 'HadronMonVeloReco'
            conf = [Velo]

            return conf

        def mondecision(type=""):
            if type.find('Conf') > -1 :
                return [Member('TF','MonConf',OutputSelection = "%Decision", FilterDescriptor = ['IsBackward,<,0.5'])]
            elif type.find('Comp') > -1 :
                from HltLine.HltDecodeRaw import DecodeIT
                return [ DecodeIT
                       , Member( 'TU',
                                'MonForward',
                                RecoName = 'Forward',
                                tools = [ Tool( HltTrackUpgradeTool,
                                                tools = [ConfiguredPR( "Forward" )]
                                              )
                                        ]
                              )
                       ]+mondecision('Conf')
            else                :
                return None # Not an allowed type

        # lifetime unbiased dihadron part
        #---------------------
        def LTUnbDiHadron(prefix,type=""):

            DocaCut                = str(self.getProp("HadLTUnbVertex_DOCACut"))
            VertexPCut             = str(self.getProp("HadLTUnbVertex_PCut"))
            MassMinCut             = str(self.getProp("HadLTUnbVertex_MassMinCut"))
            MassMaxCut             = str(self.getProp("HadLTUnbVertex_MassMaxCut"))
            CosThetaStarCut        = str(self.getProp("HadLTUnbVertex_CosThetaStarCut"))
            
            OutputOfConfirmation = confirmationguidedforward(type).outputSelection()
            from HltLine.HltDecodeRaw import DecodeIT
            dih = [ Member ( 'VM1', 'UVelo'
                         , InputSelection = '%s' %OutputOfConfirmation
                         , FilterDescriptor = [ 'DOCA,<,'           + DocaCut,
                                                'CosThetaStar,[],0,'+ CosThetaStarCut
                                                ]
                         , HistoDescriptor  = histosfilter('DOCA_'+type,0.,1.,200)
                           )
                , Member ( 'VF', 'DiHadronPT1',
                           FilterDescriptor = [ 'VertexMaxPT,>,%s'%self.getProp("HadLTUnbVertex_MaxPTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMaxPT_'+type,0.,5000.,200)
                         )                       
                , Member ( 'VF', 'DiHadronPT2',
                           FilterDescriptor = [ 'VertexMinPT,>,%s'%self.getProp("HadLTUnbVertex_MinPTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT_'+type,0.,5000.,200)
                           )
                , Member ( 'VF', 'VertexMinP',
                           FilterDescriptor = [ 'VertexMinP,>,'+VertexPCut]
                           )
                , Member ( 'VF', 'Mass',
                            FilterDescriptor = [ 'VertexDikaonMass,[],'+MassMinCut+','+MassMaxCut]
                           )          
                , Member ( 'VU', 'FitTrack' 
                           , RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                , Member ( 'VF', '2FitTrack' 
                           , OutputSelection = "%Decision"
                           , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,%s'%self.getProp('HadMain_TrackFitChi2Cut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('FitVertexMaxChi2OverNdf_'+type,0,50.,200)
                           )                   
                ]                                              
            return bindMembers(prefix,dih)
 
        from Hlt1Lines.HltL0Candidates import L0Channels
        if self.getProp('L0Channel') in L0Channels() :
            # Single Hadron Line with 3D reco upfront
            #-----------------------------------
            Line ( 'SingleHadron'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmationl0part()]+\
                            [confirmationtrackmatch()]+\
                            [confirmationguidedforward()]+\
                            singlehadron()+\
                            afterburn()
                 )

            # DiHadron Line with 3D upfront
            #-----------------------------------
            Line ( 'DiHadron'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmationl0part()]+\
                            [confirmationtrackmatch()]+\
                            [confirmationguidedforward()]+\
                            [companion()]+\
                            [dihadron()]+\
                            vafterburn()
                 )

            # Monitoring line : velo reconstruction
            #-----------------------------------
            Line ( 'HadronMonVeloReco'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos = [confirmationl0part()]+\
                           monveloreco()+\
                           mondecision('Conf') 
                 )

            # Monitoring line : pre-confirmation reconstruction
            #-----------------------------------
            Line ( 'HadronMonConf1'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos = [confirmationl0part()]+\
                           [confirmationtrackmatch()]+\
                           mondecision('Conf') 
                 )
            
            # Monitoring line : post-confirmation reconstruction
            #-----------------------------------
            Line ( 'HadronMonConf2'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos = [confirmationl0part()]+\
                           [confirmationtrackmatch()]+\
                           [confirmationguidedforward()]+\
                           mondecision('Conf')
                 )

            # Monitoring line : forward reconstruction
            #-----------------------------------
            Line ( 'HadronMonComp'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU = "scale(L0_CHANNEL('%(L0Channel)s'), RATE(%(HadMonCompRate)s))"%self.getProps()   ### TODO: make sure it is added to Hlt2Transparent!!!! -- can we do this by naming convention???
                 , algos = [confirmationl0part()]+\
                            monveloreco()+\
                            mondecision('Comp')
                 )
            Line ( 'DiHadronLTUnbiased'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos = [confirmationl0part("LTUnb")]+\
                           [confirmationtrackmatch()]+\
                           [confirmationguidedforward()]+\
                           [LTUnbDiHadron(prefix='DiHadronLTUnbiased')]
                 )     
            

