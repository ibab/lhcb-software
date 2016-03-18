#!/usr/bin/env python
import gc
import optparse
import sys
import os
import subprocess

parser = optparse.OptionParser(usage = 'usage: %prog')

parser.add_option( "--data", action = "store", type = 'string',
                   dest = "data", default = '',
                   help = "File with data to run on." )
parser.add_option( "-n", "--nevents", action = "store", type = 'int',
                   dest = "nevents", default = -1,
                   help = "Number of events to run." )
parser.add_option( "-t", "--test", action = "store_true",
                   dest = "test", default = False,
                   help = "Test with a single process and one input file." )

(options, args) = parser.parse_args()

if len(args) not in (0, 1):
    print parser.print_usage()
    sys.exit(-2)

from math import fabs
from array import array

from Gaudi.Configuration import *
from Configurables import LHCbApp, EventSelector, DstConf, PhysConf

from Configurables import DataOnDemandSvc
dod = DataOnDemandSvc()
dod.Dump = True
ApplicationMgr().ExtSvc += [ 'DataOnDemandSvc' ]

if options.data:
    importOptions(options.data)

## Moore settings
app = LHCbApp()
app.EvtMax = options.nevents
app.CondDBtag = 'sim-20131023-vc-md100'
app.DDDBtag = 'dddb-20130929-1'
app.DataType   = '2015'
app.Simulation = True

ApplicationMgr().HistogramPersistency = "ROOT"

PhysConf(EnableUnpack = ["Reconstruction", "Stripping"])
DstConf(EnableUnpack = ["Reconstruction", "Stripping"])
if app.getProp('Simulation'):
    from Configurables import SimConf
    SimConf(EnableUnpack = True, EnablePack = False)

EventSelector().PrintFreq = 100

eos_dir = '/eos/lhcb/grid/prod/lhcb/MC/Dev/XDST/00034704/0000'
p = subprocess.Popen(['/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select', 'ls', eos_dir],
                     stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
o, e = p.communicate()
input_files = sorted(['root://eoslhcb.cern.ch/' + os.path.join(eos_dir, f.strip()) for f in o.split()])

seq = GaudiSequencer( "TopSequence" )

from Configurables import ( DecodeVeloRawBuffer, RawBankToSTLiteClusterAlg,
                            FastVeloTracking, L0Conf,
                            createODIN, MuonRec )
## Configure extra debug instance
## matchVeloDebug = Hlt1TrackUpgradeConf.ConfiguredMatchVeloMuon( ToolSvc(), "MatchMuonDebug", 6000 )
## matchVeloDebug.OutputLevel = 1
from Configurables import MatchVeloTTMuon
def configureMVTTM(name, corrections, window = (400, 400),
                   histos = False, z_correction = (25.17, -701.5), tool_type = MatchVeloTTMuon):
    mvm = tool_type(name)
    mvm.XWindow = window[0]
    mvm.YWindow = window[1]
    mvm.MaxChi2DoFX = 25
    mvm.SetQOverP = True
    mvm.HistoProduce = histos
    if hasattr(tool_type, "ErrorCorrections"):
        mvm.ErrorCorrections = corrections
        mvm.MagnetCorParA = z_correction[0]
        mvm.MagnetCorParB = z_correction[1]
    mvm.OutputLevel = 3
    return mvm

corrections = {'mx' : [9.55, -0.5, -143],
               'my' : [7.46, -0.5, -112],
               'x'  : [1., 1., 1., 1.],
               'y'  : [1., 1., 1., 1.]}

# Latest iteration, iter 6
# corrections = {'mx' : [9.55, -0.5, -143],
#                'my' : [7.46, -0.5, -112],
#                'x'  : [0.75, 0.6, 0.49, 0.65],
#                'y'  : [0.483, 0.589, 0.738, 0.813]}

## Recombine RawEvent
# from Configurables import RawEventFormatConf, RawEventJuggler, RecombineRawEvent
# juggleSeq = GaudiSequencer("JuggleSeq")
# RawEventJuggler().Input = 2.0
# RawEventJuggler().Output = 0.0
# RawEventJuggler().Sequencer = juggleSeq

## Configure Reco
co = createODIN()
mr = MuonRec()

decodeVelo = DecodeVeloRawBuffer( "createVeloLiteClusters" )
recoVelo = FastVeloTracking( 'FastVeloHlt', OutputTracksName = "Hlt/Track/Velo" )
recoVelo.HLT1Only = False
decodeIT = RawBankToSTLiteClusterAlg("createITLiteClusters")
decodeIT.DetType  = "IT"

## Configure L0 emulation by hand
# from Configurables import L0DUFromRawAlg, L0DUFromRawTool
# l0du   = L0DUFromRawAlg("L0DUFromRaw")
# l0du.addTool(L0DUFromRawTool,name = "L0DUFromRawTool")
# l0du = getattr( l0du, 'L0DUFromRawTool' )
# l0du.StatusOnTES         = False
seq.Members = [ co, decodeVelo, recoVelo, decodeIT, mr ]

from Configurables import CombinedParticleMaker
from Configurables import ProtoParticleMUONFilter
from Configurables import TrackSelector
from Configurables import LoKiSvc
LoKiSvc().Welcome = False

# Muon MC truth
from Configurables import MuonCoord2MCParticleLink
mc_seq = GaudiSequencer('MCSequence', Members = [MuonCoord2MCParticleLink()])

# ApplicationMgr().TopAlg = [juggleSeq, seq, particle_seq]
ApplicationMgr().TopAlg = [seq, mc_seq]

# Track <-> MC
# from  Configurables import DataOnDemandSvc, LoKi__Track2MC
# t2mc = LoKi__Track2MC(Tracks = ['Rec/Track/Best'])
# DataOnDemandSvc().AlgMap['Relations/Rec/Track/Default' ] = t2mc.getFullName()
# DataOnDemandSvc().OutputLevel = 4

mvm = configureMVTTM('MatchVeloTTMuon', corrections, histos = True)
mvm_chi2 = configureMVTTM('MatchVeloTTMuonChi2', corrections, histos = True)

mvm_old = None
try:
    from Configurables import MatchVeloTTMuonOld
    mvm_old = configureMVTTM('MatchVeloTTMuonOld', corrections, histos = True, tool_type = MatchVeloTTMuonOld)
except ImportError:
    pass

def run(input_files, output_file):

    ## Bit of extra configuration
    from GaudiConf import IOHelper
    IOHelper().inputFiles(input_files)
    HistogramPersistencySvc().OutputFile = output_file.replace('.root', '_histos.root')

    ## GaudiPython
    import GaudiPython
    import cppyy
    cppyy.load_reflection_info('libLinkerEvent')
    cppyy.load_reflection_info('libLinkerInstancesDict')

    from GaudiPython.Bindings import AppMgr
    appMgr = AppMgr(outputlevel = 3)
    appMgr.config()

    appMgr.HistogramPersistency = 'ROOT'
    ntSvc = GaudiPython.iService('NTupleSvc')
    ntSvc.Output = ["MATCHZERRLUN DATAFILE='{0}' OPT='NEW'".format(output_file)]

    from Hlt1Muons.MatchAlgo import MatchResidualAlgo
    match_algo = MatchResidualAlgo("MatchResidualAlgo", MatchVeloMuon = mvm.getFullName())
    match_algo.NTupleLUN = 'MATCHZERRLUN'
    appMgr.addAlgorithm(match_algo)

    from Hlt1Muons.MatchAlgo import MatchChi2Algo
    chi2_algo = MatchChi2Algo("MatchChi2Algo", MatchVeloMuon = mvm_chi2.getFullName())
    chi2_algo.NTupleLUN = 'MATCHZERRLUN'
    appMgr.addAlgorithm(chi2_algo)

    if mvm_old:
        chi2_algo_old = MatchChi2Algo("MatchChi2AlgoOld", MatchVeloMuon = mvm_old.getFullName())
        chi2_algo_old.NTupleLUN = 'MATCHZERRLUN'
        appMgr.addAlgorithm(chi2_algo_old)

    appMgr.initialize()
    appMgr.run(options.nevents)
    appMgr.stop()
    appMgr.finalize()
    appMgr.exit()

def chunks(l, n):
    """Yield n evenly sized chunks from l."""
    t = len(l) / int(n)
    n1 = len(l) % n
    for i in range(0, (t + 1) * n1, t + 1):
        yield l[i:i + t + 1]
    for i in range(n1 * (t + 1), len(l), t):
        yield l[i:i + t]

def run_para(arg):
    return run(*arg)

if __name__ == '__main__':
    if options.test:
        run(input_files[:1], 'match_zerr_test.root')
    else:
        from multiprocessing import Pool
        n_proc = 5
        pool = Pool(processes = n_proc)
        output_files = ['match_zerr_%d.root' % i for i in range(1, n_proc + 1)]
        pool.map(run_para, zip([c for c in chunks(input_files, n_proc)], output_files))

        # Merge the result
        subprocess.call(['hadd', '-f', 'match_zerr.root'] + output_files),
        subprocess.call(['hadd', '-f', 'match_zerr_histos.root'] + [of.replace('.root', '_histos.root') for of in output_files]),
