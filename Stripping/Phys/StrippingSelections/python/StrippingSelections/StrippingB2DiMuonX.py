
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand, MergedSelection
from StrippingSelections.Utils import checkConfig
from StrippingConf.StrippingLine import StrippingLine  

defaultConfig = { 
    "DiMuonFD"              :       64.     # dimensionless
    ,       "MuonPT"        :       500     # MeV
    ,       "MuonIP"        :       16.     # dimensionless
    ,       "DiMuonVCHI2"   :       10.     # adimensional
    ,       "DiMuonTrDOCA"  :       0.2     # mm
    ,       "DiMuonTrSumPT" :       2500.   # MeV
    ,       "DiMuonTrMass"  :       2500.   # MeV
    ,       "DiMuonTrVCHI2" :       10      # dimensionless
    ,       "DiMuonTrDIRA"  :       0.997   # dimensionless
    ,       "DiMuonTrMCORLow"  :    4000.0  # MeV
    ,       "DiMuonTrMCORHigh" :    6000.0  # MeV
    ,       "DiMuonTrFD"   :        144.    # dimensionless
    ,       "TrPT"         :        500.    # MeV
    ,       "TrIP"         :        16.     # dimensionless 
    ,       "KSMass"       :        20.     # MeV
    ,       "KSVCHI2"      :        10      # dimensionless
    ,       "TrTrigger"    :        "HLT_PASS_RE('Hlt2.*(MuTrack|Topo).*Decision')"
    ,       "TrPrescale"   :        1.0
    ,       "KSTrigger"    :        "HLT_PASS_RE('Hlt2.*(MuTrack|Topo).*Decision')"
    ,       "KSPrescale"   :        1.0
    }

class StrippingB2DiMuonXConf(object):
    """
    Definition of B(s,d,u) -> mu mu track (or KS) stripping.

    to include this lines please do :

    from StrippingSelections.StrippingB2DiMuonX import StrippingB2DiMuonXConf
    from StrippingSelections.StrippingB2DiMuonX import defaultConfig as DiMuonXConfig
    DiMuonXConf = StrippingB2DiMuonXConf( config = DiMuonXConfig )
    lines = DiMuonXConf.lines()

    Concept of minimal mass is used to correct B mass for missing pi, K, KS0, pi0 etc.
    """
    __configuration_keys__ = (
        "DiMuonFD" 
        ,       "DiMuonVCHI2" 
        ,       "MuonPT"
        ,       "MuonIP"
        ,       "DiMuonTrFD"
        ,       "DiMuonTrDOCA" 
        ,       "DiMuonTrSumPT"
        ,       "DiMuonTrMass"
        ,       "DiMuonTrVCHI2"
        ,       "DiMuonTrDIRA"
        ,       "TrPT"        
        ,       "TrIP"        
        ,       "KSMass"      
        ,       "KSVCHI2"   
        ,       "DiMuonTrMCORLow"
        ,       "DiMuonTrMCORHigh"
        ,       "TrTrigger"    
        ,       "TrPrescale"   
        ,       "KSTrigger"    
        ,       "KSPrescale"   
        )
    
    def __init__(self, name="DiMuonX", config = None) :
        """
        Constructor makes the selection objects but doesn't make the lines
        """
        checkConfig(StrippingB2DiMuonXConf.__configuration_keys__,config)
        
        self.name = name
        
        self.DiMuon = self.__DiMuon__( config )
        self.DiMuonTr = self.__DiMuonTr__( config )
        self.DiMuonKS = self.__DiMuonKS__( config )
        
        self.TrLinePrescale = config["TrPrescale"]
        self.TrLineTrigger  = config["TrTrigger"]
        self.KSLinePrescale = config["KSPrescale"]
        self.KSLineTrigger  = config["KSTrigger"]
        
        self.TrLine = None
        self.KSLine = None

    def __CombinationCut__( self, conf ):
        """
        Defines the combination cut for the B, cutting on M(MuMuX), sum pT and DOCA.
        """
        _cut = "(AM > %(DiMuonTrMass)s *MeV ) & "\
               "(ASUM(PT) > %(DiMuonTrSumPT)s *MeV) & " \
               "(ADOCAMAX('') < %(DiMuonTrDOCA)s *mm)" % conf 
        return _cut

    def __MotherCut__( self, conf ):
        """
        Defines the mother cut for the B, cutting on FD, DIRA, minimial mass.
        """
        _cut = "(VFASPF(VCHI2/VDOF)<%(DiMuonTrVCHI2)s ) & " \
               "(BPVDIRA>%(DiMuonTrDIRA)s ) & " \
               "(BPVVDCHI2 > %(DiMuonTrFD)s ) & " \
               "(MCOR > %(DiMuonTrMCORLow)s *MeV) & " \
               "(MCOR < %(DiMuonTrMCORHigh)s *MeV)" % conf
        return _cut

    def __TrCut__( self, conf ):
        """
        Defines the cut for X=pi daughter. 
        """
        _cut = "(PT > %(TrPT)s ) & " \
               "(MIPCHI2DV(PRIMARY) > %(TrIP)s )" % conf 
        return _cut
        
    def __KS0Cut__( self, conf ):
        """
        Defines the cut for X=KS0 daughter
        """
        _cut = "(DMASS('KS0')<%(KSMass)s *MeV) & " \
               "(VFASPF(VCHI2/VDOF)<%(KSVCHI2)s ) & " \
               "(PT>%(TrPT)s *MeV) & " \
               "(MIPCHI2DV(PRIMARY)>%(TrIP)s)" % conf
        return _cut

    def __DiMuonCut__( self, conf ):
        """
        Defines the cut on the dimuon
        """
        _cut = "(2 == NINTREE(('mu-'==ABSID)&(PT > %(MuonPT)s )&(MIPCHI2DV(PRIMARY)> %(MuonIP)s) )) & " \
               "(VFASPF(VCHI2/VDOF)<%(DiMuonVCHI2)s) & " \
               "(BPVVDCHI2> %(DiMuonFD)s )" % conf
        return _cut
    
    def __DiMuon__( self , conf ):
        """
        Biased dimuon selection object from StdVeryLooseDiMuon. 
        """    
        _muons =  DataOnDemand(Location = 'Phys/StdVeryLooseDiMuon')
        _dimuon = FilterDesktop(self.name + "_FilterDiMuon")
        _dimuon.Code = self.__DiMuonCut__(conf)
        
        _sel = Selection( "Sel" + self.name + "_DiMuon", RequiredSelections=[_muons],  Algorithm=_dimuon )
        return _sel 

    def __DiMuonTr__( self, conf  ):
        """
        Biased B -> DiMuon + track (with pT, IP)  selection object  
        """
        _tracks = DataOnDemand(Location = 'Phys/StdNoPIDsPions' )
                
        _preambulo = [ "PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                       "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS" ]

        _dimuonTrack = CombineParticles(self.name + "_DiMuonTr")
        _dimuonTrack.Preambulo = _preambulo 
        _dimuonTrack.DecayDescriptor = "[B+ -> pi+ J/psi(1S)]cc"
        _dimuonTrack.DaughtersCuts  = {
            "pi+" : self.__TrCut__(conf)
            }
        _dimuonTrack.CombinationCut = self.__CombinationCut__(conf)
        _dimuonTrack.MotherCut      = self.__MotherCut__(conf) 
                
        _sel = Selection("Sel" + self.name + "_DiMuonTr", Algorithm = _dimuonTrack,
                         RequiredSelections = [ _tracks, self.DiMuon ] )
        
        return _sel
    
    def __DiMuonKS__( self, conf ):
        """
        Biased B -> DiMuon + KS0 selection object 
        """
        _KSDD = DataOnDemand(Location = 'Phys/StdLooseKsDD' )
        _KSLL = DataOnDemand(Location = 'Phys/StdLooseKsLL' )

        _preambulo = [ "PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                       "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS" ]
        
        _dimuonKS = CombineParticles(self.name + "_DiMuonKS")
        _dimuonKS.Preambulo = _preambulo 
        _dimuonKS.DecayDescriptor = "B0 -> KS0 J/psi(1S)"
        _dimuonKS.DaughtersCuts  = {
            "KS0" : self.__KS0Cut__(conf)
            }
        _dimuonKS.CombinationCut = self.__CombinationCut__(conf)
        _dimuonKS.MotherCut = self.__MotherCut__(conf) 
       
    	_KsMerged = MergedSelection( self.name + "_MergedKS", RequiredSelections = [_KSLL, _KSDD] )	
 
        _sel = Selection( "Sel" + self.name + "_DiMuonKS",
                          Algorithm = _dimuonKS,
                          RequiredSelections = [ _KsMerged, self.DiMuon ] )        
        return _sel
    
    def LineWithTr( self ):
        """
        Line with B -> MuMu X(=pi).
        """
        if self.TrLine == None:
            self.TrLine = StrippingLine( self.name + '_TrLine',
                                         HLT = self.TrLineTrigger,
                                         prescale = self.TrLinePrescale,
                                         algos = [ self.DiMuonTr ] )
        return self.TrLine
        
    def LineWithKS( self ):
        """
        Line with B -> MuMu X(=KS0).
        """
        if self.KSLine == None:
            self.KSLine = StrippingLine( self.name + '_KSLine',
                                         HLT = self.KSLineTrigger,
                                         prescale = self.KSLinePrescale,
                                         algos = [ self.DiMuonKS ] )
                                         
        return self.KSLine
        
    def lines( self ):
        """
        Returns a flat list of lines 
        """
        _lines = [ self.LineWithTr() , self.LineWithKS() ] 
        return _lines


