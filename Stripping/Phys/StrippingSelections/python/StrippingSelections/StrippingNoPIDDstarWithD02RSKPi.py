# $Id: StrippingNoPIDDstarWithD02RSKPi.py,v 1.1 2010-09-03 22:38:21 pxing Exp $

__author__ = ['Philip Xing', 'Andrew Powell']
__date__ = '1 November 2010'
__version__ = '$Revision: 3.0 $'

'''
Configurable for the RICH calibration using D*+ -> pi+ D0( K- pi+).
'''
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiKernel.SystemOfUnits import mm, cm , MeV, GeV
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand

class StrippingNoPIDDstarWithD02RSKPiConf(LHCbConfigurableUser) :
    """
    Definition of prompt D*+ -> D0( K- pi+) pi+ stripping lines.
    The the main user method is Lines(), which returns a list of the
    StrippingLine objects defined and configured by this configurable.
    """
    __slots__ = {
        'DaugPt'             : 300 * MeV      ## GeV
        , 'DaugP'            : 2.0 * GeV      ## GeV
        , 'DaugIPChi2'       : 16             ## unitless
        , 'DaugTrkChi2'      : 10             ## unitless
        , 'D0MassWin'        : 75 * MeV       ## MeV
        , 'D0Pt'             : 1.5 * GeV      ## GeV
        , 'D0VtxChi2Ndof'    : 13             ## unitless
        , 'D0FDChi2'         : 49             ## unitless
        , 'D0BPVDira'        : 0.9999         ## unitless
        , 'D0IPChi2'         : 30             ## unit
        , 'SlowPiPt'         : 150 * MeV      ## MeV
        , 'DstarPt'          : 2.2 * GeV      ## GeV
        , 'DstarVtxChi2Ndof' : 13             ## unitless
        , 'DeltaM'           : 2 * MeV        ## MeV
        ##
        , 'DCS_WrongMass'    : 25 * MeV       ## MeV (3 sigma veto)
        , 'KK_WrongMass'     : 25 * MeV       ## MeV (3 sigma veto)
        , 'PiPi_WrongMass'   : 25 * MeV       ## MeV (3 sigma veto)
        ##
        , 'Prescale'         : 0.42           ## unitless
        ##
        , 'Monitor'          : False          ## Activate the monitoring?
        }

    def Lines ( self ) :

        lines = [ self.DStLine() ]
        return lines

    def DStLine( self ):
        from StrippingConf.StrippingLine import StrippingLine
        Dstar2D0PiSel = self.Dstar()
        return StrippingLine('StripDstarNoPIDsWithD02RSKPiLine',
                             prescale = self.getProp('Prescale'),
                             algos = [Dstar2D0PiSel])  

    def D0 ( self ) :
        """
        Selection for D0
        """
        Pions = DataOnDemand( Location = "Phys/StdNoPIDsPions" )
        Kaons = DataOnDemand( Location = "Phys/StdNoPIDsKaons" )

        _D0 = CombineParticles("StripD0")
        _D0.DecayDescriptor = "[D0 -> K- pi+]cc"
        dauCutStr = "(PT > %(DaugPt)g) & (P > %(DaugP)g) & (TRCHI2DOF < %(DaugTrkChi2)g) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)g)" % self.getProps()
        _D0.DaughtersCuts = { "K+" : dauCutStr,
                              "pi+" : dauCutStr
                              }
        _D0.CombinationCut = "(ADAMASS('D0')<%(D0MassWin)g)" % self.getProps()
        mothercut = """
                    (PT>%(D0Pt)g)
                    & (VFASPF(VCHI2PDOF)<%(D0VtxChi2Ndof)g)
                    & (BPVVDCHI2>%(D0FDChi2)g)
                    & (BPVDIRA>%(D0BPVDira)g)
                    & (BPVIPCHI2()<%(D0IPChi2)g)
                    & ( ADWM( 'D0' , WM( 'pi-' , 'K+') ) > %(DCS_WrongMass)g)
                    & ( ADWM( 'D0' , WM( 'K-' , 'K+') ) > %(KK_WrongMass)g)
                    & ( ADWM( 'D0' , WM( 'pi-' , 'pi+') ) > %(PiPi_WrongMass)g)
                    """
        _D0.MotherCut = mothercut % self.getProps()

        if self.getProp ( 'Monitor' ) :    
            _D0.Preambulo    += [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "mass     = monitor ( M / GeV , Histo ( 'D0' , 1.79 , 1.94 , 100 ) , 'M'     ) " ,
                "pT       = monitor ( PT / GeV , Histo ( 'D0' , 0.00 , 20.0 , 100 ) , 'pT'   ) " ,
                "y        = monitor ( Y , Histo ( 'D0' , 1.60 , 5.40 , 100 ) , 'y'           ) "
                ]
            _D0.Monitor       = True
            _D0.HistoProduce  = True
            _D0.HistoPrint    = True
            _D0.MotherMonitor = """ process ( mass )
            >> process ( pT )
            >> process ( y )
            >> EMPTY
            """
        
        D0 = Selection("SelD02RSKPi",
                        Algorithm = _D0,
                        RequiredSelections = [Kaons,
                                              Pions])
        return D0

    def Dstar ( self ) :
        """
        Selection for D*
        """
        Pions = DataOnDemand( Location = "Phys/StdNoPIDsPions" )
        
        _DSt = CombineParticles("StripDSt")
        _DSt.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
        slowPiCuts = "(PT>%(SlowPiPt)s)" % self.getProps()
        d0Cuts = "ALL"
        _DSt.DaughtersCuts   = { 'pi+' : slowPiCuts, 'D0' : d0Cuts }
        combcut = """
        (APT>%(DstarPt)g)
        & (ADAMASS('D*(2010)+')<75)
        """
        _DSt.CombinationCut = combcut % self.getProps()
        mothercut = """
        (VFASPF(VCHI2PDOF)<%(DstarVtxChi2Ndof)g)
        & (M-MAXTREE('D0'==ABSID,M)<(145.5+%(DeltaM)g))
        & (M-MAXTREE('D0'==ABSID,M)>(145.5-%(DeltaM)g))
        """
        _DSt.MotherCut =  mothercut % self.getProps()

        if self.getProp ( 'Monitor' ) :    
            _DSt.Preambulo    += [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "mass     = monitor ( M / GeV , Histo ( 'DSt' , 1.93 , 2.09 , 100 ) , 'M'     ) " ,
                "pT       = monitor ( PT / GeV , Histo ( 'DSt' , 0.00 , 20.0 , 100 ) , 'pT'   ) " ,
                "y        = monitor ( Y , Histo ( 'DSt' , 1.60 , 5.40 , 100 ) , 'y'           ) "
                ]
            _DSt.Monitor       = True
            _DSt.HistoProduce  = True
            _DSt.HistoPrint    = True
            _DSt.MotherMonitor = """ process ( mass )
            >> process ( pT )
            >> process ( y )
            >> EMPTY
            """
            
        DSt = Selection("SelDSt2D0Pi",
                        Algorithm = _DSt,
                        RequiredSelections = [Pions,
                                              self.D0()])

        return DSt

    def getProps ( self ) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
