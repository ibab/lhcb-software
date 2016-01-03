import importlib
import os, sys, re
import Configurables as Configs
import Gaudi.Configuration as Gaudi
import GaudiKernel
#from GaudiKernel.ProcessJobOptions import PrintOff,InstallRootLoggingHandler,logging
#from Configurables import CondDB, GaudiSequencer, EventPersistencySvc, \
#    HistogramPersistencySvc, EventLoopMgr, OutputStream, Gaudi__SerializeCnvSvc, \
#    DstConf

MaxIt  = 25  ## total number of primary iterations to be done
PassIt = 12   ## number of primary iterations in each secondary iteration

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

def staticVar(varname, value):
    def decorate(func):
        setattr(func, varname, value)
        return func
    return decorate

@staticVar("Online", None)
def importOnline():
  if importOnline.Online:
    return importOnline.Online
  if 'RUNINFO' in os.environ:
    runinfo = os.environ['RUNINFO']
    d, f = os.path.split(runinfo)
    name, ext = os.path.splitext(f)
    sys.path.insert(1, d)
    Online = importlib.import_module('OnlineEnvBase')
    sys.path.remove(d)
  else:
    import OnlineEnv as Online
  Online.CondDBTag = 'cond-20150828'
  importOnline.Online = Online
  return Online

class Paths():
    ## contains the paths common for all the modules
    ## just not to get confused between them
    def __init__(self, index = 0, hn = None, data_directory = '/group/calo/CalibWork/'):
        ## determine the working directory
        #if os.path.isdir(input_file):
        #    self.__dir   = input_file
        #else:
        #    self.__dir   = os.path.dirname(input_file)

        self.dst_dir = '/localdisk/Alignment/Calo/'
        # if not os.path.exists(self.dst_dir): os.mkdir(self.dst_dir)

        self.gz_dir = data_directory
        if not os.path.exists(self.gz_dir): os.mkdir(self.gz_dir)

        self.input_dir1 = '/localdisk/Alignment/BWDivision/'
        self.input_dir2 = '/localdisk/Alignment/Rich/'

        self.__index = index
        if hn:    self.__hn    = hn

    def store_location(self):
        ## place, where the final results are stored
        sl = self.gz_dir+'/Store'
        if not os.path.exists(sl): os.mkdir(sl)
        return sl

    def fileprefix(self):
        ## for simplicity all the root/dst/fmdst/gz files
        ## have the same prefix, containing the information
        ## about the working node and worker number
        return 'Kali-%s-%i'%(self.__hn,self.__index)

    def lambdas_location_an(self):
        ## place, where calibration coefficienst on each iteration are saved
        return os.path.join(self.gz_dir    , 'Lambdas%s_%i.db'%(self.__hn,self.__index))

    def lambdas_location(self):
        ## place, where calibration coefficienst on each iteration are saved
        return os.path.join(self.gz_dir    , 'LambdasPass%i.db')

    def lambdas_location_it(self):
        ## place, where the calibration coefficients are finally stored
        return os.path.join(self.store_location(),'LamsPass%iIt%i.gz')

    def lambdas_db_location(self):
        ## place, where the coefficients for the re-reconstruction are saved
        return os.path.join(self.gz_dir    , 'ReLamsPass%i.gz' )

    def histos_location_an(self):
        ## place, where the filled histograms from each worker are saved
        fp = self.fileprefix()
        return os.path.join(self.gz_dir    , 'FilledHistograms-'+fp+'.db')

    def histos_location_it(self):
        ## place, where the filled histograms after each iteration are saved
        return os.path.join(self.store_location(),'Histograms%iPassIt%i.gz')

    def getfmdstfiles(self):
        fp     = self.fileprefix()
        ## find all the fmdsts in the working directory
        fmdsts = [os.path.join(self.dst_dir,f) for f in os.listdir(self.dst_dir) if re.match(r'^'+fp, f) and re.match(r'.*\.(fmDST|fmdst)$', f)]
        return fmdsts

    def getdstfiles(self):
        ## find all the dsts in the working directory
        fp   = self.fileprefix()
        dsts = [os.path.join(self.dst_dir,f) for f in os.listdir(self.dst_dir) if re.match(r'^'+fp, f) and re.match(r'.*\.(DST|dst)$', f)]
        return dsts

    def getrootfiles(self):
        ## find all the root files in the working directory
        fp = self.fileprefix()
        roots = [os.path.join(self.dst_dir,f) for f in os.listdir(self.dst_dir) if re.match(r'^'+fp, f) and re.match(r'.*\.(ROOT|root)$', f)]
        return roots

    def getbrunelhistos(self):
        ## find all the root files in the working directory
        fp = self.fileprefix()
        bruns  = [os.path.join(self.dst_dir,f) for f in os.listdir(self.dst_dir) if 'histos' in f]
        return bruns

    def gethistomaps(self):
        ## find all the histogram databases in the working directory
        return [os.path.join(self.gz_dir,f) for f in os.listdir(self.gz_dir) if re.match('^FilledHistograms', f)]

    def getlambdamaps(self):
        ## find all the histogram databases in the working directory
        return [os.path.join(self.gz_dir,f) for f in os.listdir(self.gz_dir) if re.match('^Lambdas', f)]

    def getlambdadbases(self):
        ## find all the histogram databases in the working directory
        return [os.path.join(self.gz_dir,f) for f in os.listdir(self.gz_dir) if re.match('^ReLamsPass', f)]

    def getinputfiles(self):
        # get the available mdfs
        Online     = importOnline()
        runnumbers = Online.DeferredRuns
        list_of_files = []

        for rn in runnumbers:
            list_of_files.extend([os.path.join(self.input_dir2,f) for f in os.listdir(self.input_dir2) if rn in f and f.endswith('.mdf')])
            list_of_files.extend([os.path.join(self.input_dir1,f) for f in os.listdir(self.input_dir1) if rn in f and f.endswith('.mdf') and not f in os.listdir(self.input_dir2)])

        return list_of_files, runnumbers[0], runnumbers[-1]
