# =============================================================================
## @file
#  Configuration of Hadron Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Hadron trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.13 $"
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
    # (J.A. Hernando 17/12/08)
    # External selection to be provided
    #    1) AllL0Hadrons (L0 Candidates above a given threshold i.e 2000 MeV)
    #    2) RZVelo, PV2D, Velo tracks
    # Replace opts-file to set the opts for the fast fit by configurables

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
    __slots__ = { 'L0Channel'               : "Hadron" 
                , 'HadSingle_IPCut'         : 0.1
                , 'HadDi_IPCut'             : 0.1
                , 'HadMain_PTCut'           : 2500.
                , 'HadMain_TrackFitChi2Cut' : 10.
                , 'HadCompanion_DOCACut'    : 0.2
                , 'HadCompanion_DZCut'      : 1.5
                , 'HadCompanion_PTCut'      : 1000.
                , 'HadCompanion_PointingCut': 0.4
                , 'SingleHadron_PTCut'      : 5000.
                , 'SoftHadMain_ETCut'       : 2500. 
                , 'SoftHadMain_PTCut'       : 1500.
                , 'SoftHadDi_IPCut'         : 0.1
                , 'Prescale'                : { 'Hlt1DiHadronSoft' : 0 }
                }
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Tool   as Tool
        from HltLine.HltLine import hlt1Lines  
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit
        from HltTracking.HltReco import RZVelo
        from HltTracking.HltPVs  import PV2D
        from Configurables import HltTrackUpgradeTool, PatForwardTool, HltGuidedForward
        from Hlt1Lines.HltConfigurePR import ConfiguredPR
        
        # confirmed track
        #------------------------
        def confirmationl0part(type=""):
            prefix = 'HadronConfL0'+type
            from Hlt1Lines.HltL0Candidates import convertL0Candidates
            L0Channel = self.getProp("L0Channel")
            # get the L0 candidates (all or L0)
            if (type!="Soft"):
                l0 = bindMembers(prefix, [ convertL0Candidates(L0Channel)] )
            else :  
                l0 = bindMembers(prefix, [ convertL0Candidates('All'+L0Channel)
                                         , Member('TF','L0Hadron'
                                                 , FilterDescriptor = ["L0ET,>,%s"%self.getProp("SoftHadMain_ETCut")]
                                                 , HistogramUpdatePeriod = 1
                                                 , HistoDescriptor  = histosfilter('L0ET',1500.,5100.,200)
                                                 )       
                                         ] )
            return l0

        def confirmationpreip(type=""):
            if (type == 'Single') or (type == 'Di') :
                type = ''
            prefix = 'HadronConfPreIP'+type
            OutputOfL0 = confirmationl0part(type).outputSelection()
            conf =                  [ RZVelo
                                    , PV2D().ignoreOutputSelection()
                                    , Member ( 'TF' ,'RZVelo'
                                             , FilterDescriptor = ['Calo2DChi2_%s,<,4'%OutputOfL0 ]
                                             , HistogramUpdatePeriod = 1
                                             , HistoDescriptor  = histosfilter('Calo2DChi2_'+OutputOfL0,0.,10.,200)
                                             )
                                    , Member ( 'TU', 'Velo',  RecoName = 'Velo')
                                    , Member ( 'TF', '1Velo'
                                             , FilterDescriptor = [ 'Calo3DChi2_%s,<,4'%OutputOfL0 ]
                                             , HistogramUpdatePeriod = 1
                                             , HistoDescriptor  = histosfilter('Calo3DChi2_'+OutputOfL0,0.,10.,200)
                                             )
                                    ]
            return bindMembers(prefix, conf)

        def veloipcut(type=""):
            prefix = "HadronConfVeloIPCut"+type
            OutputOfConf = confirmationpreip(type).outputSelection()
            
            if   type == 'Soft'   : 
                 cutvalue = self.getProp("SoftHadDi_IPCut")
            elif type == 'Di'     : 
                 cutvalue = self.getProp("HadDi_IPCut")
            elif type == 'Single' : 
                 cutvalue = self.getProp("HadSingle_IPCut")
            else                  : 
                 return None # Not an allowed value!

            conf = [Member ( 'TF', '2Velo', InputSelection = '%s' %OutputOfConf
                           , FilterDescriptor = [ 'IP_PV2D,||>,%s' %cutvalue ]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor = histosfilter('IP_PV2D_'+type,-0.2,1.8,200))]

            return bindMembers(prefix,conf)
            

        def confirmationpostip(type=""):
            OutputOfVeloIP = veloipcut(type).outputSelection()
            OutputOfL0 = confirmationl0part(type).outputSelection()
            prefix = 'HadronConfPostIP'+type
            from HltLine.HltDecodeRaw import DecodeIT
            conf =                  [ Member ( 'TM' , 'VeloCalo'
                                             , InputSelection1 = '%s' %OutputOfVeloIP
                                             , InputSelection2 = '%s' %OutputOfL0
                                             , MatchName = 'VeloCalo' , MaxQuality = 4.
                                             )
                                    , DecodeIT
                                    , Member ( 'TU', 'GuidedForward'
                                             , RecoName = 'GuidedForward'
                                             , tools = [ Tool( HltTrackUpgradeTool
                                                               , tools = [ Tool( HltGuidedForward
                                                                                 ,tools = [ConfiguredPR( "Forward" )] )] )]
                                               , HistogramUpdatePeriod = 1
                                             )         
                                    ]
            if type <> 'Single' :
                if   type == "Soft"   : cutvalue = self.getProp("SoftHadMain_PTCut")
                elif type == "Di"     : cutvalue = self.getProp("HadMain_PTCut")
                else                  : return None # Not an allowed value!
                conf += [Member ( 'TF' , 'DiHadronPT1' 
                                       , FilterDescriptor = ['PT,>,%s'%cutvalue]
                                       , HistogramUpdatePeriod = 1
                                       , HistoDescriptor  = histosfilter('PT_'+type,0.,8000.,200)
                                )]
            return bindMembers(prefix, conf)
        
        # simple hadron cut
        #---------------
        def singlehadron():
            OutputOfConfirmation = confirmationpostip('Single').outputSelection()
            sh = [ PV2D().ignoreOutputSelection(),
                   Member ( 'TF' , 'SingleHadronPT' ,
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
            if   type == "Soft"   : cutvalue = self.getProp("SoftHadDi_IPCut")
            elif type == "Di"     : cutvalue = self.getProp("HadDi_IPCut")
            else                  : return None # Not an allowed value!
            comp = [ RZVelo , PV2D().ignoreOutputSelection()
                , Member ( 'TU', 'UVelo' , RecoName = 'Velo')
                , Member ( 'TF', 'Companion'
                           , FilterDescriptor = [ 'IP_PV2D,||>,%s'%cutvalue]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('IP_PV2D_'+type,-0.3,30.,200)
                           )
                ]
            return bindMembers(prefix,comp)

        #
        # dihadron part
        #---------------------
        def dihadron(type=""):
            prefix = "HadDi"
            if type == 'Soft' : prefix = 'HadDiSoft'  
            
            OutputOfConfirmation = confirmationpostip(type).outputSelection()
            OutputOfCompanion    = companion(type).outputSelection() 
            
            if   type == "Soft"   : cutvalue = self.getProp("SoftHadMain_PTCut")
            elif type == "Di"     : cutvalue = self.getProp("HadMain_PTCut")
            else                  : return None # Not an allowed value!
            
            dih = [ PV2D().ignoreOutputSelection()
                , Member ( 'VM2', 'UVelo'
                         , InputSelection1 = '%s' %OutputOfConfirmation #TFDiHadronPT1'
                         , InputSelection2 = '%s' %OutputOfCompanion
                         , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('HadCompanion_DOCACut'))]
                         , HistoDescriptor  = histosfilter('DOCA_'+type,0.,1.,200)
                           )
                , Member ( 'VF', '1UVelo'
                           , FilterDescriptor = [ 'VertexDz_PV2D,>,%s'%self.getProp('HadCompanion_DZCut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('VertexDx_PV2D_'+type,1.,12.,200)                       
                           )
                , Member ( 'VU', 'Forward'
                           , RecoName = 'Forward'
                           , tools = [ Tool( HltTrackUpgradeTool
                                             ,tools = [ConfiguredPR( "Forward" )] )]
                           )
                , Member ( 'VF', '2UVelo'
                           , FilterDescriptor = ['VertexTracksMatchIDsFraction,<,0.9']
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('VertexTracksMatchIDsFraction',0.,1.,100)
                           )
                , Member ( 'VF', 'DiHadronPT2',
                           FilterDescriptor = [ 'VertexMinPT,>,%s'%self.getProp("HadCompanion_PTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT_'+type,0.,5000.,200)                       
                           )
                , Member ( 'VF', 'DiHadron',
                           FilterDescriptor = [ 'VertexPointing_PV2D,<,%s'%self.getProp("HadCompanion_PointingCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexPointing_PV2D_'+type,0.1,1.1,200)                       
                           )
                ]
            return bindMembers(prefix,dih)
        
        # afterburn of a line with tracks
        #--------------------------------
        def afterburn():
            after = [ PV2D().ignoreOutputSelection()
                , Member ( 'TU' , 'FitTrack' , RecoName = "FitTrack", callback = setupHltFastTrackFit )
                , Member ( 'TF' , '1FitTrack' ,
                           FilterDescriptor = ["FitIP_PV2D,||>,%s"%self.getProp('HadSingle_IPCut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitIP_PV2D',-.3,3.,200)
                           )
                , Member ( 'TF' , '2FitTrack'
                           , OutputSelection = '%Decision'
                           , FilterDescriptor = ["FitChi2OverNdf,<,%s"%self.getProp('HadMain_TrackFitChi2Cut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitChi2OverNdf',0.,100.,100)
                           )
                ]
            return after

        # afterburn of a line with vertices
        #-------------------
        def vafterburn(type=""):
            OutputOfDiHadron = dihadron(type).outputSelection()

            if   type == "Soft"   : cutvalue = self.getProp("SoftHadDi_IPCut")
            elif type == "Di"     : cutvalue = self.getProp("HadDi_IPCut")
            else                  : return None # Not an allowed value!

            vafter =  [ PV2D().ignoreOutputSelection()
                , Member ( 'VU', 'FitTrack', InputSelection='%s' %OutputOfDiHadron
                           , RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                , Member ( 'VF', '1FitTrack',
                           FilterDescriptor = [ 'FitVertexMinIP_PV2D,||>,%s'%cutvalue],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitVertexMinIP_PV2D_'+type,-0.3,3.,200)                       
                           )
                , Member ( 'VF', '2FitTrack'
                           , OutputSelection = "%Decision"
                           , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,%s'%self.getProp('HadMain_TrackFitChi2Cut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('FitVertexMaxChi2OverNdf_'+type,0,50.,200)
                           )
                ]
            return vafter
            
        from Hlt1Lines.HltL0Candidates import L0Channels
        if self.getProp('L0Channel') in L0Channels() :
            # Single Hadron Line
            #-----------------------------------
            Line ( 'SingleHadron'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmationl0part()]+\
                            [confirmationpreip()]+\
                            [veloipcut('Single')]+\
                            [confirmationpostip('Single')]+\
                            singlehadron()+\
                            afterburn()
                 )

            # DiHadron Line
            #-----------------------------------
            Line ( 'DiHadron'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmationl0part()]+\
                            [confirmationpreip()]+\
                            [veloipcut('Di')]+\
                            [confirmationpostip('Di')]+\
                            [companion('Di')]+\
                            [dihadron('Di')]+\
                            vafterburn('Di')
                 )

        # Soft DiHadron Line
        #-----------------------------------
        Line('DiHadronSoft'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_DECISION_PHYSICS"
             , algos =  [confirmationl0part('Soft')]+\
                        [confirmationpreip('Soft')]+\
                        [veloipcut('Soft')]+\
                        [confirmationpostip('Soft')]+\
                        [companion('Soft')]+\
                        [dihadron('Soft')]+\
                        vafterburn('Soft')
             )
