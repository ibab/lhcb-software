import sys
del sys.modules["GaudiPython"]
from GaudiPython.Bindings import AppMgr
from GaudiConf             import IOHelper
from Bender.Main import appMgr, get, SUCCESS, FAILURE, Algo
from array import array
import ROOT
    

# some global variables
last_open_lfn = ""
fh = None
tree = None
data_fields = {
    'stripping/C': [array('c', 1000*''), 'stripping/C'],
    'lfn/C': [array('c', 1000*''), 'lfn/C'],
    'position/I': [array('i', [0]), 'position/I'],
    'runNumber/I': [array('i', [0]), 'runNumber/I'],
    'eventNumber/I': [array('i', [0]), 'eventNumber/I'],
}
# end of global variables

evtCounter = -1
class FillIndexer(Algo):
    def analyse(self):
        global evtCounter,last_open_lfn
        evtCounter += 1

        rc = get('/Event/Rec/Header')
        if not rc :
            return FAILURE
        indexer_put(tree, {
            'lfn/C': last_open_lfn,
            'position/I': evtCounter,
            'eventNumber/I': rc.evtNumber(),
            'runNumber/I': rc.runNumber()
        })

        return SUCCESS

# should be called from 'new file open' incident handler
def update_last_lfn(lfn):
    global last_open_lfn
    last_open_lfn = lfn

def indexer_prepare(stripping, outputfilename):
    global fh, tree
    fh = ROOT.TFile(outputfilename, "recreate")
    tree_meta = indexer_create_tree('meta', ['stripping/C'])
    indexer_put(tree_meta, {'stripping/C':  stripping})
    tree = indexer_create_tree("data", ['lfn/C', 'position/I', 'runNumber/I', 'eventNumber/I'])
    fh.Write()
    
def indexer_finish():
    fh.Write()
    fh.Close()

def indexer_create_tree(name, branches):
    print "Create tree: " + name
    t = ROOT.TTree(name, "tree " + name)
    for name in branches:
        f = data_fields[name]
        t.Branch(name[:-2], f[0], f[1])
    return t

def indexer_put(tree, data):
    for (k, v) in data.items():
        if k.endswith('C'):
            del data_fields[k][0][:]
            data_fields[k][0].fromstring(v)
            data_fields[k][0].append('\0')
            tree.SetBranchAddress(k[:-2], data_fields[k][0])
        else:
            data_fields[k][0][0] = v
    tree.Fill()

def fill_input(self, filename=None, GUID=None, status=None, addevents=0):
    global last_open_lfn 
    last_open_lfn   = filename
     return self._fill_input(self, filename, GUID, status, addevents)

    
def configure( datafiles         ,
               XMLSummaryFile    ,
               catalogs  = []    ,
               castor    = False ,
               params    = {}    ) :
    
    
    from Configurables         import LHCbApp
    
    LHCbApp(XMLSummary = XMLSummaryFile)
    
    from Bender.Main import setData, appMgr, SUCCESS 
    stripping = params['stripping'] if 'stripping' in params else 'strNone'
    outputfilename = params['outputfilename'] if 'outputfilename' in params else 'i.root'

    setData ( datafiles , catalogs , castor )

    gaudi = appMgr()
    alg = FillIndexer('FillIndexer')
    gaudi.setAlgorithms([alg])
    indexer_prepare(stripping, outputfilename)
    return SUCCESS 


def main():

    '''   
    from Gaudi.Configuration import * 
    if os.path.exists('prodConf.py'):
        from prodconf import ProdConf
        importOptions('prodConf.py')
        datafiles = Prodconf().InputFiles
        allConfigurables.clear()
    else:
        pass
    '''
   
    import os
    for file in os.listdir("."):
       if (file.startswith("prodConf") and file.endswith(".py")):
           prodConf_file = file


 
    import ProdConf
    data = {}
    ProdConf.ProdConf = lambda **kwargs: data.update(kwargs)
    from Gaudi.Configuration import importOptions
    importOptions(prodConf_file)
    
    
    datafiles = data['InputFiles']
    
    XMLSummaryFile = data['XMLSummaryFile']

    s = datafiles[0]
    ss = s.split(".")
    stripping = ss[len(ss)-2]  
    
    catalogs = 'xmlcatalog_file:' + data['XMLFileCatalog']

    outputFileName = data['OutputFilePrefix']+"."+data['OutputFileTypes'][0] 
  
    import XMLSummaryBase.summary
    
    XMLSummaryBase.summary.Summary._fill_input = XMLSummaryBase.summary.Summary.fill_input
    XMLSummaryBase.summary.Summary.fill_input = fill_input

    configure ( datafiles , XMLSummaryFile , catalogs , True, {
        'outputfilename':outputFileName,
        'stripping'     :stripping,
    }) 
    
    gaudi = appMgr()
    gaudi.run(-1)
    indexer_finish()
    sys.exit(0)

main()
        
