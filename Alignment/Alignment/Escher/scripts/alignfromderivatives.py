#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <derivative_files> <opts_file.py>")
parser.add_option("-d", "--aligndb", action = 'append', dest="aligndb",help="path to file with alignment database layer")
(opts, args) = parser.parse_args()

# Prepare the "configuration script" to parse (like this it is easier than
# having a list with files and python commands, with an if statements that
# decides to do importOptions or exec)

import os
optionfiles = []
derivativefiles = []
for f in args:
   if not os.path.exists( f ):
      print "cannot find input file '", f, "'"
      exit()
   else:
      if f.find(".py")>0:
         optionfiles.append(f)
      else:
         derivativefiles.append( f )

print 'derivativefiles: ', derivativefiles
print 'optionfiles: ', optionfiles
options = [ "importOptions(%r)" % f for f in optionfiles ]

# "execute" the configuration script generated (if any)
from Gaudi.Configuration import logging
if options:
   g = {}
   l = {}
   exec "from Gaudi.Configuration import *" in g, l
   for o in options:
      logging.debug(o)
      exec o in g, l

# let the algorithm update in finalize and set the input file
from Configurables import TAlignment, AlignAlgorithm
TAlignment().UpdateInFinalize = True
AlignAlgorithm('Alignment').InputDataFiles = derivativefiles
AlignAlgorithm('Alignment').OutputDataFile = ''

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
   
from GaudiPython.Bindings import AppMgr
appMgr = AppMgr()
# we want to reinitialize, but we don't want to initialize all
# the reco algorithms, so we add just the alignment sequence:
appMgr.setAlgorithms( [ 'AlignAlgorithm/Alignment' ] )
appMgr.configure()
appMgr.initialize()
# update the geometry
# updatetool = appMgr.toolsvc().create( typ = "Al::AlignUpdateTool", name = "ToolSvc.AlignUpdateTool", interface = "IAlignUpdateTool" )
#updatetool = appMgr.toolsvc().create( "Al::AlignUpdateTool", interface = "Al::IAlignUpdateTool" )
#updatetool.process(  self.output['derivatives'].equations(), opts.iter, 1)
# now call finalize to write the conditions. there must be a better way.
appMgr.finalize()
# finally create a database layer
import os
os.system("copy_files_to_db.py -c sqlite_file:Alignment.db/LHCBCOND -s xml")

