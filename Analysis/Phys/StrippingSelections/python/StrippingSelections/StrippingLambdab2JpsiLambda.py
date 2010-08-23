# $Id: StrippingLambdab2JpsiLambda.py,v 1.4 2010-08-23 14:11:19 yamhis Exp $

__author__ = ['Yasmine Amhis']
__date__ = '29/07/2010'
__version__ = '$Revision: 1.4 $'

'''
StrippingSelections for Lambda_b0-> Jpsi Lambda ( LL and DD )
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, DataOnDemand



class StrippingLambdab2JpsiLambdaConf(LHCbConfigurableUser):
    """
    Definition  Lambad_b-> Jpsi Lambda stripping. 
    """
    __slots__ = {
        
        #---------------------------------------------------------
        #Jpsi cuts
        #---------------------------------------------------------
                "MuonTRCHI2"	: 15.0	# adimensional 
        ,	"JpsiMassWin"	: 60.0	# MeV
        ,	"JpsiVCHI2"	: 25.0	# adimensional
        #---------------------------------------------------------
        #Lambda cuts
        #---------------------------------------------------------
        ,       "ProtonTRCHI2"	: 15.0	# adimensional
        ,       "PionTRCHI2"    : 15.0  #adimensional  
      
        ,       "ProtonPT" 	: 500.0 # MeV                     
        ,       "PionPT" 	: 100.0 # MeV 

        ,	"LambdaMassWin"	: 20.0	# MeV
        ,	"LambdaVCHI2" 	: 25.0	# adimensional
        #---------------------------------------------------------
        #Lambda_b cuts
        #---------------------------------------------------------         
        ,	"LambdabMassWin"     : 500.0	# MeV
        ,	"LambdabVCHI2" 	     : 25.0	# adimensional
        ,       "LambdabDirection"   : 1  # adimentional only used to make sure that the PV-refit is done        
        }
    
    
    _Jpsi2MuMuSel = None
    def line_LL( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Lambdab2JpsiLambdaSel = self.Lambdab2JpsiLambdaLL()
	return StrippingLine('Lambdab2JpsiLambdaLLLine', prescale = 1, algos = [Lambdab2JpsiLambdaSel])   
    
    def line_DD( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Lambdab2JpsiLambdaSel = self.Lambdab2JpsiLambdaDD()
	return StrippingLine('Lambdab2JpsiLambdaDDLine', prescale = 1, algos = [Lambdab2JpsiLambdaSel])   
    #---------------------------------------------------------
    def Jpsi2MuMu( self ):
        if StrippingLambdab2JpsiLambdaConf._Jpsi2MuMuSel == None:
            StdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
            _JpsiFilter = FilterDesktop("JpsiFilterForLambdab2JpsiLambda")
            _JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2)s)" \
                               "& (ADMASS('J/psi(1S)') < %(JpsiMassWin)s *MeV)" \
                               "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2)s)" % self.getProps()
            
            StrippingLambdab2JpsiLambdaConf._Jpsi2MuMuSel = Selection("Jpsi2MuMuForLambdab2JpsiLambda",
                             Algorithm = _JpsiFilter,
                             RequiredSelections = [StdVeryLooseJpsi2MuMu])
        return StrippingLambdab2JpsiLambdaConf._Jpsi2MuMuSel 
   

     #---------------------------------------------------------
    def Lambda2ppiLL( self ):
        StdLooseLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL")
        _LambdaFilter = FilterDesktop("LambdaFilterForLambdab2JpsiLambda")
        _LambdaFilter.Code = "  (MAXTREE('p+'==ABSID, TRCHI2DOF) < %(ProtonTRCHI2)s)" \
                             "&  (MAXTREE('p+'==ABSID, PT) > %(ProtonPT)s)"\
                             "&  (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(PionTRCHI2)s)" \
                             "&  (MAXTREE('pi-'==ABSID, PT) > %(PionPT)s)"\
                             "& (ADMASS('Lambda0') < %(LambdaMassWin)s *MeV)" \
                             "& (VFASPF(VCHI2/VDOF) < %(LambdaVCHI2)s)" % self.getProps()        
        Lambda =  Selection ("LambdaLLForLambdab2JpsiLambdaLL",
                             Algorithm = _LambdaFilter,

                             RequiredSelections = [StdLooseLambdaLL])

        return Lambda
     #---------------------------------------------------------
    def Lambda2ppiDD( self ):
         StdLooseLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD")
         _LambdaFilter = FilterDesktop("LambdaFilterForLambdab2JpsiLambdaDD")
         _LambdaFilter.Code = "  (MAXTREE('p+'==ABSID, TRCHI2DOF) < %(ProtonTRCHI2)s)" \
                              "&  (MAXTREE('p+'==ABSID, PT) > %(ProtonPT)s)"\
                              "&  (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(PionTRCHI2)s)" \
                              "&  (MAXTREE('pi-'==ABSID, PT) > %(PionPT)s)"\
                              "& (ADMASS('Lambda0') < %(LambdaMassWin)s *MeV)" \
                              "& (VFASPF(VCHI2/VDOF) < %(LambdaVCHI2)s)" % self.getProps()
         
         Lambda =  Selection ("LambdaDDForLambdab2JpsiLambdaDD",
                              Algorithm = _LambdaFilter,
                              RequiredSelections = [StdLooseLambdaDD])
         return Lambda
     
     
     #---------------------------------------------------------
    def Lambdab2JpsiLambdaLL( self ):
        Jpsi   = self.Jpsi2MuMu()
        Lambda = self.Lambda2ppiLL()
        _Lambdab = CombineParticles("Lambdab2JpsiLambdaLL")
        _Lambdab.DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc"
        _Lambdab.CombinationCut = "ADAMASS('Lambda_b0') < %(LambdabMassWin)s *MeV" % self.getProps()
        
        _Lambdab.MotherCut = "(VFASPF(VCHI2/VDOF) < %(LambdabVCHI2)s)"\
                             "& (BPVDIRA < %(LambdabDirection)s)" % self.getProps()
         
        _Lambdab.ReFitPVs = True
        # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Lambda
        _Lambdab.addTool( OfflineVertexFitter() )
        _Lambdab.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Lambdab.OfflineVertexFitter.useResonanceVertex = False
        
        Lambdab = Selection("SelLambdab2JpsiLambdaLL",
                            Algorithm = _Lambdab,
                            RequiredSelections = [Lambda,Jpsi])
        
        return Lambdab
    #---------------------------------------------------------
    def Lambdab2JpsiLambdaDD ( self ):
        Jpsi   = self.Jpsi2MuMu()
        Lambda = self.Lambda2ppiDD()
        _Lambdab = CombineParticles("Lambdab2JpsiLambdaDD")
        _Lambdab.DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc"
        _Lambdab.CombinationCut = "ADAMASS('Lambda_b0') < %(LambdabMassWin)s *MeV" % self.getProps()
        _Lambdab.MotherCut = "(VFASPF(VCHI2/VDOF) < %(LambdabVCHI2)s)"\
                             "&(BPVDIRA < %(LambdabDirection)s)" % self.getProps()

        _Lambdab.ReFitPVs = True
        # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Lambda
        _Lambdab.addTool( OfflineVertexFitter() )
        _Lambdab.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Lambdab.OfflineVertexFitter.useResonanceVertex = False
        
        Lambdab = Selection("SelLambdab2JpsiLambdaDD",
                            Algorithm = _Lambdab,
                            RequiredSelections = [Lambda, Jpsi])
        
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
        
