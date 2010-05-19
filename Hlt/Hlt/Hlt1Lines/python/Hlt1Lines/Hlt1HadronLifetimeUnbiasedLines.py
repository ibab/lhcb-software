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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.4 $"
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
class Hlt1HadronLifetimeUnbiasedLinesConf(HltLinesConfigurableUser) :
    __slots__ = { 'L0Channel'                 :  "Hadron" 
                , 'HadMain_TrackFitChi2Cut'   :    10.
                , 'HadVertex_DOCACut'         :     0.2
                , 'HadVertex_PCut'            : 10000.  
                , 'HadVertex_MaxPTCut'        :  2000.
                , 'HadVertex_MinPTCut'        :  1000.
                , 'HadVertex_MassMinCut'      :  5000.0
                , 'HadVertex_MassMaxCut'      :  5800.0
                , 'HadVertex_CosThetaStarCut' :     0.9  
                , 'LTUnbHadETCut_Hard'        :  2500. 
                , 'LTUnbHadETCut_Soft'        :  1000.
                }
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Tool   as Tool
        from HltLine.HltLine import hlt1Lines  
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit
        from HltTracking.HltReco import RZVelo,Velo
        from HltTracking.HltPVs  import PV3D
        from Configurables import HltTrackUpgradeTool, PatForwardTool, HltGuidedForward
        from Hlt1Lines.HltConfigurePR import ConfiguredPR

        ## alias to get the slot associated to a name
        _cut = lambda x: str(self.getProp(x))

        
        # confirmed track
        #------------------------
        def confirmationl0part(type=""):
            prefix = 'HadronConfL0'+type
            from Hlt1Lines.HltL0Candidates import convertL0Candidates
            L0Channel = self.getProp("L0Channel")
            # get the L0 candidates (all or L0)
            thisETcut = 0
            if (type.find("Hard") > -1):
                thisETcut = self.getProp("LTUnbHadETCut_Hard")
            elif (type.find("Soft") > -1):
                thisETcut = self.getProp("LTUnbHadETCut_Soft")
            l0 = bindMembers(prefix, [ convertL0Candidates('All'+L0Channel)
                                         , Member('TF','L0Hadron'
                                                 , FilterDescriptor = ["L0ET,>,%s"%thisETcut]
                                                 , HistogramUpdatePeriod = 1
                                                 , HistoDescriptor  = histosfilter('L0ET',1500.,5100.,200)
                                                 )       
                                         ] )
            return l0

        def confirmation(type=""):
            OutputOfL0 = confirmationl0part(type).outputSelection()

            prefix = 'HadronConf'+type
            from HltLine.HltDecodeRaw import DecodeIT
            conf =                  [ Velo,
                                      PV3D().ignoreOutputSelection(),
                                      Member ( 'TM' , 'VeloCalo'
                                             , InputSelection1 = "Velo"   
                                             , InputSelection2 = '%s' %OutputOfL0
                                             , MatchName = 'VeloCalo' , MaxQuality = 4.
                                             ),
                                      DecodeIT,
                                      Member ( 'TU', 'GuidedForward',
                                               RecoName = 'GuidedForward',
                                               tools = [ Tool( HltTrackUpgradeTool
                                                               ,tools = [ConfiguredPR( "Forward" )] 
                                                             )
                                                       ]
                                             ) 
                                    ]
            return bindMembers(prefix, conf)
        
        #
        # dihadron part
        #---------------------
        def dihadron(type=""):
            prefix = "HadDiLTUnbiased"

            DocaCut                = _cut("HadVertex_DOCACut")
            VertexPCut             = _cut("HadVertex_PCut")
            MassMinCut             = _cut("HadVertex_MassMinCut")
            MassMaxCut             = _cut("HadVertex_MassMaxCut")
            CosThetaStarCut        = _cut("HadVertex_CosThetaStarCut")
            
            OutputOfConfirmationHard = confirmation("Hard").outputSelection()
            OutputOfConfirmationSoft = confirmation("Soft").outputSelection() 
            from HltLine.HltDecodeRaw import DecodeIT
            dih = [ Member ( 'VM2', 'UVelo'
                         , InputSelection1 = '%s' %OutputOfConfirmationHard 
                         , InputSelection2 = '%s' %OutputOfConfirmationSoft
                         , FilterDescriptor = [ 'DOCA,<,'           + DocaCut,
                                                'CosThetaStar,[],0,'+ CosThetaStarCut
                                                ]
                         , HistoDescriptor  = histosfilter('DOCA_'+type,0.,1.,200)
                           )
                , Member ( 'VF', 'DiHadronPT1',
                           FilterDescriptor = [ 'VertexMaxPT,>,%s'%self.getProp("HadVertex_MaxPTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMaxPT_'+type,0.,5000.,200)
                         ) 
                , Member ( 'VF', 'DiHadronPT2',
                           FilterDescriptor = [ 'VertexMinPT,>,%s'%self.getProp("HadVertex_MinPTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT_'+type,0.,5000.,200)                       
                           )
                , Member ( 'VF', 'VertexMinP',
                           FilterDescriptor = [ 'VertexMinP,>,'+VertexPCut]
                           )
                , Member ( 'VF', 'Mass',
                            FilterDescriptor = [ 'VertexDikaonMass,[],'+MassMinCut+','+MassMaxCut]
                           )
                ]
            return bindMembers(prefix,dih)
        
        # afterburn of a line with vertices
        #-------------------
        def vafterburn(type=""):
            OutputOfDiHadron = dihadron(type).outputSelection()

            vafter =  [ PV3D().ignoreOutputSelection()
                , Member ( 'VU', 'FitTrack', InputSelection='%s' %OutputOfDiHadron
                           , RecoName = 'FitTrack', callback = setupHltFastTrackFit )
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
            
            # DiHadron Line
            #-----------------------------------
            Line ( 'DiHadronLTUnbiased'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmationl0part('Soft')]+\
                            [confirmationl0part('Hard')]+\
                            [confirmation('Soft')]+\
                            [confirmation('Hard')]+\
                            [dihadron()]+\
                            vafterburn()
                 )
