# $Id: StrippingNoPIDDstarWithD02RSKPi.py,v 1.1 2010-09-03 22:38:21 pxing Exp $

__author__  = ['Philip Xing', 'Andrew Powell']
__date__    = '23rd August 2012'
__version__ = '$Revision: 7.0 $'

'''
Configurable for the RICH calibration using D*+ -> pi+ D0( K- pi+).

Usage:
 >>> stream = ...
 >>> from StrippingSelections.StrippingNoPIDDstarWithD02RSKPi import NoPIDDstarWithD02RSKPiConf as builder_dst
 >>> from StrippingSelections.StrippingNoPIDDstarWithD02RSKPi import default_config as config_dst
 >>> lb_dst = builder_dst(name='noPIDDstar', config=config_dst)
 >>> stream.appendLines( lb_dst.lines() )

Results from running over:
$STRIPPINGSELECTIONSROOT/tests/data/Reco13c_Run124134.py

StrippingReport                                                INFO Event 100000, Good event 100000
 |                                    *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingnoPIDDstarLine                           |  0.4800|       480|  1.075|  11.597|
'''

__all__ =('NoPIDDstarWithD02RSKPiConf','default_config')


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
    'NAME'       : 'noPIDDstar',
    'WGs'        : ['ALL'],
    'BUILDERTYPE': 'NoPIDDstarWithD02RSKPiConf',
    'CONFIG'     :{
    
      'DaugPt'           : 250 * MeV      ## GeV
    , 'DaugP'            : 2.0 * GeV      ## GeV
    , 'DaugIPChi2'       : 16             ## unitless
    , 'DaugTrkChi2'      : 5              ## unitless
    , 'D0MassWin'        : 75 * MeV       ## MeV
    , 'D0Pt'             : 1.5 * GeV      ## GeV
    , 'D0VtxChi2Ndof'    : 13             ## unitless
    , 'D0FDChi2'         : 49             ## unitless
    , 'D0BPVDira'        : 0.9999         ## unitless
    , 'D0IPChi2'         : 30             ## unit
    , 'SlowPiPt'         : 150 * MeV      ## MeV
    , 'SlowPiTrkChi2'    : 5              ## unitless
    , 'DstarPt'          : 2.2 * GeV      ## GeV
    , 'DstarVtxChi2Ndof' : 13             ## unitless
    , 'DeltaM_Min'       : 130 * MeV      ## MeV
    , 'DeltaM_Max'       : 155 * MeV      ## MeV
    ##
    , 'DCS_WrongMass'    : 25 * MeV       ## MeV (3 sigma veto)
    , 'KK_WrongMass'     : 25 * MeV       ## MeV (3 sigma veto)
    , 'PiPi_WrongMass'   : 25 * MeV       ## MeV (3 sigma veto)
    ##
    , 'Prescale'         : 0.800          ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ##
    , 'Monitor'          : False          ## Activate the monitoring?
      },
    'STREAMS'     :['PID']

}
class NoPIDDstarWithD02RSKPiConf(LineBuilder) :
    """
    Definition of prompt D*+ -> D0( K- pi+) pi+ stripping lines.
    The the main user method is Lines(), which returns a list of the
    StrippingLine objects defined and configured by this configurable.
    """
    __configuration_keys__ = ( 'DaugPt',
                               'DaugP',
                               'DaugIPChi2',
                               'DaugTrkChi2',
                               'D0MassWin',
                               'D0Pt', 
                               'D0VtxChi2Ndof',
                               'D0FDChi2', 
                               'D0BPVDira', 
                               'D0IPChi2',
                               'SlowPiPt',
                               'SlowPiTrkChi2',
                               'DstarPt',
                               'DstarVtxChi2Ndof',
                               'DeltaM_Min',
                               'DeltaM_Max',
                               ##
                               'DCS_WrongMass',
                               'KK_WrongMass',
                               'PiPi_WrongMass',
                               ##
                               'Prescale',
                               'Postscale',
                               ##
                               'Monitor'
                               )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.selD02KPi = D0(name,
                            config,
                            Monitor=config['Monitor']
                            )

        self.selDstar2D0Pi = Dstar(name,
                                   self.selD02KPi,
                                   config,
                                   Monitor=config['Monitor']
                                   )

        # 'StripDstarNoPIDsWithD02RSKPiLine'
        self.Dstar2D0Pi_line = StrippingLine(name+'Line',
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selDstar2D0Pi,
                                             RequiredRawEvents = ["Muon"])  

        self.registerLine(self.Dstar2D0Pi_line)


def D0 ( name,
         config,
         Monitor
         ) :
    """
    Selection for D0
    """
    
    _D0 = CombineParticles()
    _D0.DecayDescriptor = "[D0 -> K- pi+]cc"
    dauCutStr = "(PT > %(DaugPt)s) & (P > %(DaugP)s) & (TRCHI2DOF < %(DaugTrkChi2)s) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s)" %locals()['config']
    _D0.DaughtersCuts = { "K+" : dauCutStr,
                          "pi+" : dauCutStr
                          }
    _D0.CombinationCut = "(ADAMASS('D0')<85 * MeV)" %locals()['config']
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
    
    if Monitor != None :    
        _D0.Preambulo    = [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "mass     = monitor ( M / GeV , Histo ( 'D0' , 1.79 , 1.94 , 100 ) , 'M'     ) " ,
            "pT       = monitor ( PT / GeV , Histo ( 'D0' , 0.00 , 20.0 , 100 ) , 'pT'   ) " ,
            "y        = monitor ( Y , Histo ( 'D0' , 1.60 , 5.40 , 100 ) , 'y'           ) "
            ]
        _D0.Monitor       = True
        _D0.HistoProduce  = True
        _D0.MotherMonitor = """ process ( mass )
        >> process ( pT )
        >> process ( y )
        >> EMPTY
            """
            
    D0 = Selection("SelD02RSKPiFor"+name,
                   Algorithm = _D0,
                   RequiredSelections = [StdAllNoPIDsKaons,
                                         StdAllNoPIDsPions])
    return D0
    
def Dstar ( name,
            D0Sel,
            config,
            Monitor) :
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
    
    if Monitor != None :    
        _DSt.Preambulo    = [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "mass     = monitor ( M / GeV ,    Histo ( 'DSt' , 1.93 , 2.09 , 100 ) , 'M'     ) " ,
            "deltaM   = monitor ( M - M1 /MeV, Histo ( 'DSt' , 135  , 160  , 50  ) , 'DeltaM') " ,
            "pT       = monitor ( PT / GeV ,   Histo ( 'DSt' , 0.00 , 20.0 , 100 ) , 'pT'    ) " ,
            "y        = monitor ( Y ,          Histo ( 'DSt' , 1.60 , 5.40 , 100 ) , 'y'     ) "
            ]
        _DSt.Monitor       = True
        _DSt.HistoProduce  = True
        _DSt.MotherMonitor = """ process ( mass )
        >> process ( deltaM )
        >> process ( pT )
        >> process ( y )
        >> EMPTY
        """
        
    DSt = Selection("SelDSt2D0PiFor"+name,
                    Algorithm = _DSt,
                    RequiredSelections = [StdAllNoPIDsPions,
                                          D0Sel])
    
    return DSt
