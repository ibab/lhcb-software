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


Moore().WriterRequires = [ 'Hlt1' ]
Moore().outputFile = 'hlt1_reqhlt1.raw'

__replace = lambda orig, repl, members : [ m if m != orig else repl for m in members ]

def SplitHlt1() :
    from Configurables import GaudiSequencer as gs
    seq = gs('Hlt')
    seq.Members = __replace( gs('HltDecisionSequence'), gs('Hlt1'), seq.Members )

#use old turning off of HLT2

#from Gaudi.Configuration import appendPostConfigAction
#appendPostConfigAction( SplitHlt1 )

#use new splitting of Hlt2
Moore().Split='Hlt1'
