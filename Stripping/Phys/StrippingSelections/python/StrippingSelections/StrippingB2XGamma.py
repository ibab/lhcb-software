__author__ = 'Fatima Soomro'
__date__ = '18/12/2009'
__version__ = '$Revision: 1.8 $'

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
import GaudiKernel.SystemOfUnits as Units
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool


class StrippingB2XGammaConf(LHCbConfigurableUser):
    """
    Definition of B -> X Gamma stripping
    """
    __slots__ = { 
                   'TrIPchi2Phi'          : 10.       # Dimensionless
                   ,'TrIPchi2Kst'         : 10.       # Dimensionless
                   ,'PhiMassWin'          : 15.       # MeV
                   ,'KstMassWin'          : 100.      # MeV
                   ,'KstMassWinSB'        : 200.      # MeV
                   ,'BsMassWin'           : 1000.     # MeV
                   ,'B0MassWin'           : 1000.     # MeV
                   ,'BMassWinSB'          : 2000.     # MeV
                   ,'BsDirAngle'          : 0.02      # radians
                   ,'B0DirAngle'          : 0.02      # radians
                   ,'BDirAngleMoni'       : 0.06      # radians
                   ,'BsPVIPchi2'          : 15.       # Dimensionless
                   ,'B0PVIPchi2'          : 15.       # Dimensionless
                   ,'photonPT'            : 2600.     # MeV
                   ,'PhiVCHI2'            : 15.       # dimensionless
                   ,'KstVCHI2'            : 15.       # dimensionless
                   ,'TrChi2'              : 5.        # dimensionless
                   }
    
    __phiGamma = None
    __phiGammaBMass = None
    __phiGammaDira = None
    __kstGamma = None
    __kstGammaBMass = None
    __kstGammaDira = None
    __kstGammaKMass = None
    __kstGammaWide = None
    
    def phigammaWideBMass( self ) :
        if StrippingB2XGammaConf.__phiGammaBMass == None:
            mySelection = self.combineBs(name = "BsMassSideBand") #this is an object of type Selection
            WideBMassBs = mySelection.algorithm().clone("WideBMassBs", CombinationCut = "(ADAMASS('B_s0')<%(BMassWinSB)s*MeV)"% self.getProps() )
            WideBMassBs.PropertiesPrint = False
            SelWideBMassBs = mySelection.clone("SelWideBMassBs", Algorithm = WideBMassBs)
            StrippingB2XGammaConf.__phiGammaBMass = SelectionSequence('SelSeqBs2PhiGammaWideBMass', SelWideBMassBs)
        return StrippingLine('Bs2PhiGammaWideBMass',
                             prescale =0.1,
                             algos = [SelWideBMassBs] )  
    
    def phigammaLooseDira( self ) :
        if StrippingB2XGammaConf.__phiGammaDira ==None:
            mySelection = self.combineBs(name = "BsDiraSideBand") #this is an object of type Selection
            LooseDiraBs  = mySelection.algorithm().clone("LooseDiraBs", MotherCut = "(acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2() < %(BsPVIPchi2)s)" % self.getProps() )
            LooseDiraBs.PropertiesPrint = False
            SelLooseDiraBs = mySelection.clone("SelLooseDiraBs", Algorithm = LooseDiraBs)
            StrippingB2XGammaConf.__phiGammaDira = SelectionSequence('SelSeqBs2PhiGammaLooseDira', SelLooseDiraBs)
        return StrippingLine('Bs2PhiGammaLooseDira',
                             prescale = 0.1,
                             algos = [ SelLooseDiraBs ] ) 
    
    def phigamma( self ) :
        if StrippingB2XGammaConf.__phiGamma == None :
            mySelection = self.combineBs(name = "Bs2PhiGamma")
            StrippingB2XGammaConf.__phiGamma = SelectionSequence('SeqSelBs2PhiGamma', TopSelection  = mySelection)#self.combineBs(name = "Bs2PhiGamma"))
        return StrippingLine('Bs2PhiGamma',
                             prescale = 1,
                             algos = [ mySelection] )
    
    def combineBs(self, name = "MakeBs2PhiGamma" ):
        """
        Define the Bs
        """        
        _stdPhi4Bs = DataOnDemand(Location = "Phys/StdLoosePhi2KK")
        _phi4BsFilter = FilterDesktop ("PhiFilterFor"+name)
        _phi4BsFilter.Code = "(MINTREE(ABSID=='K+', MIPCHI2DV(PRIMARY))> %(TrIPchi2Phi)s) & (MINTREE(ABSID=='K+', TRCHI2DOF)< %(TrChi2)s) & (ADMASS('phi(1020)') < %(PhiMassWin)s*MeV) & (VFASPF(VCHI2/VDOF) < %(PhiVCHI2)s)" % self.getProps()
        
        Phi4Bs =  Selection ("Phi2KKFor"+name
                             ,Algorithm = _phi4BsFilter
                             ,RequiredSelections = [_stdPhi4Bs])
        
        _stdgamma = DataOnDemand(Location = "Phys/StdLooseAllPhotons")
        _gammaFilter = FilterDesktop("GammaFilterFor"+name)
        _gammaFilter.Code =  "(PT> %(photonPT)s*MeV)"  % self.getProps()
        Gamma =  Selection ("GammaFor"+name
                            ,Algorithm = _gammaFilter
                            ,RequiredSelections = [_stdgamma])
        
        _Bs2PhiGamma = CombineParticles ( name
                                          ,DecayDescriptor = "B_s0 -> phi(1020) gamma"
                                          ,CombinationCut = "(ADAMASS('B_s0')<%(BsMassWin)s*MeV)"  % self.getProps()
                                          ,MotherCut = "(acos(BPVDIRA) < %(BsDirAngle)s) & (BPVIPCHI2() < %(BsPVIPchi2)s)" % self.getProps()
                                          ,ReFitPVs = False)#True)
        _Bs2PhiGamma.addTool( OfflineVertexFitter() )
        Bs2PhiGamma = Selection ( "Sel"+name
                                  ,Algorithm = _Bs2PhiGamma
                                  ,RequiredSelections = [Gamma, Phi4Bs])
        
        return Bs2PhiGamma
    
    def kstgammaWideBMass( self ) :
        if StrippingB2XGammaConf.__kstGammaBMass == None :
            mySelection = self.combineBd(name = "BdMassSideBand" ) 
            WideBMassBd = mySelection.algorithm().clone("WideBMassBd", CombinationCut = "(ADAMASS('B0')<%(BMassWinSB)s*MeV)"% self.getProps() )
            WideBMassBd.PropertiesPrint = False
            SelWideBMassBd = mySelection.clone("SelWideBMassBd", Algorithm =  WideBMassBd )
            StrippingB2XGammaConf.__kstGammaBMass = SelectionSequence('SelSeqBd2KstGammaWideBMass', SelWideBMassBd)
        return StrippingLine('Bd2KstGammaWideBMass',
                             prescale = 0.05,
                             algos = [ SelWideBMassBd] )  
    
    def kstgammaLooseDira( self ) :
        if StrippingB2XGammaConf.__kstGammaDira == None :
            mySelection = self.combineBd(name = "BdDiraSideBand") 
            LooseDiraBd  = mySelection.algorithm().clone("LooseDiraBd", MotherCut = "(acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2() < %(B0PVIPchi2)s)" % self.getProps() )
            LooseDiraBd.PropertiesPrint = False
            SelLooseDiraBd = mySelection.clone("SelLooseDiraBd", Algorithm =LooseDiraBd  )
            StrippingB2XGammaConf.__kstGammaDira = SelectionSequence('SelSeqBsd2KstGammaLooseDira', SelLooseDiraBd)
        return StrippingLine('Bd2KstGammaLooseDira',
                             prescale =0.05,
                             algos = [ SelLooseDiraBd ] )

    def kstgammaWideKstMass( self ) :
        if StrippingB2XGammaConf.__kstGammaKMass == None :
            mySelection = self.combineBd(name = "KstMassSideBand") 
            Sel = mySelection.clone("Sel") 
            mylist = Sel.requiredSelections 
            mygamma = mylist[0].clone("myGamma")
            KstMass =  mylist[1].algorithm().clone("KstMass",
                                                   Code = "(MINTREE(ABSID=='K+', MIPCHI2DV(PRIMARY))> %(TrIPchi2Kst)s) & (MINTREE(ABSID=='pi-', MIPCHI2DV(PRIMARY))> %(TrIPchi2Kst)s) & (MINTREE(ABSID=='K+', TRCHI2DOF)< %(TrChi2)s) & (MINTREE(ABSID=='pi-', TRCHI2DOF)< %(TrChi2)s) & (VFASPF(VCHI2/VDOF) < %(KstVCHI2)s) & (ADMASS('K*(892)0')<%(KstMassWinSB)s*MeV)" % self.getProps()) # a FilterDesktop
            
            KstMass.PropertiesPrint = False
            myKst = mylist[1].clone("myKst", Algorithm = KstMass)
            makeB0 = Sel.algorithm().clone("makeB0", InputLocations = [])
            makeB0.PropertiesPrint = False
            Bd2KstGammaWideKstMass = Selection ( "Bd2KstGammaWideKMass"
                                                 ,Algorithm = makeB0
                                                 ,RequiredSelections = [ mygamma, myKst]) 
            StrippingB2XGammaConf.__kstGammaKMass = SelectionSequence('SelSeqBsd2KstGammaWideKstMassBd',  Bd2KstGammaWideKstMass )
        return StrippingLine('Bd2KstGammaWideKstMassBd',
                             prescale = 0.05,
                             algos = [Bd2KstGammaWideKstMass ] )

    def kstgammaWideLine( self ) :
        if StrippingB2XGammaConf.__kstGammaWide == None :
            mySelectionWide = self.combineBd(name = "SideBand") 
            SelWide = mySelectionWide.clone("SelWide") 
            mylist = SelWide.requiredSelections 
            myGammaWide = mylist[0].clone("myGammaWide")
            KstWide =  mylist[1].algorithm().clone("KstWide",
                                                   Code = "(MINTREE(ABSID=='K+', MIPCHI2DV(PRIMARY))> %(TrIPchi2Kst)s) & (MINTREE(ABSID=='pi-', MIPCHI2DV(PRIMARY))> %(TrIPchi2Kst)s) & (VFASPF(VCHI2/VDOF) < %(KstVCHI2)s) & (ADMASS('K*(892)0')<%(KstMassWinSB)s*MeV)" % self.getProps()) # a FilterDesktop
            myKstWide = mylist[1].clone("myKstWide", Algorithm = KstWide)
            makeWideB0 = SelWide.algorithm().clone("makeWideB0", InputLocations = [], CombinationCut = "(ADAMASS('B0')<%(BMassWinSB)s*MeV)"% self.getProps(),
                                                     MotherCut = "(acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2() < %(B0PVIPchi2)s)" % self.getProps())
            makeWideB0.PropertiesPrint = False
            Bd2KstGammaWide = Selection ( "Bd2KstGammaWide"
                                                 ,Algorithm = makeWideB0
                                                 ,RequiredSelections = [ myGammaWide, myKstWide]) 
            StrippingB2XGammaConf.__kstGammaWide = SelectionSequence('SelSeqBsd2KstGammaWide',  Bd2KstGammaWide )
        return StrippingLine('Bd2KstGammaWideLine',
                             prescale = 0.05,
                             algos = [Bd2KstGammaWide] )

    
    def kstgamma( self ) :
        if   StrippingB2XGammaConf.__kstGamma == None:
            mySelection = self.combineBd(name = "Bd2KstGamma") 
            StrippingB2XGammaConf.__kstGamma = SelectionSequence('SeqSelBd2KstGamma',
                                                                 TopSelection  = mySelection)
        return StrippingLine('Bd2KstGamma',
                             prescale = 1,
                             algos = [mySelection] )
    
    def combineBd(self, name = "Make2KstGamma" ):
        """
        Define the Bd
        """        
        _stdkst4Bd = DataOnDemand(Location = "Phys/StdVeryLooseDetachedKst2Kpi")
        _kst4BdFilter = FilterDesktop ("KstFilterFor"+name)
        _kst4BdFilter.Code = "(MINTREE(ABSID=='K+', MIPCHI2DV(PRIMARY))> %(TrIPchi2Kst)s) & (MINTREE(ABSID=='pi-', MIPCHI2DV(PRIMARY))> %(TrIPchi2Kst)s) & (MINTREE(ABSID=='K+', TRCHI2DOF)< %(TrChi2)s) & (MINTREE(ABSID=='pi-', TRCHI2DOF)< %(TrChi2)s) & (VFASPF(VCHI2/VDOF) < %(KstVCHI2)s) & (ADMASS('K*(892)0') < %(KstMassWin)s*MeV)"% self.getProps()
        
        Kst4Bd   =  Selection ("Kst2KPiFor"+name
                               ,Algorithm =  _kst4BdFilter 
                               ,RequiredSelections = [ _stdkst4Bd  ] 
                               )
        
        _stdgamma = DataOnDemand(Location = "Phys/StdLooseAllPhotons")
        _gammaFilter = FilterDesktop("GammaFilterFor"+name)
        _gammaFilter.Code =  "(PT> %(photonPT)s*MeV)"  % self.getProps()
        Gamma =  Selection ("GammaFor"+name
                            ,Algorithm = _gammaFilter
                            ,RequiredSelections = [_stdgamma])
        
        _Bd2KstGamma = CombineParticles(name
                                        ,DecayDescriptor = "[B0 -> K*(892)0 gamma]cc" 
                                        ,CombinationCut = "ADAMASS('B0') < %(B0MassWin)s*MeV" % self.getProps()
                                        ,MotherCut = "(acos(BPVDIRA) <%(B0DirAngle)s) & (BPVIPCHI2() < %(B0PVIPchi2)s)" % self.getProps()
                                        ,ReFitPVs = False)#True)
        _Bd2KstGamma.addTool( OfflineVertexFitter() )
        _Bd2KstGamma.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        
        Bd2KstGamma = Selection ( "Sel"+name
                                  ,Algorithm = _Bd2KstGamma
                                  ,RequiredSelections = [Gamma, Kst4Bd])
        
        return Bd2KstGamma
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
