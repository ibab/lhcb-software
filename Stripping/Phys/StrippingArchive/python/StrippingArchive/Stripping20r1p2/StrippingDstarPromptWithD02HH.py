# $Id: StrippingDstarPromptWithD02HH.py,v 1.7 2010-09-04 22:12:02 pxing Exp $

__author__ = ['Philip Xing', 'Patrick Spradlin']
__date__ = '2011.07.22'
__version__ = '$Revision: 1.8 $'

__all__ = ( 'StrippingDstarPromptWithD02HHConf' )

'''
Configurable for the y_CP and A_Gamma analysis selections for D*+ -> pi+ D0(h h'').

The initial implementation as a configurable borrows from StrippingBu2LLK.py and StrippingD0hh.py.

Usage:
from StrippingSelections import StrippingDstarPromptWithD02HH
confTaggedD02hh = StrippingDstarPromptWithD02HH.StrippingDstarPromptWithD02HHConf("confTaggedD02hh", StrippingDstarPromptWithD02HH.default_config)
stream.appendLines( confTaggedD02hh.lines() )

Performances:
StrippingReport                                                INFO Event 50000, Good event 46049
 |                                    *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
 |_StrippingGlobal_                                  |  0.1672|        77|       |   7.691|       |       |       |
 |_StrippingSequenceStreamTest_                      |  0.1672|        77|       |   7.686|       |       |       |
 |!StrippingStripDstarPromptWithD02RSKPiLine         |  0.0608|        28|  0.000|   6.707|      0|      0|     61|
 |!StrippingStripDstarPromptWithD02WSKPiLine         |  0.0261|        12|  0.000|   0.182|      0|      0|      0|
 |!StrippingStripDstarPromptWithD02KKLine            |  0.0391|        18|  0.000|   0.162|      0|      0|      0|
 |!StrippingStripDstarPromptWithD02PiPiLine          |  0.0413|        19|  0.000|   0.168|      0|      0|      0|
 
===============================================================================================================
                                AlgorithmCorrelationsAlg.AlgorithmCorrelations
===============================================================================================================
    Algorithm                                    Eff.       1        2        3        4        5        6   
---------------------------------------------------------------------------------------------------------------
  1 StrippingGlobal                              0.167% |  ####### 100.000% 100.000% 100.000% 100.000% 100.000%
  2 StrippingSequenceStreamTest                  0.167% | 100.000%  ####### 100.000% 100.000% 100.000% 100.000%
  3 StrippingStripDstarPromptWithD02RSKPiLine    0.061% |  36.364%  36.364%  #######   0.000%   0.000%   0.000%
  4 StrippingStripDstarPromptWithD02WSKPiLine    0.026% |  15.584%  15.584%   0.000%  #######   0.000%   0.000%
  5 StrippingStripDstarPromptWithD02KKLine       0.039% |  23.377%  23.377%   0.000%   0.000%  #######   0.000%
  6 StrippingStripDstarPromptWithD02PiPiLine     0.041% |  24.675%  24.675%   0.000%   0.000%   0.000%  #######
===============================================================================================================

'''

##############################################################
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsPions
from GaudiKernel.SystemOfUnits import MeV, GeV, mm

default_config = { 'DaugPt'            : 0.9 * GeV,
                   'DaugP'             : 5. * GeV,
                   'DaugTrkChi2'       : 5.,         ## unitless
                   'D0Pt'              : 3.3 * GeV,
                   'D0MassWindowCentre': 1.865 * GeV,
                   'D0MassWindowWidth' : 0.075 * GeV,
                   'D0VtxChi2Ndof'     : 13,         ## unitless
                   'D0PVDisp'          : 0.9 * mm,
                   'SlowPiPt'          : 0.260 * GeV,
                   'DstarPt'           : 3.6 * GeV,
                   'DstarVtxChi2Ndof'  : 13,         ## unitless
                   'Pi_negPIDK_MIN'    : 5.0,        ## unitless
                   'K_PIDK_MIN'        : 8.0,        ## unitless
                   'RSLinePrescale'    : 1.,         ## unitless
                   'RSLinePostscale'   : 1.,         ## unitless
                   'WSLinePrescale'    : 1.,         ## unitless
                   'WSLinePostscale'   : 1.,         ## unitless
                   'HHLinePrescale'    : 1.,         ## unitless
                   'HHLinePostscale'   : 1.,         ## unitless
         }

class StrippingDstarPromptWithD02HHConf(LineBuilder) :

    __configuration_keys__ = ('DaugPt',
	     		      'DaugP',
	     		      'DaugTrkChi2',
	     		      'D0Pt',
	     		      'D0MassWindowCentre',
	     		      'D0MassWindowWidth',
	     		      'D0VtxChi2Ndof',  
                  	      'D0PVDisp',            
                  	      'SlowPiPt',         
                  	      'DstarPt',          
                  	      'DstarVtxChi2Ndof',            
                              'Pi_negPIDK_MIN',
                              'K_PIDK_MIN',
                  	      'RSLinePrescale',
               	              'RSLinePostscale',
               	              'WSLinePrescale',
                  	      'WSLinePostscale',
                  	      'HHLinePrescale',
                              'HHLinePostscale'
                              )

##############################################################
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        stdNoPIDsKaons = StdAllNoPIDsKaons
        stdNoPIDsPions = StdAllNoPIDsPions
        #---------------------------------------
        # D0 -> hh' selections
        self.selD0KPi = makeD2hh( name + 'D02RSKPi',
			         config,
				 DecayDescriptor = '[D0 -> K- pi+]cc',
			         inputSel = [stdNoPIDsPions, stdNoPIDsKaons]
			        )

        self.selD0KK = makeD2hh( name + 'D02KK',  
			         config,
				 DecayDescriptor = '[D0 -> K+ K-]cc',
			         inputSel = [stdNoPIDsKaons]
 			        )

        self.selD0PiPi = makeD2hh( name + 'D02PiPi',  
			           config,
				   DecayDescriptor = '[D0 -> pi+ pi-]cc',
			           inputSel = [stdNoPIDsPions]
			          )
        #---------------------------------------
        # Dstar -> D0 pi selections
	self.selDstRS = makeDstar2D0Pi( name + 'DstarWithD02RSKPi',
				        config,
                                        '[D*(2010)+ -> D0 pi+]cc',
                                        inputSel = [self.selD0KPi, stdNoPIDsPions]
                                       )

	self.selDstWS = makeDstar2D0Pi( name + 'DstarWithD02WSKPi',
				        config,
                                        '[D*(2010)- -> D0 pi-]cc',
                                        inputSel = [self.selD0KPi, stdNoPIDsPions]
                                       )

	self.selDstKK = makeDstar2D0Pi( name + 'DstarWithD02KK',
				        config,
                                        '[D*(2010)+ -> D0 pi+]cc',
                                        inputSel = [self.selD0KK, stdNoPIDsPions]
                                       )

	self.selDstPiPi = makeDstar2D0Pi( name + 'DstarWithD02PiPi',
				           config,
                                           '[D*(2010)+ -> D0 pi+]cc',
                                           inputSel = [self.selD0PiPi, stdNoPIDsPions]
                                         )
        #---------------------------------------
        # Tagged lines
        self.dstRS_line = StrippingLine(name +    "WithD02RSKPiLine" ,
                                        prescale = config['RSLinePrescale'],
                                        postscale = config['RSLinePostscale'],
                                        selection = self.selDstRS
                                       )

        self.dstWS_line = StrippingLine(name +    "WithD02WSKPiLine" ,
                                        prescale = config['WSLinePrescale'],
                                        postscale = config['WSLinePostscale'],
                                        selection = self.selDstWS
                                       )

        self.dstKK_line = StrippingLine(name +    "WithD02KKLine" ,
                                        prescale = config['HHLinePrescale'],
                                        postscale = config['HHLinePostscale'],
                                        selection = self.selDstKK
                                       )

        self.dstPiPi_line = StrippingLine(name +    "WithD02PiPiLine" ,
                                        prescale = config['HHLinePrescale'],
                                        postscale = config['HHLinePostscale'],
                                        selection = self.selDstPiPi
                                       )

        # register lines
        self.registerLine(self.dstRS_line)
        self.registerLine(self.dstWS_line)
        self.registerLine(self.dstKK_line)
        self.registerLine(self.dstPiPi_line)

##############################################################
def makeD2hh(name,
             config,
	     DecayDescriptor,
             inputSel
            ) :
    """
    Create and return a D0 -> hh' Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    """

    _PiCuts ="(PT>%(DaugPt)s)" \
             "& (P>%(DaugP)s)" \
             "& (TRCHI2DOF<%(DaugTrkChi2)s)" \
             "& (PIDpi-PIDK>%(Pi_negPIDK_MIN)s)" %locals()['config']
    _KCuts  ="(PT>%(DaugPt)s)" \
             "& (P>%(DaugP)s)" \
             "& (TRCHI2DOF<%(DaugTrkChi2)s)" \
             "& (PIDK-PIDpi>%(K_PIDK_MIN)s)" %locals()['config']
    _daughterCuts = { 'pi+' : _PiCuts, 'K-' : _KCuts }
    _combCuts = "(ADAMASS(%(D0MassWindowCentre)s) < %(D0MassWindowWidth)s)"  % locals()['config']
    _motherCuts = "(PT>%(D0Pt)s)" \
                  "& (VFASPF(VCHI2PDOF)<%(D0VtxChi2Ndof)s)" \
                  "& (BPVVD>%(D0PVDisp)s)" % locals()['config']

    _D0 = CombineParticles( DecayDescriptor = DecayDescriptor,
                            MotherCut = _motherCuts,
                            CombinationCut = _combCuts,
                            DaughtersCuts = _daughterCuts)

    return Selection ( name+'Sel',
                       Algorithm = _D0,
                       RequiredSelections = inputSel )

##############################################################
def makeDstar2D0Pi( name
                    , config
                    , DecayDescriptor
                    , inputSel
                  ) :
    """
    Create and return a D* -> D0 pi Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    """

    slowPiCuts = "(PT>%(SlowPiPt)s)" % locals()['config']
    d0Cuts =     "(ALL)"
    combCuts =  "(APT>%(DstarPt)s)" \
                "& (ADAMASS('D*(2010)+')<90*MeV)" % locals()['config']
    dstarCuts = "(VFASPF(VCHI2PDOF)<%(DstarVtxChi2Ndof)s)" \
                "& (M-MAXTREE('D0'==ABSID,M)<(145.5+15)*MeV)" % locals()['config']

    _Dstar = CombineParticles( DecayDescriptor = DecayDescriptor
                             , DaughtersCuts = { 'pi+' : slowPiCuts, 'D0' : d0Cuts }
                             , CombinationCut = combCuts
                             , MotherCut = dstarCuts
                             )

    return Selection( name+'Sel',
                      Algorithm = _Dstar,
                      RequiredSelections = inputSel
                    )

##############################################################
