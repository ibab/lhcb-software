# $Id$
'''
Speculative code for doubly-charmed baryon searches.
The following six final states are used:
  Xicc+  -> Lambdac+ K- pi+
  Xicc++ -> Lambdac+ K- pi+ pi+
  Xicc+  -> Xic0 pi+
  Xicc++ -> Xic0 pi+ pi+
  Xicc+  -> Xic+ pi+ pi-
  Xicc++ -> Xic+ pi+
where the daughter charmed baryon is reconstructed via:
  Lambdac+ -> p K- pi+ (from StdLooseLambdac2PKPi)
  Xic0 -> Xi- pi+
  Xic+ -> Xi- pi+ pi+
and where the Xi- is reconstructed via:
  Xi- -> Lambda pi-, Lambda -> p pi- (from StdLooseLambdaDD and StdLooseLambdaLL)
In addition to the six Xicc signal lines, three control lines are written out
for the Lambdac+, Xic0, and Xic+. It will certainly be necessary to prescale
the Lambdac+ control line, and perhaps the others too.
'''

__author__ = ['Mat Charles']
__date__ = '24/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('XiccBuilder', 'makeLc', 'filterKaons', 'filterPions', 'makeXi', 'makeXicc', 'makeXicZero', 'makeXicPlus')


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

name = "Xicc"

class XiccBuilder(LineBuilder) :
    """
    Search for Xicc
    """

    __configuration_keys__ = ('LongTrackGEC'
                              ,'controlPrescaleLc'
                              ,'controlPrescaleXic'
                              ,'signalPrescaleViaLc'
                              ,'signalPrescaleViaXic')

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCuts = "( CONTAINS ('Rec/Track/Long') < %(LongTrackGEC)s )" % config

        # Pick up standard kaons, pions
        # Filter them for use as daughter particles:
        self.dauPi = filterPions(name+'FilteredPions')
        self.dauK  = filterKaons(name+'FilteredKaons')

        # Pick up standard Lambdac -> p K- pi+ then filter it to reduce rate:
        self.filterLc = makeLc(name+'FilterLc')

        # Some generic cuts for Xicc:
        _strCutComb = '(APT>2000.0*MeV)'
        _strCutMoth = '( (BPVDIRA > 0.999) & (VFASPF(VCHI2)<30.0) & (BPVVDCHI2 > 16) & (M < 4500*MeV) )'

        # Combine Lambda with pion to make Xi-
        self.stdLambdaLL = DataOnDemand(Location = 'Phys/StdLooseLambdaLL/Particles')
        self.stdLambdaDD = DataOnDemand(Location = 'Phys/StdLooseLambdaDD/Particles')
        self.combineXiLL = makeXi(name+'CombineXiLL', self.stdLambdaLL, 50, 35)
        self.combineXiDD = makeXi(name+'CombineXiDD', self.stdLambdaDD, 80, 50)

        # Combine Xi- with pion(s) to make Xic0, Xic+
        self.combineXicZero = makeXicZero(name+"CombineXicZero", [ self.combineXiLL, self.combineXiDD, self.dauPi ])
        self.combineXicPlus = makeXicPlus(name+"CombineXicPlus", [ self.combineXiLL, self.combineXiDD, self.dauPi ])

        # Combine Lc+ with a K and a pi to make a Xicc+ or Xicc++:
        self.combineXicc1 = makeXicc(name+'CombineXicc1', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc+ -> Lambda_c+ K- pi+]cc', _strCutComb, _strCutMoth)
        self.combineXicc2 = makeXicc(name+'CombineXicc2', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc++ -> Lambda_c+ K- pi+ pi+]cc', _strCutComb, _strCutMoth)
        # Combine Xic0/+ with pion(s) to make Xicc+, Xicc++
        self.combineXicc3 = makeXicc(name+'CombineXicc3', [ self.combineXicZero, self.dauPi ], '[Xi_cc+ -> Xi_c0 pi+]cc', _strCutComb, _strCutMoth)
        self.combineXicc4 = makeXicc(name+'CombineXicc4', [ self.combineXicZero, self.dauPi ], '[Xi_cc++ -> Xi_c0 pi+ pi+]cc', _strCutComb, _strCutMoth)
        self.combineXicc5 = makeXicc(name+'CombineXicc5', [ self.combineXicPlus, self.dauPi ], '[Xi_cc+ -> Xi_c+ pi+ pi-]cc', _strCutComb, _strCutMoth)
        self.combineXicc6 = makeXicc(name+'CombineXicc6', [ self.combineXicPlus, self.dauPi ], '[Xi_cc++ -> Xi_c+ pi+]cc', _strCutComb, _strCutMoth)


        # Control lines (to be prescaled!)
        self.lineControl1 = StrippingLine(name+'ControlLc',
                                          prescale = config['controlPrescaleLc'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.filterLc)
        self.lineControl2 = StrippingLine(name+'ControlXicZero',
                                          prescale = config['controlPrescaleXic'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.combineXicZero)
        self.lineControl3 = StrippingLine(name+'ControlXicPlus',
                                          prescale = config['controlPrescaleXic'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.combineXicPlus)

        # Physics lines
        self.lineXicc1 = StrippingLine(name+'XiccLine1',
                                      prescale = config['signalPrescaleViaLc'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc1)
        self.lineXicc2 = StrippingLine(name+'XiccLine2',
                                      prescale = config['signalPrescaleViaLc'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc2)
        self.lineXicc3 = StrippingLine(name+'XiccLine3',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc3)
        self.lineXicc4 = StrippingLine(name+'XiccLine4',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc4)
        self.lineXicc5 = StrippingLine(name+'XiccLine5',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc5)
        self.lineXicc6 = StrippingLine(name+'XiccLine6',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc6)

        self.registerLine(self.lineControl1)
        self.registerLine(self.lineControl2)
        self.registerLine(self.lineControl3)
        self.registerLine(self.lineXicc1)
        self.registerLine(self.lineXicc2)
        self.registerLine(self.lineXicc3)
        self.registerLine(self.lineXicc4)
        self.registerLine(self.lineXicc5)
        self.registerLine(self.lineXicc6)


def makeLc(localName) :
    # Pick up standard Lambdac -> p K- pi+
    _stdLc = DataOnDemand(Location = 'Phys/StdLooseLambdac2PKPi/Particles')

    # Filter to reduce rate:
    _strCutK  = '( CHILD(PIDK,1) - CHILD(PIDpi,1) > 5.0 )'
    _strCutp  = '( CHILD(PIDp,2) - CHILD(PIDpi,2) > 5.0 )'
    _strCutpi = '( CHILD(PIDpi,3) - CHILD(PIDK,3) > 0.0 )'
    _strCutTrackChi2 = '( (CHILD(TRCHI2DOF,1)<5.0) & (CHILD(TRCHI2DOF,2)<5.0) & (CHILD(TRCHI2DOF,3)<5.0) )'
    _strCutDIRA = '( BPVDIRA > 0.99 )'
    _strCutFD = '( BPVVDCHI2 > 25 )'
    _strCutMass = '( (M > 2185.0*MeV) & (M < 2385*MeV) )'
    _strCutIP = '( NINGENERATION( (MIPCHI2DV(PRIMARY) > 30.0), 1) >= 1 )'
    _strCutLc = '(' + _strCutK + '&' + _strCutpi + '&' + _strCutp + '&' + _strCutTrackChi2 + '&' + _strCutDIRA + '&' + _strCutFD + '&' + _strCutMass + '&' + _strCutIP + ')'
    _filterLc = FilterDesktop(Code = _strCutLc)
    return Selection ( localName,
                       Algorithm = _filterLc,
                       RequiredSelections = [ _stdLc ] )

def filterKaons(localName) :
    # Pick up standard input list
    _stdK = DataOnDemand(Location = 'Phys/StdLooseKaons/Particles')
    # Filter:
    _strCutDauK  = '( (P>2.0*GeV) & (PIDK-PIDpi>5.0) & (TRCHI2DOF<5.0) & (PT>250.0*MeV) & (MIPCHI2DV(PRIMARY)>4.0) )'
    _filterK = FilterDesktop(Code = _strCutDauK)
    return Selection ( localName,
                       Algorithm = _filterK,
                       RequiredSelections = [ _stdK ] )

def filterPions(localName) :
    # Pick up standard input list
    _stdPi = DataOnDemand(Location = 'Phys/StdLoosePions/Particles')
    # Filter:
    _strCutDauPi = '( (P>2.0*GeV) & (PIDpi-PIDK>0.0) & (TRCHI2DOF<5.0) & (PT>250.0*MeV) & (MIPCHI2DV(PRIMARY)>4.0) )'
    _filterPi = FilterDesktop(Code = _strCutDauPi)
    return Selection ( localName,
                       Algorithm = _filterPi,
                       RequiredSelections = [ _stdPi ] )

def makeXi(localName, inputList, cutWide, cutTight) :
    _stdPi = DataOnDemand(Location = 'Phys/StdLoosePions/Particles')
    _strCutPiForXi = '( (P>2.0*GeV) & (TRCHI2DOF<5.0) & (PT>250.0*MeV) & (MIPCHI2DV(PRIMARY)>25.0) )'
    _strCutCombXi = "( ADAMASS('Xi-') < %(cutWide)s * MeV )" % locals()
    _strCutMothXi = "( ( ADMASS('Xi-') < %(cutTight)s * MeV ) & (VFASPF(VCHI2)<20) )" % locals()
    _combineXi = CombineParticles( DecayDescriptor = '[Xi- -> Lambda0 pi-]cc',
                                   DaughtersCuts = { "pi-": _strCutPiForXi },
                                   CombinationCut = _strCutCombXi,
                                   MotherCut = _strCutMothXi )
    return Selection ( localName,
                       Algorithm = _combineXi,
                       RequiredSelections = [ inputList, _stdPi ] )


def makeXicc(localName, inputSelections, decay, cutComb, cutMoth) :
    _combineXicc = CombineParticles( DecayDescriptor = decay,
                                     CombinationCut = cutComb,
       	       	       	       	     MotherCut = cutMoth )
    return Selection ( localName,
                       Algorithm = _combineXicc,
                       RequiredSelections = inputSelections)

def makeXicZero(localName, inputSelections) :
    _strCutCombXicZero = "( ADAMASS('Xi_c0') < 170*MeV )"
    _strCutMothXicZero = "( ( ADMASS('Xi_c0') < 120*MeV ) & ( BPVDIRA > 0.99 ) & ( BPVVDCHI2 > 25 ) )"
    _combineXicZero = CombineParticles( DecayDescriptor = '[Xi_c0 -> Xi- pi+]cc',
                                        CombinationCut = _strCutCombXicZero,
                                        MotherCut = _strCutMothXicZero )
    return Selection( localName,
                      Algorithm = _combineXicZero,
                      RequiredSelections = inputSelections )

def makeXicPlus(localName, inputSelections) :
    _strCutCombXicPlus = "( ( ADAMASS('Xi_c+') < 170*MeV ) & ( AHASCHILD( (ABSID == 'pi+') & (MIPCHI2DV(PRIMARY) > 30.0) ) ) )"
    _strCutMothXicPlus = "( ( ADMASS('Xi_c+') < 120*MeV ) & ( BPVDIRA > 0.99 ) & ( BPVVDCHI2 > 25 ) )"
    _combineXicPlus = CombineParticles( DecayDescriptor = '[Xi_c+ -> Xi- pi+ pi+]cc',
                                        CombinationCut = _strCutCombXicPlus,
                                        MotherCut = _strCutMothXicPlus )
    return Selection( name+"CombineXicPlus",
                      Algorithm = _combineXicPlus,
                      RequiredSelections = inputSelections )
