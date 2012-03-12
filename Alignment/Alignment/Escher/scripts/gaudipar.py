#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=-1)
parser.add_option("-p","--numprocesses",type="int", dest="numprocs",help="number of processes", default=8)
parser.add_option("-d", "--aligndb", action = 'append', dest="aligndb",help="path to file with LHCBCOND database layer")
parser.add_option("--dddb", action = 'append', dest="dddb",help="path to file with DDDB database layer")
parser.add_option("-i", "--iter",type="int", dest="iter",help="number of iteration (used for loggin)", default=0)
parser.add_option("-r", "--roothistofile",dest="histofile",help="name of histogram file",default = "histograms.root")
parser.add_option("-c", "--derivativefile",dest="derivativefile",help="name of derivative file",default = "")
parser.add_option("--stagefiles",action = 'store', help="stage files locally")
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

# add the special prescaler at the beginning of the Escher sequence
from Configurables import GaudiSequencer, CountingPrescaler
prescalername = "EscherPrescaler"
GaudiSequencer("EscherSequencer").Members.insert(0,CountingPrescaler(prescalername) )

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
#from Configurables import EventSelector
#EventSelector().PrintFreq = -1

#######################################################################
# when using gaudipar we cannot use the filestager because it will
# not realize that it is reading the same file multiple
# times. instead, we'll copy the files to the local disk in advance.
####################################################################### 
from Configurables import Escher
if opts.stagefiles or (hasattr(Escher(),'UseFileStager') and Escher().UseFileStager):
   if hasattr(Escher(),'UseFileStager'): Escher().UseFileStager = False
   from Escher.Utils import stagelocally
   stagelocally()

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
     #print 'INFO: initializeLocal'

  def initializeRemote(self):
     print 'INFO: initializeRemote'
       
  def process(self, eventoffset):
     from GaudiPython.Bindings import AppMgr
     appMgr = AppMgr()
     if eventoffset==0:
        from Escher.Utils import printsequence
        printsequence(appMgr)
     # ugly: change the algorithm after initialization. otherwise it doesn't work for 'reused' applications.
     #numeventsPerProc  = opts.numevents / opts.numprocs + 1
     #numeventsThisProc = numeventsPerProc
     #if eventoffset is opts.numprocs - 1:
     #   numeventsThisProc -= opts.numprocs * numeventsPerProc - opts.numevents
     #appMgr.evtSel().FirstEvent = eventoffset * numeventsPerProc
     #appMgr.evtSel().reinitialize()
     #appMgr.run(numeventsThisProc)
     appMgr.algorithm(prescalername).Offset = eventoffset
     appMgr.algorithm(prescalername).Interval = opts.numprocs
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
     print "INFO: finalize"
     # dump the histograms to a file
     self.output['histograms'].dump(opts.histofile)
     
     numAlignEvents = self.output['derivatives'].equations().numEvents()
     print 'INFO: number of events in derivatives: ', self.output['derivatives'].equations().numEvents()
     if numAlignEvents>0 :
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
