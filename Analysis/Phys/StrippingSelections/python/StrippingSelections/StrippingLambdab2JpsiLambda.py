
__author__ = ['Yasmine Amhis']
__date__ = '29/07/2010'
__version__ = '$Revision: 1.1 $'

'''
Strippping Selections for Lambdab ->Jpsi Lambda (LL and DD)
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, DataOnDemand

class StrippingLambdab2JpsiLambdaConf(LHCbConfigurableUser):
    """
    Definition of nominal Lamdba_b-> Jpsi Lambda stripping. 
    """
    __slots__ = {
        
        #---------------------------------------------------------
        #Jpsi cuts
        #---------------------------------------------------------
                "MuonTRCHI2Loose"	 : 10.0	# adimensional 
        ,	"JpsiMassWinLoose"	: 800.0	# MeV
        ,	"JpsiVCHI2Loose"	: 25.0	# adimensional
        #---------------------------------------------------------
        #Lambda cuts
        #---------------------------------------------------------
        ,       "ProtonTRCHI2Loose"	: 10.0	# adimensional  
        ,	"ProtonPIDp"	: -5.0	# adimensional  
        ,	"LambdaMassWin"	: 15.0	# MeV
        ,	"LambdaMassWinLoose"	: 30.0	# MeV
        ,	"LambdaPT" 	: 1000.0# MeV
        ,	"LambdaPTLoose" 	: 500.0	# MeV
        ,	"LambdaVCHI2" 	: 20.0	# adimensional
        ,	"LambdaVCHI2Loose" 	: 20.0	# adimensional
        #---------------------------------------------------------
        #Lambda_b cuts
        #---------------------------------------------------------         
        ,	"LambdabMassWin"	: 300.0	# MeV
        ,	"LambdabWinLoose"	: 300.0	# MeV
        ,	"LambdabVCHI2" 	: 5.0	# adimensional
        ,	"LambdabVCHI2Loose" 	: 20.0	# adimensional
        }
    
    
    
    def line_LL( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Lambdab2JpsiLambdaSel = self.Lambdab2JpsiLambdaLL()
	return StrippingLine('Lambdab2JpsiLambdaLLLine', prescale = 1, algos = [Lambdab2JpsiLambdaSel])   
    
    def line_DD( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Lambdab2JpsiLambdaSel = self.Lambdab2JpsiLambdaDD()
	return StrippingLine('Lambdab2JpsiLambdaDDLine', prescale = 1, algos = [Lambdab2JpsiLambdaSel])   
    #---------------------------------------------------------
    def Jpsi2MuMuLoose( self ):
       StdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
       _JpsiFilter = FilterDesktop("JpsiFilterForLambdab2JpsiLambda")
       _JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
                          "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
                          "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()
       
       Jpsi = Selection("Jpsi2MuMuForLambdab2JpsiLambda",
                        Algorithm = _JpsiFilter,
                        RequiredSelections = [StdVeryLooseJpsi2MuMu])
       return Jpsi
   
    def Jpsi2MuMuForLL( self ):
       StdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
       _JpsiFilterLL = FilterDesktop("JpsiFilterForLambdab2JpsiLambdaLL")
       _JpsiFilterLL.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
                          "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
                          "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()
       
       JpsiLL = Selection("Jpsi2MuMuForLambdab2JpsiLambdaLL",
                        Algorithm = _JpsiFilterLL,
                        RequiredSelections = [StdVeryLooseJpsi2MuMu])
       return JpsiLL
    def Jpsi2MuMuForDD( self ):
       StdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
       _JpsiFilterDD = FilterDesktop("JpsiFilterForLambdab2JpsiLambdaDD")
       _JpsiFilterDD.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
                          "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
                          "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()
       
       JpsiDD = Selection("Jpsi2MuMuForLambdab2JpsiLambdaDD",
                        Algorithm = _JpsiFilterDD,
                        RequiredSelections = [StdVeryLooseJpsi2MuMu])
       return JpsiDD
     #---------------------------------------------------------
    def Lambda2ppiLL( self ):
        StdLooseLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL")
        _LambdaFilter = FilterDesktop("LambdaFilterForLambdab2JpsiLambda")
        _LambdaFilter.Code = "  (MAXTREE('p+'==ABSID, TRCHI2DOF) < %(ProtonTRCHI2Loose)s)" \
                             "& (ADMASS('Lambda0') < %(LambdaMassWinLoose)s *MeV)" \
                             "& (PT > %(LambdaPTLoose)s *MeV)" \
                             "& (VFASPF(VCHI2/VDOF) < %(LambdaVCHI2Loose)s)" % self.getProps()
        
        Lambda =  Selection ("LambdaLLForLambdab2JpsiLambdaLL",
                             Algorithm = _LambdaFilter,

                             RequiredSelections = [StdLooseLambdaLL])

        return Lambda
     #---------------------------------------------------------
    def Lambda2ppiDD( self ):
         StdLooseLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD")
         _LambdaFilter = FilterDesktop("LambdaFilterForLambdab2JpsiLambdaDD")
         _LambdaFilter.Code = "  (MAXTREE('p+'==ABSID, TRCHI2DOF) < %(ProtonTRCHI2Loose)s)" \
                              "& (ADMASS('Lambda0') < %(LambdaMassWinLoose)s *MeV)" \
                              "& (PT > %(LambdaPTLoose)s *MeV)" \
                              "& (VFASPF(VCHI2/VDOF) < %(LambdaVCHI2Loose)s)" % self.getProps()
         
         Lambda =  Selection ("LambdaDDForLambdab2JpsiLambdaDD",
                              Algorithm = _LambdaFilter,
                              RequiredSelections = [StdLooseLambdaDD])
         return Lambda
     
     
     #---------------------------------------------------------
    def Lambdab2JpsiLambdaLL( self ):
        JpsiLL   = self.Jpsi2MuMuForLL()
        Lambda = self.Lambda2ppiLL()
        _Lambdab = CombineParticles("Lambdab2JpsiLambdaLL")
        _Lambdab.DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc"
        _Lambdab.CombinationCut = "ADAMASS('Lambda_b0') < %(LambdabMassWin)s *MeV" % self.getProps()
        _Lambdab.MotherCut = "(VFASPF(VCHI2/VDOF) < %(LambdabVCHI2Loose)s)" % self.getProps()
        _Lambdab.ReFitPVs = True
        # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Lambda
        _Lambdab.addTool( OfflineVertexFitter() )
        _Lambdab.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Lambdab.OfflineVertexFitter.useResonanceVertex = False
        
        Lambdab = Selection("SelLambdab2JpsiLambdaLL",
                            Algorithm = _Lambdab,
                            RequiredSelections = [JpsiLL, Lambda])
        
        return Lambdab
    #---------------------------------------------------------
    def Lambdab2JpsiLambdaDD ( self ):
        JpsiDD   = self.Jpsi2MuMuForDD()
        Lambda = self.Lambda2ppiDD()
        _Lambdab = CombineParticles("Lambdab2JpsiLambdaDD")
        _Lambdab.DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc"
        _Lambdab.CombinationCut = "ADAMASS('Lambda_b0') < %(LambdabMassWin)s *MeV" % self.getProps()
        _Lambdab.MotherCut = "(VFASPF(VCHI2/VDOF) < %(LambdabVCHI2Loose)s)" % self.getProps()
        _Lambdab.ReFitPVs = True
        # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Lambda
        _Lambdab.addTool( OfflineVertexFitter() )
        _Lambdab.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Lambdab.OfflineVertexFitter.useResonanceVertex = False
        
        Lambdab = Selection("SelLambdab2JpsiLambdaDD",
                            Algorithm = _Lambdab,
                            RequiredSelections = [JpsiDD, Lambda])
        
        return Lambdab
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
        
