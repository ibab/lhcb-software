"""
Stripping options for (pre-)selecting B -> hh'

Authors: Stefano Perazzini
"""

########################################################################
__author__ = ['Stefano Perazzini']
__date__ = '21/08/2012'
__version__ = '$Revision: 1.5 $'

__all__ = ('D2HHBDTLines',
           'makeD2HHBDT',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles                     import StdNoPIDsKaons, StdAllNoPIDsPions

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig


default_config = {
    'WGs'         : ['BnoC'],
    'NAME'        : 'D2HHBDT',
    'BUILDERTYPE' : 'D2HHBDTLines',
    'CONFIG'      : {'PrescaleD2HHBDT' : 1.,
                     'PrescaleDSt'     : 1.,
                     'PostscaleD02HH'  : 0.1,
                     'PostscaleDSt'    : 0.3,
                     'MinPT'           : 1000,
                     'MinIP'           : 0.12,
                     'TrChi2'          : 3,
                     'TrGhostProb'     : 0.5,
                     'CombMassLow'     : 1000,
                     'CombMassHigh'    : 2800,
                     'DOCA'            : 0.1,
                     'BPT'             : 1200,
                     'BIP'             : 0.12,
                     'BTAU'            : 0.,
                     'MassLow'         : 1800,
                     'MassHigh'        : 2600,
                     'BDTCut'          : -0.3,
                     'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HH_BDT_v1r4.xml",
                     'VertexChi2'      : 64
                    },
    'STREAMS'     : ['Charm']
    }


## Change decay descriptor and re-fit decay tree

class D2HHBDTLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""

    __configuration_keys__ = ( 'PrescaleD2HHBDT',
                               'PrescaleDSt',
                               'PostscaleD02HH',
                               'PostscaleDSt',
                               'MinPT',
                               'MinIP',
                               'TrChi2',
                               'TrGhostProb',
                               'CombMassLow',
                               'CombMassHigh',
                               'DOCA',
                               'BPT',
                               'BIP',
                               'BTAU',
                               'MassLow',
                               'MassHigh',
                               'BDTCut',
                               'BDTWeightsFile',
                               'VertexChi2'
                               )

    def __init__( self,name,config ) :

        LineBuilder.__init__(self, name, config)

        D2HHBDTName   = "D2HHBDT"

        # make the various stripping selections
        self.D2HHBDT = makeD2HHBDT( D2HHBDTName,
                                    config['TrChi2'],
                                    config['TrGhostProb'],
                                    config['MinPT'],
                                    config['MinIP'],
                                    config['CombMassLow'],
                                    config['CombMassHigh'],
                                    config['DOCA'],
                                    config['BPT'],
                                    config['BIP'],
                                    config['BTAU'],
                                    config['MassLow'],
                                    config['MassHigh']
                                  )

        self.CutBDT  = applyBDT( "CutBDT_" + D2HHBDTName,
                                 LineName       = D2HHBDTName + "Line",
                                 SelD2HHBDT     = self.D2HHBDT,
                                 BDTCutValue    = config['BDTCut'],
                                 BDTWeightsFile = config['BDTWeightsFile']
                               )

        self.lineD2HHBDT = StrippingLine( D2HHBDTName+"Line",
                                          prescale  = config['PrescaleD2HHBDT'],
                                          postscale = config['PostscaleD02HH'],
                                          selection = self.CutBDT )

        self.registerLine(self.lineD2HHBDT)

        self.DStD0Pi = makeDStD0Pi( name = "DStD0Pi_"+D2HHBDTName,
                                    cut  = config['VertexChi2'],
                                    presel = self.lineD2HHBDT.selection())

        self.lineDStD0PiD2HHBDT = StrippingLine( "DStD0PiWith"+D2HHBDTName+"Line",
                                                 prescale  = config['PrescaleDSt'],
                                                 postscale = config['PostscaleDSt'],
                                                 selection = self.DStD0Pi )

        self.registerLine(self.lineDStD0PiD2HHBDT)

def makeD2HHBDT( name,
                 trChi2,trGhostProb,minPT,minIP,
                 combMassLow,combMassHigh,doca,
                 bPT,bIP,bTAU,massLow,massHigh ) :

    _daughters_cuts = "(TRGHOSTPROB < %(trGhostProb)s) & (TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & ( MIPDV(PRIMARY) > %(minIP)s )" %locals()

    _combination_cuts = "( AMAXDOCA('') < %(doca)s ) & ( AM > %(combMassLow)s * MeV ) & ( AM < %(combMassHigh)s * MeV )" %locals()

    _mother_cuts = "( PT > %(bPT)s * MeV ) & ( M > %(massLow)s * MeV ) & ( M < %(massHigh)s * MeV ) & ( BPVIP() < %(bIP)s ) & ( BPVLTIME() > %(bTAU)s )" %locals()

    CombineD2HHBDT = CombineParticles( DecayDescriptor = 'D0 -> K+ K-',
                                       DaughtersCuts = { "K+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts,
                                       ReFitPVs = True )

    return Selection( name,
                      Algorithm = CombineD2HHBDT,
                      RequiredSelections = [ StdNoPIDsKaons ] )

def makeDStD0Pi( name, cut, presel):

    _mother_cuts = "(VFASPF(VCHI2) < %(cut)s) & ( M < 2800 * MeV ) & ( M - M1 < 170 * MeV )" %locals()

    CombineDStD0Pi = CombineParticles( DecayDescriptors = [ 'D*(2010)- -> D0 pi-',
                                                            'D*(2010)+ -> D0 pi+' ],
                                       MotherCut = _mother_cuts )

    return Selection( name,
                      Algorithm = CombineDStD0Pi,
                      RequiredSelections = [ presel, StdAllNoPIDsPions ])

def applyBDT( name,
              LineName,
              SelD2HHBDT,
              BDTCutValue,
              BDTWeightsFile ):

    _FilterD2HH = FilterDesktop( Code = "FILTER('D2hhBDTSelection/D2hhBDT')" )

    BDTSel = Selection( name,
                        Algorithm = _FilterD2HH,
                        RequiredSelections = [ SelD2HHBDT ]
                        )

    """
    Name is special here, since this is the last algorithm,
    whose name seems to be the one of the stripping line....
    """
    from Configurables import D2hhBDTSelection
    MyBDT = D2hhBDTSelection( LineName + '.D2hhBDT' )
    MyBDT.BDTCut = BDTCutValue
    MyBDT.BDTWeightsFile = BDTWeightsFile

    return BDTSel


########################################################################

