#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=1000)
parser.add_option("-p","--numprocesses",type="int", dest="numprocs",help="number of processes", default=8)
parser.add_option("-d", "--aligndb", action = 'append', dest="aligndb",help="path to file with LHCBCOND database layer")
parser.add_option("--dddb", action = 'append', dest="dddb",help="path to file with DDDB database layer")
parser.add_option("-i", "--iter",type="int", dest="iter",help="number of iteration (used for loggin)", default=0)
parser.add_option("-r", "--roothistofile",dest="histofile",help="name of histogram file",default = "histograms.root")
parser.add_option("-c", "--derivativefile",dest="derivativefile",help="name of derivative file",default = "")
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
   counter = 1
   for db in opts.aligndb:
      from Configurables import ( CondDB, CondDBAccessSvc )
      alignCond = CondDBAccessSvc( 'AlignCond' + str(counter) )
      alignCond.ConnectionString = 'sqlite_file:' + db + '/LHCBCOND'
      CondDB().addLayer( alignCond )
      counter += 1
   print 'added databases: ', opts.aligndb
   
if opts.dddb:
   counter  = 1
   for db in opts.dddb:
      from Configurables import ( CondDB, CondDBAccessSvc )
      alignCond = CondDBAccessSvc( 'AlignDDDB' + str(counter) )
      alignCond.ConnectionString = 'sqlite_file:' + db + '/DDDB'
      CondDB().addLayer( alignCond )
      counter += 1
   print 'added databases: ', opts.dddb
   
# turn off the printfreq
from Configurables import EventSelector
EventSelector().PrintFreq = -1

#from Gaudi.Configuration import *

############################################################################
from GaudiMP.GMPBase import aida2root, aidatypes

class HistStore:
   histDict = {}
   def collect(self, histsvc ) :
      self.histDict = {}
      nlist = histsvc.getHistoNames()
      for n in nlist :
         o = histsvc[ n ]
         if type(o) in aidatypes :
            #print 'adding histogram to store: \'', n, '\''
            o = aida2root(o)
            self.histDict[ n ] = o.Clone()

   def Add(self, rhs ):
      for k, o in rhs.histDict.iteritems():
         if k in self.histDict:
            self.histDict[k].Add( o )
         else:
            self.histDict[k] = o.Clone()
         
   def dump(self, filename ):
      from ROOT import TFile
      import os
      outputfile = TFile(filename,"RECREATE")
      for k, o in self.histDict.iteritems():
         # skip Calo
         if not str(k).startswith('/stat/CaloPIDs') :
            fullname = str(k).replace('/stat/','/')
            dirname  = os.path.dirname(fullname)
            histname = os.path.basename(fullname)
            #print "dirname=\'", dirname, "\'"
            #print "histname=\'", histname, "\'"
            outputfile.cd()
            curdir = outputfile
            directories = dirname.split('/')
            for i in directories:
               subdir = curdir.GetDirectory(i)
               if not subdir:
                  curdir.mkdir( i )
                  subdir = curdir.GetDirectory(i)
               subdir.cd()
               curdir = subdir
            o.Write( histname )
      outputfile.Write()
      outputfile.Close()

############################################################################
from GaudiMP.Parallel import Task, WorkManager
from GaudiPython.Bindings import AppMgr
import copy

class AlignmentTask(Task):
  def initializeLocal(self):
     from GaudiPython.Bindings import gbl
     self.output = { 'derivatives' : gbl.LHCb.AlignSummaryData(),
                     'histograms' : HistStore() }
     
  def initializeRemote(self):
     print 'initializeRemote'
     #appConf = AppMgr()
     
  def process(self, eventoffset):
     from GaudiPython.Bindings import AppMgr
     appMgr = AppMgr()
     # ugly: change the algorithm after initialization. otherwise it doesn't work for 'reused' applications.
     #numeventsPerProc  = opts.numevents / opts.numprocs + 1
     #numeventsThisProc = numeventsPerProc
     #if eventoffset is opts.numprocs - 1:
     #   numeventsThisProc -= opts.numprocs * numeventsPerProc - opts.numevents
     #appMgr.evtSel().FirstEvent = eventoffset * numeventsPerProc
     #appMgr.evtSel().reinitialize()
     #appMgr.run(numeventsThisProc)
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
     if alignderivatives : self.output['derivatives'].add( alignderivatives )
     histsvc = appMgr.histsvc()
     self.output['histograms'].collect( histsvc )
                
  def finalize(self):
     # dump the histograms to a file
     self.output['histograms'].dump(opts.histofile)
     
     numAlignEvents = self.output['derivatives'].equations().numEvents()
     if numAlignEvents>0 :
        print 'number of events in derivatives: ', self.output['derivatives'].equations().numEvents()
        # write the derivative file
        if len(opts.derivativefile)>0 :
           self.output['derivatives'].equations().writeToFile(derivativefile)

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
