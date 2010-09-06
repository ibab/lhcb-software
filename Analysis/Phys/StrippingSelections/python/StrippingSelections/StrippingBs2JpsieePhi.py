
__author__ = 'Artur Ukleja, Jibo He'
__date__ = '2010/07/16'
__version__ = '$Revision: 1.4 $'

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
                  'ElectronPTLoose'          :   700.    # MeV
                , 'JpsiVertexCHI2pDOFLoose'  :    16.    # adimensional
                , 'JpsiMassMinLoose'         :  2500.    # MeV
                , 'JpsiMassMaxLoose'         :  3300.    # MeV
                , 'PhiPTLoose'               :  1000.    # MeV
                , 'PhiVertexCHI2pDOFLoose'   :    16.    # adimensional
                , 'PhiMassMinLoose'          :   995.    # MeV
                , 'PhiMassMaxLoose'          :  1045.    # MeV
                , 'BsVertexCHI2pDOFLoose'    :    16.    # adimensional
                , 'BsMassMinLoose'           :  3500.    # MeV
                , 'BsMassMaxLoose'           :  7000.    # MeV

                , 'ElectronPT'          :   800.    # MeV
                , 'ElectronPID'         :     2.    # adimensional
                , 'JpsiVertexCHI2pDOF'  :    15.    # adimensional
                , 'JpsiMassMin'         :  2500.    # MeV
                , 'JpsiMassMax'         :  3300.    # MeV
                , 'PhiPT'               :  1000.    # MeV
                , 'PhiVertexCHI2pDOF'   :    15.    # adimensional
                , 'PhiMassMin'          :  1005.    # MeV
                , 'PhiMassMax'          :  1035.    # MeV
                , 'BsVertexCHI2pDOF'    :     6.    # adimensional
                , 'BsMassMin'           :  4000.    # MeV
                , 'BsMassMax'           :  6000.    # MeV
                , 'BsDIRA'              :     0.99  # adimensional
                }


    def UnbiasedPT_line( self ) :
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2JpsieePhiLoose', prescale = 0.01, algos = [ self.Bs2JpsieePhiLoose() ])


    def Signalbox_line( self ) :
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2JpsieePhi', prescale = 1., algos = [ self.Bs2JpsieePhi() ])
      

    def Jpsi2eeLoose( self ):
        _stdJpsi = DataOnDemand( Location="Phys/StdLooseJpsi2ee" )
        _jpsi = FilterDesktop("Jpsi2eeFilterForBs2JpsieePhiLoose",
                              Code = "   (MINTREE('e+'==ABSID,PT) > %(ElectronPTLoose)s *MeV)" \
                                     " & (VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOFLoose)s)" \
                                     " & (MM > %(JpsiMassMinLoose)s *MeV)" \
                                     " & (MM < %(JpsiMassMaxLoose)s *MeV)" % self.getProps()
                             )
        Jpsi = Selection("SelJpsi2eeForBs2JpsieePhiLoose",
                         Algorithm = _jpsi,
                         RequiredSelections = [_stdJpsi])
        return Jpsi
    

    def Jpsi2ee( self ):
        _stdJpsi = DataOnDemand( Location="Phys/StdLooseJpsi2ee" )
        _jpsi = FilterDesktop("Jpsi2eeFilterForBs2JpsieePhi",
                              Code = "   (MINTREE('e+'==ABSID,PT) > %(ElectronPT)s *MeV)" \
                                     " & (MINTREE('e+'==ABSID,PIDe-PIDpi) > %(ElectronPID)s )" \
                                     " & (VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOF)s)" \
                                     " & (MM > %(JpsiMassMin)s *MeV)" \
                                     " & (MM < %(JpsiMassMax)s *MeV)" % self.getProps()
                             )
        Jpsi = Selection("SelJpsi2eeForBs2JpsieePhi",
                         Algorithm = _jpsi,
                         RequiredSelections = [_stdJpsi])
        return Jpsi



    def Phi2KKLoose( self ):
        _stdPhi = DataOnDemand(Location="Phys/StdLoosePhi2KK")
        _phi = FilterDesktop("Phi2KKFilterForBs2JpsieePhiLoose",
                             Code = "   (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOFLoose)s)" \
                                    " & (PT > %(PhiPTLoose)s *MeV)"\
                                    " & (MM > %(PhiMassMinLoose)s *MeV)" \
                                    " & (MM < %(PhiMassMaxLoose)s *MeV)" % self.getProps()
                            )
        Phi = Selection("SelPhi2KKForBs2JpsieePhiLoose",
                        Algorithm = _phi,
                        RequiredSelections = [_stdPhi])
        return Phi
    

    def Phi2KK( self ):
        _stdPhi = DataOnDemand(Location="Phys/StdLoosePhi2KK")  
        _phi = FilterDesktop("Phi2KKFilterForBs2JpsieePhi",
                             Code = "   (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOF)s)" \
                                    " & (PT > %(PhiPT)s *MeV)"\
                                    " & (MM > %(PhiMassMin)s *MeV)" \
                                    " & (MM < %(PhiMassMax)s *MeV)" % self.getProps()
                            )
        Phi = Selection("SelPhi2KKForBs2JpsieePhi",
                        Algorithm = _phi,
                        RequiredSelections = [_stdPhi])
        return Phi           



    def Bs2JpsieePhiLoose( self ):
        Jpsi = self.Jpsi2eeLoose()
        Phi = self.Phi2KKLoose()
        CC = "(AM > %(BsMassMinLoose)s *MeV) & (AM < %(BsMassMaxLoose)s *MeV)" % self.getProps()
        MC = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOFLoose)s)" % self.getProps()
        _Bs = CombineParticles("Bs2JpsieePhiLoose",
                               DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               CombinationCut = CC , 
                               MotherCut = MC ,
                               ReFitPVs = True
                               )
        Bs = Selection("SelBs2JpsieePhiLoose",
                       Algorithm = _Bs,
                       RequiredSelections = [Phi, Jpsi])
        return Bs
    


    def Bs2JpsieePhi( self ):
        Jpsi = self.Jpsi2ee()
        Phi = self.Phi2KK()
        CC = "(AM > %(BsMassMin)s *MeV) & (AM < %(BsMassMax)s *MeV)" % self.getProps()
        MC = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOF)s) & (BPVDIRA > %(BsDIRA)s)" % self.getProps()
        _Bs = CombineParticles("Bs2JpsieePhi",
                               DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               CombinationCut = CC ,
                               MotherCut = MC ,
                               ReFitPVs = True
                               )
        Bs = Selection("SelBs2JpsieePhi",
                       Algorithm = _Bs,
                       RequiredSelections = [Phi, Jpsi])
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








