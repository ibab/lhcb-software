"""
Stripping options for (pre-)selecting B -> hh'

Authors: Stefano Perazzini
"""

########################################################################
__author__ = ['Stefano Perazzini']
__date__ = '21/08/2012'
__version__ = '$Revision: 1.5 $'

__all__ = ('B2HHBDTLines',
           'makeB2HHBDT',
           'applyBDT',
           'default_config')

from Gaudi.Configuration import *

from GaudiKernel.SystemOfUnits import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles                     import StdNoPIDsPions

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig


default_config = {
  'B2HHBDT' : {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2HHBDTLines',
    'CONFIG'      : { 'PrescaleB2HHBDT' : 1.,
                      'TrChi2'          : 3,
                      'TrGhostProb'     : 3,
                      'PionPT'          : 1000,
                      'SumPT'           : 4500,
                      'DOCACHI2'        : 9,
                      'BIPCHI2'         : 9,
                      'BDIRA'           : 0.99,
                      'BPT'             : 1200,
                      'BMassWinLow'     : 4700,
                      'BMassWinHigh'    : 6200,
                      'BMassLow'        : 4800,
                      'BMassHigh'       : 6200,
                      'PionIPCHI2'      : 16,
                      'BFDCHI2'         : 100,
                      'BDTCut'          : -0.3,
                      'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HH_BDT_v1r5.xml"
                    },
    'STREAMS'     : ['Bhadron']
    }
}



## Change decay descriptor and re-fit decay tree

class B2HHBDTLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""

    __configuration_keys__ = ('PrescaleB2HHBDT',
                              'TrChi2',
                              'TrGhostProb',
                              'PionPT',
                              'SumPT',
                              'DOCACHI2',
                              'BIPCHI2',
                              'BDIRA', 
                              'BPT',  
                              'BMassWinLow', 
                              'BMassWinHigh',
                              'BMassLow',
                              'BMassHigh',
                              'PionIPCHI2',
                              'BFDCHI2',
                              'BDTCut', 
                              'BDTWeightsFile' 
                               )

    def __init__( self,name,config ) :

        LineBuilder.__init__(self, name, config)

        B2HHBDTName   = "B2HH"

        # make the various stripping selections
        self.B2HHBDT = makeB2HHBDT( B2HHBDTName,
                                    config['TrChi2'],
                                    config['TrGhostProb'],
                                    config['PionPT'],
                                    config['SumPT'],
                                    config['DOCACHI2'],      
                                    config['BIPCHI2'],       
                                    config['BDIRA'],         
                                    config['BPT'],           
                                    config['BMassWinLow'],   
                                    config['BMassWinHigh'],  
                                    config['PionIPCHI2'],    
                                    config['BFDCHI2'],
                                    config['BMassLow'],
                                    config['BMassHigh']       
                                  )

        self.lineB2HHBkg = StrippingLine( B2HHBDTName+"BkgLine",
                                          prescale  = config['PrescaleB2HHBDT'],
                                          selection = self.B2HHBDT,
                                          EnableFlavourTagging = True,
                                          MDSTFlag = True )

        _bdtVars = { "minPT"      : "MINTREE(ABSID=='pi+',PT) / GeV",
                     "minIPCHI2"  : "MINTREE(ABSID=='pi+',MIPCHI2DV(PRIMARY))",
                     "maxPT"      : "MAXTREE(ABSID=='pi+',PT) / GeV",
                     "maxIP"      : "MAXTREE(ABSID=='pi+',MIPCHI2DV(PRIMARY))",
                     "doca"       : "PFUNA ( ADOCA ( 1 , 2 ) )",
                     "vertexCHI2" : "VFASPF(VCHI2/VDOF)",
                     "bPT"        : "PT / GeV",
                     "bIPCHI2"    : "BPVIPCHI2()",
                     "bFDCHI2"    : "BPVVDCHI2" }

        self.CutBDT  = applyBDT( "CutBDT_" + B2HHBDTName,
                                 SelB2HHBDT     = self.B2HHBDT,
                                 BDTVars        = _bdtVars,
                                 BDTCutValue    = config['BDTCut'],
                                 BDTWeightsFile = config['BDTWeightsFile']
                               )


        self.lineB2HHBDT = StrippingLine( B2HHBDTName+"BDTLine",
                                          prescale  = config['PrescaleB2HHBDT'],
                                          selection = self.CutBDT,
                                          EnableFlavourTagging = True,
                                          MDSTFlag = True )

        self.registerLine(self.lineB2HHBDT)
        self.registerLine(self.lineB2HHBkg)

def makeB2HHBDT( name,
                 trChi2,trGhostProb,minPT,sumPT,docachi2,
                 bipchi2,bdira,bpt,massWinLow,massWinHigh,pionipchi2,bfdchi2,
                 massLow,massHigh) :

    _daughters_cuts = "(TRGHOSTPROB < %(trGhostProb)s) & (TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & (MIPCHI2DV(PRIMARY) > %(pionipchi2)s )" %locals()

    _combination_cuts = "( (APT1 + APT2) > %(sumPT)s ) & ( AM > %(massWinLow)s ) & ( AM < %(massWinHigh)s ) & ( ACUTDOCACHI2( %(docachi2)s, '' ) )" %locals()

    _mother_cuts = "( PT > %(bpt)s ) & ( BPVDIRA > %(bdira)s ) & ( BPVIPCHI2() < %(bipchi2)s ) & ( BPVVDCHI2 > %(bfdchi2)s ) & ( M > %(massLow)s * MeV ) & ( M < %(massHigh)s * MeV )" %locals()


    CombineB2HHBDT = CombineParticles( DecayDescriptor = 'B0 -> pi+ pi-',
                                       DaughtersCuts = { "pi+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts,
                                       ReFitPVs = True )

    return Selection( name,
                      Algorithm = CombineB2HHBDT,
                      RequiredSelections = [ StdNoPIDsPions ] )


def applyBDT( name,
              SelB2HHBDT,
              BDTVars,
              BDTCutValue,
              BDTWeightsFile ):

    from MVADictHelpers import addTMVAclassifierValue
    from Configurables import FilterDesktop as MVAFilterDesktop

    _FilterB = MVAFilterDesktop( name + "Filter",
                                 Code = "VALUE('LoKi::Hybrid::DictValue/" + name + "')>" + str(BDTCutValue)  )

    addTMVAclassifierValue( Component = _FilterB,
                            XMLFile   = BDTWeightsFile,
                            Variables = BDTVars,
                            ToolName  = name )

    return Selection( name,
                      Algorithm =  _FilterB,
                      RequiredSelections = [ SelB2HHBDT ] )


########################################################################

