# $Id: StrippingCC2DD.py, v 0.4 2015-12-01  $
''' 
Module for construction of CC->D D, where CC is a particle decaying to D D
                                    and D is a D0(Kpi) or a D+(K-pi+pi+)
                                          or a Ds(KKpi)
                                          or one of their antipartciles
          v .03: added "D+charmed baryon" and "2-charmed baryons" combinations,
                 where "charmed baryon" = Lambda_c+(Xi_c+) -> p+ K- pi+
                                    or    Xi_c0(Omega_c0)  -> p+ K- K- pi+
'''

__author__ = ['Lucio Anderlini','Andrea Bizzeti']
__date__ = '2015-12-01'
__version__ = '$Revision: 0.4 $'

__all__ = ('CC2DDConf',
           'makeD02HH',
           'makeDp2HHH',
           'makeLc2HHH',
           'makeXc2HHHH',
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
                   'D0daughterTrkChi2'      : 3,
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
                   'DpmdaughterTrkChi2'      : 3,
                   'DpmdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'DpmdaughterKaonProbNNk'  : 0.1,
                   'DpmdaughterPionProbNNpi' : 0.1,

######## Lambda_c+, Xi_c+ -> p+ K- pi+ cuts
                   'LcMassWin'     : "60*MeV",
                   'LcPT'          : "1000*MeV",
                   'LcVtxChi2Ndof' : 10,
                   'LcBpvdira'     : -10.,
                   'LcBpvdls'      : 4.,               # standard cut
                   'LcdaughterBpvIpChi2'    : 4.,
                   'LcdaughterPT'           : "500*MeV",
                   'LcdaughterP'            : "5*GeV",
                   'LcdaughterTrkChi2'      : 3,
                   'LcdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'LcdaughterKaonProbNNk'  : 0.1,
                   'LcdaughterPionProbNNpi' : 0.1,
                   'LcdaughterProtonProbNNp': 0.1,

######## Xi_c0, Omega_c0 -> p+ K- K- pi+ cuts
                   'XcMassWin'     : "60*MeV",
                   'XcPT'          : "1000*MeV",
                   'XcVtxChi2Ndof' : 10,
                   'XcBpvdira'     : -10.,
                   'XcBpvdls'      : -10.,             # no cut
                   'XcdaughterBpvIpChi2'    : 2.,      # loose cut
                   'XcdaughterPT'           : "500*MeV",
                   'XcdaughterP'            : "5*GeV",
                   'XcdaughterTrkChi2'      : 3,
                   'XcdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'XcdaughterKaonProbNNk'  : 0.1,
                   'XcdaughterPionProbNNpi' : 0.1,
                   'XcdaughterProtonProbNNp': 0.1,

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
                              'LcMassWin',
                              'LcPT',
                              'LcVtxChi2Ndof',
                              'LcBpvdira',
                              'LcBpvdls',
                              'LcdaughterPT',
                              'LcdaughterP',
                              'LcdaughterTrkChi2',
                              'LcdaughterTrkGhostProb',
                              'LcdaughterBpvIpChi2',
###                           'LcdaughterKaonPIDK',
###                           'LcdaughterPionPIDK',
                              'LcdaughterProtonProbNNp',
                              'LcdaughterKaonProbNNk',
                              'LcdaughterPionProbNNpi',
####
                              'XcMassWin',
                              'XcPT',
                              'XcVtxChi2Ndof',
                              'XcBpvdira',
                              'XcBpvdls',
                              'XcdaughterPT',
                              'XcdaughterP',
                              'XcdaughterTrkChi2',
                              'XcdaughterTrkGhostProb',
                              'XcdaughterBpvIpChi2',
###                           'XcdaughterKaonPIDK',
###                           'XcdaughterPionPIDK',
                              'XcdaughterProtonProbNNp',
                              'XcdaughterKaonProbNNk',
                              'XcdaughterPionProbNNpi',
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


        self.selLc2HHH = makeLc2HHH( 'LcFor'+name, 
                                     LcMassWin     = config['LcMassWin'],
                                     LcPT          = config['LcPT'],
                                     LcVtxChi2Ndof = config['LcVtxChi2Ndof'],
                                     LcBpvdira     = config['LcBpvdira'],
                                     LcBpvdls      = config['LcBpvdls'],
                                     LcdaughterPT           = config['LcdaughterPT'],
                                     LcdaughterP            = config['LcdaughterP'],
                                     LcdaughterTrkChi2      = config['LcdaughterTrkChi2'],
                                     LcdaughterTrkGhostProb = config['LcdaughterTrkGhostProb'],
                                     LcdaughterBpvIpChi2    = config['LcdaughterBpvIpChi2'],
###                                     LcdaughterKaonPIDK     = config['LcdaughterKaonPIDK'],
###                                     LcdaughterPionPIDK     = config['LcdaughterPionPIDK'],
                                     LcdaughterProtonProbNNp= config['LcdaughterProtonProbNNp'],
                                     LcdaughterKaonProbNNk  = config['LcdaughterKaonProbNNk'],
                                     LcdaughterPionProbNNpi = config['LcdaughterPionProbNNpi'],
                                   )

        self.selXc2HHHH = makeXc2HHHH( 'XcFor'+name, 
                                     XcMassWin     = config['XcMassWin'],
                                     XcPT          = config['XcPT'],
                                     XcVtxChi2Ndof = config['XcVtxChi2Ndof'],
                                     XcBpvdira     = config['XcBpvdira'],
                                     XcBpvdls      = config['XcBpvdls'],
                                     XcdaughterPT           = config['XcdaughterPT'],
                                     XcdaughterP            = config['XcdaughterP'],
                                     XcdaughterTrkChi2      = config['XcdaughterTrkChi2'],
                                     XcdaughterTrkGhostProb = config['XcdaughterTrkGhostProb'],
                                     XcdaughterBpvIpChi2    = config['XcdaughterBpvIpChi2'],
###                                     XcdaughterKaonPIDK     = config['XcdaughterKaonPIDK'],
###                                     XcdaughterPionPIDK     = config['XcdaughterPionPIDK'],
                                     XcdaughterProtonProbNNp  = config['XcdaughterProtonProbNNp'],
                                     XcdaughterKaonProbNNk  = config['XcdaughterKaonProbNNk'],
                                     XcdaughterPionProbNNpi = config['XcdaughterPionProbNNpi'],
                                   )


        self.selCC2DD = makeCC2DD(name,  
                                  D0Sel    = self.selD02HH,
                                  DplusSel = self.selDp2HHH,
                                  DsSel    = self.selDs2HHH,
                                  LcSel    = self.selLc2HHH,
                                  XcSel    = self.selXc2HHHH,
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
    Create and return a D_s+ -> K- K+ pi+
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






def makeLc2HHH(name, LcMassWin, LcPT, LcVtxChi2Ndof, LcBpvdira, LcBpvdls,
                     LcdaughterPT, LcdaughterP, LcdaughterTrkChi2,
                     LcdaughterTrkGhostProb, LcdaughterBpvIpChi2,
###                     LcdaughterKaonPIDK, LcdaughterPionPIDK,
                     LcdaughterProtonProbNNp,
                     LcdaughterKaonProbNNk, LcdaughterPionProbNNpi,
               ) :

    """
    Create and return a Lambda_c+/Xi_c+ -> p K- pi+
    """

###    _LcotherCuts  = "(ALL)"   # no cuts
    _LcotherCuts  = "( (ADMASS('Lambda_c+')<%(LcMassWin)s) | (ADMASS('Xi_c+')<%(LcMassWin)s) ) " % locals()
    _LcotherCuts += "& (PT> %(LcPT)s) & (VFASPF(VCHI2PDOF) < %(LcVtxChi2Ndof)s) " % locals()
    _LcotherCuts += "& (BPVDIRA>%(LcBpvdira)s) & (BPVDLS > %(LcBpvdls)s)" % locals()
###    _LcdaughtersCuts  = "(ALL)"   # no cuts
    _LcdaughtersCuts  = "(PT>%(LcdaughterPT)s) & (P>%(LcdaughterP)s) & (TRCHI2DOF<%(LcdaughterTrkChi2)s) " % locals()
    _LcdaughtersCuts += "& (TRGHP<%(LcdaughterTrkGhostProb)s) & (BPVIPCHI2()>%(LcdaughterBpvIpChi2)s)" % locals()

###    _KCutsPID     = "&( (PIDK>%(LcdaughterKaonPIDK)s) | (PROBNNk>%(LcdaughterKaonProbNNk)s) )" % locals()
###    _PiCutsPID    = "&( (PIDK<%(LcdaughterPionPIDK)s) | (PROBNNpi>%(LcdaughterPionProbNNpi)s) )" % locals()
    _PCutsPID     = "&(PROBNNp>%(LcdaughterProtonProbNNp)s)" % locals()
    _KCutsPID     = "&(PROBNNk>%(LcdaughterKaonProbNNk)s)" % locals()
    _PiCutsPID    = "&(PROBNNpi>%(LcdaughterPionProbNNpi)s)" % locals()
    _LcPCuts  = _LcdaughtersCuts + _PCutsPID
    _LcKCuts  = _LcdaughtersCuts + _KCutsPID
    _LcPiCuts = _LcdaughtersCuts + _PiCutsPID

    _LcFilter = CombineParticles( DecayDescriptor = "[Lambda_c+ -> p+ K- pi+]cc",
                                  MotherCut = _LcotherCuts,
                                  DaughtersCuts =  { "p+" : _LcPCuts, "pi+" : _LcPiCuts, "K+" : _LcKCuts }
                                  )
#    _stdTightProtons = DataOnDemand(Location = "Phys/StdAllLooseProtons/Particles")
#    _stdTightKaons   = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
#    _stdTightPions   = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    _stdTightProtons = DataOnDemand(Location = "Phys/StdAllLooseANNProtons/Particles")
    _stdTightKaons   = DataOnDemand(Location = "Phys/StdAllLooseANNKaons/Particles")
    _stdTightPions   = DataOnDemand(Location = "Phys/StdAllLooseANNPions/Particles")

    return Selection (name,
                      Algorithm = _LcFilter,
                      RequiredSelections = [_stdTightProtons, _stdTightKaons, _stdTightPions])











def makeXc2HHHH(name, XcMassWin, XcPT, XcVtxChi2Ndof, XcBpvdira, XcBpvdls,
                     XcdaughterPT, XcdaughterP, XcdaughterTrkChi2,
                     XcdaughterTrkGhostProb, XcdaughterBpvIpChi2,
###                     XcdaughterKaonPIDK, XcdaughterPionPIDK,
                     XcdaughterProtonProbNNp,
                     XcdaughterKaonProbNNk, XcdaughterPionProbNNpi,
               ) :

    """
    Create and return a Xi_c0/Omega_c0 -> p K- K- pi+
    """

###    _XcotherCuts  = "(ALL)"
    _XcotherCuts  = "( (ADMASS('Xi_c0')<%(XcMassWin)s) | (ADMASS('Omega_c0')<%(XcMassWin)s) ) " % locals()
    _XcotherCuts += "& (PT> %(XcPT)s) & (VFASPF(VCHI2PDOF) < %(XcVtxChi2Ndof)s) " % locals()
    _XcotherCuts += "& (BPVDIRA>%(XcBpvdira)s) & (BPVDLS > %(XcBpvdls)s)" % locals()
###    _XcdaughtersCuts  = "(ALL)"
    _XcdaughtersCuts  = "(PT>%(XcdaughterPT)s) & (P>%(XcdaughterP)s) & (TRCHI2DOF<%(XcdaughterTrkChi2)s) " % locals()
    _XcdaughtersCuts += "& (TRGHP<%(XcdaughterTrkGhostProb)s) & (BPVIPCHI2()>%(XcdaughterBpvIpChi2)s)" % locals()
###    _KCutsPID     = "&( (PIDK>%(XcdaughterKaonPIDK)s) | (PROBNNk>%(XcdaughterKaonProbNNk)s) )" % locals()
###    _PiCutsPID    = "&( (PIDK<%(XcdaughterPionPIDK)s) | (PROBNNpi>%(XcdaughterPionProbNNpi)s) )" % locals()

    _PCutsPID     = "&(PROBNNp>%(XcdaughterProtonProbNNp)s)" % locals()
    _KCutsPID     = "&(PROBNNk>%(XcdaughterKaonProbNNk)s)" % locals()
    _PiCutsPID    = "&(PROBNNpi>%(XcdaughterPionProbNNpi)s)" % locals()
    _XcPCuts  = _XcdaughtersCuts + _PCutsPID
    _XcKCuts  = _XcdaughtersCuts + _KCutsPID
    _XcPiCuts = _XcdaughtersCuts + _PiCutsPID

    _XcFilter = CombineParticles( DecayDescriptor = "[Xi_c0 -> p+ K- K- pi+]cc",
                                  MotherCut = _XcotherCuts,
                                  DaughtersCuts =  { "p+" : _XcPCuts, "pi+" : _XcPiCuts, "K+" : _XcKCuts }
                                  )
    _stdTightProtons = DataOnDemand(Location = "Phys/StdAllLooseANNProtons/Particles")
    _stdTightKaons   = DataOnDemand(Location = "Phys/StdAllLooseANNKaons/Particles")
    _stdTightPions   = DataOnDemand(Location = "Phys/StdAllLooseANNPions/Particles")

    return Selection (name,
                      Algorithm = _XcFilter,
                      RequiredSelections = [_stdTightProtons, _stdTightKaons, _stdTightPions])






def makeCC2DD(name,
                   D0Sel,
                   DplusSel,
                   DsSel,
                   LcSel,
                   XcSel,
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
    D0Sel, etc.   : (D0 -> HH; Dplus,Ds,Lc -> HHH, Xc -> HHHH) Selection objects.
    CCMassCut     : CC invariant mass cuts (not used)
    CCVtxChi2Ndof : CC vertex Chi2/Ndof cut
    CCMaxD0ChildPT  : highest PT between D0 and D~0
    CCMaxD0TreePT   : highest PT between all D0/D~0 daughters
    CCMaxD0MinTreeIpChi2 : max between D0/D~0 minima of daughter's MINIPCHI2
    """

    _motherCuts = "(ALL)" 
#    _motherCuts  = "(VFASPF(VCHI2PDOF)<%(CCVtxChi2Ndof)s)" % locals()
#    _motherCuts += "&(MAXTREE(ISBASIC,PT)>%(CCMaxD0TreePT)s)" % locals()
    _combinationCuts  = "(AALL)"
#    _combinationCuts  = CCMassCut
#    _combinationCuts += "&(AMAXCHILD(PT)>%(CCMaxD0ChildPT)s)" % locals()
##    _combinationCuts += "&(AMAXCHILD(MINTREE(BPVIPCHI2()))>%(CCMaxD0MinTreeIpChi2)s)" % locals()  ## unused for the moment

    #print 'makeBs2JpsiPhi', name, 'MotherCuts:', _motherCuts
    _X = CombineParticles( DecayDescriptors = [
                  ################################# pure particle-(anti)particle
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
                  #################################  baryon mixed (with Lambda_c+/Xi_c+)
                 "[psi(3770) -> Lambda_c+ D0]cc",
                 "[psi(3770) -> Lambda_c+ D~0]cc",
                 "[psi(3770) -> Lambda_c+ D+]cc",
                 "[psi(3770) -> Lambda_c+ D-]cc",
                 "[psi(3770) -> Lambda_c+ D_s+]cc",
                 "[psi(3770) -> Lambda_c+ D_s-]cc",
                  #################################  baryon mixed (with Xi_c0/Omega_c0)
                 "[psi(3770) -> Xi_c0 D0]cc",
                 "[psi(3770) -> Xi_c0 D~0]cc",
                 "[psi(3770) -> Xi_c0 D+]cc",
                 "[psi(3770) -> Xi_c0 D-]cc",
                 "[psi(3770) -> Xi_c0 D_s+]cc",
                 "[psi(3770) -> Xi_c0 D_s-]cc",
                  #################################  baryon + baryon
                 "psi(3770) -> Lambda_c+ Lambda_c~-",
                 "[psi(3770) -> Lambda_c+ Lambda_c+]cc",
                 "psi(3770) -> Xi_c0 Xi_c~0",
                 "[psi(3770) -> Xi_c0 Xi_c0]cc",
                 "[psi(3770) -> Lambda_c+ Xi_c0]cc",
                 "[psi(3770) -> Lambda_c+ Xi_c~0]cc",
                                               ],
                           MotherCut = _motherCuts,
                           CombinationCut = _combinationCuts
                           )

    DSel = MergedSelection ( name + "MergedDSelection",
                             RequiredSelections = [D0Sel, DplusSel, DsSel, LcSel, XcSel]
                           )

    return Selection ( name,
                       Algorithm = _X,
                       RequiredSelections = [DSel])

