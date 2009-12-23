# $Id: StrippingBs2PhiPhi.py,v 1.3 2009-12-23 14:31:22 schleich Exp $

__author__ = 'Sebastian Schleich'
__date__ = '2009/12/23'
__version__ = '$Revision: 1.3 $'

'''
Bs->PhiPhi stripping selection
'''
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBs2PhiPhiConf(LHCbConfigurableUser):
    """
    Definition of Bs->PhiPhi stripping
    """
    __slots__ = { 
                  'KaonPT'              : 300      # MeV
                , 'KaonIP'              : 0.04     # mm
                , 'KaonIPCHI2'          : 1.5      # adimensional
                , 'PhiPT'               : 650      # MeV
                , 'PhiPTsq'             : 3.0      # GeV*GeV
                , 'PhiDOCA'             : 0.3      # mm
                , 'PhiVertexCHI2pDOF'   : 25       # adimensional
                , 'PhiMassWindow'       : 25       # MeV
                , 'PhiMassMax'          : 1100     # MeV
                , 'BsDOCA'              : 0.3      # mm
                , 'BsVertexCHI2pDOF'    : 25       # adimensional
                , 'BsMassWindow'        : 300      # MeV
                , 'WidePrescale'        : 0.06     # adimensional
                }


    def normal( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        line1 = self.BsPhiPhiLine()
        return line1


    def wide( self ) :
        ps =  self.getProps()['WidePrescale']
        print "Bs2PhiPhiWideLine prescale = " , ps
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        line1 = self.BsPhiPhiLine()
        line2 = line1.clone('Bs2PhiPhiWideLine'
                  , prescale = ps
                  , postscale = 1
                  , FilterDesktopBs2PhiPhiPhi2KK = {"Code": "MM < 1100.*MeV"}
                  )  
        return line2


    def BsPhiPhiLine( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
        from Configurables import FilterDesktop, CombineParticles #, OfflineVertexFitter, PhysDesktop
        import GaudiKernel.SystemOfUnits as Units

        Phi2KK_DC = "(MIPDV(PRIMARY)> %(KaonIP)s )&(PT> %(KaonPT)s *MeV)" % self.getProps()
        Phi2KK_CC = "(AMAXDOCA('') < %(PhiDOCA)s *mm)&(AM < %(PhiMassMax)s *MeV)" % self.getProps()
        Phi2KK_MC = "ALL"
        PhiFilter = "(ADMASS('phi(1020)') < %(PhiMassWindow)s*MeV)" % self.getProps()
        Bs2PhiPhi_DC = "(PT> %(PhiPT)s *MeV)" % self.getProps()
        Bs2PhiPhi_CC = "(AMAXDOCA('') < %(BsDOCA)s *mm)&(ADAMASS('B_s0')< %(BsMassWindow)s *MeV)&(ACHILD(PT,1)*ACHILD(PT,2)> %(PhiPTsq)s *GeV*GeV)" % self.getProps()
        #Bs2PhiPhi_MC = ""
        #Add error estimate cuts:
        Phi2KK_DC = "(MIPCHI2DV(PRIMARY)> %(KaonIPCHI2)s )&"% self.getProps()+ Phi2KK_DC
        Phi2KK_MC = "(VFASPF(VCHI2/VDOF)< %(PhiVertexCHI2pDOF)s )" % self.getProps()
        Bs2PhiPhi_MC = "(VFASPF(VCHI2/VDOF)< %(BsVertexCHI2pDOF)s )" % self.getProps()
        print "Cuts for Bs2PhiPhi  Stripping line"
        print Phi2KK_DC
        print Phi2KK_CC
        print Phi2KK_MC
        print PhiFilter
        print Bs2PhiPhi_DC
        print Bs2PhiPhi_CC
        print Bs2PhiPhi_MC
        

        Bs2PhiPhiLooseDetachedPhi2KK = StrippingMember ( CombineParticles
                          , "Bs2PhiPhiLooseDetachedPhi2KK"
                          , InputLocations = ["StdNoPIDsKaons"]
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                          , DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )

        Bs2PhiPhiPhi2KK = StrippingMember ( FilterDesktop
                , "Bs2PhiPhiPhi2KK"
                , InputLocations = ["%Bs2PhiPhiLooseDetachedPhi2KK"]
                , Code = PhiFilter
                )

        Bs2PhiPhi = StrippingMember( CombineParticles
          , "Bs2PhiPhi"
          , DecayDescriptor =  "B_s0 -> phi(1020) phi(1020)"
          , InputLocations  = [ "%Bs2PhiPhiPhi2KK" ]
          , DaughtersCuts   = {"phi(1020)" : Bs2PhiPhi_DC}
          , CombinationCut  = Bs2PhiPhi_CC
          , MotherCut       = Bs2PhiPhi_MC 
          , ReFitPVs        = True
          )

        line = StrippingLine('Bs2PhiPhiLine'
              , prescale = 1
              , postscale = 1
              , algos = [Bs2PhiPhiLooseDetachedPhi2KK, Bs2PhiPhiPhi2KK, Bs2PhiPhi]
              )

        return line


    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

