#!/usr/bin/env python
# =============================================================================
## @file PartProp/lhcbLFT2Cmap.py
#  Dumps lifetime information from the LHCb Particle Property Service
#  to a file containing a huge std::map<std::int, double>. The keys in the
#  map are the particle PDG ids and the values are ctau values expressed in [m].
#  Module can be used both as a script or imported to generate the HepPDT file.
#  @author Alex Grecu <alexandru.grecu@gmail.com>
#  @date 2013-06-04
# =============================================================================
"""
Create std::map with LHCb particles ctau values to hardcode into RIVET.
We don't like HepPDT!!!

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = "Alex GRECU alexandru.grecu@gmail.com"
__version__ = "version $Revision: 1.0 $"
# =============================================================================
try:
    import PartProp.PartPropAlg
    import PartProp.Service
    from   GaudiPython.Bindings import AppMgr
    from   GaudiKernel import SystemOfUnits
    from   Configurables import DDDBConf, CondDB, LHCbApp
except Exception, exx:
    print(exx.message)
    print('')
    print('Please, setup the GAUSS environment first.')
import os
import sys
import datetime
# ============================================================================

codePreamble = """// LHCb particle lifetime table for
// DDDB tag: %s
// Data type: %s
// Generated: %s
// using lhcbLFT2StdMap.py script from Gen/GenTune package

// You will need to further customize the C++ code according to your needs...
// Don't forget to call fillMap in your object constructor

// Mapping between PDG id and particle lifetimes in seconds
std::map<int, double> _partLftMap;

// Function that fills the map at runtime
bool fillMap(map<int, double> &m) {
"""

codePreamble2 = """// Definition as static class variable
static const int _stablePDGIds[%u];

// Set of PDG Ids for stable particles
const int %s::stablePDGIds[%u] = {
"""

timestampFormat = "%a, %d %b %Y @ %H:%M:%S UTC"
analysisName = "<RivetAnalysis_Plugin_Class>"
codeMakePair = "m[%u] = %s;"

outputFileName = "codefrag.cpp"

dddbTag = "dddb-20130503-1"
dataType = "2012"

# ignore abs(pdg_id) > 9999999 -> eliminate nuclei and other odd particles from table
def CreateLifetimeMapCode():
    global codePreamble, codeMakePair, codePostamble, outputFileName, analysisName, timestampFormat, dataType, dddbTag
    LHCbApp().DDDBtag = dddbTag
    DDDBConf(DataType = dataType)
    CondDB().UseLatestTags = [dataType]
    gaudi = AppMgr()
    gaudi.initialize()
    pps = gaudi.ppSvc()
    if len(pps.all()) == 0:
        print("ERROR: No particles returned from Particle Property Service!\n")
        return False
    pmap = {}
    pids = []
    for p in pps.all():
        if p.name().startswith('anti-'):
            continue
        pid = p.pid().pid()
        if pid < 0:
            continue
        if len(str(pid)) > 7:
            print ("Ignored " + str(p))
            continue
        lft = p.lifetime()
        if lft < 0.0:
            print("Negative lifetime! " + str(p))
            continue
        if pmap.has_key(pid):
            print("Duplicate particle in list with PID " + str(pid))
            continue
        if lft < 1.0e-32:
            pids.append(pid)
            continue
        #express lifetime in seconds
        lft = lft/SystemOfUnits.s
        pmap[pid] = lft
    if os.path.isfile(outputFileName):
        print("File '%s' already exists." % (outputFileName))
        ans = raw_input('Do you really want to overwrite (yes/no)? ')
        if ans.lower() == 'no':
            print('Aborting execution. Please, rename output file.')
            exit(5)
    fp = open(outputFileName,"w")
    fp.write(codePreamble % (dddbTag, dataType, datetime.datetime.utcnow().strftime(timestampFormat)))
    fp.write("  ")
    sortedKeys = pmap.keys()
    sortedKeys.sort()
    k = 2
    for pdgid in sortedKeys:
        svdbl = "%.8E" % (pmap[pdgid])
        smant = svdbl[0:svdbl.rfind('E')-1]
        sexp  = svdbl[svdbl.rfind('E'):]
        smant = smant.rstrip('0')
        svdbl = (smant + sexp)
        spair = codeMakePair % (pdgid,svdbl)
        lts = len(spair)
        k += lts
        if k > 80:
            k = lts
            fp.write("\n  ")
        fp.write(spair)
    fp.write("\n  return true;\n}\n")
    fp.flush()
    if len(pids) > 0:
        pids.sort()
        fp.write( codePreamble2 % ( len(pids), analysisName, len(pids) ) )
        k = 0
        jj = 0
        for jj in xrange(0,len(pids)):
            ii = pids[jj]
            #ignore partons and particle ids below 100
            if ii <= 100:
                continue;
            spid = "%u" % (ii)
            if jj < len(pids)-1:
                spid += ","
            k += len(spid)
            fp.write(spid)
            if k > 80:
                fp.write("\n")
                k = 0
        fp.write("};\n")
    fp.flush()
    fp.close()

def showUsage():
    print('LHCb Life-time table to C++ std::map convertor')
    print('==============================================')
    print('Usage:')
    print('   python %s <DDDB_tag> <data_type_CondDB> <RivetAnalysis_code_name> [<output_file_name.cpp>]' % (sys.argv[0]))
    print('')
    print('Example:')
    print('   python %s %s %s LHCB_2012_I1119400 lftcodefrag.cpp' % (sys.argv[0], dddbTag, dataType))
    print('')

if __name__ == '__main__':
    if len(sys.argv) < 4:
        showUsage()
        exit(1)
    dddbTag = sys.argv[1]
    dataType = sys.argv[2]
    analysisName = sys.argv[3]
    if len(sys.argv) > 4:
        outputFileName = sys.argv[4]
    CreateLifetimeMapCode()
    print ("Done!")

# =============================================================================
# The END
# =============================================================================
