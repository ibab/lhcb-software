#!/usr/bin/env python
# =============================================================================
## @file Gen/GenTune/lhcbLFT2StdMap.py
#  Dumps lifetime information from the LHCb Particle Property Service
#  to a file containing a huge std::map<std::int, double>. The keys in the
#  map are the particle PDG ids and the values are ctau values expressed in [m].
#  Module can be used both as a script or imported to generate the HepPDT file.
#  @author Alex Grecu <alexandru.grecu@gmail.com>
#  @date 2013-06-04
#  @lastmod 2014-06-15
# =============================================================================
"""
Create std::map with LHCb particles ctau values to hardcode into RIVET.
We don't like HepPDT!!!

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = "Alex GRECU <alexandru.grecu@gmail.com>"
__version__ = "version $Revision: 1.1 $"
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
import getopt
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
const int %s::_stablePDGIds[%u] = {
"""

timestampFormat = "%a, %d %b %Y @ %H:%M:%S UTC"
analysisName = "<RivetAnalysis_Plugin_Class>"
codeMakePair = "m[%u] = %s;"

outStream = None
dddbTag = "dddb-20130503-1"
dataType = "2007"
bUseLatestCondDB = False

class lbRivetAnaName(object):
    ###
    def __init__(self, prefix='LHCB', year=2009, code='I<InspireRecord>/S<SpiresRecord>'):
        if isinstance(year, int):
            self.year = year
        elif self._testInt(year):
            self.year = int(year)
        else:
            raise TypeError('Invalid type for year argument.')
        self.prefix = prefix.upper()
        object.__setattr__(self,'code',None)
        object.__setattr__(self,'_type','exp')
        if self._testCode(code):
            self.code = code
        else:
            self._parse(code)
    ####
    def _parse(self, code):
        its = code.upper().split('_')
        if len(its) != 3 :
            eprint('Invalid experiment analysis full name.','err')
            return False
        if its[0] == 'MC':
            object.__setattr__(self,'_type','mc')
            self.code = code
            self.prefix = 'MC'
            self.year = -1
        else:
            if not self._testCode(its[2]) or not self._testInt(its[1]):
                return False
            self.year = int(its[1])
            self.prefix = its[0]
            self.code = its[2]
            object.__setattr__(self,'_type','exp')
        return True
    ###
    def _testCode(self, code):
        if code is None or not isinstance(code, str) or len(code) == 0:
            return False
        if not code[0] in ['I','S']:
            eprint('Invalid prefix for analysis Inspire/Spires code.', 'err')
            return False
        return True
    ###
    def _testInt(self, sval):
        if not isinstance(sval, str) and not isinstance(sval,int):
            return False
        ret = True
        try:
            if isinstance(sval, str):
                val = int(sval)
        except Exception,exx:
            ret = False
        return ret
    ###
    def __setattr__(self,name,value):
        if name.startswith('_'):
            eprint('Cannot set private properties of %s objects.\n' % (self.__class__.__name__),'warn')
            return
        if name == 'code' and self._testCode(value):
            object.__setattr__(self,'code',value)
        if name == 'prefix' and isinstance(value,str):
            object.__setattr__(self,'prefix',value.upper())
        if name == 'year' and ( isinstance(value, int) or self._testInt(value) ):
            if isinstance(value, str):
                object.__setattr__(self,'year',int(value))
            else:
                object.__setattr__(self,'year',value)
    ###
    def _isValid(self):
        ret = not self.code is None
        if ret:
            ret = self._testCode(self.code) and self._testInt(self.year)
        return ret
    ###
    def experiment(self):
        ret = 'unknown'
        if self._isValid() and self._type == 'exp':
            ret = self.prefix
        return ret
    ###
    def getURL(self):
        ret = 'unknown'
        if self._isValid():
            if self.code.startswith('I'):
                ret = 'http://inspirehep.net/record/%s' % (self.code[1:])
            else:
                print('Please provide a valid InspireHep code.')
        return ret
    ###
    def fullName(self):
        ret = 'unvalid'
        if self._isValid():
            if self._type == 'mc':
                ret = self.code
            else:
                ret = '%s_%d_%s' % (self.prefix, self.year, self.code)
        return ret

def eprint(msg, level=None):
    if msg.endswith('\n'):
        msg = msg.rstrip('\n')
    if level == 'err':
        msg = '\x1B[41m%s\x1B[0m' % (msg)
    if level == 'warn':
        msg = '\x1B[1m%s\x1B[0m' % (msg)
    sys.stderr.write(msg + '\n')

# ignore abs(pdg_id) > 9999999 -> eliminate nuclei and other odd particles from table
def CreateLifetimeMapCode(config=None, bRetLists=False):
    global codePreamble, codeMakePair, codePostamble, outStream, analysisName, timestampFormat, dataType, dddbTag, bUseLatestCondDB
    if not config is None:
        dataType = config[0]	#year
        dddbTag = config[1]	#DDDB tag
    lbApp = LHCbApp()
    lbApp.DDDBtag = dddbTag
    DDDBConf(DataType = dataType)
    if bUseLatestCondDB:
        CondDB().UseLatestTags = [dataType]
    gaudi = AppMgr()
    gaudi.initialize()
    pps = gaudi.ppSvc()
    if len(pps.all()) == 0:
        eprint("ERROR: No particles returned from Particle Property Service!\n",'err')
        return False
    pmap = {}
    pids = []
    for p in pps.all():
        #if p.name().startswith('anti-'):
        #    continue
        pid = p.pid().pid()
        if pid < 0 or pmap.has_key(pid):
            apid = ((pid < 0) and (-pid) or pid)
            if pmap.has_key(apid):
                nlft = "%.8E" % (p.lifetime()/SystemOfUnits.s)
                olft = "%.8E" % (pmap[apid])
                if nlft.startswith(olft):
                    continue
                else:
                    eprint("(anti-)/Particle lifetimes differ [%d] (%s vs. %s). Use asymmetric algorithm with PID:Lifetime map." % (pid, nlft,olft), 'warn')
                    if pid > 0:
                        pmap[-pid] = pmap[pid]
                        del pmap[pid]
            else:
                pid = apid
        #if len(str(pid)) > 7: #????
        #    print ("Ignored " + str(p))
        #    continue
        if pid > 79 and pid < 101:
            eprint('Ignoring generator dependent PIDs in interval [80-100].')
            eprint(str(p))
            continue
        if len(str(pid)) > 9:   #ignore nuclei
            eprint ("Ignoring nucleus " + str(p))
            continue
        lft = p.lifetime()
        if lft < 0.0:
            eprint("Negative lifetime! " + str(p), 'err')
            continue
        if pmap.has_key(pid):
            eprint("Duplicate particle in list with PID " + str(pid), 'warn')
            continue
        if lft < 1.0e-32:
            if not pid in pids:
                pids.append(pid)
            continue
        #express lifetime in seconds
        lft = lft/SystemOfUnits.s
        pmap[pid] = lft
    if not bRetLists:
        if not outStream is None:
            if os.path.isfile(outStream):
                eprint("File '%s' already exists." % (outStream))
                ans = raw_input('Do you really want to overwrite (yes/no)? ')
                if ans.lower() == 'no':
                    eprint('Aborting execution. Please, rename output file.', 'warn')
                    exit(9)
            fp = open(outStream,"w")
        else:
            fp = sys.stdout
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
        if not outStream is None:
            fp.flush()
            fp.close()
    gaudi.finalize()
    gaudi.exit()
    del gaudi, lbApp
    return (pmap, pids)


def showUsage():
    print('''
LHCb Life-time table to C++ std::map convertor
==============================================
Usage:''')
    print('   python %s -d <DDDB_tag> -t <data_type_CondDB> [-y <RivetAnalysis_year>] -c <RivetAnalysis_code_name> [-o <output_file_name.cpp>]' % (sys.argv[0]))
    print('')
    print('Example:')
    print('   python %s -d %s -t %s -c LHCB_2012_I1119400 -o lftcodefrag.cpp' % (sys.argv[0], dddbTag, dataType))
    print('''
  -- default output is stdout; can be modified by using -o/--output option
  -- specify DDDB tag with -d/--dddb option; use 'latest' to pick up latest tag for data type
  -- specify data type (year) with --t/--dataType option
  -- optionally specify Rivet analysis year with -y/--anayear option (especially if not contained in analysis code name)
  -- specify Rivet analysis code name with -c/--anacode option; it is automatically reconstructed from year if it doesn't
     start with LHCB_; minimally should contain Inspire record number and prefix, e.g. I1119400
  -- -h/--help option will print this help message and exit

Life-times are expressed in seconds. Mind long options in detailed help above.''')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        showUsage()
        sys.exit(1)
    try:
        sOpts, sArgs = getopt.getopt(sys.argv[1:], "hd:t:y:c:o:",["help","dddb=","dataType=","anayear=","anacode=","output="])
    except getopt.GetoptError, exx:
        print(exx.message)
        showUsage()
        sys.exit(3)
    anaYear = 0
    anaCode = None
    for opt, val in sOpts:
        if opt in ("-h","--help"):
            showUsage()
            sys.exit(0)
        if opt in ("-d","--dddb"):
            dddbTag = val
        elif opt in ("-t", "--dataType"):
            dataType = val
        elif opt in ("-y","--anayear"):
            anaYear = int(val)
        elif opt in ("-c","--anacode"):
            anaCode = val
        elif opt in ('-o','--output'):
            outStream = val
    if dddbTag == 'latest':
        if dataType == '2007':
            eprint('Please, provide a valid data type year.\n', 'err')
            sys.exit(5)
        bUseLatestCondDB = True
    if anaCode is None:
        eprint('Please, provide a valid (full/partial) Rivet analysis name.', 'err')
        sys.exit(5)
    anaName = lbRivetAnaName(year=anaYear,code=anaCode)
    analysisName = anaName.fullName()
    if analysisName == 'unvalid':
        eprint('Could not determine a valid Rivet analysis name from provided arguments.', 'err')
        #sys.stderr.write(str(anaName.__dict__) + '\n')
        sys.exit(7)
    eprint('Generating lifetime table for analysis \x1B[1m%s\x1B[0m...' % (analysisName))
    
    CreateLifetimeMapCode()
    eprint('Done!\n')

# =============================================================================
# The END
# =============================================================================
