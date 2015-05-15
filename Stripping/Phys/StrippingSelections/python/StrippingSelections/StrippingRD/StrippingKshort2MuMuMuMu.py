'''
stripping code for  Kshort-->MuMuMuMu
derived, in large measure, from StrippingKshort2PiPiMuMu.py

'''

__author__ = ['Mike Sokoloff']
__date__ = '07 January 2014'
__version__ = '$Revision: 0.1 $'


default_config =  {
    'NAME' : 'Kshort2MuMuMuMu',
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Kshort2MuMuMuMuConf',
    'CONFIG' : {'MuonP'         : 3000. ,    #MeV
                'MuonPT'        : 500.  ,    #MeV
                'MuonMINIPCHI2' : 2     ,    #adminensional
                'MuonTRCHI2'    : 5     ,    #adminensional


                                                  #4body
                'MINIPCHI2_mumumumu' : 4. ,  #adminensional --  for the individual pions
                'PT_mumumumu'        : 300 , #MeV
                'MuonPIDmu_mumumumu' : -1,   #adimensional
                'DPT_mumumumu'           : 2500.,     #MeV
                'DVCHI2DOF_mumumumu'     : 8     ,    #adminensional
                'DMAXDOCA_mumumumu'      : 0.2  ,    #mm
                'KsDauMAXIPCHI2_mumumumu' : 15    ,    #adimensinal
                'DFDCHI2_mumumumu'       : 9     ,   #adimensional
                'DIPCHI2_mumumumu'       : 20    ,    #adimensional


                'DDIRA'         : 0.9999,    #adimensional
                'MaxDimuonMass'    : 260.  ,    #MeV
                'MaxKsMass'                   : 550.  ,    #MeV, comb mass window
                'KsMotherMassCut'             : 540.  ,    # MeV, mother mass cut

                'Kshort2MuMuMuMuLinePrescale'    : 1,
                'Kshort2MuMuMuMuLinePostscale'   : 1
                                },
    'STREAMS' : ['Leptonic']
        }


__all__ = ('Kshort2MuMuMuMuConf',
           'default_config'
           )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdLoosePions, StdLooseMuons, StdNoPIDsPions
from StandardParticles import  StdAllLoosePions, StdAllLooseMuons, StdAllNoPIDsPions
from StandardParticles import  StdNoPIDsDownPions, StdLooseDownMuons


class Kshort2MuMuMuMuConf(LineBuilder) :
    """
    Builder for Kshort --> mu,mu,mu,mu
    """
    #### This is the dictionary of all tunable cuts ########
    __configuration_keys__ = (  'MuonP'
                                , 'MuonPT'
                                , 'MuonMINIPCHI2'
                                , 'MuonTRCHI2'

                                #4body
                                , 'MINIPCHI2_mumumumu' #  Added by B. VIAUD for mumumumu modes
                                , 'PT_mumumumu' #   Added by B. VIAUD for mumumumu modes
                                , 'MuonPIDmu_mumumumu'#   Added by O. KOCHEBINA for mumumumu modes
                                , 'DPT_mumumumu' #   Added by O. KOCHEBINA for mumumumu modes
                                , 'KsDauMAXIPCHI2_mumumumu'# Added by B. VIAUD for mumumumu modes
                                , 'DVCHI2DOF_mumumumu'
                                , 'DMAXDOCA_mumumumu'
                                , 'DFDCHI2_mumumumu'
                                , 'DIPCHI2_mumumumu'
                                , 'DDIRA'
                                , 'MaxDimuonMass'
                                , 'MaxKsMass'
                                , 'KsMotherMassCut'

                                , 'Kshort2MuMuMuMuLinePrescale'
                                , 'Kshort2MuMuMuMuLinePostscale'
                                )


    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        LongLine_name = name+"_Long"
        DownstreamLine_name = name+"_Down"

        # 5 : Make Muons for Kshort2MuMuMuMu
        selLongMuonsForMuMuMuMu = makeLongMuonsForMuMuMuMu(name="LongMuonsFor"+name
                                               , MuonP = config['MuonP']
                                               , MuonPT = config['PT_mumumumu']
                                               , MuonMINIPCHI2 = config['MINIPCHI2_mumumumu']
                                               , MuonTRCHI2 = config['MuonTRCHI2']
                                               , MuonPIDmu_mumumumu = config['MuonPIDmu_mumumumu'])

        selDownstreamMuonsForMuMuMuMu = makeDownstreamMuonsForMuMuMuMu(name="DownMuonsFor"+name
                                               , MuonP = config['MuonP']
                                               , MuonPT = config['PT_mumumumu']
                                               , MuonMINIPCHI2 = config['MINIPCHI2_mumumumu']
                                               , MuonTRCHI2 = config['MuonTRCHI2']
                                               , MuonPIDmu_mumumumu = config['MuonPIDmu_mumumumu'])

        # 7 : Combine

        selLongKshort2MuMuMuMu = self._makeKshort2MuMuMuMu(name=LongLine_name,
                                           muonSel = selLongMuonsForMuMuMuMu,
                                           config = config)

        selDownstreamKshort2MuMuMuMu = self._makeKshort2MuMuMuMu(name=DownstreamLine_name,
                                           muonSel = selDownstreamMuonsForMuMuMuMu,
                                           config = config)

        # 8 : Declare Lines

        self.LongLine = StrippingLine(LongLine_name+"Line",
                                    prescale = config['Kshort2MuMuMuMuLinePrescale'],
                                    postscale = config['Kshort2MuMuMuMuLinePostscale'],
                                    selection = selLongKshort2MuMuMuMu
                                    )

        self.DownstreamLine = StrippingLine(DownstreamLine_name+"Line",
                                    prescale = config['Kshort2MuMuMuMuLinePrescale'],
                                    postscale = config['Kshort2MuMuMuMuLinePostscale'],
                                    selection = selDownstreamKshort2MuMuMuMu
                                    )





        # 9 : register Line

        self.registerLine( self.LongLine )
        self.registerLine( self.DownstreamLine )


#####################################################
    def _makeKshort2MuMuMuMu(self, name,  muonSel, config):
        """
        Handy interface for Kshort2MuMuMuMu
        """
        return makeKshort2MuMuMuMu(name
                               , muonSel
                               , DMAXDOCA = config['DMAXDOCA_mumumumu']
                               , DPT = config['DPT_mumumumu']
                               , KsDauMAXIPCHI2= config['KsDauMAXIPCHI2_mumumumu']
                               , DFDCHI2= config['DFDCHI2_mumumumu']
                               , DVCHI2DOF = config['DVCHI2DOF_mumumumu']
                               , DDIRA = config['DDIRA']
                               , DIPCHI2 = config['DIPCHI2_mumumumu']
                               , MaxKsMass = config['MaxKsMass']
                               , KsMotherMassCut = config['KsMotherMassCut']
                               , MaxDimuonMass = config['MaxDimuonMass'])



#####################################################
def makeKshort2MuMuMuMu(name, muonSel, DMAXDOCA, DPT,  KsDauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, MaxKsMass, KsMotherMassCut, MaxDimuonMass):

    """
    Makes the KS0 -> mu+ mu+ mu- mu-
    """

    _combcut = "(AMAXDOCA('')<%(DMAXDOCA)s) & "\
               "(AM13 < %(MaxDimuonMass)s *MeV) &"\
               "(AM24 < %(MaxDimuonMass)s *MeV) &"\
               "(AM34 < %(MaxDimuonMass)s *MeV) &"\
               "(AM < %(MaxKsMass)s *MeV) & "\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(KsDauMAXIPCHI2)s)  )   )" %locals()



    _mothercut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
                   "(PT > %(DPT)s *MeV) &"\
                   "(M < %(KsMotherMassCut)s *MeV) &"\
                   "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
                   "(BPVDIRA > %(DDIRA)s)" % locals()


    _Combine = CombineParticles(DecayDescriptor = "KS0 -> mu+ mu+  mu- mu-",
                                CombinationCut = _combcut,
                                MotherCut = _mothercut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel] )

#####################################################
def makeLongMuonsForMuMuMuMu(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2, MuonPIDmu_mumumumu):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(PIDmu-PIDpi > %(MuonPIDmu_mumumumu)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseMuons ] )

def makeDownstreamMuonsForMuMuMuMu(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2, MuonPIDmu_mumumumu):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(PIDmu-PIDpi > %(MuonPIDmu_mumumumu)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseDownMuons ] )

#####################################################

