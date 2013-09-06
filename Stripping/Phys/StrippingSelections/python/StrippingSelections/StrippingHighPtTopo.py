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

confdict_HighPtTopoJets = { 'HighPtTopo40000_Prescale'    : 1.0, 
                          'HighPtTopo20000_Prescale'    : 1.0, 
                          'HighPtTopo10000_Prescale'    : 0.0014,
                          'HighPtTopo1000_Prescale'    : 0.00017,
                          'HighPtTopo20000_Cone55_Prescale'  : 0.1, 
                          'HighPtTopo_Postscale'   : 1.0,
                          'pT'        : 40000,
                          'pT20'      : 20000,
                          'pT10'      : 10000,
                          'pT1'       : 1000,
                          'conesize'  : 0.3,
                          'pTcone'    : 55000

                          }

default_name = 'HighPtTopo'

class HighPtTopoJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'HighPtTopo40000_Prescale',
                               'HighPtTopo20000_Prescale',
                               'HighPtTopo10000_Prescale',
                               'HighPtTopo1000_Prescale',
                               'HighPtTopo20000_Cone55_Prescale'
                               'HighPtTopo_Postscale',
                               'pT',
                               'pT20',
                               'pT10',
                               'pT1',
                               'conesize',
                               'pTcone'
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
                              prescale  = config[ 'HighPtTopo40000_Prescale' ],
                              postscale = config[ 'HighPtTopo_Postscale' ],
                              selection=sel,
                              HLT=hlt)
        self.registerLine(sline)


        # High pT Topo line
        from Configurables import HighPtTopoTool as HighpTDT
        code = "ACCEPT('HighPtTopoTool/HighPtTopoLine_DT_20000')"
        alg = LoKi__VoidFilter('HighPtTopoLineFilter_20000',Code=code)
        sel = EventSelection('HighPtTopoEventSel_20000',Algorithm=alg)
        dt = HighpTDT('HighPtTopoLine_DT_20000')
        dt.minpT = config['pT20']
        hlt = "HLT_PASS_RE('Hlt2Topo.*Decision')"
        sline = StrippingLine(name+'%(pT20)s'%config,
                              prescale  = config[ 'HighPtTopo20000_Prescale' ],
                              postscale = config[ 'HighPtTopo_Postscale' ],
                              selection=sel,
                              HLT=hlt)
        self.registerLine(sline)


        # High pT Topo line
        from Configurables import HighPtTopoTool as HighpTDT
        code = "ACCEPT('HighPtTopoTool/HighPtTopoLine_DT_10000')"
        alg = LoKi__VoidFilter('HighPtTopoLineFilter_10000',Code=code)
        sel = EventSelection('HighPtTopoEventSel_10000',Algorithm=alg)
        dt = HighpTDT('HighPtTopoLine_DT_10000')
        dt.minpT = config['pT10']
        hlt = "HLT_PASS_RE('Hlt2Topo.*Decision')"
        sline = StrippingLine(name+'%(pT10)s'%config,
                              prescale  = config[ 'HighPtTopo10000_Prescale' ],
                              postscale = config[ 'HighPtTopo_Postscale' ],
                              selection=sel,
                              HLT=hlt)
        self.registerLine(sline)


        # High pT Topo line
        from Configurables import HighPtTopoTool as HighpTDT
        code = "ACCEPT('HighPtTopoTool/HighPtTopoLine_DT_1000')"
        alg = LoKi__VoidFilter('HighPtTopoLineFilter_1000',Code=code)
        sel = EventSelection('HighPtTopoEventSel_1000',Algorithm=alg)
        dt = HighpTDT('HighPtTopoLine_DT_1000')
        dt.minpT = config['pT1']
        hlt = "HLT_PASS_RE('Hlt2Topo.*Decision')"
        sline = StrippingLine(name+'%(pT1)s'%config,
                              prescale  = config[ 'HighPtTopo1000_Prescale' ],
                              postscale = config[ 'HighPtTopo_Postscale' ],
                              selection=sel,
                              HLT=hlt)
        self.registerLine(sline)

        from Configurables import HighPtTopoTool as HighpTDT
        code = "ACCEPT('HighPtTopoTool/HighPtTopoLine_DT_20000_Cone55')"
        alg = LoKi__VoidFilter('HighPtTopoLineFilter_20000_Cone55',Code=code)
        sel = EventSelection('HighPtTopoEventSel_20000_Cone55',Algorithm=alg)
        dt = HighpTDT('HighPtTopoLine_DT_20000_Cone55')
        dt.minconepT = config['pTcone']
        dt.conesize = config['conesize']
        dt.minpT = config['pT20']
        hlt = "HLT_PASS_RE('Hlt2Topo.*Decision')"
        sline = StrippingLine(name+'%(pT20)s_Cone55'%config,
                              prescale  = config[ 'HighPtTopo20000_Cone55_Prescale' ],
                              postscale = config[ 'HighPtTopo_Postscale' ],
                              selection=sel,
                              HLT=hlt)
        self.registerLine(sline)



#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
