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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.34 $"
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
        pass
