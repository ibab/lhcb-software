# $Id$

__author__ = 'Sebastian Bachmann'
__date__ = '21/10/2010'
__version__ = '$Revision$'

'''
Tag-and-probe strip of events with two D0 canidates, based on
Bd->Dstar mu nu loose stripping selection using LoKi::Hybrid and python
configurables.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingUtils.Utils import checkConfig
from Configurables import FilterDesktop, CombineParticles

import GaudiKernel.SystemOfUnits as Units



class StrippingX2D0D0Conf(object):
    """
    Definition of LOOSE Bd->D*MuNu stripping. 
    """
    __configuration_keys__ = (
        'pDaugPt'                   # MeV/c
        , 'pDaugP'                    # MeV/c
        , 'pDaugTrkChi2'              # unitless
        , 'pDaugPIDK'                 # unitless
        , 'pDaugPIDpi'                # unitless
        , 'pD0Pt'                      # GeV
        , 'pD0VtxChi2Ndof'            # unitless
        , 'tDaugPt'                   # MeV/c
        , 'tDaugP'                    # MeV/c
        , 'tDaugTrkChi2'              # unitless
        , 'tDaugPIDK'                 # unitless
        , 'tDaugPIDpi'                # unitless
        , 'tDaugIpChi2'                 # unitless
        , 'tD0Pt'                      # GeV
        , 'tD0VtxChi2Ndof'            # unitless
        , 'tD0IpChi2'                 # unitless
        # this cut is only for the tagging D0
        , 'D0Dira'                   # unitless
        , 'D0MassWindow'             # MeV/cc
        )
    
    config_X2D0D0 = {
        #                  'DaugPt'               : 900      # MeV
        #                , 'DaugP'                : 5000     # MeV
        #                , 'DaugTrkChi2'          : 5        # unitless
        'pDaugPt'                : 280      # MeV    (old 250)
        , 'pDaugP'                 : 900     # MeV   (old 800)
        , 'pDaugTrkChi2'           : 9        # unitless  (old 16)
        , 'pDaugPIDK'              : 6        # unitless   (old 0 )
        , 'pDaugPIDpi'             : 0       # unitless   (old 0)
        , 'pD0Pt'                  : 900     # MeV        (old 1000)
        , 'pD0VtxChi2Ndof'         : 12        # unitless  (old 16) 
        , 'tDaugPt'                : 900      # MeV  (old 900)
        , 'tDaugP'                 : 2000     # MeV  (old 4000)
        , 'tDaugTrkChi2'           : 4        # unitless (old 9)
        , 'tDaugPIDK'              : 8        # unitless (old 8)
        , 'tDaugPIDpi'             : 5        # unitless (old 5)
        , 'tDaugIpChi2'            : 16        # unitless (old 4)
        , 'tD0Pt'                   : 1000     # MeV  (old 3300)
        , 'tD0VtxChi2Ndof'         : 4        # unitless (old 13)
        , 'tD0IpChi2'              : 4       # unitless (0ld 9)
        , 'D0Dira'                : 0.9995      # unitless (old 0.9995)
        , 'D0MassWindow'          : 120      # MeV (old 16MeV)
        #
        # ---> Pseudo cut on pseudo mother particle
        #
        }
   

    def __init__( self, prefix = "" ):
	self.prefix = prefix
 
    def line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        config = StrippingX2D0D0Conf.config_X2D0D0
        checkConfig(StrippingX2D0D0Conf.__configuration_keys__, config)
#        self.Bd2DstarDstarLine = self.X2D0D0(config)
        self.lines = [self.X2D0D0(config)]
	return StrippingLine(self.prefix + "X2D0D0"
              , prescale = 1
              , postscale = 1
              , algos = self.lines
                             )
     	
    def X2D0D0( self, config):
        #        config = StrippingX2D0D0Conf.config_D0D0
        tKaon_DC = "(ISLONG) & (PT>%(tDaugPt)s) & (P>%(tDaugP)s) & (TRCHI2DOF<%(tDaugTrkChi2)s) & (PIDK - PIDpi >%(tDaugPIDK)s) & (BPVIPCHI2()>%(tDaugIpChi2)s)" % config
        tPion_DC = "(ISLONG) & (PT>%(tDaugPt)s) & (P>%(tDaugP)s) & (TRCHI2DOF<%(tDaugTrkChi2)s) & (PIDpi - PIDK >%(tDaugPIDpi)s) & (BPVIPCHI2()>%(tDaugIpChi2)s)" % config
        pKaon_DC = "(ISLONG) & (PT>%(pDaugPt)s) & (P>%(pDaugP)s) & (TRCHI2DOF<%(pDaugTrkChi2)s) & (PIDK - PIDpi >%(pDaugPIDK)s) " % config
        pPion_DC = "(ISLONG) & (PT>%(pDaugPt)s) & (P>%(pDaugP)s) & (TRCHI2DOF<%(pDaugTrkChi2)s) & (PIDpi - PIDK >%(pDaugPIDpi)s) " % config
        D02Kpi_CC = "(APT>900) & (ADAMASS('D0') < %(D0MassWindow)s)" % config
        tD02Kpi_MC = "(PT>%(tD0Pt)s) & (VFASPF(VCHI2PDOF)<%(tD0VtxChi2Ndof)s) & (BPVIPCHI2()<%(tD0IpChi2)s) & (BPVDIRA> %(D0Dira)s)" % config
        pD02Kpi_MC = "(PT>%(pD0Pt)s) & (VFASPF(VCHI2PDOF)<%(pD0VtxChi2Ndof)s) " % config
        


        pD2Kpi = CombineParticles(
            self.prefix + "pD2Kpi"
            , InputLocations = ["StdTightKaons", "StdTightPions"]
            , DecayDescriptor = '[D0 -> K- pi+]cc'
            , DaughtersCuts = {"K-": pKaon_DC, "pi+": pPion_DC}
            , CombinationCut = D02Kpi_CC
            , MotherCut = pD02Kpi_MC
            )    

        tD2Kpi = CombineParticles(
            self.prefix + "tD2Kpi"
            , InputLocations = ["StdTightKaons", "StdTightPions"]
            , DecayDescriptor = '[D0 -> K- pi+]cc'
            , DaughtersCuts = {"K+": tKaon_DC, "pi-": tPion_DC}
            , CombinationCut = D02Kpi_CC
            , MotherCut = tD02Kpi_MC
            )    

        tagAndProbe = CombineParticles(
            self.prefix + "tpD2Kpi"
            , InputLocations = [ self.prefix + "pD2Kpi", self.prefix + "tD2Kpi" ]
            , DecayDescriptors = [ 'B0 -> D0 D0', 'B0 -> D0 D~0', 'B0 -> D~0 D~0' ]
            , CombinationCut = "(AP>0)"
            , MotherCut = "(P>0)"
            )


	combine = FilterDesktop(self.prefix + "CombineX")
	combine.InputLocations = [self.prefix + "tD2Kpi", self.prefix + "pD2Kpi"]
	combine.Code = "(ALL)"

	selseq = GaudiSequencer(self.prefix + "X2D0D0SelSequence")
	selseq.Members += [tD2Kpi, pD2Kpi, tagAndProbe, combine]
	selseq.ModeOR = False
	selseq.ShortCircuit = True
	selseq.IgnoreFilterPassed = False


        return selseq

