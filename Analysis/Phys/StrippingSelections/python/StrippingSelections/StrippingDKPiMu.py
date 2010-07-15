__author__ = 'M. Patel, T.Blake'
__date__ = '15/07/2010'
__version__ = '$Revision: 1.1 $'

"""
Stripping Selection for D+ -> K- pi+ mu+
Cuts aim to mimic mu + track
"""


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingDKPiMuConf(LHCbConfigurableUser):
    """
    Class defining D+ -> K- pi+ mu+ stripping line
    Callable functions are: self.line_DKpimu()
    """
    __slots__ = {
        'DKpimuTrackIPCHI2'   : 9        #dimensionless
        , 'DKpimuTrackCHI2'   : 6        #dimensionless
        , 'DKpimuFD'          : 1.0      #mm
        , 'DKpimuDIRA'        : 0.9995   #dimensionless
        , 'DKpimuVertexCHI2'  : 16       #dimensionless
        , 'DKpimuTrackDOCA'   : 1.0      #mm
        , 'DKpimuMuPT'        : 1300.0   #MeV
        , 'DKpimuTrackPT'     : 600.0    #MeV
        , 'DKpimuMassLowWin'  : 1820.0   #MeV
        , 'DKpimuMassHighWin' : 2200.0   #MeV
        }

    def _DKpimu_D(self):
        """
        CombineParticles for D+ > K- pi+ mu+
        """
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        _pions = DataOnDemand(Location='Phys/StdNoPIDsPions')
        _muons = DataOnDemand(Location='Phys/StdLooseMuons')
        _kaons = DataOnDemand(Location='Phys/StdLooseKaons')
        
        _trackCuts = "(TRCHI2DOF < %(DKpimuTrackCHI2)s ) & " \
                     "(MIPCHI2DV(PRIMARY) > %(DKpimuTrackIPCHI2)s )" % self.getProps()
        
        _muonCuts = "(TRCHI2DOF < %(DKpimuTrackCHI2)s ) & " \
                    "(PT > %(DKpimuMuPT)s *MeV) & " \
                    "(MIPCHI2DV(PRIMARY) > %(DKpimuTrackIPCHI2)s )" % self.getProps()
        
        _DKpimu = CombineParticles('D_DKpimu')
        _DKpimu.DecayDescriptor = '[D+ -> K- pi+ mu+]cc'
        _DKpimu.DaughtersCuts = {  'mu+' : _muonCuts, 'K-' : _trackCuts, 'pi+' : _trackCuts }
        
        _DKpimu.CombinationCut = "(AM > %(DKpimuMassLowWin)s *MeV) &" \
                                 "(AM < %(DKpimuMassHighWin)s *MeV)" % self.getProps()
        
        _DKpimu.MotherCut = "(VFASPF(VCHI2/VDOF) < %(DKpimuVertexCHI2)s ) & " \
                            "(1 < NINTREE((PT > %(DKpimuTrackPT)s *MeV))) & " \
                            "(BPVVD > %(DKpimuFD)s *mm) & " \
                            "(BPVDIRA > %(DKpimuDIRA)s )" % self.getProps()
        
        
        _sel = Selection( 'Sel_D_DKpimu',
                          Algorithm=_DKpimu,
                          RequiredSelections=[ _pions, _kaons, _muons ] )
        return _sel
    
    
    def line_DKpimu(self):
        """
        Returns stripping line for DKpimu
        """
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import Selection

        _line = StrippingLine( 'Line_DKpimu',  prescale = 1, algos = [ self._DKpimu_D() ] )
        return _line
    

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

