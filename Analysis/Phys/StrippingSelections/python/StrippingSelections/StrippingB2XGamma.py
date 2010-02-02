__author__ = 'Fatima Soomro'
__date__ = '18/12/2009'
__version__ = '$Revision: 1.3 $'

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
                   'TrIPchi2Phi'          : 10       # Dimensionless
                   ,'TrIPchi2Kst'         : 10       # Dimensionless
                   ,'PhiMassWinL'         : 40       # MeV
                   ,'PhiMassWinT'         : 15       # MeV
                   ,'KstMassWinL'         : 200      # MeV
                   ,'KstMassWinT'         : 100      # MeV
                   ,'KstMassWinSB'        : 200      # MeV
                   ,'BsMassWin'           : 1000     # MeV
                   ,'B0MassWin'           : 1000     # MeV
                   ,'BMassWinSB'          : 2000     # MeV
                   ,'BsDirAngle'          : 0.02     # radians
                   ,'B0DirAngle'          : 0.02     # radians
                   ,'BDirAngleMoni'       : 0.06     # radians
                   ,'BsPVIPchi2'          : 15       # Dimensionless
                   ,'B0PVIPchi2'          : 15       # Dimensionless
                   ,'photonPT'            : 2000     # MeV
                   ,'PhiVCHI2'            : 15       # dimensionless
                   ,'KstVCHI2'            : 15       # dimensionless
                   }
    
    __phiGamma = None
    __phiGammaBMass = None
    __phiGammaDira = None
    __kstGamma = None
    __kstGammaBMass = None
    __kstGammaDira = None
    __kstGammaKMass = None
    
    def phigammaWideBMass( self ) :
        if StrippingB2XGammaConf.__phiGammaBMass == None:
            mySelection = self.combineBs(name = "BsMassSideBand") #this is an object of type Selection
            WideBMassBs = mySelection.algorithm().clone("WideBMassBs", CombinationCut = "(ADAMASS('B_s0')<%(BMassWinSB)s*MeV)"% self.getProps() )
            WideBMassBs.PropertiesPrint = False
            SelWideBMassBs = mySelection.clone("SelWideBMassBs", Algorithm = WideBMassBs)
            StrippingB2XGammaConf.__phiGammaBMass = SelectionSequence('SelSeqBs2PhiGammaWideBMass', SelWideBMassBs)
        return StrippingLine('Bs2PhiGammaWideBMass', prescale =0.1, algos = [ StrippingB2XGammaConf.__phiGammaBMass ] )  
    
    def phigammaLooseDira( self ) :
        if StrippingB2XGammaConf.__phiGammaDira ==None:
            mySelection = self.combineBs(name = "BsDiraSideBand") #this is an object of type Selection
            LooseDiraBs  = mySelection.algorithm().clone("LooseDiraBs", MotherCut = "(acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2() < %(BsPVIPchi2)s)" % self.getProps() )
            LooseDiraBs.PropertiesPrint = False
            SelLooseDiraBs = mySelection.clone("SelLooseDiraBs", Algorithm = LooseDiraBs)
            StrippingB2XGammaConf.__phiGammaDira = SelectionSequence('SelSeqBs2PhiGammaLooseDira', SelLooseDiraBs)
        return StrippingLine('Bs2PhiGammaLooseDira', prescale = 0.1, algos = [   StrippingB2XGammaConf.__phiGammaDira ] ) 
    
    def phigamma( self ) :
        if StrippingB2XGammaConf.__phiGamma == None :
            mySelection = self.combineBs(name = "Bs2PhiGamma")
            StrippingB2XGammaConf.__phiGamma = SelectionSequence('SeqSelBs2PhiGamma', TopSelection  = mySelection)#self.combineBs(name = "Bs2PhiGamma"))
        return StrippingLine('Bs2PhiGamma', prescale = 1, algos = [ StrippingB2XGammaConf.__phiGamma] )
    
    def combineBs(self, name = "MakeBs2PhiGamma" ):
        """
        Define the Bs
        """        
        _stdPhi4Bs = DataOnDemand("LoosePhi2KK4Bs", Location = "Phys/StdLoosePhi2KK")
        _phi4BsFilter = FilterDesktop ("PhiFilterFor"+name)
        _phi4BsFilter.Code = "(MINTREE(ABSID=='K+', MIPCHI2DV(PRIMARY))> %(TrIPchi2Phi)s) & (ADMASS('phi(1020)') < %(PhiMassWinT)s*MeV) & (VFASPF(VCHI2/VDOF) < %(PhiVCHI2)s)" % self.getProps()
        
        Phi4Bs =  Selection ("Phi2KKFor"+name
                             ,Algorithm = _phi4BsFilter
                             ,RequiredSelections = [_stdPhi4Bs])
        
        _stdgamma = DataOnDemand("stdLooseAllPhotons", Location = "Phys/StdLooseAllPhotons")
        _gammaFilter = FilterDesktop("GammaFilterFor"+name)
        _gammaFilter.Code =  "(PT> %(photonPT)s*MeV)"  % self.getProps()
        Gamma =  Selection ("GammaFor"+name
                            ,Algorithm = _gammaFilter
                            ,RequiredSelections = [_stdgamma])
        
        _Bs2PhiGamma = CombineParticles ( name
                                          ,DecayDescriptor = "B_s0 -> phi(1020) gamma"
                                          ,CombinationCut = "(ADAMASS('B_s0')<%(BsMassWin)s*MeV)"  % self.getProps()
                                          ,MotherCut = "(acos(BPVDIRA) < %(BsDirAngle)s) & (BPVIPCHI2() < %(BsPVIPchi2)s)" % self.getProps()
                                          ,ReFitPVs = True)
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
        return StrippingLine('Bd2KstGammaWideBMass', prescale = 0.1, algos = [ StrippingB2XGammaConf.__kstGammaBMass] )  
    
    def kstgammaLooseDira( self ) :
        if StrippingB2XGammaConf.__kstGammaDira == None :
            mySelection = self.combineBd(name = "BdDiraSideBand") 
            LooseDiraBd  = mySelection.algorithm().clone("LooseDiraBd", MotherCut = "(acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2() < %(B0PVIPchi2)s)" % self.getProps() )
            LooseDiraBd.PropertiesPrint = False
            SelLooseDiraBd = mySelection.clone("SelLooseDiraBd", Algorithm =LooseDiraBd  )
            StrippingB2XGammaConf.__kstGammaDira = SelectionSequence('SelSeqBsd2KstGammaLooseDira', SelLooseDiraBd)
        return StrippingLine('Bd2KstGammaLooseDira', prescale =0.1, algos = [  StrippingB2XGammaConf.__kstGammaDira] )

    def kstgammaWideKstMass( self ) :
        if StrippingB2XGammaConf.__kstGammaKMass == None :
            mySelection = self.combineBd(name = "KstMassSideBand") 
            Sel = mySelection.clone("Sel") 
            mylist = Sel.requiredSelections 
            mygamma = mylist[0].clone("myGamma")
            KstMass =  mylist[1].algorithm().clone("KstMass", CombinationCut ="(ADAMASS('K*(892)0')<%(KstMassWinSB)s*MeV)" % self.getProps() ) # a combineParticles
            KstMass.PropertiesPrint = False
            myKst = mylist[1].clone("myKst", Algorithm = KstMass)
            makeB0 = Sel.algorithm().clone("makeB0")
            makeB0.PropertiesPrint = False
            Bd2KstGammaWideKstMass = Selection ( "Bd2KstGammaWideKMass"
                                                 ,Algorithm = makeB0
                                                 ,RequiredSelections = [ mygamma, myKst]) 
            StrippingB2XGammaConf.__kstGammaKMass = SelectionSequence('SelSeqBsd2KstGammaWideKstMassBd',  Bd2KstGammaWideKstMass )
        return StrippingLine('Bd2KstGammaWideKstMassBd', prescale = 0.1, algos = [ StrippingB2XGammaConf.__kstGammaKMass ] )

    
    def kstgamma( self ) :
        if   StrippingB2XGammaConf.__kstGamma == None:
            mySelection = self.combineBd(name = "Bd2KstGamma") 
            StrippingB2XGammaConf.__kstGamma = SelectionSequence('SeqSelBd2KstGamma', TopSelection  = mySelection)
        return StrippingLine('Bd2KstGamma', prescale = 1, algos = [ StrippingB2XGammaConf.__kstGamma] )
    
    def combineBd(self, name = "Make2KstGamma" ):
        """
        Define the Bd
        """        
        _K4Kst  = DataOnDemand("K4Kst",  Location = "Phys/StdLooseKaons")
        _pi4Kst = DataOnDemand("pi4Kst", Location = "Phys/StdLoosePions")

        LooseKstar2Kpi = CombineParticles ("LooseKstar2Kpi")
        LooseKstar2Kpi.InputLocations = ["StdLooseKaons", "StdLoosePions"]
        LooseKstar2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
        LooseKstar2Kpi.DaughtersCuts = { "K+" :  "(MIPCHI2DV(PRIMARY)> %(TrIPchi2Kst)s)"% self.getProps(),"pi-" :  "MIPCHI2DV(PRIMARY)>%(TrIPchi2Kst)s"% self.getProps() }
        LooseKstar2Kpi.CombinationCut = "(ADAMASS('K*(892)0') < %(KstMassWinT)s*MeV)"% self.getProps()
        LooseKstar2Kpi.MotherCut = "(VFASPF(VCHI2/VDOF) < %(KstVCHI2)s)"% self.getProps()
        
        Kst4Bd   =  Selection ("Kst2KPiFor"+name
                               ,Algorithm = LooseKstar2Kpi
                               ,RequiredSelections = [ _K4Kst, _pi4Kst ]
                               )
        
        _stdgamma = DataOnDemand("stdLooseAllPhotons", Location = "Phys/StdLooseAllPhotons")
        _gammaFilter = FilterDesktop("GammaFilterFor"+name)
        _gammaFilter.Code =  "(PT> %(photonPT)s*MeV)"  % self.getProps()
        Gamma =  Selection ("GammaFor"+name
                            ,Algorithm = _gammaFilter
                            ,RequiredSelections = [_stdgamma])
        
        _Bd2KstGamma = CombineParticles(name
                                        ,DecayDescriptor = "[B0 -> K*(892)0 gamma]cc" 
                                        ,CombinationCut = "ADAMASS('B0') < %(B0MassWin)s*MeV" % self.getProps()
                                        ,MotherCut = "(acos(BPVDIRA) <%(B0DirAngle)s) & (BPVIPCHI2() < %(B0PVIPchi2)s)" % self.getProps()
                                        ,ReFitPVs = True)
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
