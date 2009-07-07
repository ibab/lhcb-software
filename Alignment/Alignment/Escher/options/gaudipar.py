#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=1000)
parser.add_option("-p","--numprocesses",type="int", dest="numprocs",help="number of processes", default=8)
parser.add_option("-d", "--aligndb",type="str", dest="aligndb",help="path to file with alignment database layer", default = '')
parser.add_option("-i", "--iter",type="int", dest="iter",help="number of iteration (used for loggin)", default=0)
(opts, args) = parser.parse_args()

# Prepare the "configuration script" to parse (like this it is easier than
# having a list with files and python commands, with an if statements that
# decides to do importOptions or exec)
options = [ "importOptions(%r)" % f for f in args ]

# "execute" the configuration script generated (if any)
from Gaudi.Configuration import logging
if options:
   g = {}
   l = {}
   exec "from Gaudi.Configuration import *" in g, l
   for o in options:
      logging.debug(o)
      exec o in g, l

# Modify the configuration

# make sure that the algorithm jobs do not update in finalize
from Configurables import TAlignment
TAlignment().UpdateInFinalize = False

# set the database layer
if opts.aligndb:
   from Configurables import ( CondDB, CondDBAccessSvc )
   alignCond = CondDBAccessSvc( 'AlignCond' )
   alignCond.ConnectionString = 'sqlite_file:' + opts.aligndb + '/LHCBCOND'
   CondDB().addLayer( alignCond )
   
# turn off the printfreq
from Configurables import EventSelector
EventSelector().PrintFreq = -1

#from Gaudi.Configuration import *

############################################################################
from GaudiPython.Parallel import Task, WorkManager
from GaudiPython.Bindings import AppMgr
import copy

class AlignmentTask(Task):
  def initializeLocal(self):
     from GaudiPython.Bindings import gbl
     self.output = { 'derivatives' : gbl.LHCb.AlignSummaryData() }
     
  def initializeRemote(self):
     print 'initializeRemote'
     #appConf = AppMgr()
     
  def process(self, eventoffset):
     from GaudiPython.Bindings import AppMgr
     appMgr = AppMgr()
     # ugly: change the algorithm after initialization. otherwise it doesn't work for 'reused' applications.
     appMgr.algorithm('EscherPrescaler').Offset = eventoffset
     appMgr.algorithm('EscherPrescaler').Interval = opts.numprocs
     appMgr.run(opts.numevents)
     #evt = appMgr.evtsvc()
     #while 0 < 1:>
     # check if there are still valid events
     # if evt['Rec/Header'] == None : break
     det = appMgr.detsvc()
     alignderivatives = det['AlignDerivativeData']
     #self.output['derivatives'] = copy.deepcopy(alignderivatives)
     self.output['derivatives'].add( alignderivatives )
                
  def finalize(self):
     print 'number of events in derivatives: ', self.output['derivatives'].equations().numEvents()
     from GaudiPython.Bindings import AppMgr
     appMgr = AppMgr()
     # we want to reinitialize, but we don't want to initialize all
     # the reco algorithms, so we add just the alignment sequence:
     appMgr.setAlgorithms( [ 'GaudiSequencer/AlignSequence' ] )
     appMgr.configure()
     appMgr.initialize()
     # update the geometry
     # updatetool = appMgr.toolsvc().create( typ = "Al::AlignUpdateTool", name = "ToolSvc.AlignUpdateTool", interface = "IAlignUpdateTool" )
     updatetool = appMgr.toolsvc().create( "Al::AlignUpdateTool", interface = "Al::IAlignUpdateTool" )
     updatetool.process(  self.output['derivatives'].equations(), opts.iter, 1)
     # now call finalize to write the conditions. there must be a better way.
     appMgr.finalize()
     # finally create a database layer
     import os
     os.system("copy_files_to_db.py -c sqlite_file:Alignment.db/LHCBCOND -s xml")

############################################################################

if __name__ == '__main__' :
  task = AlignmentTask()
  #ppservers = ('lxbrg4601.cern.ch')
  #wmgr = WorkManager(ppservers = ppservers)
  wmgr = WorkManager()
  wmgr.process( task, range(opts.numprocs) )
