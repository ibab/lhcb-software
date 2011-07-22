# $Id: StrippingX2D0D0.py,v 1.1 2011-05-25 12:53:17 Sebastian Bachmann $
'''
Module for construction of X->D0 D0(~), where X is a particle decaying to D0D0~, like the Psi(3770) resonance.
There is no constraint on X
'''

__author__ = ['Sebastian Bachmann']
__date__ = '07/22/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('X2D0D0Conf',
           'makeD02KK')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class X2D0D0Conf(LineBuilder) :

    __configuration_keys__ = ('D0MassWin',
                              'D0PT',
                              'D0VtxChi2Ndof',
                              'D0daughterPT',
                              'D0daughterP',
                              'D0daughterTrkChi2',
                              'D0daughterHighPIDK',
                              'D0daughterLowPIDK',
                              'XMassWin',
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        self.selD02HH = makeD02HH( 'D0For'+name, 
                                     D0MassWin = config['D0MassWin'],
                                     D0PT = config['D0PT'],
                                     D0VtxChi2Ndof = config['D0VtxChi2Ndof'],
                                     D0daughterPT = config['D0daughterPT'],
                                     D0daughterP = config['D0daughterP'],
                                     D0daughterTrkChi2 = config['D0daughterTrkChi2'],
                                     D0daughterHighPIDK = config['D0daughterHighPIDK'],
                                     D0daughterLowPIDK = config['D0daughterLowPIDK'],
                                   )
        
        self.selX2D0D0 = makeX2D0D0(name,  
                                    D0Sel = self.selD02HH,
                                    XMassWin = config['XMassWin'])

        self.myLine = StrippingLine(name+"Line",
                                            prescale = 1.,
                                            postscale = 1.,
                                            selection = self.selX2D0D0
                                            )

        self.registerLine(self.myLine)


def makeD02HH(name, D0MassWin, D0PT, D0VtxChi2Ndof, D0daughterPT, D0daughterP, D0daughterTrkChi2, D0daughterHighPIDK, D0daughterLowPIDK ) :
    """
    Create and return a D0 -> K pi 
    """

    _motherCuts = "(ADMASS('D0')<%(D0MassWin)s) & (PT> %(D0PT)s) & (VFASPF(VCHI2PDOF) < %(D0VtxChi2Ndof)s) " % locals()
    _daughtersCuts = "(PT> %(D0daughterPT)s) & (P>%(D0daughterP)s) & (TRCHI2DOF < %(D0daughterTrkChi2)s)" % locals()
    _KCutsPIDK = "&(PIDK>%(D0daughterHighPIDK)s)" % locals()
    _PiCutsPIDK = "&(PIDK<%(D0daughterLowPIDK)s)" % locals()
    _KCuts = _daughtersCuts + _KCutsPIDK
    _PiCuts = _daughtersCuts + _PiCutsPIDK

    _D0Filter = CombineParticles( DecayDescriptor = "[D0 -> K- pi+]cc",
                                  MotherCut = _motherCuts,
                                  DaughtersCuts =  { "pi+" : _PiCuts, "K+" : _KCuts }
                                  )


    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")

    return Selection (name,
                      Algorithm = _D0Filter,
                      RequiredSelections = [_stdTightKaons, _stdTightPions])

def makeX2D0D0(name,
                   D0Sel,
                   XMassWin
                   ) :
    """

    Create and return a X -> D0 (Kpi) D (Kpi) Selection object.
    Arguments:
    name        : name of the Selection.
    D0Sel       : D0 -> HH Selection object.
    XMassWin   : X invariant mass window (not used)
    """
    _motherCuts = "(ADMASS('B_s0')>%(XMassWin)s) " % locals()
    #print 'makeBs2JpsiPhi', name, 'MotherCuts:', _motherCuts
    _X = CombineParticles( DecayDescriptors = [ "B_s0 -> D0 D~0", "[B_s0 -> D0 D0]cc" ],
                           MotherCut = _motherCuts )

    return Selection ( name,
                       Algorithm = _X,
                       RequiredSelections = [D0Sel])
