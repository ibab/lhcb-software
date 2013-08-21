#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
import Gaudi.Configuration
from Configurables import Moore
from GaudiConf import IOHelper
IOHelper().inputFiles([],clear=True)

Moore().inputFiles = [ 'hlt1_reqhlt1.raw' ]

__replace = lambda orig, repl, members : [ m if m != orig else repl for m in members ]
__remove  = lambda remove,     members : [ m for m in members if m.name() not in remove ]
__remove_re  = lambda re,      members : [ m for m in members if not re.match(m.name()) ] 
def walkAlgorithms( c ) :
    for p in [ 'Members','Filter0','Filter1' ] :
        if not hasattr(c,p) : continue
        x = getattr(c,p)
        import GaudiKernel
        if isinstance(x,GaudiKernel.Configurable.Configurable) : x = [x]
        for i in x :
            for j in walkAlgorithms(i) : yield j
    yield c

from Configurables import HltDecReportsDecoder
decoder = HltDecReportsDecoder()
def SplitHlt2() :
    from Configurables import GaudiSequencer as gs
    seq = gs('Hlt')
    seq.Members = __replace( gs('HltDecisionSequence'), gs('Hlt2'), seq.Members )
    # TODO: shunt lumi nano events...
    # globally prepend a HltDecReportsDecoder for Hlt1...
    # TODO: make sure Hlt2 lines write to a DIFFERENT HltDecReports object...
    seq.Members.insert( seq.Members.index(gs('Hlt2')), decoder )
    # replace Hlt1 filter in endsequence by Hlt2 filter...
    # remove LumuWriter, LumiStripper
    end = gs('HltEndSequence')
    end.Members = __remove( ('Hlt1Global','HltLumiWriter','LumiStripper','HltL0GlobalMonitor'), end.Members )
    ## for debugging remove everything except dimuons lines...
    import re
    #seq = gs('Hlt2')
    #seq.Members = __remove_re(re.compile('Hlt2(?!DiMuon).*') , seq.Members )
    
    #for c in walkAlgorithms( gs('Hlt2') ) : c.OutputLevel = 2

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( SplitHlt2 )
