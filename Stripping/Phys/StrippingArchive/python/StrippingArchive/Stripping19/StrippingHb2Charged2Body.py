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
           'makeB2Charged2Body')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles                     import StdNoPIDsPions, StdLooseProtons, StdLooseKaons

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
from Configurables               import SubstitutePID, FilterDesktop


default_config = {
           'PrescaleB2Charged2Body'   : 1,      'PrescaleBs2KK_NoIPCutOnB' : 1,
           'MinPTB2Charged2Body'      : 1100,
           'MinIPB2Charged2Body'      : 0.12,
           'MinIPChi2B2Charged2Body'  : 100, 
           'TrChi2'                   : 5,
           'MaxPTB2Charged2Body'      : 2500, 
           'MaxIPB2Charged2Body'      : 0.25,
           'MaxIPChi2B2Charged2Body'  : 200,  
           'CombMassLow'              : 4800,
           'CombMassHigh'             : 5800,
           'DOCA'                     : 0.08,
           'BPT'                      : 1200,
           'BIP'                      : 0.1,
           'BIPChi2B2Charged2Body'    : 12,  
           'BTAU'                     : 0.0006,
           'MassLow'                  : 4800,
           'MassHigh'                 : 5800,
           'PIDKBs2KK_NoIPCutOnB'     : 0.
           }

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
    
    __configuration_keys__ = ( 'PrescaleB2Charged2Body',  'PrescaleBs2KK_NoIPCutOnB',
                               'MinPTB2Charged2Body',    
                               'MinIPB2Charged2Body',
                               'MinIPChi2B2Charged2Body',
                               'TrChi2',
                               'MaxPTB2Charged2Body',  
                               'MaxIPB2Charged2Body',
                               'MaxIPChi2B2Charged2Body', 
                               'CombMassLow',
                               'CombMassHigh',        
                               'DOCA',               
                               'BPT',              
                               'BIP',
                               'BIPChi2B2Charged2Body', 
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
        
        self.lineBs2KK_NoIPCutOnB = StrippingLine( Bs2KK_NoIPCutOnBName+"Line",
                                                   prescale = config['PrescaleBs2KK_NoIPCutOnB'],
                                                   selection = self.Bs2KK_NoIPCutOnB)      
 
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

