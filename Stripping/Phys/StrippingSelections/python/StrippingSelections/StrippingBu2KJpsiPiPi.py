# $Id: StrippingBu2KJpsiPiPi.py,v 1.2 2010-08-26 13:16:50 nmangiaf Exp $

__author__ = ['Nicola Mangiafave']
__date__ = '19/03/2010'
__version__ = '$Revision: 1.2 $'

'''
Bu->K J/psi(MuMu) Pi Pi stripping selection using LoKi::Hybrid and python configurables.
Implements a looser stripping line can for both Bu->K X(3872)(J/psi(MuMu) Rho(PiPi)) and
Bu->K Psi(2S)(J/psi(MuMu)PiPi) and one tighter line for each channel.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

class StrippingBu2KJpsiPiPiConf(LHCbConfigurableUser):
    """
    Definition of Bd->K J/psi(MuMu) Pi Pi stripping.
    """
    __slots__ = { 
    			"PionMIPCHI2DVLoose"	: 2.0	# adimensional
                ,	"PionMIPCHI2DV" 	: 3.0	# adimensional        
		,	"MuonMIPCHI2DVLoose"	: 1.0	# adimensional
		,	"MuonMIPCHI2DV" 	: 1.0	# adimensional       
		,	"KaonMIPCHI2DVLoose"	: 2.0	# adimensional
		,	"KaonMIPCHI2DV"	        : 3.0	# adimensional
                ,	"MuonPT"   	        : 500.0	# MeV
                ,	"MuonTRCHI2DOF"         : 10.0	# adimensional
		,	"JpsiMassWinLoose"   	: 100.0	# MeV
                ,	"JpsiMassWin"   	: 80.0	# MeV    
		,	"JpsiVCHI2VDOF" 	: 15.0	# adimensional
		,	"JpsiPiPiMassWinLoose"	: 190.0	# MeV
                ,	"X3872MassWinADA"	: 180.0	# MeV, equal to Psi2S mass window, Used in ADAMASS 
		,	"X3872MassWin"		: 150.0	# MeV, equal to Psi2S mass window 
		,	"JpsiPiPiVCHI2VDOFLoose": 10.0	# adimensional
                ,	"X3872VCHI2VDOF"        : 8.0	# adimensional, equal to Psi2S vertex chi2/dof
		,	"BuVCHI2DOFLoose" 	: 10.0	# adimensional
		,	"BuVCHI2DOF" 		: 8.0	# adimensional
		,	"BuMIPCHI2DVLoose"	: 40.0	# adimensional                        
		,	"BuMIPCHI2DV"    	: 20.0	# adimensional
		,	"BuBPVVDCHI2Loose"	: 10.0	# adimensional                        
		,	"BuBPVVDCHI2"    	: 20.0	# adimensional
		,	"BuMassWinLoose"	: 500.0	# MeV
		,	"BuMassWinADA"  	: 430.0	# MeV, used in ADAMASS    
		,	"BuMassWin"		: 400.0	# MeV
             }

    # Selections:

    # These declarations are needed,
    # otherwise I make multiple isnstances of the loose selections and the selection.py
    # complains because I use the same name for two different selections.
    # With the following procedure I create only one instance of the loose selection.
    
    Jpsi_loose = None
    Rho_loose = None
    X3872_loose = None
    Bu_Loose = None
    
    # Loose Selection:
     	
    def Jpsi2MuMuLoose( self ):
        StdLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdLooseJpsi2MuMu")
	_JpsiFilter = FilterDesktop("JpsiFilterForBu2KJpsiPiPiLoose")
	_JpsiFilter.Code = " (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
                           "& (MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV)" \
                           "& (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" \
        	           "& (MINTREE('mu+'==ABSID, (MIPCHI2DV(PRIMARY))) > %(MuonMIPCHI2DVLoose)s)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2VDOF)s)" % self.getProps()

	Jpsi = Selection("SelJpsi2MuMuForBu2KJpsiPiPiLoose",
                         Algorithm = _JpsiFilter,
                         RequiredSelections = [StdLooseJpsi2MuMu])
         
        StrippingBu2KJpsiPiPiConf.Jpsi_loose = Jpsi        
	return Jpsi
   
    def Rho2PiPiLoose( self ):
        '''
        Despite is called Rho -> Pi Pi it is just a Pi Pi selection,
        the Rho request is unnecessary.
        '''
        Pi = DataOnDemand(Location = "Phys/StdLoosePions")
	_RhoLoose = CombineParticles("Rho2PiPiLoose")
      	_RhoLoose.DecayDescriptor = "rho(770)0 -> pi+ pi-"
        _RhoLoose.CombinationCut = "(AM < 850)"
        _RhoLoose.DaughtersCuts = { "pi+" : " MIPCHI2DV(PRIMARY) > %(PionMIPCHI2DVLoose)s " % self.getProps()}
        _RhoLoose.MotherCut = "ALL"
        
	RhoLoose = Selection("SelRho2PiPiForBu2KJpsiPiPiLoose",
                             Algorithm = _RhoLoose,
                             RequiredSelections = [Pi])

        StrippingBu2KJpsiPiPiConf.Rho_loose = RhoLoose
        return RhoLoose

    def X38722JpsiRhoLoose( self ):
        '''
        Despite is called X(3872) -> J/Psi Rho it is just a J/Psi pi pi selection with a
        large mass window around the X(3872) mass. This mass window includes also the Psi(2S).
        '''
        if StrippingBu2KJpsiPiPiConf.Jpsi_loose == None:
            self.Jpsi2MuMuLoose( )
        if StrippingBu2KJpsiPiPiConf.Rho_loose == None:
            self.Rho2PiPiLoose( )            
	_X3872Loose = CombineParticles("X38722JpsiRhoLoose")
      	_X3872Loose.DecayDescriptor = "X_1(3872) -> J/psi(1S) rho(770)0"
        _X3872Loose.CombinationCut = "(ADAMASS('X_1(3872)') < %(JpsiPiPiMassWinLoose)s *MeV) "\
                                     "| (ADAMASS('psi(2S)') < %(JpsiPiPiMassWinLoose)s *MeV)" % self.getProps()
        _X3872Loose.MotherCut = " (VFASPF(VCHI2/VDOF) < %(JpsiPiPiVCHI2VDOFLoose)s)" % self.getProps()

	X3872Loose = Selection("SelX38722JpsiRhoForBu2KJpsiPiPiLoose",
                               Algorithm = _X3872Loose,
                               RequiredSelections = [StrippingBu2KJpsiPiPiConf.Rho_loose,
                                                     StrippingBu2KJpsiPiPiConf.Jpsi_loose])

        StrippingBu2KJpsiPiPiConf.X3872_loose = X3872Loose
        return X3872Loose

    def Bu2KX3872Loose( self ):
        '''
        Despite is called B+ -> K+ X_1(3872) it is just a B+ -> K+ J/Psi pi pi selection with a
        large J/Psi pi pi mass window around the X(3872) mass. This mass window includes also the Psi(2S).
        '''
        if StrippingBu2KJpsiPiPiConf.X3872_loose == None:
            self.X38722JpsiRhoLoose( )
        StdLooseKaons = DataOnDemand(Location = "Phys/StdLooseKaons")
	_BuLoose = CombineParticles("Bu2KJpsiPiPiLoose")
      	_BuLoose.DecayDescriptors = ["B+ -> K+ X_1(3872)", "B- -> K- X_1(3872)"]
        _BuLoose.DaughtersCuts = { "K+" :  "MIPCHI2DV(PRIMARY) > %(KaonMIPCHI2DVLoose)s " % self.getProps() }   
        _BuLoose.CombinationCut = "ADAMASS('B+') < %(BuMassWinLoose)s *MeV" % self.getProps()
        _BuLoose.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BuVCHI2DOFLoose)s) "\
                             "& (MIPCHI2DV(PRIMARY) < %(BuMIPCHI2DVLoose)s) "\
                             "& (BPVVDCHI2 > %(BuBPVVDCHI2Loose)s)" % self.getProps()

	BuLoose = Selection("SelBu2KJpsiPiPiLoose",
                            Algorithm = _BuLoose,
                            RequiredSelections = [StrippingBu2KJpsiPiPiConf.X3872_loose,
                                                  StdLooseKaons])

        StrippingBu2KJpsiPiPiConf.Bu_Loose = BuLoose
	return BuLoose

    # X(3872) tight selection
    def Bu2KX3872( self ):
        '''
        B+ -> K+ X_1(3872) tight selection
        '''
        if StrippingBu2KJpsiPiPiConf.Bu_Loose == None:
            self.Bu2KX3872Loose( )
        _BuFilter = FilterDesktop("BuFilterForBu2KX3872")
        _BuFilter.Code = " (ADMASS('B+') < %(BuMassWin)s) " \
                         "& (MINTREE('mu+'==ABSID, (MIPCHI2DV(PRIMARY))) > %(MuonMIPCHI2DV)s)"\
                         "& (MAXTREE('J/psi(1S)'==ABSID, (ADMASS('J/psi(1S)'))) < %(JpsiMassWin)s *MeV)" \
                         "& (MINTREE('pi+'==ABSID, (MIPCHI2DV(PRIMARY))) > %(PionMIPCHI2DV)s)" \
                         "& (MAXTREE('X_1(3872)'==ABSID, VFASPF(VCHI2/VDOF)) < %(X3872VCHI2VDOF)s)" \
                         "& (MAXTREE('X_1(3872)'==ABSID, (ADMASS('X_1(3872)'))) < %(X3872MassWin)s *MeV)" \
                         "& (MINTREE('K+'==ABSID, MIPCHI2DV(PRIMARY)) > %(KaonMIPCHI2DV)s) " \
                         "& (VFASPF(VCHI2/VDOF) < %(BuVCHI2DOF)s) "\
                         "& (MIPCHI2DV(PRIMARY) < %(BuMIPCHI2DV)s) "\
                         "& (BPVVDCHI2 > %(BuBPVVDCHI2)s)" % self.getProps() 
	Bu = Selection("SelBu2KX3872",
                 	Algorithm = _BuFilter,
                 	RequiredSelections = [StrippingBu2KJpsiPiPiConf.Bu_Loose])
	return Bu 

    # Psi2S tight selection
    def Bu2KPsi2S( self ):
        '''
        B+ -> K+ psi(2S) tight selection. Identical to the Bu2KX3872 except for the J/Psi pi+ pi- mass cut.
        '''
        if StrippingBu2KJpsiPiPiConf.Jpsi_loose == None:
            self.Jpsi2MuMuLoose( )
        Pi = DataOnDemand(Location = "Phys/StdLoosePions")
        _Psi2S = CombineParticles("Psi2SJPsiPiPi")
        _Psi2S.DecayDescriptor = "psi(2S) -> J/psi(1S) pi+ pi-"
        _Psi2S.DaughtersCuts = { "pi+" : "MIPCHI2DV(PRIMARY) > %(PionMIPCHI2DV)s" % self.getProps() }
        _Psi2S.CombinationCut = "(ADAMASS('psi(2S)') < %(X3872MassWinADA)s *MeV)" % self.getProps() 
        _Psi2S.MotherCut = "  (MAXTREE('J/psi(1S)'==ABSID, (ADMASS('J/psi(1S)'))) < %(JpsiMassWin)s *MeV)" \
                           "& (ADMASS('psi(2S)') < %(X3872MassWin)s *MeV)" \
                           "& (MINTREE('mu+'==ABSID, (MIPCHI2DV(PRIMARY))) > %(MuonMIPCHI2DV)s)" \
                           "& (VFASPF(VCHI2/VDOF) < %(X3872VCHI2VDOF)s)" % self.getProps() 
        
        Psi2S = Selection("SelPsi2SJPsiPiPiForBu2KJpsiPiPiLoose",
                          Algorithm = _Psi2S,
                          RequiredSelections = [  Pi,
                                                  StrippingBu2KJpsiPiPiConf.Jpsi_loose ])
        
        StdLooseKaons = DataOnDemand(Location = "Phys/StdLooseKaons")
        _BuPsi2S = CombineParticles("BuBu2KPsi2S")
        _BuPsi2S.DecayDescriptors = ["B+ -> K+ psi(2S)", "B- -> K- psi(2S)"]
        _BuPsi2S.DaughtersCuts = { "K+" :  "MIPCHI2DV(PRIMARY) > %(KaonMIPCHI2DV)s" % self.getProps() }
        _BuPsi2S.CombinationCut = "(ADAMASS('B+') < %(BuMassWinADA)s *MeV) " % self.getProps()
        _BuPsi2S.MotherCut = " (ADMASS('B+') < %(BuMassWin)s *MeV) "\
                             "& (MIPCHI2DV(PRIMARY) < %(BuMIPCHI2DV)s) "\
                             "& (VFASPF(VCHI2/VDOF) < %(BuVCHI2DOF)s) "\
                             "& (BPVVDCHI2 > %(BuBPVVDCHI2)s)" % self.getProps()
          
        Bu = Selection("SelBu2KPsi2S",
                       Algorithm = _BuPsi2S,
                       RequiredSelections = [ StdLooseKaons,
                                              Psi2S ])
	return Bu 

    # Stripping lines

    def loose_line( self ):
        '''
        B+ -> K+ J/Psi pi pi loose line with large J/Psi pi pi mass window for both the Psi(2S) and X(3872).
        In the output location the J/Psi pi pi are reconstructed as X(3872)->J/psi rho(770)0(-> pi+ pi-)
        '''
        if StrippingBu2KJpsiPiPiConf.Bu_Loose == None:
            self.Bu2KX3872Loose()
        from StrippingConf.StrippingLine import StrippingLine
	Bu2KJpsiPiPiLooseSeq = SelectionSequence("SeqBu2KJpsiPiPiLoose", TopSelection = StrippingBu2KJpsiPiPiConf.Bu_Loose)
	return StrippingLine('Bu2KJpsiPiPiLooseLine', prescale = 1, algos = [Bu2KJpsiPiPiLooseSeq])
    
    def X3872_line( self ):
        '''
        B+ -> K+ X_1(3872) tight line.
        '''
        if StrippingBu2KJpsiPiPiConf.Bu_Loose == None:
            self.Bu2KX3872Loose()
        from StrippingConf.StrippingLine import StrippingLine
	Bu2KX3872Sel = self.Bu2KX3872()
	Bu2KX3872Seq = SelectionSequence("SeqBu2KX3872", TopSelection = Bu2KX3872Sel)
        return StrippingLine('Bu2KX3872Line', prescale = 1, algos = [Bu2KX3872Seq])

    def Psi2S_line( self ):
        '''
        B+ -> K+ psi(2S) tight line.
        '''
        if StrippingBu2KJpsiPiPiConf.Bu_Loose == None:
            self.Bu2KX3872Loose()
        from StrippingConf.StrippingLine import StrippingLine
        Bu2KPsi2SSel = self.Bu2KPsi2S()
	Bu2KPsi2SSeq = SelectionSequence("SeqBu2KPsi2S", TopSelection = Bu2KPsi2SSel)
        return StrippingLine('Bu2KPsi2SLine', prescale = 1, algos = [Bu2KPsi2SSeq])


    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
