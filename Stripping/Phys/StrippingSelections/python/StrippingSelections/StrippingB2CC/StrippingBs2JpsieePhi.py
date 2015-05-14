__author__ = 'Artur Ukleja, Jibo He, Konrad Klimaszewski'
__date__ = '2011/03/01'

'''
Bs->JpsieePhi stripping selection

Exports the following stripping lines
- BetaSBs2JpsieePhiLine           - Line _without_ Bs lifetime cut, prescaled by 0.1
- BetaSBs2JpsieePhiDetachedLine   - Line _with_ Bs lifetime cut based on StdLooseDielectron
- BetaSBs2JpsieePhiFromTracksLine - Line _with_ Bs lifetime cut based on DielectronMaker
'''

__all__ = (
    'Bs2JpsieePhiConf',
    'default_config'
    )

default_config = {
    'NAME'              : 'BetaSBs2JpsieePhi',
    'BUILDERTYPE'       : 'Bs2JpsieePhiConf',
    'CONFIG'    : {
        # BetaSBs2JpsieePhiDetachedLine and BetaSBs2JpsieePhiFromTracksLine
          'ElectronPTLoose'            :   500.    # MeV
        , 'ElectronPIDLoose'           :     0.    # adimensional
        , 'ElectronTrackCHI2pDOFLoose' :     5.    # adimensional
        , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
        , 'JpsiMassMinLoose'           :  1800.    # MeV
        , 'JpsiMassMaxLoose'           :  3600.    # MeV
        , 'KaonTrackCHI2pDOFLoose'     :     5.    # adimensional
        , 'PhiPTLoose'                 :  1000.    # MeV
        , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
        , 'PhiMassMinLoose'            :   990.    # MeV
        , 'PhiMassMaxLoose'            :  1050.    # MeV
        , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
        , 'BsMassMinLoose'             :  4000.    # MeV
        , 'BsMassMaxLoose'             :  6000.    # MeV
        , 'LifetimeCut'                : " & (BPVLTIME()>0.3*ps)"
        , 'PrescaleLoose'              :     1.    # adamenssional
        # BetaSBs2JpsieePhiLine
        , 'ElectronPT'            :   500.    # MeV
        , 'ElectronPID'           :     0.    # adimensional
        , 'ElectronTrackCHI2pDOF' :     5.    # adimensional
        , 'JpsiVertexCHI2pDOF'    :    15.    # adimensional
        , 'JpsiMassMin'           :  1800.    # MeV
        , 'JpsiMassMax'           :  3600.    # MeV
        , 'KaonTrackCHI2pDOF'     :     5.    # adimensional
        , 'KaonPID'               :    -2.    # adimensional
        , 'PhiPT'                 :  1000.    # MeV
        , 'PhiVertexCHI2pDOF'     :    15.    # adimensional
        , 'PhiMassMin'            :   990.    # MeV
        , 'PhiMassMax'            :  1050.    # MeV
        , 'BsVertexCHI2pDOF'      :    10.    # adimensional
        , 'BsMassMin'             :  4000.    # MeV
        , 'BsMassMax'             :  6000.    # MeV
        , 'BsDIRA'                :     0.99  # adimensional
        , 'Prescale'              :     0.1   # adamenssional
        },
    'STREAMS' : [ 'Leptonic' ],
    'WGs'    : [ 'B2CC' ]
    }

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
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
                , 'PrescaleLoose'                  # adimensional
                  
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
                , 'Prescale'                  # adimensional
                )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        # if name not set outside, set it to empty 
        if name == None:
            name = ""        
        self.name = name
        DiElectrons           = DataOnDemand(Location = "Phys/StdLooseDiElectron/Particles")
        DiElectronsFromTracks = DataOnDemand(Location = "Phys/StdDiElectronFromTracks/Particles")
        self.Bs2JpsieePhiLine      = self._Bs2JpsieePhiLine( DiElectrons, name, config )
        self.Bs2JpsieePhiDetachedLine = self._Bs2JpsieePhiDetachedLine( DiElectrons, name+"Detached", config )
        self.Bs2JpsieePhiFromTracksLine = self._Bs2JpsieePhiDetachedLine( DiElectronsFromTracks, name+"FromTracks", config )

        # Tests CPU time required for construction of StdLooseDiElectron
        # self.DielectronTestLine      = self._DielectronTestLine( DiElectrons, "DielectronTest", config )
        # self.registerLine( self.DielectronTestLine )

        self.registerLine( self.Bs2JpsieePhiDetachedLine )
        self.registerLine( self.Bs2JpsieePhiFromTracksLine )
        self.registerLine( self.Bs2JpsieePhiLine )


    def _Bs2JpsieePhiLine( self, dielectron, name, config ) :

        _jpsi = FilterDesktop("FilterJpsi2eeFor"+name,
                              Code = "   (MM > %(JpsiMassMin)s *MeV)" \
                                     " & (MM < %(JpsiMassMax)s *MeV)" \
                                     " & (MINTREE('e+'==ABSID,PIDe-PIDpi) > %(ElectronPID)s )" \
                                     " & (MINTREE('e+'==ABSID,PT) > %(ElectronPT)s *MeV)" \
                                     " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTrackCHI2pDOF)s)" \
                                     " & (VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOF)s)" % config
                             )
        Jpsi = Selection("SelJpsi2eeFor"+name,
                         Algorithm = _jpsi,
                         RequiredSelections = [dielectron])

        _stdPhi = DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles")
        _phi = FilterDesktop("FilterPhi2KKFor"+name,
                             Code = "   (PT > %(PhiPT)s *MeV)" \
                                    " & (MINTREE('K+'==ABSID,PIDK-PIDpi) > %(KaonPID)s )" \
                                    " & (MAXTREE('K+'==ABSID,TRCHI2DOF) < %(KaonTrackCHI2pDOF)s)" \
                                    " & (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOF)s)" \
                                    " & (MM > %(PhiMassMin)s *MeV)" \
                                    " & (MM < %(PhiMassMax)s *MeV)" % config
                            )
        Phi = Selection("SelPhi2KKFor"+name,
                        Algorithm = _phi,
                        RequiredSelections = [_stdPhi])

        CC = "(AM > %(BsMassMin)s *MeV) & (AM < %(BsMassMax)s *MeV)" % config
        MC = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOF)s) & (BPVDIRA > %(BsDIRA)s)" % config
        _Bs = CombineParticles("CombineBsFor"+name,
                               DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               CombinationCut = CC ,
                               MotherCut = MC,
                               ReFitPVs = False
                               )
        Bs = Selection(name,
                       Algorithm = _Bs,
                       RequiredSelections = [Jpsi, Phi])

        return StrippingLine(name+"Line"
              , prescale = config['Prescale']
              , postscale = 1
              , selection = Bs
              , EnableFlavourTagging = True
              , MDSTFlag = True
              )


    def _Bs2JpsieePhiDetachedLine( self, dielectron, name, config ) :
        
        _jpsi = FilterDesktop("FilterJpsi2eeFor"+name,
                              Code = "   (MM > %(JpsiMassMinLoose)s *MeV)" \
                                     " & (MM < %(JpsiMassMaxLoose)s *MeV)" \
                                     " & (MINTREE('e+'==ABSID,PIDe-PIDpi) > %(ElectronPIDLoose)s )" \
                                     " & (MINTREE('e+'==ABSID,PT) > %(ElectronPTLoose)s *MeV)" \
                                     " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTrackCHI2pDOFLoose)s)" \
                                     " & (VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOFLoose)s)" % config
                             )
        Jpsi = Selection("SelJpsi2eeFor"+name,
                         Algorithm = _jpsi,
                         RequiredSelections = [dielectron])
    
        _stdPhi = DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles")
        _phi = FilterDesktop("FilterPhi2KKFor"+name,
                             Code = "   (PT > %(PhiPTLoose)s *MeV)" \
                                    " & (MAXTREE('K+'==ABSID,TRCHI2DOF) < %(KaonTrackCHI2pDOFLoose)s)" \
                                    " & (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOFLoose)s)" \
                                    " & (MM > %(PhiMassMinLoose)s *MeV)" \
                                    " & (MM < %(PhiMassMaxLoose)s *MeV)" % config
                            )
        Phi = Selection("SelPhi2KKFor"+name,
                        Algorithm = _phi,
                        RequiredSelections = [_stdPhi])

        CC = "(AM > %(BsMassMinLoose)s *MeV) & (AM < %(BsMassMaxLoose)s *MeV)" % config
        MC = "(VFASPF(VCHI2/VDOF) < %(BsVertexCHI2pDOFLoose)s)" % config
        _Bs = CombineParticles("CombineBsFor"+name,
                               DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                               CombinationCut = CC , 
                               MotherCut = MC + config['LifetimeCut'],
                               ReFitPVs = True
                               )
        Bs = Selection(name,
                       Algorithm = _Bs,
                       RequiredSelections = [Jpsi, Phi])

        return StrippingLine(name+"Line"
              , prescale = config['PrescaleLoose']
              , postscale = 1
              , selection = Bs
              , EnableFlavourTagging = True
              , MDSTFlag = True
              )

    def _DielectronTestLine( self, dielectron, name, config ) :
        return StrippingLine(name+"Line"
              , prescale = 1
              , postscale = 1
              , selection = dielectron
              )
