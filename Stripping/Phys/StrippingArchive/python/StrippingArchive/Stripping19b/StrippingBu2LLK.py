__author__ = 'Patrick Koppenburg'
__date__ = '08/12/2009'
__version__ = '$Revision: 2 $'

"""
B->llK selections for B->eeK versus B->MuMuK
"""

config_params =  {    'BFlightCHI2'         : 100       # adimentional 
                      ,  'BDIRA'               : 0.9995     # adimentional    TIGHTENED
                      ,  'BIPCHI2'             : 25        # adimentional  
                      ,  'BVertexCHI2'         : 16        # adimentional
                      ,  'DiLeptonPT'          : 0         # MeV (not used)
                      ,  'DiLeptonFDCHI2'      : 16        # adimentional
                      ,  'DiLeptonIPCHI2'      : 9         # adimentional
                      ,  'LeptonIPCHI2'        : 16        # adimentional      TIGHTENED
                      ,  'LeptonPT'            : 800       # MeV               
                      ,  'KaonIPCHI2'          : 16        # adimentional      TIGHTENED
                      ,  'KaonPT'              : 800       # MeV               LOOSENED
                      ,  'UpperMass'           : 5500      # MeV (Higher bound of signal box)
                      ,  'Bu2eeKLinePrescale'  : 1
                      ,  'Bu2eeKLinePostscale' : 1
                      ,  'Bu2mmKLinePrescale'  : 1
                      ,  'Bu2mmKLinePostscale' : 1 }

__all__ = ('Bu2LLKConf', 'makeDiLepton', 'makeBu2LLK' )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons

name = "Bu2LLK"

class Bu2LLKConf(LineBuilder) :
    """
    Builder for Bu2LLK_mm and Bu2LL_ee lines with dimuon and dielectron.
    """

    mmLine = None
    eeLine = None
    eeLine2 = None
    
    __configuration_keys__ = ( # now just define keys. Default values are fixed later
        'BFlightCHI2'        
        ,  'BDIRA'             
        ,  'BIPCHI2'           
        ,  'BVertexCHI2'       
        ,  'DiLeptonPT'        
        ,  'DiLeptonFDCHI2'     
        ,  'DiLeptonIPCHI2'     
        ,  'LeptonIPCHI2'       
        ,  'LeptonPT'          
        ,  'KaonIPCHI2'        
        ,  'KaonPT'             
        ,  'UpperMass'           
        ,  'Bu2eeKLinePrescale'
        ,  'Bu2eeKLinePostscale'
        ,  'Bu2mmKLinePrescale'
        ,  'Bu2mmKLinePostscale'
       )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        mmLine_name = name+"_mm"
        eeLine_name = name+"_ee"
        
        # 1 : Make high IP, Pt kaons
        Kaons = StdLooseKaons
        selKaons = makeKaons(name="KaonsFor"+name
                             , KaonIPCHI2 = config['KaonIPCHI2']
                             , KaonPT = config['KaonPT'])
        # 2 : Dileptons
        Electrons = DataOnDemand(Location = "Phys/StdLooseDiElectron/Particles")
        Electrons2 = DataOnDemand(Location = "Phys/StdDiElectronFromTracks/Particles") # TEST
        Muons = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
        selDiElectron = self._makeDiLepton(name='Dilepton_For'+eeLine_name,
                                           leptonSel = Electrons,
                                           config=config)
        selDiElectron2 = self._makeDiLepton(name='Dilepton2_For'+eeLine_name, # TEST
                                           leptonSel = Electrons2,
                                           config=config)
        selDiMuon = self._makeDiLepton(name='Dilepton_For'+mmLine_name,
                                       leptonSel = Muons,
                                       config=config)
        # 3 : Combine
        selBu2LLK_ee = self._makeBu2LLK(name=eeLine_name,
                                        dileptonSel = selDiElectron,
                                        kaonSel = selKaons,
                                        config=config)
        
        selBu2LLK_ee2 = self._makeBu2LLK(name=eeLine_name+"2",
                                        dileptonSel = selDiElectron2,
                                        kaonSel = selKaons,
                                        config=config)
        
        selBu2LLK_mm = self._makeBu2LLK(name=mmLine_name,
                                        dileptonSel = selDiMuon,
                                        kaonSel = selKaons,
                                        config=config)
        
        # 4 : Declare Lines
        self.eeLine = StrippingLine(eeLine_name+"Line",
                                    prescale = config['Bu2eeKLinePrescale'],
                                    postscale = config['Bu2eeKLinePostscale'],
                                    selection = selBu2LLK_ee
                                    )
        self.eeLine2 = StrippingLine(eeLine_name+"Line2",
                                    prescale = config['Bu2eeKLinePrescale'],
                                    postscale = config['Bu2eeKLinePostscale'],
                                    selection = selBu2LLK_ee2
                                    )
        self.mmLine = StrippingLine(mmLine_name+"Line",
                                    prescale = config['Bu2mmKLinePrescale'],
                                    postscale = config['Bu2mmKLinePostscale'],
                                    selection = selBu2LLK_mm
                                    )
        # 5 : register Line
        self.registerLine( self.eeLine )
        self.registerLine( self.eeLine2 )
        self.registerLine( self.mmLine )
        
#####################################################
    def _makeBu2LLK(self, name, dileptonSel, kaonSel, config):
        """
        Handy interface for Bu2LLK
        """
        return makeBu2LLK(name
                          , dileptonSel
                          , kaonSel
                          , BFlightCHI2 = config['BFlightCHI2']
                          , BDIRA = config['BDIRA']
                          , BIPCHI2 = config['BIPCHI2']
                          , BVertexCHI2 = config['BVertexCHI2'])
    
#####################################################
    def _makeDiLepton(self, name,leptonSel, config) :
        """
        Handy interface for dileptons
        """
        return makeDiLepton(name
                            ,leptonSel
                            ,DiLeptonPT = config['DiLeptonPT']
                            ,DiLeptonFDCHI2 = config['DiLeptonFDCHI2']
                            ,DiLeptonIPCHI2 = config['DiLeptonIPCHI2']
                            ,LeptonPT = config['LeptonPT']
                            ,LeptonIPCHI2 = config['LeptonIPCHI2']
                            ,UpperMass = config['UpperMass'])
    
#
# Out of class
#####################################################
def makeBu2LLK(name, dileptonSel, kaonSel, BFlightCHI2,BDIRA,BIPCHI2,BVertexCHI2):
    """
    Makes the Bd
    """
    # 
    # B mass cuts : Hard-coded as we _need_ the full B mass window for the final fit. Nobody dare touch that!
    _combcut = "(ADAMASS('B0')<600*MeV)"
    # 
    # B candidate cuts : ((VFASPF(VCHI2/VDOF)<3) & (BPVIPCHI2()<25) & (BPVDIRA>0.9998) & (BPVVDCHI2>50))
    _bcut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) "\
              "& (BPVIPCHI2()< %(BIPCHI2)s ) "\
              "& (BPVDIRA> %(BDIRA)s ) "\
              "& (BPVVDCHI2> %(BFlightCHI2)s ))" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[ B+ -> J/psi(1S) K+ ]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, dileptonSel ] )

#####################################################
def makeKaons(name, KaonIPCHI2, KaonPT):
    """
    Kaon selection
    """
    _code = "(PT > %(KaonPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s)" % locals()
    _Filter = FilterDesktop(Code = _code)
    _stdKaons = StdLooseKaons
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ _stdKaons ] )


#####################################################
def makeDiLepton(name,leptonSel,DiLeptonPT,DiLeptonFDCHI2,DiLeptonIPCHI2,LeptonPT,LeptonIPCHI2,UpperMass):
    """
    makes the dilepton
    """
    _code = "(PT > %(DiLeptonPT)s *MeV) & "\
            "(MM >100*MeV) & "\
            "(MM < %(UpperMass)s *MeV) & "\
            "(MINTREE(ABSID<14,PT)>%(LeptonPT)s *MeV) & "\
            "(MINTREE(ABSID<14,MIPCHI2DV(PRIMARY))>%(LeptonIPCHI2)s) & "\
            "(VFASPF(VCHI2/VDOF)<9) & (BPVVDCHI2> %(DiLeptonFDCHI2)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(DiLeptonIPCHI2)s )" % locals()
    
    _dilFilter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _dilFilter,
                     RequiredSelections = [ leptonSel ] )

