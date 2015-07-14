import os, sys
from multiprocessing import Process

try:
    import AllHlt1
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import AllHlt1

class FileMerger(object):
    def __init__(self, input_files, output_file):
        self.__process = Process(target = self.run)
        self.__input_files = input_files
        self.__output_file = output_file

    def run(self):
        from Configurables import LHCbApp
        app = LHCbApp()
        app.EvtMax = -1
        app.DataType = '2015'
        app.CondDBtag = 'cond-20150601'
        app.DDDBtag = 'dddb-20150526'
        # app.Simulation = True

        from Configurables import CondDB
        CondDB().Simulation = False
        CondDB().UseDBSnapshot = True
        CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
        CondDB().EnableRunChangeHandler = False
        CondDB().Tags["ONLINE"] = 'fake'
        CondDB().setProp("IgnoreHeartBeat", True)
        CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

        from Configurables import EventSelector
        EventSelector().PrintFreq = 10000

        from GaudiConf import IOHelper
        IOHelper("MDF", "MDF").inputFiles(self.__input_files)

        # Output writers
        # L0 filter
        from Configurables import bankKiller
        hlt_banks = [ 'HltDecReports','HltRoutingBits','HltSelReports','HltVertexReports','HltLumiSummary','HltTrackReports' ]
        killer = bankKiller( 'RemoveInputHltRawBanks',  BankTypes = hlt_banks )


        # InputCopyStream
        from Configurables import LHCb__MDFWriter as MDFWriter
        mdf_writer = MDFWriter('MDFWriter', Compress = 0, ChecksumType = 1, GenerateMD5 = True,
                               Connection = 'file://%s.mdf' % self.__output_file)

        # Sequence
        from Configurables import GaudiSequencer
        seq = GaudiSequencer("Sequence", Members = [killer, mdf_writer])
        from Gaudi.Configuration import ApplicationMgr
        ApplicationMgr().TopAlg = [seq]

        # Timing table to make sure things work as intended
        from Configurables import AuditorSvc, LHCbTimingAuditor
        ApplicationMgr().AuditAlgorithms = 1
        if 'AuditorSvc' not in ApplicationMgr().ExtSvc:
            ApplicationMgr().ExtSvc.append('AuditorSvc')
        AuditorSvc().Auditors.append(LHCbTimingAuditor(Enable = True))

        from GaudiPython import AppMgr
        gaudi = AppMgr()
        gaudi.initialize()
        gaudi.run(app.EvtMax)
        gaudi.stop()
        gaudi.finalize()
        gaudi.exit()

    def process(self):
        return self.__process

    def start( self ):
        self.__process.start()

    def join( self ):
        self.__process.join()


base_dir = '/localdisk/hlt1/data2015/0506NB'
files = sorted((os.path.join(base_dir, f) for f in os.listdir(base_dir)))

def chunks(l, n):
    """ Yield successive n-sized chunks from l.
    """
    n = len(l) / n + 1
    for i, j in zip(xrange(0, len(l), n), xrange(1, len(l) + 1)):
        yield l[i:i+n], j

of_pat = '/localdisk/hlt1/data2015/0506NB_hlt_stripped/153770_NoHLT_%02d'
mergers = [FileMerger(input_files, of_pat % i) for input_files, i in chunks(files, 15)]
for m in mergers:
    m.start()
for m in mergers:
    m.join()
