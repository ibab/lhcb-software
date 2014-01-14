__author__ = 'Patrick Koppenburg'
__date__ = '08/12/2009'
__version__ = '$Revision: 2 $'

"""
B->llK selections for B->eeK(*) versus B->MuMuK(*)
"""

config_params =  {    'BFlightCHI2'         : 100       # adimentional 
                      ,  'BDIRA'               : 0.9995     # adimentional    TIGHTENED
                      ,  'BIPCHI2'             : 25        # adimentional  
                      ,  'BVertexCHI2'         : 9        # adimentional
                      ,  'DiLeptonPT'          : 0         # MeV (not used)
                      ,  'DiLeptonFDCHI2'      : 16        # adimentional
                      ,  'DiLeptonIPCHI2'      : 0         # adimentional
                      ,  'LeptonIPCHI2'        : 16        # adimentional      TIGHTENED
                      ,  'LeptonPT'            : 500       # MeV               
                      ,  'KaonIPCHI2'          : 9        # adimentional                       CHANGED
                      ,  'KaonPT'              : 800       # MeV               LOOSENED
                      ,  'UpperMass'           : 5500      # MeV (Higher bound of signal box)
                      ,  'Bu2eeKLinePrescale'  : 1
                      ,  'Bu2mmKLinePrescale'  : 1
                      ,  'Bu2meKLinePrescale'  : 1
                      ,  'ExtraInfoTools'      : [ { "Type" : "ConeVariables"
                                                , "ConeAngle" : 1.5
                                                , "ConeNumber" : 1
                                                , "Variables" : ['angle', 'mult', 'ptasy']} 
                                                , {'Type' : 'VertexIsolation'} 
                                                 ]
}

__all__ = ('Bu2LLKConf', 'makeDiLepton', 'makeB2LLK' )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons

default_name = "B2LLK"

class Bu2LLKConf(LineBuilder) :
    """
    Builder for B2LLK_mm and B2LL_ee lines with dimuon and dielectron.
    """

    mmKLine = None
    eeKLine = None
    eeKLine2 = None
    meKLine = None
    
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
        ,  'Bu2mmKLinePrescale'
        ,  'Bu2meKLinePrescale'  
      )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        mmKLine_name = name+"_mm"
        eeKLine_name = name+"_ee"
        meKLine_name = name+"_me"
         
        # 1 : Make high IP, Pt kaons
        Kaons = StdLooseKaons
        selKaons = makeKaons(name="KaonsFor"+name
                             , KaonIPCHI2 = config['KaonIPCHI2']
                             , KaonPT = config['KaonPT'])
        selKstars = makeKstars(name="KstarsFor"+name
                              , KaonIPCHI2 = config['KaonIPCHI2']
                              , KaonPT = config['KaonPT'])
        # 2 : Dileptons
        Electrons = DataOnDemand(Location = "Phys/StdLooseDiElectron/Particles")
        Electrons2 = DataOnDemand(Location = "Phys/StdDiElectronFromTracks/Particles") # TEST
        Muons = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
        MuE = makeMuE()
        
        selDiElectron = self._makeDiLepton(name='Dilepton_For'+eeKLine_name,
                                           leptonSel = Electrons,
                                           config=config)
        selDiElectron2 = self._makeDiLepton(name='Dilepton2_For'+eeKLine_name, # TEST
                                           leptonSel = Electrons2,
                                           config=config)
        selDiMuon = self._makeDiLepton(name='Dilepton_For'+mmKLine_name,
                                       leptonSel = Muons,
                                       config=config)
        selMuE = self._makeDiLepton(name='Dilepton_For'+meKLine_name,
                                       leptonSel = MuE,
                                       config=config)
        # 3 : Combine
        selB2LLK_ee = self._makeB2LLK(name=eeKLine_name,
                                      dileptonSel = selDiElectron,
                                      kaonSel = selKaons,
                                      kstarSel = selKstars,
                                      config=config,
                                      massCut="1000*MeV" )
        
        selB2LLK_ee2 = self._makeB2LLK(name=eeKLine_name+"2",
                                        dileptonSel = selDiElectron2,
                                        kaonSel = selKaons,
                                        kstarSel = selKstars,
                                        config=config,
                                       massCut="1000*MeV" )
        
        selB2LLK_mm = self._makeB2LLK(name=mmKLine_name,
                                      dileptonSel = selDiMuon,
                                      kaonSel = selKaons,
                                      kstarSel = selKstars,
                                      config=config,
                                      massCut="600*MeV" )
        
        selB2LLK_me = self._makeB2LLK(name=meKLine_name,
                                      dileptonSel = selMuE,
                                      kaonSel = selKaons,
                                      kstarSel = selKstars,
                                      config=config,
                                      massCut="600*MeV" )
        
        
        # 4 : Declare Lines
        self.eeKLine = StrippingLine(eeKLine_name+"Line",
                                    prescale = config['Bu2eeKLinePrescale'],
                                    postscale = 1,
                                    selection = selB2LLK_ee
                                    ,ExtraInfoTools = config['ExtraInfoTools']
                                    )
        self.eeKLine2 = StrippingLine(eeKLine_name+"Line2",
                                    prescale = config['Bu2eeKLinePrescale'],
                                    postscale = 1,
                                    selection = selB2LLK_ee2
                                    ,ExtraInfoTools = config['ExtraInfoTools']
                                    )
        self.mmKLine = StrippingLine(mmKLine_name+"Line",
                                    prescale = config['Bu2mmKLinePrescale'],
                                    postscale = 1,
                                    selection = selB2LLK_mm
                                    ,ExtraInfoTools = config['ExtraInfoTools']
                                    )
        self.meKLine = StrippingLine(meKLine_name+"Line",
                                    prescale = config['Bu2meKLinePrescale'],
                                    postscale = 1,
                                    selection = selB2LLK_me
                                    ,ExtraInfoTools = config['ExtraInfoTools']
                                    )

        # 5 : register Line
        self.registerLine( self.eeKLine )
        self.registerLine( self.eeKLine2 )
        self.registerLine( self.mmKLine )
        self.registerLine( self.meKLine )
        
#####################################################
    def _makeB2LLK(self, name, dileptonSel, kaonSel, kstarSel, config, massCut):
        """
        Handy interface for B2LLK
        """
        # 
        # B mass cuts : Hard-coded as we _need_ the full B mass window for the final fit. Nobody dare touch that!
        _combcut = "(ADAMASS('B+')<"+massCut+")"
        # 
        # B candidate cuts : ((VFASPF(VCHI2/VDOF)<3) & (BPVIPCHI2()<25) & (BPVDIRA>0.9998) & (BPVVDCHI2>50))
        _bcut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) "\
                  "& (BPVIPCHI2()< %(BIPCHI2)s ) "\
                  "& (BPVDIRA> %(BDIRA)s ) "\
                  "& (BPVVDCHI2> %(BFlightCHI2)s ))" % config
        
        return makeB2LLK(name
                         , dileptonSel
                         , kaonSel
                         , kstarSel
                         , _combcut
                         , _bcut) 
    
#####################################################
    def _makeDiLepton(self, name,leptonSel, config) :
        """
        Handy interface for dileptons
        """
        _code = "(PT > %(DiLeptonPT)s *MeV) & "\
                "(MM >100*MeV) & "\
                "(MM < %(UpperMass)s *MeV) & "\
                "(MINTREE(ABSID<14,PT)>%(LeptonPT)s *MeV) & "\
                "(MINTREE(ABSID<14,MIPCHI2DV(PRIMARY))>%(LeptonIPCHI2)s) & "\
                "(VFASPF(VCHI2/VDOF)<9) & (BPVVDCHI2> %(DiLeptonFDCHI2)s) & "\
                "(MIPCHI2DV(PRIMARY) > %(DiLeptonIPCHI2)s )" % config
    
        return makeDiLepton(name
                            ,leptonSel
                            ,_code)
    
#
# Out of class
#####################################################
def makeB2LLK(name, dileptonSel, kaonSel, kstarSel, _combcut, _bcut ):
    """
    Makes the B+   -> clone this
    """
    _Decays =  ["[ B+ -> J/psi(1S) K+ ]cc",
                "[ B0 -> J/psi(1S) K*(892)0 ]cc"]

    _Combine = CombineParticles(DecayDescriptors = _Decays,
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    
    _Merge   = MergedSelection("Merge"+name, 
                               RequiredSelections = [ kaonSel, kstarSel ])
    
    
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ _Merge, dileptonSel ]) 

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
def makeKstars(name, KaonIPCHI2, KaonPT):
    """
    Kaon selection
    """
    _code = "(PT > %(KaonPT)s *MeV) & "\
            "(INTREE((ABSID=='K+') & (MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s ))) & "\
            "(INTREE((ABSID=='pi+') & (MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s )))" % locals()
    _Filter = FilterDesktop(Code = _code)
    _stdKstar = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles")   
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ _stdKstar ] )


#####################################################
def makeDiLepton(name,leptonSel,_code):
    """
    makes the dilepton
    """
    _dilFilter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _dilFilter,
                     RequiredSelections = [ leptonSel ] )

#####################################################
def makeMuE():
    """
    makes the dilepton
    """
    from StandardParticles import StdLooseElectrons as Electrons 
    from StandardParticles import StdLooseMuons as Muons
    _combMuE = CombineParticles(DecayDescriptor = "[J/psi(1S) -> mu+ e-]cc",
                                DaughtersCuts = { "e+" : "(PT>500*MeV)", "mu+" : "(PT>500*MeV)" },
                                CombinationCut="(ADOCACHI2CUT(30, '')) & (AM>30*MeV) ",
                                MotherCut = "(VFASPF(VCHI2) < 25)")
    return Selection("SelMuEForBu2LLK",
                     Algorithm = _combMuE,
                     RequiredSelections = [ Muons, Electrons ] )

