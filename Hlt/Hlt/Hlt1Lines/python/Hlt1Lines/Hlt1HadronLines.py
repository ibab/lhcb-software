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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.6 $"
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
        from HltLine.HltReco import RZVelo
	from HltLine.HltPVs  import PV2D

        
        # confirmed track
        #------------------------
        def confirmation(type=""):
            prefix = 'HadronConfirmation'+type
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

            from HltLine.HltDecodeRaw import DecodeIT
            conf =  l0.members()  + [ RZVelo
                                    , PV2D().ignoreOutputSelection()
                                    , Member ( 'TF' ,'RZVelo'
                                             , FilterDescriptor = ['Calo2DChi2_%s,<,4'%l0.outputSelection() ]
                                             , HistogramUpdatePeriod = 1
                                             , HistoDescriptor  = histosfilter('Calo2DChi2_'+l0.outputSelection(),0.,10.,200)
                                             )
                                    , Member ( 'TU', 'Velo',  RecoName = 'Velo')
                                    , Member ( 'TF', '1Velo'
                                             , FilterDescriptor = [ 'Calo3DChi2_%s,<,4'%l0.outputSelection() ]
                                             , HistogramUpdatePeriod = 1
                                             , HistoDescriptor  = histosfilter('Calo3DChi2_'+l0.outputSelection(),0.,10.,200)
                                             )
                                    , Member ( 'TF', '2Velo'
                                             , FilterDescriptor = [ 'IP_PV2D,||>,%s' % min( self.getProp(type+"HadDi_IPCut"),self.getProp("HadSingle_IPCut") )]
                                             , HistogramUpdatePeriod = 1
                                             , HistoDescriptor = histosfilter('IP_PV2D',-0.2,1.8,200)
                                             )
                                    , Member ( 'TM' , 'VeloCalo'
                                             , InputSelection1 = '%TF2Velo'
                                             , InputSelection2 = l0.outputSelection()
                                             , MatchName = 'VeloCalo' , MaxQuality = 4.
                                             )
                                    , DecodeIT
                                    , Member ( 'TU', 'GuidedForward'
                                             , RecoName = 'GuidedForward'
                                             , HistogramUpdatePeriod = 1
                                             )
                                    , Member ( 'TF' , 'Confirmed'
                                             , FilterDescriptor = ['PT,>,%s'%self.getProp(type+"HadMain_PTCut")]
                                             , HistogramUpdatePeriod = 1
                                             , HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                                             )
                                    ]
            return bindMembers(prefix, conf)
        
        # simple hadron cut
        #---------------
        def singlehadron():
            sh = [ Member ( 'TF' , 'SingleHadronPT' ,
                            FilterDescriptor = ['PT,>,%(SingleHadron_PTCut)s'%self.getProps()],
                            HistogramUpdatePeriod = 1,
                            HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                            ),
                   Member ( 'TF' , 'SingleHadronIP' ,
                            FilterDescriptor = [ 'IP_PV2D,||>,%(HadSingle_IPCut)s'% self.getProps()],
                            HistogramUpdatePeriod = 1,
                            HistoDescriptor = histosfilter('IP_PV2D',-0.2,1.8,200)
                          )
                   ]
            return sh


        # companion track - dihadron part
        #------------------------------------
        def companion(type=""):
            OutputOfConfirmation = confirmation(type).outputSelection()
            comp = [ RZVelo , PV2D().ignoreOutputSelection()
                , Member ( 'TU', 'UVelo' , RecoName = 'Velo')
                , Member ( 'TF', '1UVelo'
                           , FilterDescriptor = ['MatchIDsFraction_%s,<,0.9' %OutputOfConfirmation ]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('MatchIDsFraction_%s' %OutputOfConfirmation,0.,8000.,200)
                           )
                , Member ( 'TF', 'Companion'
                           , FilterDescriptor = [ 'IP_PV2D,||>,%s'%self.getProp(type+"HadDi_IPCut")]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('IP_PV2D',-0.3,30.,200)
                           )
                ]
            return comp

        #
        # dihadron part
        #---------------------
        def dihadron(type=""):
            OutputOfConfirmation = confirmation(type).outputSelection()
            dih = [ PV2D().ignoreOutputSelection()
                , Member ( 'TF' , 'DiHadronIP' ,
                            InputSelection = '%s' %OutputOfConfirmation,
                            FilterDescriptor = [ 'IP_PV2D,||>,%s'% self.getProp(type+"HadDi_IPCut")],
                            HistogramUpdatePeriod = 1,
                            HistoDescriptor = histosfilter('IP_PV2D',-0.2,1.8,200)
                          )
                , Member ( 'VM2', 'UVelo'
                         , InputSelection1 = '%TFDiHadronIP'
                         , InputSelection2 = '%TFCompanion'
                         , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('HadCompanion_DOCACut'))]
                         , HistoDescriptor  = histosfilter('DOCA',0.,1.,200)
                           )
                , Member ( 'VF', 'UVelo'
                           , FilterDescriptor = [ 'VertexDz_PV2D,>,%s'%self.getProp('HadCompanion_DZCut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('VertexDx_PV2D',1.,12.,200)                       
                           )
                , Member ( 'VU', 'Forward', RecoName = 'Forward')
                , Member ( 'VF', '1Forward',
                           FilterDescriptor = [ 'VertexMinPT,>,%s'%self.getProp("HadCompanion_PTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT',0.,5000.,200)                       
                           )
                , Member ( 'VF', 'DiHadron',
                           FilterDescriptor = [ 'VertexPointing_PV2D,<,%s'%self.getProp("HadCompanion_PointingCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexPointing_PV2D',0.1,1.1,200)                       
                           )
                ]
            return dih
        
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
            vafter =  [ PV2D().ignoreOutputSelection()
                , Member ( 'VU', 'FitTrack',   RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                , Member ( 'VF', '1FitTrack',
                           FilterDescriptor = [ 'FitVertexMinIP_PV2D,||>,%s'%self.getProp(type+'HadDi_IPCut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitVertexMinIP_PV2D',-0.3,3.,200)                       
                           )
                , Member ( 'VF', '2FitTrack'
                           , OutputSelection = "%Decision"
                           , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,%s'%self.getProp('HadMain_TrackFitChi2Cut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('FitVertexMaxChi2OverNdf',0,50.,200)
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
                 , algos = [confirmation()]+singlehadron()+afterburn()
                 )

            # DiHadron Line
            #-----------------------------------
            Line ( 'DiHadron'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmation()]+companion()+dihadron()+vafterburn()
                 )

        # Soft DiHadron Line
        #-----------------------------------
        Line('DiHadronSoft'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_DECISION"
             , algos = [confirmation('Soft')]+companion('Soft')+dihadron('Soft')+vafterburn('Soft')
             )
