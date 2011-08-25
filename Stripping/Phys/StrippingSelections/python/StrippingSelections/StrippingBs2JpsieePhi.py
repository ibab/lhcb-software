__author__ = 'Artur Ukleja, Jibo He'
__date__ = '2011/03/01'

'''
Bs->JpsieePhi stripping selection

Exports the following stripping lines
- Bs2JpsieePhiLine
- Bs2JpsieePhiDetachedLine
'''

__all__ = (
    'Bs2JpsieePhiConfConf',
    )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class Bs2JpsieePhiConf(LineBuilder):

    __configuration_keys__ = (
                  'ElectronPTLoose'                # MeV
                , 'ElectronPIDLoose'               # adimensional
                , 'ElectronTrackCHI2pDOFLoose'     # adimensional
                , 'JpsiVertexCHI2pDOFLoose'        # adimensional
                , 'JpsiMassMinLoose'               # MeV
                , 'JpsiMassMaxLoose'               # MeV
                , 'KaonTrackCHI2pDOFLoose'         # adimensional
                , 'PhiPTLoose'                     # MeV
                , 'PhiVertexCHI2pDOFLoose'         # adimensional
                , 'PhiMassMinLoose'                # MeV
                , 'PhiMassMaxLoose'                # MeV
                , 'BsVertexCHI2pDOFLoose'          # adimensional
                , 'BsMassMinLoose'                 # MeV
                , 'BsMassMaxLoose'                 # MeV
                , 'LifetimeCut'                    # cut
                  
                , 'ElectronPT'                # MeV
                , 'ElectronPID'               # adimensional
                , 'ElectronTrackCHI2pDOF'     # adimensional
                , 'JpsiVertexCHI2pDOF'        # adimensional
                , 'JpsiMassMin'               # MeV
                , 'JpsiMassMax'               # MeV
                , 'KaonTrackCHI2pDOF'         # adimensional
                , 'KaonPID'                   # adimensional
                , 'PhiPT'                     # MeV
                , 'PhiVertexCHI2pDOF'         # adimensional
                , 'PhiMassMin'                # MeV
                , 'PhiMassMax'                # MeV
                , 'BsVertexCHI2pDOF'          # adimensional
                , 'BsMassMin'                 # MeV
                , 'BsMassMax'                 # MeV
                , 'BsDIRA'                    # adimensional
                )

    config_default = {
                  'ElectronPTLoose'            :   500.    # MeV
                , 'ElectronPIDLoose'           :     0.    # adimensional
                , 'ElectronTrackCHI2pDOFLoose' :     5.    # adimensional
                , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
                , 'JpsiMassMinLoose'           :  2500.    # MeV
                , 'JpsiMassMaxLoose'           :  3300.    # MeV
                , 'KaonTrackCHI2pDOFLoose'     :     5.    # adimensional
                , 'PhiPTLoose'                 :  1000.    # MeV
                , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
                , 'PhiMassMinLoose'            :   990.    # MeV
                , 'PhiMassMaxLoose'            :  1050.    # MeV
                , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
                , 'BsMassMinLoose'             :  4500.    # MeV
                , 'BsMassMaxLoose'             :  6000.    # MeV
                , 'LifetimeCut'                : " & (BPVLTIME()>0.3*ps)"
                  
                , 'ElectronPT'            :   800.    # MeV
                , 'ElectronPID'           :     2.    # adimensional
                , 'ElectronTrackCHI2pDOF' :     5.    # adimensional
                , 'JpsiVertexCHI2pDOF'    :    15.    # adimensional
                , 'JpsiMassMin'           :  2700.    # MeV
                , 'JpsiMassMax'           :  3300.    # MeV
                , 'KaonTrackCHI2pDOF'     :     5.    # adimensional
                , 'KaonPID'               :     0.    # adimensional
                , 'PhiPT'                 :  1000.    # MeV
                , 'PhiVertexCHI2pDOF'     :    15.    # adimensional
                , 'PhiMassMin'            :  1005.    # MeV
                , 'PhiMassMax'            :  1035.    # MeV
                , 'BsVertexCHI2pDOF'      :     6.    # adimensional
                , 'BsMassMin'             :  4500.    # MeV
                , 'BsMassMax'             :  6000.    # MeV
                , 'BsDIRA'                :     0.99  # adimensional
                }



    def __init__(self, name, config) :
      LineBuilder.__init__(self, name, config)

      self.name = name
      self.Bs2JpsieePhiLine      = self._Bs2JpsieePhiLine( name, config )
      self.Bs2JpsieePhiDetachedLine = self._Bs2JpsieePhiDetachedLine( name+"Detached", config )
      self.registerLine( self.Bs2JpsieePhiLine )
      self.registerLine( self.Bs2JpsieePhiDetachedLine )


    def _Bs2JpsieePhiLine( self, name, config ) :

        _stdJpsi = DataOnDemand( Location="Phys/StdLooseJpsi2ee/Particles" )
        _jpsi = FilterDesktop(Code = "   (MINTREE('e+'==ABSID,PT) > %(ElectronPT)s *MeV)" \
                                     " & (MINTREE('e+'==ABSID,PIDe-PIDpi) > %(ElectronPID)s )" \
                                     " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTrackCHI2pDOF)s)" \
                                     " & (VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOF)s)" \
                                     " & (MM > %(JpsiMassMin)s *MeV)" \
                                     " & (MM < %(JpsiMassMax)s *MeV)" % config
                             )
        Jpsi = Selection("SelJpsi2eeFor"+name,
                         Algorithm = _jpsi,
                         RequiredSelections = [_stdJpsi])

        _stdPhi = DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles")
        _phi = FilterDesktop(Code = "   (MINTREE('K+'==ABSID,PIDK-PIDpi) > %(KaonPID)s )" \
                                    " & (MAXTREE('K+'==ABSID,TRCHI2DOF) < %(KaonTrackCHI2pDOF)s)" \
                                    " & (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOF)s)" \
                                    " & (PT > %(PhiPT)s *MeV)"\
                                    " & (MM > %(PhiMassMin)s *MeV)" \
                                    " & (MM < %(PhiMassMax)s *MeV)" % config
                            )
        Phi = Selection("SelPhi2KKFor"+name,
                        Algorithm = _phi,
                        RequiredSelections = [_stdPhi])

        CC = "(AM > %(BsMassMin)s *MeV) & (AM < %(BsMassMax)s *MeV)" % config
        MC = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOF)s) & (BPVDIRA > %(BsDIRA)s)" % config
        _Bs = CombineParticles(DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               CombinationCut = CC ,
                               MotherCut = MC  
                               )
        Bs = Selection(name,
                       Algorithm = _Bs,
                       RequiredSelections = [Jpsi, Phi])

        return StrippingLine(name+"Line"
              , prescale = 1
              , postscale = 1
              , selection = Bs
              )


    def _Bs2JpsieePhiDetachedLine( self, name, config ) :
        
        _stdJpsi = DataOnDemand( Location="Phys/StdLooseJpsi2ee/Particles" )
        _jpsi = FilterDesktop(Code = "   (MINTREE('e+'==ABSID,PT) > %(ElectronPTLoose)s *MeV)" \
                                     " & (MINTREE('e+'==ABSID,PIDe-PIDpi) > %(ElectronPIDLoose)s )" \
                                     " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTrackCHI2pDOFLoose)s)" \
                                     " & (VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOFLoose)s)" \
                                     " & (MM > %(JpsiMassMinLoose)s *MeV)" \
                                     " & (MM < %(JpsiMassMaxLoose)s *MeV)" % config
                             )
        Jpsi = Selection("SelJpsi2eeFor"+name,
                         Algorithm = _jpsi,
                         RequiredSelections = [_stdJpsi])
    
        _stdPhi = DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles")
        _phi = FilterDesktop(Code = "   (MAXTREE('K+'==ABSID,TRCHI2DOF) < %(KaonTrackCHI2pDOFLoose)s)" \
                                    " & (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOFLoose)s)" \
                                    " & (PT > %(PhiPTLoose)s *MeV)"\
                                    " & (MM > %(PhiMassMinLoose)s *MeV)" \
                                    " & (MM < %(PhiMassMaxLoose)s *MeV)" % config
                            )
        Phi = Selection("SelPhi2KKFor"+name,
                        Algorithm = _phi,
                        RequiredSelections = [_stdPhi])

        CC = "(AM > %(BsMassMinLoose)s *MeV) & (AM < %(BsMassMaxLoose)s *MeV)" % config
        MC = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOFLoose)s)" % config
        _Bs = CombineParticles(DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               CombinationCut = CC , 
                               MotherCut = MC + config['LifetimeCut'] 
                               )
        Bs = Selection(name,
                       Algorithm = _Bs,
                       RequiredSelections = [Jpsi, Phi])

        return StrippingLine(name+"Line"
              , prescale = 1
              , postscale = 1
              , selection = Bs
              )

    









