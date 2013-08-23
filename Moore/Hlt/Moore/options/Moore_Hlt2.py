#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
import Gaudi.Configuration
from Moore.Configuration import Moore

# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'Default'
#Moore().ThresholdSettings = 'Commissioning_PassThrough'
#Moore().configLabel = 'ODINRandom acc=0, TELL1Error acc=1'

Moore().ThresholdSettings = 'Physics_September2012'

Moore().Verbose = False
#Moore().EvtMax = 10000

Moore().UseDBSnapshot = False
Moore().ForceSingleL0Configuration = False

from Configurables import EventSelector
EventSelector().PrintFreq = 100

# /data/bfys/graven/0x46
from PRConfig.TestFileDB import test_file_db
input = test_file_db['2012_raw_default']
input.run(configurable=Moore()) 
Moore().inputFiles = [ '/data/bfys/graven/0x46/hlt1_reqhlt1.raw' ]

__replace = lambda orig, repl, members : [ m if m != orig else repl for m in members ]
__remove  = lambda remove,     members : [ m for m in members if m.name() not in remove ]
__remove_re  = lambda re,      members : [ m for m in members if not re.match(m.name()) ] 
## FIXME: how to catch HltUnit's RunAll???
def __walkAlgorithms( c , descend = [ 'Members','Prescale','ODIN','L0DU','HLT','Filter0','Filter1','Postscale'] ) :
    for p in descend :
        if not hasattr(c,p) : continue
        x = getattr(c,p)
        import GaudiKernel
        if isinstance(x,GaudiKernel.Configurable.Configurable) : x = [x]
        for i in x :
            for j in __walkAlgorithms(i,descend) : yield j
    yield c
def __updateProperties( top, table, value ) :
    for conf in __walkAlgorithms( top ) : 
        prop = table.get( conf.getType().replace(':','_') , None )
        if prop :
            setattr( conf, prop, value )
            print ' %s.%s -> %s '%(conf.name(),prop,getattr(conf,prop))

def SplitHlt2() :
    from Configurables import GaudiSequencer as gs
    seq = gs('Hlt')
    seq.Members = __replace( gs('HltDecisionSequence'), gs('Hlt2'), seq.Members )
    # TODO: shunt lumi nano events...
    # globally prepend a HltDecReportsDecoder for Hlt1...
    # TODO: find a better way of doing this... ditto for L0 decoding...
    from Configurables import HltDecReportsDecoder
    seq.Members.insert( seq.Members.index(gs('Hlt2')), HltDecReportsDecoder('Hlt1DecReportsDecoder') )
    # TODO: replace Hlt1 filter in endsequence by Hlt2 filter...
    # remove LumuWriter, LumiStripper
    end = gs('HltEndSequence')
    end.Members = __remove( ( 'HltL0GlobalMonitor','Hlt1Global','HltLumiWriter','LumiStripper'), end.Members )
    ## for debugging remove everything except Hlt2DiMuon.* lines...
    #import re
    #seq = gs('Hlt2')
    #seq.Members = __remove_re(re.compile('Hlt2(?!DiMuon).*') , seq.Members )

    ## adapt HltGlobalMonitor for Hlt2 only...
    from Configurables import HltGlobalMonitor
    HltGlobalMonitor().DecToGroupHlt1 = {}

    # shunt Hlt1 decreports
    __updateProperties( gs('Hlt')
                      , dict( HltDecReportsDecoder = 'OutputHltDecReportsLocation'
                            , LoKi__HDRFilter      = 'Location'
                            , HltRoutingBitsWriter = 'Hlt1DecReportsLocation'
                            )
                      , '/Event/Hlt1/DecReports'
                      )

    # shunt Hlt2 decreports 
    __updateProperties( gs('Hlt')
                      , dict( Hlt__Line            = 'HltDecReportsLocation'
                            , HltRoutingBitsWriter = 'Hlt2DecReportsLocation'
                            , HltDecReportsWriter  = 'InputHltDecReportsLocation' 
                            , HltSelReportsMaker   = 'InputHltDecReportsLocation'
                            , HltGlobalMonitor     = 'HltDecReports'
                            )
                      , '/Event/Hlt2/DecReports'
                      )

    # shunt selreports
    __updateProperties( gs('Hlt')
                      , dict( HltSelReportsMaker   = 'OutputHltSelReportsLocation'
                            , HltSelReportsWriter  = 'InputHltSelReportsLocation' )
                      , '/Event/Hlt2/SelReports'
                      )
            

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( SplitHlt2 )
