
__author__ = 'Regis Lefevre'
__date__ = '2012.09.05'
__version__ = '$Revision: 1.6 $'

'''
Stripping selection for B -> h h pi0
'''
#################################################################
#  This strip is used both for Bd -> pi pi pi0 and Bs -> K pi pi0
#  B Mass window : 4200 to 6400 MeV/c2 
#  2 lines : one for merged, one for resolved pi0
#################################################################

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions,StdLooseMergedPi0,StdLooseResolvedPi0

default_config = {'PiMinPT'              : 500,
                  'PiMinP'               : 5000,
                  'PiMinTrackProb'       : 0.000001,
                  'PiMaxGhostProb'       : 0.5,
                  'PiMinIPChi2'          : 25,
                  'Pi0MinPT_M'           : 2500,
                  'Pi0MinPT_R'           : 1500,
                  'ResPi0MinGamCL'       : 0.2,
                  'BMinM'                : 4200,
                  'BMaxM'                : 6400,
                  'BMinPT_M'             : 3000,
                  'BMinPT_R'             : 2500,
                  'BMinVtxProb'          : 0.001,
                  'BMaxIPChi2'           : 9,
                  'BMinDIRA'             : 0.99995,
                  'BMinVVDChi2'          : 64,
                  'MergedLinePrescale'   : 1.,
                  'MergedLinePostscale'  : 1.,
                  'ResolvedLinePrescale' : 1.,
                  'ResolvedLinePostscale': 1.
                 }

class StrippingB2HHPi0Conf(LineBuilder) :

    __configuration_keys__ = ( 'PiMinPT'
                               ,'PiMinP'
                               ,'PiMinTrackProb'
                               ,'PiMaxGhostProb'
			       ,'PiMinIPChi2'
			       ,'Pi0MinPT_M'
			       ,'Pi0MinPT_R'
			       ,'ResPi0MinGamCL'
			       ,'BMinM'
			       ,'BMaxM'
			       ,'BMinPT_M'
			       ,'BMinPT_R'
			       ,'BMinVtxProb'
			       ,'BMaxIPChi2'
			       ,'BMinDIRA'
			       ,'BMinVVDChi2'
			       ,'MergedLinePrescale'
			       ,'MergedLinePostscale'
			       ,'ResolvedLinePrescale'
			       ,'ResolvedLinePostscale'
                               )

##############################################################
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        myPions       = StdNoPIDsPions
	myMergedPi0   = StdLooseMergedPi0
	myResolvedPi0 = StdLooseResolvedPi0

        #---------------------------------------
        # B -> HHPi0 selections
	self.selresolved = makeB2HHPi0R( name + 'R',
	                                 config,
                                         DecayDescriptor = 'B0 -> pi+ pi- pi0',
					 inputSel = [myPions, myResolvedPi0]
                                         )				       
        self.selmerged = makeB2HHPi0M( name + 'M',
	                               config,
                                       DecayDescriptor = 'B0 -> pi+ pi- pi0',
                                       inputSel = [myPions, myMergedPi0]
                                       )
        #---------------------------------------
        # Stripping lines
        self.B2HHPi0R_line = StrippingLine(name + "_R" %locals()['config'],
                                           prescale = config['ResolvedLinePrescale'],
                                           postscale = config['ResolvedLinePostscale'],
                                           selection = self.selresolved
                                           )
        self.B2HHPi0M_line = StrippingLine(name + "_M" %locals()['config'],
                                           prescale = config['MergedLinePrescale'],
                                           postscale = config['MergedLinePostscale'],
                                           selection = self.selmerged
                                           )
        # register lines
        self.registerLine(self.B2HHPi0R_line)
        self.registerLine(self.B2HHPi0M_line)

##############################################################
def makeB2HHPi0R( name,
                  config,
                  DecayDescriptor,
                  inputSel
                  ) :

    _piCuts = "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRPCHI2>%(PiMinTrackProb)s) & (TRGHOSTPROB<%(PiMaxGhostProb)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % locals()['config']
    _pi0Cuts = "(PT>%(Pi0MinPT_R)s *MeV) & (CHILD(CL,1)>%(ResPi0MinGamCL)s) & (CHILD(CL,2)>%(ResPi0MinGamCL)s)" % locals()['config']
    _daughterCuts = { 'pi+' : _piCuts, 'pi-' : _piCuts, 'pi0' : _pi0Cuts }
    _combCuts = "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % locals()['config']
    _motherCuts = "(PT>%(BMinPT_R)s *MeV) & (VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVVDCHI2>%(BMinVVDChi2)s) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s)" % locals()['config']

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts
                           )

    return Selection( name+'Sel',
                      Algorithm = _B,
                      RequiredSelections = inputSel
                      )
##############################################################
def makeB2HHPi0M( name,
                  config,
                  DecayDescriptor,
                  inputSel
                  ) :

    _piCuts = "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRPCHI2>%(PiMinTrackProb)s) & (TRGHOSTPROB<%(PiMaxGhostProb)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % locals()['config']
    _pi0Cuts = "(PT>%(Pi0MinPT_M)s *MeV)" % locals()['config']
    _daughterCuts = { 'pi+' : _piCuts, 'pi-' : _piCuts, 'pi0' : _pi0Cuts }
    _combCuts = "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % locals()['config']
    _motherCuts = "(PT>%(BMinPT_M)s *MeV) & (VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVVDCHI2>%(BMinVVDChi2)s) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s)" % locals()['config']

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts
                           )

    return Selection( name+'Sel',
                      Algorithm = _B,
                      RequiredSelections = inputSel
                      )
##############################################################
