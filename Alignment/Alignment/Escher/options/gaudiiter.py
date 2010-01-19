#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-n","--numiter",type="int", dest="numiter",help="number of iterations", default=3)
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=100000)
parser.add_option("-d", "--aligndb",action="append", dest="aligndb",help="path to file with alignment database layer. you can use this option multiple times")
(opts, args) = parser.parse_args()

# Prepare the "configuration script" to parse (like this it is easier than
# having a list with files and python commands, with an if statements that
# decides to do importOptions or exec)
options = [ "importOptions(%r)" % f for f in args ]

# The option lines are inserted into the list of commands using their
# position on the command line 
#optlines = list(opts.options)
#optlines.reverse() # this allows to avoid to have to care about corrections of the positions
#for pos, l in optlines:
#   options.insert(pos,l)
    
# "execute" the configuration script generated (if any)
from Gaudi.Configuration import logging
if options:
   g = {}
   l = {}
   exec "from Gaudi.Configuration import *" in g, l
   for o in options:
      logging.debug(o)
      exec o in g, l
    
#options = [ "importOptions(%r)" % f for f in args ]

#importOptions("$ESCHERROOT/options/Escher.py")
#importOptions("$ESCHERROOT/options/DC06-Data.py")

#from Configurables import Escher
#escher = Escher()

# mae sure that the algorithms know how many iterations are coming
from Configurables import TAlignment
TAlignment().NumIterations = opts.numiter
TAlignment().UpdateInFinalize = False

# set the database layer
if opts.aligndb:
   counter = 1
   for db in opts.aligndb:
      from Configurables import ( CondDB, CondDBAccessSvc )
      alignCond = CondDBAccessSvc( 'AlignCond' + str(counter) )
      alignCond.ConnectionString = 'sqlite_file:' + db + '/LHCBCOND'
      CondDB().addLayer( alignCond )
      counter += 1

# This does not work yet, so I comment it out.
# Hack to overide default EvtSel open
#from GaudiPython.Bindings import iEventSelector
#iEventSelector.__open_orig__ = iEventSelector.open
#def _my_open_(self,stream, typ = 'POOL_ROOT', opt = 'READ', sel = None, fun = None, collection = None):
#   if typ == "MDF":
#       if type(stream) != list : stream = [stream]
#       fixpart = "TYP=\'%s\' OPT=\'%s\' SVC='LHCb::MDFSelector'" % ( typ, opt )
#       if sel        : fixpart += " SEL=\'%s\'" % sel
#       if fun        : fixpart += " FUN=\'%s\'" % fun
#       if collection : fixpart += " COLLECTION=\'%s\'" % collection
#       cstream = ["DATAFILE=\'%s\' %s" % ( s, fixpart) for s in stream]
#       self.Input = cstream
#       self.reinitialize()
#   else:
#      self.__open_orig__(stream,typ,opt,sel,fun,collection)
#iEventSelector.open = _my_open_

## Instantiate application manager
from GaudiPython.Bindings import AppMgr
appMgr = AppMgr()
evtSel = appMgr.evtSel()
evtSel.OutputLevel = 1
mainSeq = appMgr.algorithm( "EscherSequencer" )

print evtSel.Input

for i in range( opts.numiter ) :
    print "Iteration nr: ", i
    # rewind
    mainSeq.Enable = False
    evtSel.rewind()
    mainSeq.Enable = True

    # steer the monitor sequence depending on the iteration
    appMgr.algorithm('AlignMonitorSeq').Enable = ( i == 0 )
    appMgr.algorithm('Moni').Enable = ( i == 0 )
    if opts.numiter>1 :
       appMgr.algorithm('AlignPostMonitorSeq').Enable = ( i == opts.numiter-1 )
    
    # event loop
    appMgr.run( opts.numevents )

    # fire incident for update
    from GaudiPython import gbl
    incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
    updateConstants = gbl.Incident( 'Alignment', 'GlobalMPedeFit' )
    incSvc.fireIncident( updateConstants )

#exit the appmgr for finalize
appMgr.exit()
