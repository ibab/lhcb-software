from GaudiPython.Bindings import AppMgr
from GaudiConf             import IOHelper
from Bender.Main import appMgr, get, SUCCESS 
from array import array
import ROOT
    

# some global variables
last_open_lfn = "LFN:/lhcb/LHCb/Collision12/BHADRONCOMPLETEEVENT.DST/00020241/0000/00020241_00000015_1.bhadroncompleteevent.dst"
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

# should be called from 'new file open' incident handler
def update_last_lfn(lfn):
    global last_open_lfn
    last_open_lfn = lfn

# basic routine
def run ( numevents ) :
    gaudi = appMgr()
    for i in range ( 0 , numevents ) :
        gaudi.run(1) 
        rc = get('/Event/Rec/Header')
        if not rc :
            print 'RC !!!! '
            break
        indexer_put(tree, {
            'lfn/C': last_open_lfn,
            'position/I': i,
            'eventNumber/I': rc.evtNumber(),
            'runNumber/I': rc.runNumber()
        })
    indexer_finish()

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

    
def configure( datafiles         ,
               catalogs  = []    ,
               castor    = False ,
               params    = {}    ) :
    
    
    from Configurables         import LHCbApp
    
    LHCbApp()
    
    from Bender.Main import setData, appMgr, SUCCESS 
    stripping = params['stripping'] if 'stripping' in params else 'strNone'
    outputfilename = params['outputfilename'] if 'outputfilename' in params else 'i.root'

    setData ( datafiles , catalogs , castor )

    
    gaudi = appMgr()
    gaudi.setAlgorithms([])
    indexer_prepare(stripping, outputfilename)
    
    return SUCCESS 
##


if '__main__' == __name__ :
    
    from Gaudi.Configuration import * 
    datafiles = [
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000007_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000020_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000033_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000046_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000059_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000069_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000076_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000090_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000097_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000105_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000118_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000120_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000122_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000124_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000126_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000132_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000138_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000143_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000144_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000148_1.dimuon.dst'
        ]
    
    catalogs = [ 'xmlcatalog_file:qqq.xml' ]
    
    
    configure ( datafiles , catalogs , True, {
        'outputfilename':'i.root',
        'stripping'     :'DIMUON',
    }) 
    run(10)
    
    
