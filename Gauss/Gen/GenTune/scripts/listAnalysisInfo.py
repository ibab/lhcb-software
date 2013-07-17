#!/usr/bin/env python
###############################################################################
# Author: Alexandru T. Grecu <alexandru.grecu@nipne.ro>
# Description: Displays the .info file(s) associated to a specific set 
#              of analyses given a fragment of the analysis name
###############################################################################

import sys
import os
import rivet

nmfrag = 'lhcb'

def showHelp():
    print('Rivet analysis info dumper script')
    print('using Rivet v. %s' % rivet.version())
    print('Command line:')
    print("\t [python] %s [<analysis_name_frag>|lhcb]" % (sys.argv[0]))

def showAnalysis(anaObj):
    infoFn = rivet.findAnalysisInfoFile(anaObj.name()+'.info')
    fp = open(infoFn, 'r')
    cnt = fp.read()
    fp.close()
    print(cnt)

if __name__ != '__main__' or len(sys.argv) < 2:
    showHelp()
    exit(3)

anaNames = rivet.AnalysisLoader_analysisNames()
if len(sys.argv) > 1:
    nmfrag = sys.argv[1]
nmfrag = nmfrag.upper()
nmatch = 0
for an in anaNames:
    if an.find(nmfrag) > -1:
        showAnalysis(rivet.AnalysisLoader_getAnalysis(an))
        print("="*80)
        nmatch += 1
print("%d analyses matched name search criteria." % (nmatch))

