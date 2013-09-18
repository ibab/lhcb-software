'''
Jpsi line for D0mumu
'''

__author__=['Walter Bonivento', 'Francesco Dettori']
__date__ = '22/10/2010'
__version__= '$Revision: 1.0 $'

__all__ = (
    'JpsiMuMuforD0MuMuConf'
    )


config_default= {
        # Jpsi2MuMu line for D0mumu

        'Jpsi2MuMuForD0mumu_Prescale'              :     1.   ,
        'Jpsi2MuMuForD0mumu_Postscale'             :     1.   ,
        'Jpsi2MuMuForD0mumu_MuonPT'                :  750.    ,
        'Jpsi2MuMuForD0mumu_MuonP'                 : 5000.    ,
        'Jpsi2MuMuForD0mumu_MuonTRCHI2DOF'         :   5.     ,
        'Jpsi2MuMuForD0mumu_MuonBPVIPCHI2'         :   3.     ,
        'Jpsi2MuMuForD0mumu_MinMass'               : 3000     , 
        'Jpsi2MuMuForD0mumu_MaxMass'               : 3180     ,
        'Jpsi2MuMuForD0mumu_VCHI2PDOF'             :  10.     ,
        'Jpsi2MuMuForD0mumu_PT'                    : 1800.    , 
        'Jpsi2MuMuForD0mumu_BPVVDCHI2'             :   20.    ,
        'Jpsi2MuMuForD0mumu_BPVDIRA'               : 0.9997   ,
        'Jpsi2MuMuForD0mumu_BPVIPCHI2'             : 15.      ,
        'Jpsi2MuMuForD0mumu_MuonMaxBPVIPCHI2'          : 8.       ,
        'Jpsi2MuMuForD0mumu_MuonMaxPT'                 : 1100.    ,

        }



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder



class JpsiMuMuforD0MuMuConf(LineBuilder):
    
    __configuration_keys__ = (

        # Jpsi line for D0mumu
        
        'Jpsi2MuMuForD0mumu_Prescale',
        'Jpsi2MuMuForD0mumu_Postscale',
        'Jpsi2MuMuForD0mumu_MuonPT',
        'Jpsi2MuMuForD0mumu_MuonP',
        'Jpsi2MuMuForD0mumu_MuonTRCHI2DOF',
        'Jpsi2MuMuForD0mumu_MuonBPVIPCHI2',
        'Jpsi2MuMuForD0mumu_MuonMaxBPVIPCHI2',
        'Jpsi2MuMuForD0mumu_MuonMaxPT',
        'Jpsi2MuMuForD0mumu_MinMass',
        'Jpsi2MuMuForD0mumu_MaxMass',
        'Jpsi2MuMuForD0mumu_VCHI2PDOF',
        'Jpsi2MuMuForD0mumu_PT',
        'Jpsi2MuMuForD0mumu_BPVVDCHI2',
        'Jpsi2MuMuForD0mumu_BPVDIRA',
        'Jpsi2MuMuForD0mumu_BPVIPCHI2'
        
        
      
        )
    

    
    def __init__(self,
                 name = None,
                 config = None
                 ):
        
        LineBuilder.__init__(self, name, config)

        """
        Jpsi-> mumu for D0mumu line
        """
        self.SelJpsi2MuMuForD0mumu = filterJpsi2MuMuForD0mumu( name ,
                                             MuonPT        = config['Jpsi2MuMuForD0mumu_MuonPT'],
                                             MuonP         = config['Jpsi2MuMuForD0mumu_MuonP'],
                                             MuonTRCHI2DOF = config['Jpsi2MuMuForD0mumu_MuonTRCHI2DOF'],
                                             MuonBPVIPCHI2 = config['Jpsi2MuMuForD0mumu_MuonBPVIPCHI2'],
                                             MuonMaxBPVIPCHI2 = config['Jpsi2MuMuForD0mumu_MuonMaxBPVIPCHI2'],
                                             MuonMaxPT = config['Jpsi2MuMuForD0mumu_MuonMaxPT'],
                                             MuMuMinMass   = config['Jpsi2MuMuForD0mumu_MinMass'],
                                             MuMuMaxMass   = config['Jpsi2MuMuForD0mumu_MaxMass'],
                                             MuMuVCHI2PDOF = config['Jpsi2MuMuForD0mumu_VCHI2PDOF'],
                                             MuMuPT        = config['Jpsi2MuMuForD0mumu_PT'],
                                             MuMuBPVVDCHI2 = config['Jpsi2MuMuForD0mumu_BPVVDCHI2'],
                                             MuMuBPVDIRA   = config['Jpsi2MuMuForD0mumu_BPVDIRA'],
                                             MuMuBPVIPCHI2 = config['Jpsi2MuMuForD0mumu_BPVIPCHI2']
                                             )
        self.Jpsi2MuMuForD0mumuLine = StrippingLine( name + 'Line',
                                                     prescale  = config['Jpsi2MuMuForD0mumu_Prescale'],
                                                     postscale = config['Jpsi2MuMuForD0mumu_Postscale'],
                                                     selection = self.SelJpsi2MuMuForD0mumu
                                                     )
        
        self.registerLine( self.Jpsi2MuMuForD0mumuLine )
        
#        self.MicroDSTLines = [
#            self.Jpsi2MuMuForD0mumuLine
#            ]



def filterJpsi2MuMuForD0mumu(name,
                     MuonPT,
                     MuonP,
                     MuonTRCHI2DOF,
                     MuonBPVIPCHI2,
                     MuonMaxBPVIPCHI2,
                     MuonMaxPT,        
                     MuMuMinMass,
                     MuMuMaxMass, 
                     MuMuVCHI2PDOF,
                     MuMuPT,
                     MuMuBPVVDCHI2,
                     MuMuBPVDIRA,
                     MuMuBPVIPCHI2
                     ):
    
    _StdLooseJpsi2MuMu = DataOnDemand( Location = 'Phys/StdLooseJpsi2MuMu/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s) & (MINTREE('mu+'==ABSID,MIPCHI2DV(PRIMARY)) > %(MuonBPVIPCHI2)s *MeV)  & (MAXTREE('mu+'==ABSID,MIPCHI2DV(PRIMARY)) > %(MuonMaxBPVIPCHI2)s *MeV) & (MAXTREE('mu+'==ABSID,PT)>  %(MuonMaxPT)s *MeV) "% locals()
    
    MuonCut += " & (MM > %(MuMuMinMass)s) & (MM < %(MuMuMaxMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s) & (BPVDIRA> %(MuMuBPVDIRA)s)  & (BPVVDCHI2> %(MuMuBPVVDCHI2)s) & (MIPCHI2DV(PRIMARY)< %(MuMuBPVIPCHI2)s)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut  )
    
    return Selection( name ,
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseJpsi2MuMu ]
                      )




