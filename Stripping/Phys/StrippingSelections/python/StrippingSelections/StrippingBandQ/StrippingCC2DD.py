# $Id: StrippingCC2DD.py, v 0.1 2014-01-03  $
'''
Module for construction of CC->D D, where CC is a particle decaying to D D
                                    and D is a D0(Kpi) or a D+(K-pi+pi+)
                                    or one of their antipartciles
'''

__author__ = ['Lucio Anderlini','Andrea Bizzeti']
__date__ = '2014-01-17'
__version__ = '$Revision: 0.2f $'

__all__ = ('CC2DDConf',
           'makeD02HH',
           'makeDp2HHH',
           'makeDs2HHH',
           'default_config')


default_config =  {
    'NAME' : 'CC2DD',
    'WGs'  : ['BandQ'],
    'BUILDERTYPE'       : 'CC2DDConf',
    'CONFIG'   :  {
######## D0 / D~0 -> K pi cuts
                   'D0MassWin'     : "60*MeV",
                   'D0PT'          : "1000*MeV",
                   'D0VtxChi2Ndof' : 10,
                   'D0Bpvdira'     : -10.,
                   'D0Bpvdls'      : 4.,
                   'D0daughterBpvIpChi2'    : 4.,
                   'D0daughterPT'           : "600*MeV",
                   'D0daughterP'            : "5*GeV",
                   'D0daughterTrkChi2'      : 100000, #disabled
                   'D0daughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'D0daughterKaonProbNNk'  : 0.1,
                   'D0daughterPionProbNNpi' : 0.1,
######## Dplus/Dminus -> Kpipi cuts, used also for D_s+/D_s- ->KKpi
                   'DpmMassWin'     : "60*MeV",
                   'DpmPT'          : "1000*MeV",
                   'DpmVtxChi2Ndof' : 10,
                   'DpmBpvdira'     : -10.,
                   'DpmBpvdls'      : 4.,
                   'DpmdaughterBpvIpChi2'    : 4.,
                   'DpmdaughterPT'           : "500*MeV",
                   'DpmdaughterP'            : "5*GeV",
                   'DpmdaughterTrkChi2'      : 100000, #disabled
                   'DpmdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'DpmdaughterKaonProbNNk'  : 0.1,
                   'DpmdaughterPionProbNNpi' : 0.1,
######## psi(3779) -> D D  cuts
#                   'CCMassCut'     : "(AM<5000*MeV)",
# no mass constraint
                   'CCMassCut'      : "(AM>0)",
                   'CCVtxChi2Ndof'  : 10,
                   'CCMaxD0ChildPT' : "1500*MeV",
                   'CCMaxD0TreePT'  : "1200*MeV",
                   'CCMaxD0MinTreeIpChi2'   : "0.",    ## unused for the moment
                   },
    'STREAMS' : [
    'CharmCompleteEvent' 
    ],
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
###                              'D0daughterKaonPIDK',
###                              'D0daughterPionPIDK',
                              'D0daughterKaonProbNNk',
                              'D0daughterPionProbNNpi',
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
###                              'DpmdaughterKaonPIDK',
###                              'DpmdaughterPionPIDK',
                              'DpmdaughterKaonProbNNk',
                              'DpmdaughterPionProbNNpi',
####
                              'CCMassCut',
                              'CCVtxChi2Ndof',
                              'CCMaxD0ChildPT',
                              'CCMaxD0TreePT',  
                              'CCMaxD0MinTreeIpChi2',
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
###                                     D0daughterKaonPIDK     = config['D0daughterKaonPIDK'],
###                                     D0daughterPionPIDK     = config['D0daughterPionPIDK'],
                                     D0daughterKaonProbNNk  = config['D0daughterKaonProbNNk'],
                                     D0daughterPionProbNNpi = config['D0daughterPionProbNNpi'],
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
###                                     DpmdaughterKaonPIDK     = config['DpmdaughterKaonPIDK'],
###                                     DpmdaughterPionPIDK     = config['DpmdaughterPionPIDK'],
                                     DpmdaughterKaonProbNNk  = config['DpmdaughterKaonProbNNk'],
                                     DpmdaughterPionProbNNpi = config['DpmdaughterPionProbNNpi'],
                                   )

        self.selDs2HHH = makeDs2HHH( 'DsFor'+name, 
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
###                                     DpmdaughterKaonPIDK     = config['DpmdaughterKaonPIDK'],
###                                     DpmdaughterPionPIDK     = config['DpmdaughterPionPIDK'],
                                     DpmdaughterKaonProbNNk  = config['DpmdaughterKaonProbNNk'],
                                     DpmdaughterPionProbNNpi = config['DpmdaughterPionProbNNpi'],
                                   )

        self.selCC2DD = makeCC2DD(name,  
                                  D0Sel    = self.selD02HH,
                                  DplusSel = self.selDp2HHH,
                                  DsSel    = self.selDs2HHH,
                                  CCMassCut      = config['CCMassCut'],
                                  CCVtxChi2Ndof  = config['CCVtxChi2Ndof'],
                                  CCMaxD0ChildPT = config['CCMaxD0ChildPT'],
                                  CCMaxD0TreePT  = config['CCMaxD0TreePT'],
                                  CCMaxD0MinTreeIpChi2 = config['CCMaxD0MinTreeIpChi2'],
                                 )

        self.myLine = StrippingLine(name+"Line",
                                            prescale = 1.,
                                            postscale = 1.,
                                            selection = self.selCC2DD
                                            )

        self.registerLine(self.myLine)


def makeD02HH(name, D0MassWin, D0PT, D0VtxChi2Ndof, D0Bpvdira, D0Bpvdls,
                    D0daughterPT, D0daughterP, D0daughterTrkChi2,
                    D0daughterTrkGhostProb, D0daughterBpvIpChi2,
###                    D0daughterKaonPIDK, D0daughterPionPIDK,
                    D0daughterKaonProbNNk, D0daughterPionProbNNpi, 
              ) :

    """
    Create and return a D0 -> K pi 
    """

    _motherCuts = "(ADMASS('D0')<%(D0MassWin)s) & (PT>%(D0PT)s) & (VFASPF(VCHI2PDOF)<%(D0VtxChi2Ndof)s) " % locals()
    _motherCuts += "& (BPVDIRA>%(D0Bpvdira)s) & (BPVDLS>%(D0Bpvdls)s)" % locals()
    _daughtersCuts = "(PT> %(D0daughterPT)s) & (P>%(D0daughterP)s) & (TRCHI2DOF<%(D0daughterTrkChi2)s) " % locals()
    _daughtersCuts += "& (TRGHP<%(D0daughterTrkGhostProb)s) & (BPVIPCHI2()>%(D0daughterBpvIpChi2)s)" % locals()
###    _KCutsPID     = "&( (PIDK>%(D0daughterKaonPIDK)s) | (PROBNNk>%(D0daughterKaonProbNNk)s) )" % locals()
###    _PiCutsPID    = "&( (PIDK<%(D0daughterPionPIDK)s) | (PROBNNpi>%(D0daughterPionProbNNpi)s) )" % locals()
    _KCutsPID     = "&(PROBNNk>%(D0daughterKaonProbNNk)s)" % locals()
    _PiCutsPID    = "&(PROBNNpi>%(D0daughterPionProbNNpi)s)" % locals()
    _KCuts    = _daughtersCuts + _KCutsPID
    _PiCuts   = _daughtersCuts + _PiCutsPID
    _D0Filter = CombineParticles( DecayDescriptor = "[D0 -> K- pi+]cc",
                                  MotherCut = _motherCuts,
                                  DaughtersCuts =  { "pi+" : _PiCuts, "K+" : _KCuts }
                                  )

#    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
#    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseANNKaons/Particles")
    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLooseANNPions/Particles")

    return Selection (name,
                      Algorithm = _D0Filter,
                      RequiredSelections = [_stdTightKaons, _stdTightPions])




def makeDp2HHH(name, DpmMassWin, DpmPT, DpmVtxChi2Ndof, DpmBpvdira, DpmBpvdls,
                     DpmdaughterPT, DpmdaughterP, DpmdaughterTrkChi2,
                     DpmdaughterTrkGhostProb, DpmdaughterBpvIpChi2,
###                     DpmdaughterKaonPIDK, DpmdaughterPionPIDK,
                     DpmdaughterKaonProbNNk, DpmdaughterPionProbNNpi,
               ) :

    """
    Create and return a D+ -> K- pi+ pi+
    """

    _DpmotherCuts  = "(ADMASS('D+')<%(DpmMassWin)s) & (PT> %(DpmPT)s) & (VFASPF(VCHI2PDOF) < %(DpmVtxChi2Ndof)s) " % locals()
    _DpmotherCuts += "& (BPVDIRA>%(DpmBpvdira)s) & (BPVDLS > %(DpmBpvdls)s)" % locals()
    _DpdaughtersCuts  = "(PT>%(DpmdaughterPT)s) & (P>%(DpmdaughterP)s) & (TRCHI2DOF<%(DpmdaughterTrkChi2)s) " % locals()
    _DpdaughtersCuts += "& (TRGHP<%(DpmdaughterTrkGhostProb)s) & (BPVIPCHI2()>%(DpmdaughterBpvIpChi2)s)" % locals()
###    _KCutsPID     = "&( (PIDK>%(DpmdaughterKaonPIDK)s) | (PROBNNk>%(DpmdaughterKaonProbNNk)s) )" % locals()
###    _PiCutsPID    = "&( (PIDK<%(DpmdaughterPionPIDK)s) | (PROBNNpi>%(DpmdaughterPionProbNNpi)s) )" % locals()
    _KCutsPID     = "&(PROBNNk>%(DpmdaughterKaonProbNNk)s)" % locals()
    _PiCutsPID    = "&(PROBNNpi>%(DpmdaughterPionProbNNpi)s)" % locals()
    _DpKCuts  = _DpdaughtersCuts + _KCutsPID
    _DpPiCuts = _DpdaughtersCuts + _PiCutsPID

    _DplusFilter = CombineParticles( DecayDescriptor = "[D+ -> K- pi+ pi+]cc",
                                  MotherCut = _DpmotherCuts,
                                  DaughtersCuts =  { "pi+" : _DpPiCuts, "K+" : _DpKCuts }
                                  )

    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseANNKaons/Particles")
    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLooseANNPions/Particles")

    return Selection (name,
                      Algorithm = _DplusFilter,
                      RequiredSelections = [_stdTightKaons, _stdTightPions])




def makeDs2HHH(name, DpmMassWin, DpmPT, DpmVtxChi2Ndof, DpmBpvdira, DpmBpvdls,
                     DpmdaughterPT, DpmdaughterP, DpmdaughterTrkChi2,
                     DpmdaughterTrkGhostProb, DpmdaughterBpvIpChi2,
###                     DpmdaughterKaonPIDK, DpmdaughterPionPIDK,
                     DpmdaughterKaonProbNNk, DpmdaughterPionProbNNpi,
              ) :

    """
    Create and return a D_s+ -> K- pi+ pi+
    """

    _DsmotherCuts  = "(ADMASS('D_s+')<%(DpmMassWin)s) & (PT> %(DpmPT)s) & (VFASPF(VCHI2PDOF) < %(DpmVtxChi2Ndof)s) " % locals()
    _DsmotherCuts += "& (BPVDIRA>%(DpmBpvdira)s) & (BPVDLS > %(DpmBpvdls)s)" % locals()
    _DsdaughtersCuts  = "(PT>%(DpmdaughterPT)s) & (P>%(DpmdaughterP)s) & (TRCHI2DOF<%(DpmdaughterTrkChi2)s) " % locals()
    _DsdaughtersCuts += "& (TRGHP<%(DpmdaughterTrkGhostProb)s) & (BPVIPCHI2()>%(DpmdaughterBpvIpChi2)s)" % locals()
###    _KCutsPID     = "&( (PIDK>%(DpmdaughterKaonPIDK)s) | (PROBNNk>%(DpmdaughterKaonProbNNk)s) )" % locals()
###    _PiCutsPID    = "&( (PIDK<%(DpmdaughterPionPIDK)s) | (PROBNNpi>%(DpmdaughterPionProbNNpi)s) )" % locals()
    _KCutsPID     = "&(PROBNNk>%(DpmdaughterKaonProbNNk)s)" % locals()
    _PiCutsPID    = "&(PROBNNpi>%(DpmdaughterPionProbNNpi)s)" % locals()
    _DsKCuts  = _DsdaughtersCuts + _KCutsPID
    _DsPiCuts = _DsdaughtersCuts + _PiCutsPID

    _DsFilter = CombineParticles( DecayDescriptor = "[D_s+ -> K- K+ pi+]cc",
                                  MotherCut = _DsmotherCuts,
                                  DaughtersCuts =  { "pi+" : _DsPiCuts, "K+" : _DsKCuts }
                                  )

#    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
#    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    _stdTightKaons = DataOnDemand(Location = "Phys/StdAllLooseANNKaons/Particles")
    _stdTightPions = DataOnDemand(Location = "Phys/StdAllLooseANNPions/Particles")

    return Selection (name,
                      Algorithm = _DsFilter,
                      RequiredSelections = [_stdTightKaons, _stdTightPions])


def makeCC2DD(name,
                   D0Sel,
                   DplusSel,
                   DsSel,
                   CCMassCut,
                   CCVtxChi2Ndof,
                   CCMaxD0ChildPT,
                   CCMaxD0TreePT,
                   CCMaxD0MinTreeIpChi2,
                   ) :
    """
    Create and return a X -> DD  Selection Object, with  D = D0(Kpi) or Dp(Kpipi)
    Arguments:
    name          : name of the Selection.
    D0Sel         : (D0 -> HH; Dplus,Ds -> HHH) Selection object.
    CCMassCut     : CC invariant mass cuts (not used)
    CCVtxChi2Ndof : CC vertex Chi2/Ndof cut
    CCMaxD0ChildPT  : highest PT between D0 and D~0
    CCMaxD0TreePT   : highest PT between all D0/D~0 daughters
    CCMaxD0MinTreeIpChi2 : max between D0/D~0 minima of daughter's MINIPCHI2
    """

    ###_motherCuts = "(ALL)" 
    _motherCuts  = "(VFASPF(VCHI2PDOF)<%(CCVtxChi2Ndof)s)" % locals()
    _motherCuts += "&(MAXTREE(ISBASIC,PT)>%(CCMaxD0TreePT)s)" % locals()
    _combinationCuts  = CCMassCut
    _combinationCuts += "&(AMAXCHILD(PT)>%(CCMaxD0ChildPT)s)" % locals()
##    _combinationCuts += "&(AMAXCHILD(MINTREE(BPVIPCHI2()))>%(CCMaxD0MinTreeIpChi2)s)" % locals()  ## unused for the moment

    #print 'makeBs2JpsiPhi', name, 'MotherCuts:', _motherCuts
    _X = CombineParticles( DecayDescriptors = [ ### pure particle-(anti)particle
                  "psi(3770) -> D0 D~0",
                 "[psi(3770) -> D0 D0]cc",      ### C=2,       Q=0
                  "psi(3770) -> D+ D-",
                 "[psi(3770) -> D+ D+]cc",      ### C=2,       Q=+2
                  "psi(3770) -> D_s+ D_s-",
                 "[psi(3770) -> D_s+ D_s+]cc",  ### C=2, S=2,  Q=+2
                  #################################  mixed (without Ds)
                 "[psi(3770) -> D0 D-]cc",      ###            Q=-1
                 "[psi(3770) -> D0 D+]cc",      ### C=2,       Q=+1
                  #################################  mixed (with Ds)
                 "[psi(3770) -> D0 D_s-]cc",    ###      S=-1, Q=-1
                 "[psi(3770) -> D+ D_s-]cc",    ###      S=-1, Q=0
                 "[psi(3770) -> D0 D_s+]cc",    ### C=2, S=1,  Q=+1
                 "[psi(3770) -> D+ D_s+]cc",    ### C=2, S=1,  Q=+2
                                               ],
                           MotherCut = _motherCuts,
                           CombinationCut = _combinationCuts
                           )

    DSel = MergedSelection ( name + "MergedDSelection",
                             RequiredSelections = [D0Sel, DplusSel, DsSel]
                           )

    return Selection ( name,
                       Algorithm = _X,
                       RequiredSelections = [DSel])

