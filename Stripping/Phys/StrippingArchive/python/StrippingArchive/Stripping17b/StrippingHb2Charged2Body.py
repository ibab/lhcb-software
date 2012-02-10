"""
Stripping options for (pre-)selecting B -> hh', B -> p pbar
and Bs -> mu mu events.
Authors: Angelo Carboni, Lars Eklumd, Vava Gligorov, Stefano Perazzini, Eduardo Rodrigues
"""

########################################################################
__author__ = ['Stefano Perazzini','Angelo Carbone','Eduardo Rodrigues', 'Lars Eklund', 'Vava Gligorov']
__date__ = '25/01/2012'
__version__ = '$Revision: 1.5 $'

__all__ = ('Hb2Charged2BodyLines',
           'makeB2Charged2Body',
           'makeB2PPbar')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles                     import StdNoPIDsPions, StdLooseProtons, StdLooseKaons

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
from Configurables               import SubstitutePID, FilterDesktop

## Change decay descriptor and re-fit decay tree
def subPID(name, input, mother, plusD, minusD):
    ddChangeAlg = SubstitutePID( name+"SubPIDAlg",
                                 Code = "DECTREE('B0 -> pi+ pi-')",
                                 Substitutions = { ' Beauty -> ^pi+  X- ' : plusD,
                                                   ' Beauty ->  X+  ^pi-' : minusD,
                                                   ' Beauty ->  X+   X- ' : mother},
                                 MaxChi2PerDoF = 100 )
    
    newDDescr =  Selection( name+"SubPIDSel",
                            Algorithm = ddChangeAlg,
                            RequiredSelections = [input])

    return Selection(name+"pickDecay",
                     Algorithm = FilterDesktop( name+"decayFltr",
                                                Code = "DECTREE('%s -> %s %s')" %(mother, plusD, minusD) ),
                     RequiredSelections = [newDDescr])


class Hb2Charged2BodyLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""
    
    __configuration_keys__ = ( 'PrescaleB2Charged2Body',  'PrescaleB2PPbar', 'PrescaleBs2KK_NoIPCutOnB',
                               'MinPTB2Charged2Body',     'MinPTB2PPbar',
                               'MinIPB2Charged2Body',
                               'MinIPChi2B2Charged2Body', 'MinIPChi2B2PPbar',
                               'TrChi2',
                               'PIDppi',
                               'PIDpk',
                               'MaxPTB2Charged2Body',     'MaxPTB2PPbar',
                               'MaxIPB2Charged2Body',
                               'MaxIPChi2B2Charged2Body', 'MaxIPChi2B2PPbar',
                               'CombMassLow',
                               'CombMassHigh',            'CombMassWindow',
                               'DOCA',                    'VertexChi2B2PPbar',
                               'BPT',                     'BPTB2PPbar',
                               'BIP',
                               'BIPChi2B2Charged2Body',   'BIPChi2B2PPbar',
                               'BDIRA',
                               'BTAU',
                               'MassLow',
                               'MassHigh',
                               'PIDKBs2KK_NoIPCutOnB' 
                               )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        B2Charged2BodyName   = name + "B2Charged2Body"
        B2KPlusPiMinusName   = name + "B2KPlusPiMinus"
        B2PiPlusKMinusName   = name + "B2PiPlusKMinus"
        Bs2KPlusKMinusName   = name + "Bs2KPlusKMinus"
        Lb2PPlusPiMinusName  = name + "Lb2PPlusPiMinus"
        Lb2PiPlusPMinusName  = name + "Lb2PiPlusPMinus"
        Lb2PPlusKMinusName   = name + "Lb2PPlusKMinus"
        Lb2KPlusPMinusName   = name + "Lb2KPlusPMinus"
        B2PPbarName          = name + "B2PPbar"
        Bs2KK_NoIPCutOnBName = name + "Bs2KK_NoIPCutOnB"
        
        # make the various stripping selections
        self.B2Charged2Body = makeB2Charged2Body( B2Charged2BodyName,
                                                  config['TrChi2'],
                                                  config['MinPTB2Charged2Body'],
                                                  config['MinIPB2Charged2Body'],
                                                  config['MinIPChi2B2Charged2Body'],
                                                  config['MaxPTB2Charged2Body'],
                                                  config['MaxIPB2Charged2Body'],
                                                  config['MaxIPChi2B2Charged2Body'],
                                                  config['CombMassLow'],
                                                  config['CombMassHigh'],
                                                  config['DOCA'],
                                                  config['BPT'],
                                                  config['BIP'],
                                                  config['BIPChi2B2Charged2Body'],
                                                  config['BTAU'],
                                                  config['MassLow'],
                                                  config['MassHigh']
                                                )
        
        self.B2KPlusPiMinus  = subPID(B2KPlusPiMinusName, self.B2Charged2Body,
                                      "B0", "K+", "pi-" )

        self.B2PiPlusKMinus  = subPID(B2PiPlusKMinusName, self.B2Charged2Body,
                                      "B0", "pi+", "K-" )

        self.Bs2KPlusKMinus  = subPID(Bs2KPlusKMinusName, self.B2Charged2Body,
                                      "B_s0", "K+", "K-" )

        self.Lb2PPlusPiMinus = subPID(Lb2PPlusPiMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "p+", "pi-" )
        
        self.Lb2PiPlusPMinus = subPID(Lb2PiPlusPMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "pi+", "p~-" )

        self.Lb2PPlusKMinus  = subPID(Lb2PPlusKMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "p+", "K-" )
        
        self.Lb2KPlusPMinus  = subPID(Lb2KPlusPMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "K+", "p~-" )


        """
        self.B2KPlusPiMinus  = subPID(B2KPlusPiMinusName, self.B2Charged2Body,
                                      "B0", "K+", "pi-" )

        self.B2PiPlusKMinus  = subPID(B2PiPlusKMinusName, self.B2Charged2Body,
                                      "B0", "pi+", "K-" )

        self.Bs2KPlusKMinus  = subPID(Bs2KPlusKMinusName, self.B2Charged2Body,
                                      "B_s0", "K+", "K-" )

        self.Lb2PPlusPiMinus = subPID(Lb2PPlusPiMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "p+", "pi-" )
        
        self.Lb2PiPlusPMinus = subPID(Lb2PiPlusPMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "pi+", "p~-" )

        self.Lb2PPlusKMinus  = subPID(Lb2PPlusKMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "p+", "K-" )
        
        self.Lb2KPlusPMinus  = subPID(Lb2KPlusPMinusName, self.B2Charged2Body,
                                      "Lambda_b0", "K+", "p~-" )
        """

        self.B2PPbar = makeB2PPbar( B2PPbarName,
                                    config['MinPTB2PPbar'],
                                    config['TrChi2'],
                                    config['PIDppi'],
                                    config['PIDpk'],
                                    config['MinIPChi2B2PPbar'],
                                    config['MaxIPChi2B2PPbar'],
                                    config['CombMassWindow'],
                                    config['MaxPTB2PPbar'], 
                                    config['VertexChi2B2PPbar'],
                                    config['BIPChi2B2PPbar'],
                                    config['BPTB2PPbar'],
                                    config['BDIRA'] )
        
        self.Bs2KK_NoIPCutOnB = makeBs2KK_NoIPCutOnB( Bs2KK_NoIPCutOnBName,
                                                  config['PIDKBs2KK_NoIPCutOnB'],
                                                  config['TrChi2'],
                                                  config['MinPTB2Charged2Body'],
                                                  config['MinIPB2Charged2Body'],
                                                  config['MinIPChi2B2Charged2Body'],
                                                  config['MaxPTB2Charged2Body'],
                                                  config['MaxIPB2Charged2Body'],
                                                  config['MaxIPChi2B2Charged2Body'],
                                                  config['CombMassLow'],
                                                  config['CombMassHigh'],
                                                  config['DOCA'],
                                                  config['BPT'],
                                                  config['BTAU'],
                                                  config['MassLow'],
                                                  config['MassHigh']
                                                ) 

        self.lineB2Charged2Body = StrippingLine( B2Charged2BodyName+"Line",
                                                 prescale  = config['PrescaleB2Charged2Body'],
                                                 selection = self.B2Charged2Body )
        
        self.lineB2KPlusPiMinus  = StrippingLine( B2KPlusPiMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.B2KPlusPiMinus )
        
        self.lineB2PiPlusKMinus  = StrippingLine( B2PiPlusKMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.B2PiPlusKMinus )
        
        self.lineBs2KPlusKMinus  = StrippingLine( Bs2KPlusKMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Bs2KPlusKMinus )
        
        self.lineLb2PPlusPiMinus = StrippingLine( Lb2PPlusPiMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2PPlusPiMinus )
        
        self.lineLb2PiPlusPMinus = StrippingLine( Lb2PiPlusPMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2PiPlusPMinus )
        
        self.lineLb2PPlusKMinus  = StrippingLine( Lb2PPlusKMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2PPlusKMinus )
        
        self.lineLb2KPlusPMinus  = StrippingLine( Lb2KPlusPMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2KPlusPMinus )
        
        self.lineB2PPbar = StrippingLine( B2PPbarName+"Line",
                                          prescale = config['PrescaleB2PPbar'],
                                          selection = self.B2PPbar )
        
        
        self.lineBs2KK_NoIPCutOnB = StrippingLine( Bs2KK_NoIPCutOnBName+"Line",
                                                   prescale = config['PrescaleBs2KK_NoIPCutOnB'],
                                                   selection = self.Bs2KK_NoIPCutOnB)      
 
        """
        self.lineB2PiPlusKMinus  = StrippingLine( B2PiPlusKMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.B2PiPlusKMinus )
        
        self.lineBs2KPlusKMinus  = StrippingLine( Bs2KPlusKMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Bs2KPlusKMinus )
        
        self.lineLb2PPlusPiMinus = StrippingLine( Lb2PPlusPiMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2PPlusPiMinus )
        
        self.lineLb2PiPlusPMinus = StrippingLine( Lb2PiPlusPMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2PiPlusPMinus )
        
        self.lineLb2PPlusKMinus  = StrippingLine( Lb2PPlusKMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2PPlusKMinus )
        
        self.lineLb2KPlusPMinus  = StrippingLine( Lb2KPlusPMinusName+"Line",
                                                  prescale  = config['PrescaleB2Charged2Body'],
                                                  selection = self.Lb2KPlusPMinus )
        
        self.lineB2PPbar         = StrippingLine( B2PPbarName+"Line",
                                                  prescale = config['PrescaleB2PPbar'],
                                                  selection = self.B2PPbar )
        """

        self.registerLine(self.lineB2PPbar)    
        self.registerLine(self.lineB2Charged2Body)
        self.registerLine(self.lineB2KPlusPiMinus)
        self.registerLine(self.lineB2PiPlusKMinus)
        self.registerLine(self.lineBs2KPlusKMinus)
        self.registerLine(self.lineLb2PPlusPiMinus)
        self.registerLine(self.lineLb2PiPlusPMinus)
        self.registerLine(self.lineLb2PPlusKMinus)
        self.registerLine(self.lineLb2KPlusPMinus)
        self.registerLine(self.lineBs2KK_NoIPCutOnB)

def makeB2Charged2Body( name, 
                        trChi2,minPT,minIP,minIPChi2,
                        maxPT,maxIP,maxIPChi2,combMassLow,combMassHigh,doca,
                        bPT,bIP,bIPChi2,bTAU,massLow,massHigh ) : 
    
    _daughters_cuts = "(TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & ( (MIPDV(PRIMARY) > %(minIP)s ) | ( MIPCHI2DV(PRIMARY) > %(minIPChi2)s  ) )" %locals()
    
    _combination_cuts = "(AMAXCHILD(MAXTREE('pi+'==ABSID,PT)) > %(maxPT)s ) & ( (AMAXCHILD(MAXTREE('pi+'==ABSID,MIPDV(PRIMARY))) > %(maxIP)s) | (AMAXCHILD(MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s ) ) & (AMAXDOCA('') < %(doca)s ) & (AM > %(combMassLow)s * MeV) & (AM < %(combMassHigh)s * MeV)" %locals()
    
    _mother_cuts = "(PT > %(bPT)s * MeV) & (M > %(massLow)s * MeV) & (M < %(massHigh)s * MeV) & ( ( BPVIP() < %(bIP)s ) | ( BPVIPCHI2() < %(bIPChi2)s ) ) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(bTAU)s )" %locals()
    
    CombineHb2Charged2Body = CombineParticles( DecayDescriptor = 'B0 -> pi+ pi-',
                                               DaughtersCuts = { "pi+" : _daughters_cuts },
                                               CombinationCut = _combination_cuts,
                                               MotherCut = _mother_cuts )
    
    return Selection( name,
                      Algorithm = CombineHb2Charged2Body,
                      RequiredSelections = [ StdNoPIDsPions ] )    

def makeB2PPbar( name,
                 minPT, trChi2, pidPPi, pidPK, minIPChi2, maxIPChi2,
                 combMassWindow, maxPT,
                 vertexChi2, bIPChi2, bPT, bDIRA ) :

    _daughters_cuts = "(PT > %(minPT)s * MeV) & (TRCHI2DOF < %(trChi2)s) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s)" %locals()
    _combination_cuts = "(ADAMASS('B0') < %(combMassWindow)s * MeV) & ( AMAXCHILD(MAXTREE('p+'==ABSID,PT)) > %(maxPT)s * MeV ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s )" %locals()
    _mother_cuts = "(PT > %(bPT)s * MeV) & ( VFASPF(VCHI2PDOF) < %(vertexChi2)s ) & ( BPVDIRA > %(bDIRA)s ) & ( BPVIPCHI2() < %(bIPChi2)s )" %locals()

    CombineB2PPbar = CombineParticles( DecayDescriptor = 'B0 -> p+ p~-',
                                       DaughtersCuts = { "p+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts )

    return Selection( name,
                      Algorithm = CombineB2PPbar,
                      RequiredSelections = [ StdLooseProtons ] )

def makeBs2KK_NoIPCutOnB( name, pidk,trChi2,minPT,minIP,minIPChi2,
                        maxPT,maxIP,maxIPChi2,combMassLow,combMassHigh,doca,
                        bPT,bTAU,massLow,massHigh ) : 
                        
    _daughters_cuts = "(PIDK > %(pidk)s) & (TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & ( (MIPDV(PRIMARY) > %(minIP)s ) | ( MIPCHI2DV(PRIMARY) > %(minIPChi2)s  ) )" %locals()    
    _combination_cuts = "(AMAXCHILD(MAXTREE('K+'==ABSID,PT)) > %(maxPT)s ) & ( (AMAXCHILD(MAXTREE('K+'==ABSID,MIPDV(PRIMARY))) > %(maxIP)s) | (AMAXCHILD(MAXTREE('K+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s ) ) & (AMAXDOCA('') < %(doca)s ) & (AM > %(combMassLow)s * MeV) & (AM < %(combMassHigh)s * MeV)" %locals()    
    _mother_cuts = "(PT > %(bPT)s * MeV) & (M > %(massLow)s * MeV) & (M < %(massHigh)s * MeV) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(bTAU)s )" %locals()    
    CombineHb2Charged2Body = CombineParticles( DecayDescriptor = 'B0 -> K+ K-',
                                               DaughtersCuts = { "K+" : _daughters_cuts },
                                               CombinationCut = _combination_cuts,
                                               MotherCut = _mother_cuts )
                                               
    return Selection( name,
                      Algorithm = CombineHb2Charged2Body,
                      RequiredSelections = [ StdLooseKaons ] )
########################################################################  

