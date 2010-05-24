# $Id: StrippingBs2JpsieePhi.py,v 1.1 2010-05-24 15:06:59 jhe Exp $

__author__ = 'Artur Ukleja, Stephane TJampens, Jibo He'
__date__ = '2010/02/26'
__version__ = '$Revision: 1.1 $'

'''
Bs->JpsieePhi stripping selection
'''
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles  
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBs2JpsieePhiConf(LHCbConfigurableUser):
    """
    Definition of Bs->JpsieePhi stripping
    """
    __slots__ = { 
                  'ElectronPT'          : 800      # MeV
                , 'ElectronMIPCHI2'     : 4        # adimensional

                , 'JpsiVertexCHI2pDOF'  : 25       # adimensional
                , 'JpsiMassMin'         : 2500     # MeV
                , 'JpsiMassMax'         : 3300     # MeV

                , 'KaonMIPCHI2'         : 4        # adimensional

                , 'PhiPT'               : 1000     # MeV
                , 'PhiVertexCHI2pDOF'   : 25       # adimensional
                , 'PhiMassMin'          :  995     # MeV
                , 'PhiMassMax'          : 1045     # MeV

                , 'BsDIRA'              : 0.99     # adimensional
                , 'BsVertexCHI2pDOF'    : 9        # adimensional
                , 'BsPseudoMassMin'     : 3800     # MeV
                , 'BsPseudoMassMax'     : 6800     # MeV
                }


    def UnbiasedPT_line( self ) :
        from StrippingConf.StrippingLine import StrippingLine
        SelBs2JpsieePhiLoose = self.Bs2JpsieePhiLoose()
        SeqBs2JpsieePhiLoose = SelectionSequence("SeqBs2JpsieePhiLoose", TopSelection = SelBs2JpsieePhiLoose )
        return StrippingLine('Bs2JpsieePhiUnbiasedPT', prescale = 0.01, algos = [ SeqBs2JpsieePhiLoose ])


    def Signalbox_line( self ) :
        from StrippingConf.StrippingLine import StrippingLine
        SelBs2JpsieePhi = self.Bs2JpsieePhi()
        SeqBs2JpsieePhi = SelectionSequence("SeqBs2JpsieePhi", TopSelection = SelBs2JpsieePhi )
        return StrippingLine('Bs2JpsieePhiSignalBox', prescale = 1., algos = [ SeqBs2JpsieePhi ])
      

    def Jpsi2eeLoose( self, name ):
        _stdJpsi = DataOnDemand( Location="Phys/StdLooseElectrons" )
        _jpsi = CombineParticles("Jpsi2eeLoose" + name,
                                 DecayDescriptor = "J/psi(1S) -> e+ e-" ,
                                 DaughtersCuts = {"e+" : "(PT > %(ElectronPT)s *MeV)" % self.getProps() },
                                 MotherCut = "(VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOF)s ) & (MM > %(JpsiMassMin)s *MeV) & (MM < %(JpsiMassMax)s *MeV)" % self.getProps()
                                 )
        
        Jpsi = Selection("SelJpsi2eeLooseFor"+name,
                         Algorithm = _jpsi,
                         RequiredSelections = [_stdJpsi])
        return Jpsi
    


    def Phi2KKLoose( self, name ):
        _stdPhi = DataOnDemand(Location="Phys/StdNoPIDsKaons")
        _phi = CombineParticles("Phi2KKLoose" + name,
                                DecayDescriptor = "phi(1020) -> K+ K-" ,
                                MotherCut = "(VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOF)s ) & (PT > %(PhiPT)s *MeV) & (MM > %(PhiMassMin)s *MeV) & (MM < %(PhiMassMax)s *MeV)" % self.getProps()
                                )
        Phi = Selection("SelPhi2KKLoose" + name,
                        Algorithm = _phi,
                        RequiredSelections = [_stdPhi])
        return Phi
    

    def Bs2JpsieePhiLoose( self ):
        Jpsi = self.Jpsi2eeLoose( name = 'Bs2JpsieePhiLoose')
        Phi = self.Phi2KKLoose( name = 'Bs2JpsieePhiLoose' )
        _Bs = CombineParticles("Bs2JpsieePhiLoose",
                               DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               MotherCut = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOF)s) & ((M-M1+3096.916*MeV) > %(BsPseudoMassMin)s *MeV) & ((M-M1+3096.916*MeV) < %(BsPseudoMassMax)s *MeV)" % self.getProps() ,
                               ReFitPVs = True
                               )
        Bs = Selection("SelBs2JpsieePhiLoose",
                       Algorithm = _Bs,
                       RequiredSelections = [Jpsi, Phi])
        return Bs
    

    def Bs2JpsieePhi( self ):

        Jpsi = self.Jpsi2eeLoose( name = 'Bs2JpsieePhi' )
        Phi = self.Phi2KKLoose( name = 'Bs2JpsieePhi' )
        
        _Bs = CombineParticles("Bs2JpsieePhi",
                               DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               DaughtersCuts = {"J/psi(1S)" : "(2==NINTREE((ABSID=='e+') & (MIPCHI2DV(PRIMARY) > %(ElectronMIPCHI2)s )))" % self.getProps(),
                                                "phi(1020)" : "(2==NINTREE((ABSID=='K+') & (MIPCHI2DV(PRIMARY) > %(KaonMIPCHI2)s )))" % self.getProps() },
                               MotherCut = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOF)s) & ((M-M1+3096.916*MeV) >  %(BsPseudoMassMin)s *MeV) & ((M-M1+3096.916*MeV) < %(BsPseudoMassMax)s *MeV) & (BPVDIRA > %(BsDIRA)s )" % self.getProps() ,
                               ReFitPVs = True
                              )
        
        
        Bs = Selection("SelBs2JpsieePhi",
                       Algorithm = _Bs,
                       RequiredSelections = [Jpsi, Phi])
        return Bs



    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d








