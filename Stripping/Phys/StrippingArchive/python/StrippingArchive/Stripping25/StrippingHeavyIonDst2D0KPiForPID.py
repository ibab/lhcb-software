# $Id: StrippingDst2D0KPiForPID.py,v 1.1 2010-09-03 22:38:21 pxing Exp $

__author__  = ['Philip Xing', 'Andrew Powell']
__date__    = '23rd August 2012'
__version__ = '$Revision: 7.0 $'

'''
Configurable for the RICH calibration using D*+ -> pi+ D0( K- pi+).

Usage:
 >>> stream = ...
 >>> from StrippingSelections.StrippingDst2D0KPiForPID import Dst2D0KPiForPIDConf as builder_dst
 >>> from StrippingSelections.StrippingDst2D0KPiForPID import default_config as config_dst
 >>> lb_dst = builder_dst(name='noPIDDstar', config=config_dst)
 >>> stream.appendLines( lb_dst.lines() )

'''

__all__ =('HeavyIonDst2D0KPiForPIDConf','default_config',"D0","Dstar")


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiKernel.SystemOfUnits import mm, cm , MeV, GeV
## from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsPions


default_config =  {
    'NAME'       : 'HeavyIonNoPIDDstar',
    'WGs'        : ['ALL'],
    'BUILDERTYPE': 'HeavyIonDst2D0KPiForPIDConf',
    'CONFIG'     :{
    
      'DaugPt'           : 250 * MeV      ## GeV
    , 'DaugP'            : 2.0 * GeV      ## GeV
    , 'DaugIPChi2'       : 16             ## unitless
    , 'DaugTrkChi2'      : 3              ## unitless
    , 'D0MassWin'        : 85 * MeV       ## MeV
    , 'D0Pt'             : 1.5 * GeV      ## GeV
    , 'D0VtxChi2Ndof'    : 10             ## unitless
    , 'D0FDChi2'         : 49             ## unitless
    , 'D0BPVDira'        : 0.9999         ## unitless
    , 'D0IPChi2'         : 20             ## unit
    , 'SlowPiPt'         : 100 * MeV      ## MeV
    , 'SlowPiTrkChi2'    : 3              ## unitless
    , 'DstarPt'          : 2.2 * GeV      ## GeV
    , 'DstarVtxChi2Ndof' : 15             ## unitless
    , 'DeltaM_Min'       : 130 * MeV      ## MeV
    , 'DeltaM_Max'       : 155 * MeV      ## MeV
    ##
    , 'DCS_WrongMass'    : 25 * MeV       ## MeV (3 sigma veto)
    , 'KK_WrongMass'     : 25 * MeV       ## MeV (3 sigma veto)
    , 'PiPi_WrongMass'   : 25 * MeV       ## MeV (3 sigma veto)
    ##
    , 'Prescale'         : 1.00          ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ,"odin": ["NoBeam","Beam1","Beam2","BeamCrossing"]
    , 'Hlt1Filter'          : None  # "HLT_PASS_RE('Hlt1SMOGKPiDecision')"
    ,'Hlt2Filter'          : "HLT_PASS_RE('Hlt2PIDD02KPiTagDecision')"   # no events in 1000, The 'name' is doubly checked correct, because rate too low?

    ##
      },
    'STREAMS'     :['ALL']

}
class HeavyIonDst2D0KPiForPIDConf(LineBuilder) :
    """
    Definition of prompt D*+ -> D0( K- pi+) pi+ stripping lines.
    The the main user method is Lines(), which returns a list of the
    StrippingLine objects defined and configured by this configurable.
    """
    __configuration_keys__ =  default_config['CONFIG'].keys()

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        odin = "|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in config['odin'] if odin_type in ["NoBeam","Beam1","Beam2","BeamCrossing"]])
        self.selD02KPi = D0(name, config)

        self.selDstar2D0Pi = Dstar(name, self.selD02KPi, config)

        # 'StripDstarNoPIDsWithD02RSKPiLine'
        self.Dstar2D0Pi_line = StrippingLine(name+'Line',
             prescale = config['Prescale'],
             postscale = config['Postscale'],
             selection = self.selDstar2D0Pi,
             RequiredRawEvents = ["Muon"],
             HLT1       =config['Hlt1Filter'],
             HLT2       =config['Hlt2Filter'],
             ODIN      = odin
             )  

        self.registerLine(self.Dstar2D0Pi_line)


def D0 ( name, config) :
    """
    Selection for D0
    """
    
    _D0 = CombineParticles()
    _D0.DecayDescriptor = "[D0 -> K- pi+]cc"
    dauCutStr = "(PT > %(DaugPt)s) & (P > %(DaugP)s) & (TRCHI2DOF < %(DaugTrkChi2)s) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s)" %locals()['config']
    _D0.DaughtersCuts = { "K+" : dauCutStr,
                          "pi+" : dauCutStr
                          }
    _D0.CombinationCut = "(AMINDOCA('')<0.5)&(ADAMASS('D0')<85 * MeV)" %locals()['config']
    mothercut = """
    (PT>%(D0Pt)s)
    & (VFASPF(VCHI2PDOF)<%(D0VtxChi2Ndof)s)
    & (BPVVDCHI2>%(D0FDChi2)s)
    & (BPVDIRA>%(D0BPVDira)s)
    & (BPVIPCHI2()<%(D0IPChi2)s)
    & (ADMASS('D0') < %(D0MassWin)s )
    & ( ADWM( 'D0' , WM( 'pi-' , 'K+') ) > %(DCS_WrongMass)s)
    """
# take these out as they shape the sidebands
   # & ( ADWM( 'D0' , WM( 'K-' , 'K+') ) > %(KK_WrongMass)s)
   # & ( ADWM( 'D0' , WM( 'pi-' , 'pi+') ) > %(PiPi_WrongMass)s)
   # """
    _D0.MotherCut = mothercut %locals()['config']
    
    D0 = Selection("SelD02RSKPiFor"+name,
                   Algorithm = _D0,
                   RequiredSelections = [StdAllNoPIDsKaons,
                                         StdAllNoPIDsPions])
    return D0
    
def Dstar ( name, D0Sel, config) :
    """
    Selection for D*
    """
    
    _DSt = CombineParticles()
    _DSt.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
    slowPiCuts = "(PT>%(SlowPiPt)s) & (TRCHI2DOF < %(SlowPiTrkChi2)s)" %locals()['config']
    d0Cuts = "ALL"
    _DSt.DaughtersCuts   = { 'pi+' : slowPiCuts, 'D0' : d0Cuts }
    combcut = """
    (APT>%(DstarPt)s)
    & (ADAMASS('D*(2010)+')<75)
    & (AM - AM1 < 165 * MeV) 
    """
    _DSt.CombinationCut = combcut %locals()['config']
    mothercut = """
    (VFASPF(VCHI2PDOF)<%(DstarVtxChi2Ndof)s)
    & (M-MAXTREE('D0'==ABSID,M)<%(DeltaM_Max)s)
    & (M-MAXTREE('D0'==ABSID,M)>%(DeltaM_Min)s)
    """
    _DSt.MotherCut =  mothercut %locals()['config']
    
    DSt = Selection("SelDSt2D0PiFor"+name,
                    Algorithm = _DSt,
                    RequiredSelections = [StdAllNoPIDsPions,
                                          D0Sel])
    
    return DSt
