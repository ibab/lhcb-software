
__author__ = ['Mike Sokoloff']
__date__ = '25/10/2012'
__version__ = '$Revision: 0.1 $'

'''
stripping code for Kshort --> PiPiMuMu
derived from StrippingD2XMuMuSS.py by Malcolm John, B. Viaud, and O. Kochebina
D->Xu(Xu) mu mu , Xu=pi/K channels and control samples.
'''

default_config =  {
    'NAME' : 'Kshort2PiPiMuMu',
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Kshort2PiPiMuMuConf',
    'CONFIG' : {
          'MuonP'         : 3000. ,    #MeV
          'MuonPT'        : 500.  ,    #MeV
          'MuonMINIPCHI2' : 2     ,    #adminensional
          'MuonTRCHI2'    : 5     ,    #adminensional
          
          'PionP'         : 2000. ,    #MeV
          'PionPT'        : 300.  ,    #MeV
          'PionMINIPCHI2' : 2     ,    #adminensional
          'PionTRCHI2'    : 5     ,    #adminensional
          
          
          #4body
          'MINIPCHI2_hhmumu' : 4. ,  #adminensional --  for the individual pions
          'PT_hhmumu'        : 300 , #MeV
          'MuonPIDmu_hhmumu' : -1,   #adimensional
          'DPT_hhmumu'           : 300., # was 2500    #MeV
          'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional         
          'DMAXDOCA_hhmumu'      : 0.2  ,    #mm
          'KsDauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
          'DFDCHI2_hhmumu'       : 9     ,   #adimensional
          'DIPCHI2_hhmumu'       : 20    ,    #adimensional
          
          
          'DDIRA'         : 0.9999,    #adimensional
          'MaxDimuonMass'    : 260.  ,    #MeV
          'MaxKsMass'                   : 550.  ,    #MeV, comb mass window
          'KsMotherMassCut'             : 540.  ,    # MeV, mother mass cut
          
          'Kshort2PiPiMuMuLinePrescale'    : 1 ,
          'Kshort2PiPiMuMuLinePostscale'   : 1
          },
    'STREAMS' : ['Leptonic']
    }


__all__ = ('Kshort2PiPiMuMuConf',
           'default_config')


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdLoosePions, StdLooseMuons, StdNoPIDsPions
from StandardParticles import  StdAllLoosePions, StdAllLooseMuons, StdAllNoPIDsPions
from StandardParticles import  StdNoPIDsDownPions, StdLooseDownMuons

default_name = "Ks2PiPiMuMu"

class Kshort2PiPiMuMuConf(LineBuilder) :
    """
    Builder for Kshort --> pi,pi,mu,mu 
    """
    
    LongLine = None

        
    __configuration_keys__ = (  'MuonP'
                                , 'MuonPT'
                                , 'MuonMINIPCHI2'
                                , 'MuonTRCHI2'
                                , 'PionP'
                                , 'PionPT'
                                , 'PionMINIPCHI2'
                                , 'PionTRCHI2'
                                
                                #4body
                                , 'MINIPCHI2_hhmumu' #  Added by B. VIAUD for hhmumu modes
                                , 'PT_hhmumu' #   Added by B. VIAUD for hhmumu modes
                                , 'MuonPIDmu_hhmumu'#   Added by O. KOCHEBINA for hhmumu modes
                                , 'DPT_hhmumu' #   Added by O. KOCHEBINA for hhmumu modes
                                , 'KsDauMAXIPCHI2_hhmumu'# Added by B. VIAUD for hhmumu modes
                                , 'DVCHI2DOF_hhmumu'        
                                , 'DMAXDOCA_hhmumu'  
                                , 'DFDCHI2_hhmumu'     
                                , 'DIPCHI2_hhmumu'   
                                , 'DDIRA'
                                , 'MaxDimuonMass'
                                , 'MaxKsMass'
                                , 'KsMotherMassCut'
                                
                                , 'Kshort2PiPiMuMuLinePrescale'
                                , 'Kshort2PiPiMuMuLinePostscale'
                                )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        

        LongLine_name = name+"_Long"
        DownstreamLine_name = name+"_Down"
        
        
        
        # 5 : Make Muons for Kshort2PiPiMuMu 
        selLongMuonsForPiPiMuMu = makeLongMuonsForPiPiMuMu(name="LongMuonsFor"+name
                                               , MuonP = config['MuonP']
                                               , MuonPT = config['PT_hhmumu']
                                               , MuonMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                               , MuonTRCHI2 = config['MuonTRCHI2']
                                               , MuonPIDmu_CS_hhmumu = config['MuonPIDmu_hhmumu'])

        selDownstreamMuonsForPiPiMuMu = makeDownstreamMuonsForPiPiMuMu(name="DownMuonsFor"+name
                                               , MuonP = config['MuonP']
                                               , MuonPT = config['PT_hhmumu']
                                               , MuonMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                               , MuonTRCHI2 = config['MuonTRCHI2']
                                               , MuonPIDmu_CS_hhmumu = config['MuonPIDmu_hhmumu'])

        # 7 : Make Pions for Kshort2PiPiMuMu 
        selLongPionsForPiPiMuMu = makeLongPionsForPiPiMuMu(name="LongPionsFor"+name
                             , PionP = config['PionP']
                             , PionPT = config['PT_hhmumu']
                             , PionMINIPCHI2 = config['MINIPCHI2_hhmumu']
                             , PionTRCHI2 = config['PionTRCHI2'])

        selDownstreamPionsForPiPiMuMu = makeDownstreamPionsForPiPiMuMu(name="DownPionsFor"+name
                             , PionP = config['PionP']
                             , PionPT = config['PT_hhmumu']
                             , PionMINIPCHI2 = config['MINIPCHI2_hhmumu']
                             , PionTRCHI2 = config['PionTRCHI2'])




                                        
        # 7 : Combine
        
        selLongKshort2PiPiMuMu = self._makeKshort2PiPiMuMu(name=LongLine_name,
                                           pionSel = selLongPionsForPiPiMuMu,
                                           muonSel = selLongMuonsForPiPiMuMu,
                                           config = config)
        
        selDownstreamKshort2PiPiMuMu = self._makeKshort2PiPiMuMu(name=DownstreamLine_name,
                                           pionSel = selDownstreamPionsForPiPiMuMu,
                                           muonSel = selDownstreamMuonsForPiPiMuMu,
                                           config = config)
        
        # 8 : Declare Lines
        
        self.LongLine = StrippingLine(LongLine_name+"Line",
                                    prescale = config['Kshort2PiPiMuMuLinePrescale'],
                                    postscale = config['Kshort2PiPiMuMuLinePostscale'],
                                    selection = selLongKshort2PiPiMuMu
                                    )
        
        self.DownstreamLine = StrippingLine(DownstreamLine_name+"Line",
                                    prescale = config['Kshort2PiPiMuMuLinePrescale'],
                                    postscale = config['Kshort2PiPiMuMuLinePostscale'],
                                    selection = selDownstreamKshort2PiPiMuMu
                                    )
        
        
        

        # 9 : register Line

        self.registerLine( self.LongLine )
        self.registerLine( self.DownstreamLine )


#####################################################
    def _makeKshort2PiPiMuMu(self, name, pionSel, muonSel, config):
        """
        Handy interface for Kshort2PiPiMuMu
        """
        return makeKshort2PiPiMuMu(name
                               , pionSel
                               , muonSel
                               , DMAXDOCA = config['DMAXDOCA_hhmumu']
                               , DPT = config['DPT_hhmumu']
                               , KsDauMAXIPCHI2= config['KsDauMAXIPCHI2_hhmumu']
                               , DFDCHI2= config['DFDCHI2_hhmumu']
                               , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                               , DDIRA = config['DDIRA']
                               , DIPCHI2 = config['DIPCHI2_hhmumu']
                               , MaxKsMass = config['MaxKsMass']
                               , KsMotherMassCut = config['KsMotherMassCut']
                               , MaxDimuonMass = config['MaxDimuonMass'])




#####################################################
def makeKshort2PiPiMuMu(name, pionSel, muonSel, DMAXDOCA, DPT,  KsDauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, MaxKsMass, KsMotherMassCut, MaxDimuonMass):

    """
    Makes the KS0 -> pi+ pi- mu+ mu-
    """

    _combcut = "(AM < %(MaxKsMass)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM34 < %(MaxDimuonMass)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(KsDauMAXIPCHI2)s)  )   )" %locals()



    _mothercut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
                   "(PT > %(DPT)s *MeV) &"\
                   "(M < %(KsMotherMassCut)s *MeV) &"\
                   "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
                   "(BPVDIRA > %(DDIRA)s)" % locals()


    _Combine = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi- mu+ mu-",
                                CombinationCut = _combcut,
                                MotherCut = _mothercut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel] )                    
                    
#####################################################
def makeLongMuonsForPiPiMuMu(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2, MuonPIDmu_CS_hhmumu):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(PIDmu-PIDpi > %(MuonPIDmu_CS_hhmumu)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseMuons ] )

def makeDownstreamMuonsForPiPiMuMu(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2, MuonPIDmu_CS_hhmumu):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(PIDmu-PIDpi > %(MuonPIDmu_CS_hhmumu)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseDownMuons ] )

#####################################################
def makeLongPionsForPiPiMuMu(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
						"(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLoosePions ] )


def makeDownstreamPionsForPiPiMuMu(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
						"(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdNoPIDsDownPions ] )




