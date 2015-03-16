'''
Module for selecting J/psi, Psi(2S), and Upsilon 
'''

__author__=['Jibo He']
__date__ = '28/03/2012'
__version__= '$Revision: 1.0 $'


__all__ = (
    'DiMuonForXsectionConf',
    'default_config'
    )

default_config =  {
    'NAME'       : 'DiMuonForXsection',
    'WGs'        : ['BandQ'],
    'STREAMS' : ['Dimuon'],
    'BUILDERTYPE': 'DiMuonForXsectionConf',
    'CONFIG'     : {
    'TotalCuts'  :  """
    (MINTREE('mu+'==ABSID,PT)>0.65*GeV)
    & (VFASPF(VCHI2PDOF)<25)
    & ((ADMASS('J/psi(1S)')<150*MeV) | (ADMASS('psi(2S)')<150*MeV) | (MM>8.5*GeV))
    """,
    'Prescale'   :  1.,
    'CheckPV'    :  False
    }
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class DiMuonForXsectionConf(LineBuilder):
    
    __configuration_keys__ = (
        'TotalCuts',
        'Prescale',
        'CheckPV'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        """
        DiMuon
        """
        self.SelDiMuon = self.createSubSel( OutputList = self.name + "SelDiMuon",
                                            InputList =  DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' ), 
                                            Cuts = config['TotalCuts']
                                            )

        self.DiMuonForXsectionLine = StrippingLine( self.name + 'Line',
                                                    prescale  = config['Prescale'],                                                    
                                                    checkPV   = config['CheckPV'],
                                                    algos     = [ self.SelDiMuon ]
                                                    )
        self.registerLine( self.DiMuonForXsectionLine )
        

    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                          Algorithm = filter,
                          RequiredSelections = [ InputList ] )
