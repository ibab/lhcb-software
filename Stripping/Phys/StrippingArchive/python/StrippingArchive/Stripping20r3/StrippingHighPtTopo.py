#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
'''
b jets tagged with topo candidates

Author: C. Potterat
'''
__author__ = ['Cedric Potterat']


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#


from Gaudi.Configuration import *
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter
from PhysSelPython.Wrappers import EventSelection

confdict_HighPtTopoJets = { 'HighPtTopo_Prescale'    : 1.0,
                          'HighPtTopoLow_Prescale' : 1.0, ##changed from 0.05
                          'HighPtTopo_Postscale'   : 1.0,
                          'pT'        : 40000,
                          'pTlow'     : 20000

                          }

default_name = 'HighPtTopo'

class HighPtTopoJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'HighPtTopo_Prescale',
                               'HighPtTopoLow_Prescale',
                               'HighPtTopo_Postscale',
                               'pT',
                               'pTlow'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name



        # High pT Topo line
        from Configurables import HighPtTopoTool as HighpTDT
        code = "ACCEPT('HighPtTopoTool/HighPtTopoLine_DT_40000')"
        alg = LoKi__VoidFilter('HighPtTopoLineFilter_40000',Code=code)
        sel = EventSelection('HighPtTopoEventSel_40000',Algorithm=alg)
        dt = HighpTDT('HighPtTopoLine_DT_40000')
        dt.minpT = config['pT']
        hlt = "HLT_PASS_RE('Hlt2Topo.*Decision')"
        sline = StrippingLine(name+'%(pT)s'%config,
                              prescale  = config[ 'HighPtTopo_Prescale' ],
                              postscale = config[ 'HighPtTopo_Postscale' ],
                              selection=sel,
                              HLT=hlt)
        self.registerLine(sline)

        from Configurables import HighPtTopoTool as HighpTDT
        code = "ACCEPT('HighPtTopoTool/HighPtTopoLine_DT_20000')"
        alg = LoKi__VoidFilter('HighPtTopoLineFilter_20000',Code=code)
        sel = EventSelection('HighPtTopoEventSel_20000',Algorithm=alg)
        dt = HighpTDT('HighPtTopoLine_DT_20000')
        dt.minpT = config['pTlow']
        hlt = "HLT_PASS_RE('Hlt2Topo.*Decision')"
        sline = StrippingLine(name+'%(pTlow)s'%config,
                              prescale  = config[ 'HighPtTopoLow_Prescale' ],
                              postscale = config[ 'HighPtTopo_Postscale' ],
                              selection=sel,
                              HLT=hlt)
        self.registerLine(sline)



#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
