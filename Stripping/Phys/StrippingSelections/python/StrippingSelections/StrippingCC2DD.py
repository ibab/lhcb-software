# $Id: StrippingCC2DD.py, v 0.1 2014-01-03  $
'''
Module for construction of CC->D D, where CC is a particle decaying to D D
                                    and D is a D0(Kpi) or a D+(K-pi+pi+)
                                    or one of their antipartciles
'''

__author__ = ['Lucio Anderlini, Andrea Bizzeti']
__date__ = '2014-01-03'
__version__ = '$Revision: 0.1 $'

__all__ = ('CC2DDConf',
           'makeD02HH',
           'makeDp2HHH')


config_default =  {'D0MassWin'     : "60*MeV",
                   'D0PT'          : "0*MeV",
                   'D0VtxChi2Ndof' : 10,
                   'D0Bpvdira'     : -10.,
                   'D0Bpvdls'      : 4.,
###                   'D0daughterMinIpChi2'    : 3.,   # identical to BpvIPChi2 ???
                   'D0daughterBpvIpChi2'    : 3.,
                   'D0daughterPT'           : "500*MeV",
                   'D0daughterP'            : "5*GeV",
                   'D0daughterTrkChi2'      : 3,
                   'D0daughterTrkGhostProb' : 0.3,
                   'D0daughterKaonPIDK'     : 5,
                   'D0daughterPionPIDK'     : 10,
############################
                   'DpmMassWin'     : "60*MeV",
                   'DpmPT'          : "0*MeV",
                   'DpmVtxChi2Ndof' : 10,
                   'DpmBpvdira'     : -10.,
                   'DpmBpvdls'      : 4.,
                   'DpmdaughterBpvIpChi2'    : 3.,
                   'DpmdaughterPT'           : "500*MeV",
                   'DpmdaughterP'            : "5*GeV",
                   'DpmdaughterTrkChi2'      : 3,
                   'DpmdaughterTrkGhostProb' : 0.3,
                   'DpmdaughterKaonPIDK' : 5,
                   'DpmdaughterPionPIDK' : 10,
#                   'CCMassCut' : "(AM<5000*MeV)",
# no mass constraint
                   'CCMassCut' : "(AM>0)",
                   'CCVtxChi2Ndof' : 10,
                   }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class CC2DDConf(LineBuilder) :

    __configuration_keys__ = ('D0MassWin',
                              'D0PT',
                              'D0VtxChi2Ndof',
                              'D0Bpvdira',
                              'D0Bpvdls',
                              'D0daughterPT',
                              'D0daughterP',
                              'D0daughterTrkChi2',
                              'D0daughterTrkGhostProb',
                              'D0daughterBpvIpChi2',
                              'D0daughterKaonPIDK',
                              'D0daughterPionPIDK',
####
                              'DpmMassWin',
                              'DpmPT',
                              'DpmVtxChi2Ndof',
                              'DpmBpvdira',
                              'DpmBpvdls',
                              'DpmdaughterBpvIpChi2',
                              'DpmdaughterPT',
                              'DpmdaughterP',
                              'DpmdaughterTrkChi2',
                              'DpmdaughterTrkGhostProb',
                              'DpmdaughterKaonPIDK',
                              'DpmdaughterPionPIDK',
###
                              'CCMassCut',
                              'CCVtxChi2Ndof',
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        self.selD02HH = makeD02HH( 'D0For'+name, 
                                     D0MassWin     = config['D0MassWin'],
                                     D0PT          = config['D0PT'],
                                     D0VtxChi2Ndof = config['D0VtxChi2Ndof'],
                                     D0Bpvdira     = config['D0Bpvdira'],
                                     D0Bpvdls      = config['D0Bpvdls'],
                                     D0daughterPT       = config['D0daughterPT'],
                                     D0daughterP        = config['D0daughterP'],
                                     D0daughterTrkChi2  = config['D0daughterTrkChi2'],
                                     D0daughterTrkGhostProb = config['D0daughterTrkGhostProb'],
                                     D0daughterBpvIpChi2    = config['D0daughterBpvIpChi2'],
                                     D0daughterKaonPIDK = config['D0daughterKaonPIDK'],
                                     D0daughterPionPIDK = config['D0daughterPionPIDK'],
                                   )

        self.selDp2HHH = makeDp2HHH( 'DpFor'+name, 
                                     DpmMassWin     = config['DpmMassWin'],
                                     DpmPT          = config['DpmPT'],
                                     DpmVtxChi2Ndof = config['DpmVtxChi2Ndof'],
                                     DpmBpvdira     = config['DpmBpvdira'],
                                     DpmBpvdls      = config['DpmBpvdls'],
                                     DpmdaughterPT           = config['DpmdaughterPT'],
                                     DpmdaughterP            = config['DpmdaughterP'],
                                     DpmdaughterTrkChi2      = config['DpmdaughterTrkChi2'],
                                     DpmdaughterTrkGhostProb = config['DpmdaughterTrkGhostProb'],
                                     DpmdaughterBpvIpChi2    = config['DpmdaughterBpvIpChi2'],
                                     DpmdaughterKaonPIDK     = config['DpmdaughterKaonPIDK'],
                                     DpmdaughterPionPIDK     = config['DpmdaughterPionPIDK'],
                                   )

        self.selCC2DD = makeCC2DD(name,  
                                  D0Sel    = self.selD02HH,
                                  DplusSel = self.selDp2HHH,
                                  CCMassCut     = config['CCMassCut'],
                                  CCVtxChi2Ndof = config['CCVtxChi2Ndof'],
                                 )

        self.myLine = StrippingLine(name+"Line",
                                            prescale = 1.,
                                            postscale = 1.,
                                            selection = self.selCC2DD
                                            )

        self.registerLine(self.myLine)


def makeD02HH(name, D0MassWin, D0PT, D0VtxChi2Ndof, D0Bpvdira, D0Bpvdls, D0daughterPT, D0daughterP, D0daughterTrkChi2,
                    D0daughterTrkGhostProb, D0daughterBpvIpChi2, D0daughterKaonPIDK, D0daughterPionPIDK ) :
    """
    Create and return a D0 -> K pi 
    """

    _motherCuts = "(ADMASS('D0')<%(D0MassWin)s) & (PT>%(D0PT)s) & (VFASPF(VCHI2PDOF)<%(D0VtxChi2Ndof)s) " % locals()
    _motherCuts += "& (BPVDIRA>%(D0Bpvdira)s) & (BPVDLS>%(D0Bpvdls)s)" % locals()
    _daughtersCuts = "(PT> %(D0daughterPT)s) & (P>%(D0daughterP)s) & (TRCHI2DOF<%(D0daughterTrkChi2)s) " % locals()
    _daughtersCuts += "& (TRGHP<%(D0daughterTrkGhostProb)s) & (BPVIPCHI2()>%(D0daughterBpvIpChi2)s)" % locals()
    _KCutsPIDK     = "&(PIDK>%(D0daughterKaonPIDK)s)" % locals()
    _PiCutsPIDK    = "&(PIDK<%(D0daughterPionPIDK)s)" % locals()
    _KCuts    = _daughtersCuts + _KCutsPIDK
    _PiCuts   = _daughtersCuts + _PiCutsPIDK
    _D0Filter = CombineParticles( DecayDescriptor = "[D0 -> K- pi+]cc",
                                  MotherCut = _motherCuts,
                                  DaughtersCuts =  { "pi+" : _PiCuts, "K+" : _KCuts }
                                  )

    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")

    return Selection (name,
                      Algorithm = _D0Filter,
                      RequiredSelections = [_stdTightKaons, _stdTightPions])


def makeDp2HHH(name, DpmMassWin, DpmPT, DpmVtxChi2Ndof, DpmBpvdira, DpmBpvdls, DpmdaughterBpvIpChi2, DpmdaughterKaonPIDK, DpmdaughterPionPIDK,
                     DpmdaughterPT, DpmdaughterP, DpmdaughterTrkChi2, DpmdaughterTrkGhostProb ) :
    """
    Create and return a D+ -> K- pi+ pi+
    """

    _DpmotherCuts  = "(ADMASS('D+')<%(DpmMassWin)s) & (PT> %(DpmPT)s) & (VFASPF(VCHI2PDOF) < %(DpmVtxChi2Ndof)s) " % locals()
    _DpmotherCuts += "& (BPVDIRA>%(DpmBpvdira)s) & (BPVDLS > %(DpmBpvdls)s)" % locals()
    _DpdaughtersCuts  = "(PT>%(DpmdaughterPT)s) & (P>%(DpmdaughterP)s) & (TRCHI2DOF<%(DpmdaughterTrkChi2)s) " % locals()
    _DpdaughtersCuts += "& (TRGHP<%(DpmdaughterTrkGhostProb)s) & (BPVIPCHI2()>%(DpmdaughterBpvIpChi2)s)" % locals()
    _DpKCutsPIDK  = "&(PIDK>%(DpmdaughterKaonPIDK)s)" % locals()
    _DpPiCutsPIDK = "&(PIDK<%(DpmdaughterPionPIDK)s)" % locals()
    _DpKCuts  = _DpdaughtersCuts + _DpKCutsPIDK
    _DpPiCuts = _DpdaughtersCuts + _DpPiCutsPIDK

    _DplusFilter = CombineParticles( DecayDescriptor = "[D+ -> K- pi+ pi+]cc",
                                  MotherCut = _DpmotherCuts,
                                  DaughtersCuts =  { "pi+" : _DpPiCuts, "K+" : _DpKCuts }
                                  )

    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")

    return Selection (name,
                      Algorithm = _DplusFilter,
                      RequiredSelections = [_stdTightKaons, _stdTightPions])


def makeCC2DD(name,
                   D0Sel,
                   DplusSel,
                   CCMassCut,
                   CCVtxChi2Ndof
                   ) :
    """
    Create and return a X -> DD  Selection Object, with  D = D0(Kpi) or Dp(Kpipi)
    Arguments:
    name          : name of the Selection.
    D0Sel         : D0 -> HH Selection object.
    CCMassCut     : CC invariant mass cuts (not used)
    CCVtxChi2Ndof : CC vertex Chi2/Ndof cut
    """

    ###_motherCuts = "(ALL)" 
    _motherCuts = "(VFASPF(VCHI2PDOF)<%(CCVtxChi2Ndof)s)" % locals()
    _combinationCuts = CCMassCut
    #print 'makeBs2JpsiPhi', name, 'MotherCuts:', _motherCuts
    _X = CombineParticles( DecayDescriptors = [ "psi(3770) -> D0 D~0",
                                                "psi(3770) -> D+ D-",
                                               "[psi(3770) -> D0 D0]cc",
                                               "[psi(3770) -> D+ D+]cc", 
                                               "[psi(3770) -> D0 D+]cc",
                                               "[psi(3770) -> D0 D-]cc",
                                               ],
                           MotherCut = _motherCuts,
                           CombinationCut = _combinationCuts
                           )

    DSel = MergedSelection ( name + "MergedDSelection",
                             RequiredSelections = [D0Sel, DplusSel]
                           )

    return Selection ( name,
                       Algorithm = _X,
                       RequiredSelections = [DSel])

