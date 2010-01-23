__author__ = 'Fatima Soomro'
__date__ = '18/12/2009'
__version__ = '$Revision: 1.1 $'

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
    
    def phigammaWideBMass( self ) :
        mySelection = self.combineBs() #this is an object of type Selection
        WideBMassBs = mySelection.algorithm().clone("WideBMassBs", CombinationCut = "(ADAMASS('B_s0')<%(BMassWinSB)s*MeV)"% self.getProps() )
        WideBMassBs.PropertiesPrint = True
        SelWideBMassBs = mySelection.clone("SelWideBMassBs", Algorithm = WideBMassBs)
        SelSeqBs2PhiGammaWideBMass = SelectionSequence('SelSeqBs2PhiGammaWideBMass', SelWideBMassBs)
        return StrippingLine('Bs2PhiGammaWideBMass', prescale = 1, algos = [ SelSeqBs2PhiGammaWideBMass ] )  
    
    def phigammaLooseDira( self ) :
        mySelection = self.combineBs() #this is an object of type Selection
        LooseDiraBs  = mySelection.algorithm().clone("LooseDiraBs", MotherCut = "(acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2() < %(BsPVIPchi2)s)" % self.getProps() )
        LooseDiraBs.PropertiesPrint = True
        SelLooseDiraBs = mySelection.clone("SelLooseDiraBs", Algorithm = LooseDiraBs)
        SelSeqBs2PhiGammaLooseDira = SelectionSequence('SelSeqBs2PhiGammaLooseDira', SelLooseDiraBs)
        return StrippingLine('Bs2PhiGammaLooseDira', prescale = 1, algos = [ SelSeqBs2PhiGammaLooseDira ] ) 
    
    def phigamma( self ) :
        StripBs2PhiGamma = SelectionSequence('SeqSelBs2PhiGamma', TopSelection  = self.combineBs())
        return StrippingLine('Bs2PhiGamma', prescale = 1, algos = [ StripBs2PhiGamma] )
    
    def combineBs(self):
        """
        Define the Bs
        """        
        name = "Bs2PhiGamma" 
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
        mySelection = self.combineBd() #this is an object of type Selection
        WideBMassBd = mySelection.algorithm().clone("WideBMassBd", CombinationCut = "(ADAMASS('B0')<%(BMassWinSB)s*MeV)"% self.getProps() )
        WideBMassBd.PropertiesPrint = True
        SelWideBMassBd = mySelection.clone("SelWideBMassBd", Algorithm =  WideBMassBd )
        SelSeqBd2KstGammaWideBMass = SelectionSequence('SelSeqBd2KstGammaWideBMass', SelWideBMassBd)
        return StrippingLine('Bd2KstGammaWideBMass', prescale = 1, algos = [ SelSeqBd2KstGammaWideBMass] )  
    
    def kstgammaLooseDira( self ) :
        mySelection = self.combineBd() #this is an object of type Selection
        LooseDiraBd  = mySelection.algorithm().clone("LooseDiraBd", MotherCut = "(acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2() < %(B0PVIPchi2)s)" % self.getProps() )
        LooseDiraBd.PropertiesPrint = True
        SelLooseDiraBd = mySelection.clone("SelLooseDiraBd", Algorithm =LooseDiraBd  )
        SelSeqBd2KstGammaLooseDira = SelectionSequence('SelSeqBsd2KstGammaLooseDira', SelLooseDiraBd)
        return StrippingLine('Bd2KstGammaLooseDira', prescale = 1, algos = [ SelSeqBd2KstGammaLooseDira] )

    def kstgammaWideKstMass( self ) :
        mySelection = self.combineBd() # this is an object of type Selection
        Sel = mySelection.clone("Sel") # this is a selection
        mylist = Sel.requiredSelections # a list
        mygamma = mylist[0].clone("myGamma")
        KstMass =  mylist[1].algorithm().clone("KstMass", CombinationCut ="(ADAMASS('K*(892)0')<%(KstMassWinSB)s*MeV)" % self.getProps() ) # a combineParticles
        KstMass.PropertiesPrint = True
        myKst = mylist[1].clone("myKst", Algorithm = KstMass)
        makeB0 = Sel.algorithm().clone("makeB0")
        makeB0.PropertiesPrint = True
        Bd2KstGammaWideKstMass = Selection ( "Bd2KstGammaWideKMass"
                                             ,Algorithm = makeB0
                                             ,RequiredSelections = [ mygamma, myKst]) 
        SelSeqBd2KstGammaWideKstMassBd = SelectionSequence('SelSeqBsd2KstGammaWideKstMassBd',  Bd2KstGammaWideKstMass )#SelWideKstMassBd)
        return StrippingLine('Bd2KstGammaWideKstMassBd', prescale = 1, algos = [ SelSeqBd2KstGammaWideKstMassBd ] )

    
    def kstgamma( self ) :
        StripBd2KstGamma = SelectionSequence('SeqSelBd2KstGamma', TopSelection  = self.combineBd())
        return StrippingLine('Bd2KstGamma', prescale = 1, algos = [ StripBd2KstGamma] )
    
    def combineBd(self):
        """
        Define the Bd
        """        
        name = "Bd2KstGamma" 
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
