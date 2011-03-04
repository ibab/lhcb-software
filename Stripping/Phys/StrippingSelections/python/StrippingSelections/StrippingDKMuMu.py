__author__ = 'Thomas Blake'
__date__ = '27-10-10'
__version__ = '$Revision: 1.0 $'


"""
Stripping object for Ds+ -> K- mu+ mu+ search for Majorana neutrinos
"""


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingUtils.Utils import checkConfig
from StrippingConf.StrippingLine import StrippingLine  

defaultConfig = { 
    "D_DIRA"       : 0.9999
    , "D_IPCHI2"   : 9.
    , "D_CHI2"     : 9.
    , "Tr_PT"      : 500.
    , "Tr_IPCHI2"  : 25.
    , "K_PIDK"     : 5.
    , "Ds_MINMASS" : 1900.
    , "D_MAXMASS"  : 2200.
    , "D_MINMASS"  : 1800.
    }

defaultLines = {
    "DsKKPi"   : 0.02
    , "DsKPiPi"  : 0.02
    , "DsKKMu"   : 1.0
    , "DsKPiMu"  : 1.0
    , "DsKMuMu"  : 1.0
    }

class StrippingDKMuMuConf(object):
    """
    Definition of D(s)+ -> K- mu+ mu+ stripping 

    Includes signal and control lines. To include these lines please do :

    from StrippingSelections.StrippingDKMuMu import StrippingDKMuMuConf
    from StrippingSelections.StrippingDKMuMu import defaultConfig
    from StrippingSelections.StrippingDKMuMu import defaultLines

    conf  = StrippingDKMuMuConf( config = defaultConfig, activeLines = defaultLines )
    lines = conf.lines()

    to include a single line do, e.g.

    line_KMuMu = conf.line('DsKMuMu')
    """
    __configuration_keys__ = (
        "D_DIRA"
        , "D_IPCHI2"
        , "D_CHI2"
        , "Ds_MINMASS"
        , "D_MAXMASS"
        , "D_MINMASS"
        , "Tr_PT"
        , "Tr_IPCHI2"
        , "K_PIDK"
        )

    __allowed_lines__ = (
        "DsKKPi", "DsKPiPi" , "DsKKMu" , "DsKPiMu", "DsKMuMu" 
        )
        
    def __init__(self, name="DKMuMu", config = None, activeLines = None ) :
        """
        Constructor makes the selection objects but doesn't make the lines
        """
        checkConfig(StrippingDKMuMuConf.__configuration_keys__,config)

        if activeLines == None:
            raise Exception("No linelist specified")

        for linename in activeLines:
            if not linename in StrippingDKMuMuConf.__allowed_lines__:
                raise Exception("Line " + line + " does not appear in allowed list") 

            
        self.name = name
        self.LineList = activeLines 
        
        self.TrCut      = "(PT> %(Tr_PT)s ) & " \
                          "(MIPCHI2DV(PRIMARY)> %(Tr_IPCHI2)s )" % config

        self.MassWindow = "(AM> %(D_MINMASS)s *MeV) & " \
                          "(AM< %(D_MAXMASS)s *MeV)" % config
        self.DCut       = "(VFASPF(VCHI2PDOF) < %(D_CHI2)s ) & " \
                          "(BPVDIRA> %(D_DIRA)s ) & " \
                          "(BPVIPCHI2() < %(D_IPCHI2)s )" % config 
        
        self.DCut += "&(((ID>0)&(INTREE((ID=='K-')&(PIDK > %(K_PIDK)s )))) | "\
                     "(((ID<0)&(INTREE((ID=='K+')&(PIDK > %(K_PIDK)s ))))))" % config

        self.DsMass     = "(M> %(Ds_MINMASS)s *MeV)" % config

        self.CombKPiPi = self.__DsKPiPi__()
        
        self.DsKKPi  = self.__DsKKPi__()
        self.DsKMuMu = self.__DsKMuMu__()
        self.DsKKMu  = self.__FilterMuon__( "DsKKMu" , self.DsKKPi )
        self.DsKPiMu = self.__FilterMuon__( "DsKPiMu", self.CombKPiPi ) 
        self.DsKPiPi = self.__FilterMass__( "DsKPiPiMassCut", self.CombKPiPi )

        self.ActiveLines = {}
        

    def __FilterMass__( self, suffix, selObject ):
        """
        Filter a selection with lower mass cut
        """
        _filter = FilterDesktop( self.name + "_" + suffix, Code = self.DsMass )
        _sel = Selection( "Sel" + self.name + "_" + suffix,
                          Algorithm = _filter,
                          RequiredSelections = [ selObject ] )
        return _sel
        
    def __FilterMuon__( self, suffix, selObject ):
        """
        Filter a selection, requiring pi to be have ISMUON set. 
        """
        _filter = FilterDesktop( self.name + "_" + suffix ) 
        _filter.Code = "INTREE((ABSID=='pi+')&(ISMUON))" 
        _sel = Selection( "Sel" + self.name + "_" + suffix,
                          Algorithm = _filter, RequiredSelections = [ selObject ] )
        return _sel

    def __DsKKPi__( self ):
        """
        Combine Paricles for Ds -> K K Pi combinations,
        PID required on K with opposite sign to pi.
        """
        _kaons = DataOnDemand( Location = "Phys/StdNoPIDsKaons" )
        _pions = DataOnDemand( Location = "Phys/StdNoPIDsPions" ) 

        _combine = CombineParticles( self.name + "_DsKKPi" )
        _combine.DecayDescriptor = "[D_s+ -> K- K+ pi+]cc"
        _combine.DaughtersCuts = {
            "K+"  : self.TrCut ,
            "pi+" : self.TrCut
            }
        _combine.CombinationCut = self.MassWindow
        _combine.MotherCut = self.DCut 

        _sel = Selection( "Sel" + self.name + "_DsKKPi",
                          Algorithm = _combine,
                          RequiredSelections = [ _kaons, _pions ] )
        return _sel 

    def __DsKPiPi__( self ):
        """
        Combine Paricles for Ds -> K Pi Pi combinations,
        PID required on K with opposite sign to pions.
        """
        _kaons = DataOnDemand( Location = "Phys/StdNoPIDsKaons" )
        _pions = DataOnDemand( Location = "Phys/StdNoPIDsPions" )

        _combine = CombineParticles( self.name+"_DsKPiPi" )
        _combine.DecayDescriptor = "[D_s+ -> K- pi+ pi+]cc"
        _combine.DaughtersCuts =  {
            "K-"  : self.TrCut ,
            "pi+" : self.TrCut
            }
        _combine.CombinationCut = self.MassWindow
        _combine.MotherCut = self.DCut
        
        _sel = Selection( "Sel" + self.name + "_DsKPiPi",
                          Algorithm = _combine,
                          RequiredSelections = [ _kaons, _pions ]  )
        return _sel

    def __DsKMuMu__( self ):
        """
        Combine Paricles for Ds -> K Mu Mu combinations,
        PID required on K.
        """
        _kaons = DataOnDemand( Location = "Phys/StdNoPIDsKaons" )
        _muons = DataOnDemand( Location = "Phys/StdLooseMuons" )

        _combine = CombineParticles( self.name + "_DsKMuMu" )
        _combine.DecayDescriptor = "[D_s+ -> K- mu+ mu+]cc"
        _combine.DaughtersCuts = {
            "K-"  : self.TrCut ,
            "mu+" : self.TrCut
            }
        _combine.CombinationCut = self.MassWindow
        _combine.MotherCut = self.DCut

        _sel = Selection( "Sel" + self.name + "_DsKMuMu",
                          Algorithm = _combine,
                          RequiredSelections = [ _kaons, _muons ] )
        return _sel 

    def line( self, linename ):
        """
        Returns a single line based on its alias
        """
        if not linename in self.LineList:
            raise Exception("Line " + linename + " does not appear in allowed list")

        if not linename in self.ActiveLines:
            _sel = {
                "DsKKPi"  : self.DsKKPi,
                "DsKKMu"  : self.DsKKMu,
                "DsKPiPi" : self.DsKPiPi,
                "DsKPiMu" : self.DsKPiMu,
                "DsKMuMu" : self.DsKMuMu,
                }[linename]
            self.ActiveLines[linename] = StrippingLine( self.name + "With" +linename,
                                                        prescale = self.LineList[linename],
                                                        algos = [ _sel ] )

        _line = self.ActiveLines[linename]
        return _line
                
    def lines( self ):
        """
        Returns a flat list of lines 
        """
        _lines = [ self.line( linename ) for linename in self.LineList ]
        return _lines


