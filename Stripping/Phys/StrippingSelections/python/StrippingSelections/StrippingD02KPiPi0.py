
__author__ = 'Regis Lefevre'
__date__ = '2012.05.02'
__version__ = '$Revision: 1.4 $'

'''
Stripping selection for D0 -> K pi pi0 
'''
####################################################################
# Stripping selection for D0 -> K pi pi0 
#  2 lines : one for merged, one for resolved pi0
####################################################################

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdTightPions,StdTightKaons,StdLooseMergedPi0,StdLooseResolvedPi0

class StrippingD02KPiPi0Conf(LineBuilder) :

    __configuration_keys__ = ( 'TrackMinPT'
                               ,'TrackMinPT_R'
                               ,'TrackMinTrackProb'
                               ,'TrackMinIPChi2'
                               ,'Pi0MinPT_M'
                               ,'Pi0MinPT_R'
                               ,'ResPi0MinGamCL'
                               ,'D0MinM'
                               ,'D0MaxM'
                               ,'D0MinVtxProb'
                               ,'D0MaxIPChi2'
                               ,'D0MinDIRA'
                               ,'D0MinVVDChi2'
                               ,'MergedLinePrescale'
                               ,'MergedLinePostscale'
                               ,'ResolvedLinePrescale'
                               ,'ResolvedLinePostscale'
                               )

##############################################################
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        myPions       = StdTightPions
        myKaons       = StdTightKaons
	myMergedPi0   = StdLooseMergedPi0
	myResolvedPi0 = StdLooseResolvedPi0

        #---------------------------------------
        # B -> HHPi0 selections
	self.selresolved = makeD02KPiPi0R( name + 'R',
                                           config,
                                           DecayDescriptor = '[D0 -> K- pi+ pi0]cc',
                                           inputSel = [myKaons, myPions, myResolvedPi0]
                                         )				       
	self.selmerged = makeD02KPiPi0M( name + 'M',
                                         config,
                                         DecayDescriptor = '[D0 -> K- pi+ pi0]cc',
                                         inputSel = [myKaons, myPions, myMergedPi0]
                                         )
        #---------------------------------------
        # Stripping lines
        self.D02KPiPi0R_line = StrippingLine(name + "_R" %locals()['config'],
                                             prescale = config['ResolvedLinePrescale'],
                                             postscale = config['ResolvedLinePostscale'],
                                             selection = self.selresolved
                                             )
        self.D02KPiPi0M_line = StrippingLine(name + "_M" %locals()['config'],
                                             prescale = config['MergedLinePrescale'],
                                             postscale = config['MergedLinePostscale'],
                                             selection = self.selmerged
                                             )
        # register lines
        self.registerLine(self.D02KPiPi0R_line)
        self.registerLine(self.D02KPiPi0M_line)

##############################################################
def makeD02KPiPi0R( name,
                  config,
                  DecayDescriptor,
                  inputSel
                  ) :

    _TrackCuts    = "(PT>%(TrackMinPT_R)s *MeV) & (TRPCHI2>%(TrackMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(TrackMinIPChi2)s)" %locals()['config']    
    _pi0Cuts      = "(PT>%(Pi0MinPT_R)s *MeV) & (CHILD(CL,1)>%(ResPi0MinGamCL)s) & (CHILD(CL,2)>%(ResPi0MinGamCL)s)" %locals()['config']
    _daughterCuts = { 'K-' : _TrackCuts, 'pi+' : _TrackCuts, 'pi0' : _pi0Cuts }
    _combCuts     = "(AM>%(D0MinM)s *MeV) & (AM<%(D0MaxM)s *MeV)" % locals()['config']
    _motherCuts   = "(VFASPF(VPCHI2)>%(D0MinVtxProb)s) & (BPVVDCHI2>%(D0MinVVDChi2)s) & (BPVIPCHI2()<%(D0MaxIPChi2)s) & (BPVDIRA>%(D0MinDIRA)s)" % locals()['config']

    _D = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts
                           )

    return Selection( name+'Sel',
                      Algorithm = _D,
                      RequiredSelections = inputSel
                      )
##############################################################
def makeD02KPiPi0M( name,
                  config,
                  DecayDescriptor,
                  inputSel
                  ) :

    _TrackCuts    = "(PT>%(TrackMinPT)s *MeV) & (TRPCHI2>%(TrackMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(TrackMinIPChi2)s)" %locals()['config']    
    _pi0Cuts      = "(PT>%(Pi0MinPT_M)s *MeV)" %locals()['config']
    _daughterCuts = { 'K-' : _TrackCuts, 'pi+' : _TrackCuts, 'pi0' : _pi0Cuts }
    _combCuts     = "(AM>%(D0MinM)s *MeV) & (AM<%(D0MaxM)s *MeV)" % locals()['config']
    _motherCuts   = "(VFASPF(VPCHI2)>%(D0MinVtxProb)s) & (BPVVDCHI2>%(D0MinVVDChi2)s) & (BPVIPCHI2()<%(D0MaxIPChi2)s) & (BPVDIRA>%(D0MinDIRA)s)" % locals()['config']

    _D = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts
                           )

    return Selection( name+'Sel',
                      Algorithm = _D,
                      RequiredSelections = inputSel
                      )
##############################################################
