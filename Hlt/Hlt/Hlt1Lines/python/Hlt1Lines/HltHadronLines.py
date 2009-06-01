# =============================================================================
# $Id: HltHadronLines.py,v 1.3 2009-06-01 20:35:08 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $"
# =============================================================================

from Gaudi.Configuration import * 

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line   as Line
from HltLine.HltLine import bindMembers
from HltLine.HltLine import Hlt1Member as Member
from HltLine.HltLine import Hlt1Tool   as Tool
from HltLine.HltLine import hlt1Lines  
from Hlt1Lines.HltL0Candidates import *
from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit

def histosfilter(name,xlower=0.,xup=100.,nbins=100):
    """ return the dictonary with the booking of the histograms associated to a filter
    @param filter name i.e 'PT
    """
    histosfs = { name : ( name,xlower,xup,nbins),
                 name+"Best" : (name+"Best",xlower,xup,nbins) }
    return histosfs


class HltHadronLinesConf(HltLinesConfigurableUser) :
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
                , 'HadMain_IPCut'           : 0.1
                , 'HadMain_PTCut'           : 2500.
                , 'HadMain_TrackFitChi2Cut' : 10.
                , 'HadCompanion_DOCACut'    : 0.2
                , 'HadCompanion_DZCut'      : 1.5
                , 'HadCompanion_IPCut'      : 0.1
                , 'HadCompanion_PTCut'      : 1000.
                , 'HadCompanion_PointingCut': 0.4
                , 'SingleHadron_PTCut'      : 5000.
                , 'SoftHadMain_ETCut'       : 2500. 
                , 'SoftHadMain_PTCut'       : 1500.
                , 'SoftHadMain_IPCut'       : 0.1
                , 'Prescale'                : { 'Hlt1SoftDiHadron' : 0 }
                }
    
    def __apply_configuration__(self) : 
        from HltConf.HltReco import RZVelo, PV2D

        ## alias to get the slot associated to a name
        _cut = lambda x: str(self.getProp(x))
        
        # confirmed track
        #------------------------
        def confirmation(type=""):
            candidates = _cut("L0Channel")
            L0candidates = "Hlt1L0"+candidates+"Decision"
            if (type=="Soft"):
                candidates = "All"+candidates
                L0candidates = "%TFL0Hadron"
            IPCut = _cut(type+"HadMain_IPCut")
            PTCut = _cut(type+"HadMain_PTCut")
            # get the L0 candidates (all or L0)
            conf = [convertL0Candidates(candidates)]
            if (type == "Soft"):
                conf.append(
                    Member('TF','L0Hadron'
                           , FilterDescriptor = ["L0ET,>,"+_cut("SoftHadMain_ETCut")]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('L0ET',1500.,5100.,200)
                           )
                    )    
            conf += [ RZVelo
                    , PV2D.ignoreOutputSelection()
                    , Member ( 'TF' ,'RZVelo'
                           , FilterDescriptor = ['Calo2DChi2_%s,<,4'%L0candidates ]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('Calo2DChi2_'+L0candidates,0.,10.,200)
                           )
                    , Member ( 'TU', 'Velo',  RecoName = 'Velo')
                    , Member ( 'TF', '1Velo',
                               FilterDescriptor = [ 'Calo3DChi2_%s,<,4'%L0candidates ],
                               HistogramUpdatePeriod = 1,
                               HistoDescriptor  = histosfilter('Calo3DChi2_'+L0candidates,0.,10.,200)
                               )
                    , Member ( 'TF', '2Velo',
                           FilterDescriptor = [ 'IP_PV2D,||>,'+IPCut],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor = histosfilter('IP_PV2D',-0.2,1.8,200)
                           )
                    , Member ( 'TM' , 'VeloCalo'
                               , InputSelection1 = '%TF2Velo'
                               , InputSelection2 = L0candidates
                               , MatchName = 'VeloCalo' , MaxQuality = 4.
                               )
                    , Member ( 'TU', 'GuidedForward'
                               , RecoName = 'GuidedForward'
                               , HistogramUpdatePeriod = 1
                               )
                    , Member ( 'TF' , 'Confirmed' ,
                               FilterDescriptor = ['PT,>,'+PTCut],
                               HistogramUpdatePeriod = 1,
                               HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                               )
                    ]
            return conf
        
        # simple hadron cut
        #---------------
        def singlehadron():
            sh = [ Member ( 'TF' , 'SingleHadron' ,
                            FilterDescriptor = ['PT,>,'+_cut('SingleHadron_PTCut')],
                            HistogramUpdatePeriod = 1,
                            HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                            )
                   ]
            return sh


        # companion track - dihadron part
        #------------------------------------
        def companion(type=""):
            IP2Cut = _cut(type+"HadCompanion_IPCut")
            comp = [ RZVelo , PV2D.ignoreOutputSelection()
                , Member ( 'TU', 'UVelo' , RecoName = 'Velo')
                , Member ( 'TF', '1UVelo'
                           , FilterDescriptor = ['MatchIDsFraction_%TFConfirmed,<,0.9' ]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('MatchIDsFraction_%TFGuidedForward',0.,8000.,200)
                           )
                , Member ( 'TF', 'Companion'
                           , FilterDescriptor = [ 'IP_PV2D,||>,'+IP2Cut]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('IP_PV2D',-0.3,30.,200)
                           )
                ]
            return comp

        #
        # dihadron part
        #---------------------
        def dihadron(type=""):
            PT2Cut = _cut(type+"HadCompanion_PTCut")
            dih = [ PV2D.ignoreOutputSelection()
                , Member ( 'VM2', 'UVelo'
                         , InputSelection1 = '%TFConfirmed'
                         , InputSelection2 = '%TFCompanion'
                         , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('HadCompanion_DOCACut'))]
                         , HistoDescriptor  = histosfilter('DOCA',0.,1.,200)
                           )
                , Member ( 'VF', 'UVelo'
                           , FilterDescriptor = [ 'VertexDz_PV2D,>,'+_cut('HadCompanion_DZCut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('VertexDx_PV2D',1.,12.,200)                       
                           )
                , Member ( 'VU', 'Forward', RecoName = 'Forward')
                , Member ( 'VF', '1Forward',
                           FilterDescriptor = [ 'VertexMinPT,>,'+PT2Cut],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT',0.,5000.,200)                       
                           )
                , Member ( 'VF', 'DiHadron',
                           FilterDescriptor = [ 'VertexPointing_PV2D,<,'+_cut("HadCompanion_PointingCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexPointing_PV2D',0.1,1.1,200)                       
                           )
                ]
            return dih
        
        # afterburn of a line with tracks
        #--------------------------------
        def afterburn():
            after = [ PV2D.ignoreOutputSelection()
                , Member ( 'TU' , 'FitTrack' , RecoName = "FitTrack")
                , Member ( 'TF' , '1FitTrack' ,
                           FilterDescriptor = ["FitIP_PV2D,||>,"+_cut('HadMain_IPCut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitIP_PV2D',-.3,3.,200)
                           )
                , Member ( 'TF' , '2FitTrack'
                           , OutputSelection = '%Decision'
                           , FilterDescriptor = ["FitChi2OverNdf,<,"+_cut('HadMain_TrackFitChi2Cut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitChi2OverNdf',0.,100.,100)
                           )
                ]
            return after

        # afterburn of a line with vertices
        #-------------------
        def vafterburn(type=""):
            IPCut = _cut(type+'HadMain_IPCut')
            vafter =  [ PV2D.ignoreOutputSelection()
                , Member ( 'VU', 'FitTrack',   RecoName = 'FitTrack')
                , Member ( 'VF', '1FitTrack',
                           FilterDescriptor = [ 'FitVertexMinIP_PV2D,||>,'+IPCut],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitVertexMinIP_PV2D',-0.3,3.,200)                       
                           )
                , Member ( 'VF', '2FitTrack'
                           , OutputSelection = "%Decision"
                           , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,'+_cut('HadMain_TrackFitChi2Cut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('FitVertexMaxChi2OverNdf',0,50.,200)
                           )
                ]
            return vafter
            
        # Single Hadron Line
        #-----------------------------------
        Line ( 'SingleHadron'
               , prescale = self.prescale
               , postscale = self.postscale
               , L0DU  = "L0_CHANNEL('"+_cut("L0Channel")+"')"
               , algos = confirmation()+singlehadron()+afterburn()
               )
        setupHltFastTrackFit('Hlt1SingleHadronTUFitTrack')

        # DiHadron Line
        #-----------------------------------
        Line ('DiHadron'
              , prescale = self.prescale
              , postscale = self.postscale
              , L0DU  = "L0_CHANNEL('"+_cut("L0Channel")+"')"
              , algos =  confirmation()+companion()+dihadron()+vafterburn()
              )
        setupHltFastTrackFit('Hlt1DiHadronVUFitTrack')

        # Soft DiHadron Line
        #-----------------------------------
        Line('SoftDiHadron'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_ALL"
             , algos = confirmation('Soft')+companion()+dihadron()+vafterburn()
             )
        setupHltFastTrackFit('Hlt1SoftDiHadronVUFitTrack')
