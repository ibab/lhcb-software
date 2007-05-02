import os, time, platform
ld_library_path = os.environ['LD_LIBRARY_PATH']
systype = 'slc3_ia32_gcc323'
dbg = '_dbg'
sw = '/sw/lib/testsite'
inst = '/home/frankm/cmtuser/Gaudi_v19r2'
root = sw+'/lcg/external/root/5.14.00c/'+systype+'/root/lib'
python = sw+'/lcg/external/Python/2.4.2/'+systype
pvss   = '/localdisk/pvss/STORAGE/bin'

def makeLink(dir,file):
    target = pvss+'/'+file
    try:
        os.unlink(target)
    except:
        pass
    cmd = 'ln -s '+dir+'/'+file +' ' + target
    print cmd
    #os.system(cmd)
    

makeLink(root,'libPyROOT.so')
makeLink(root,'libCintex.so')
# makeLink(root,'/libGraf3d.so)
makeLink(inst+'/Online/PVSSManager/'+systype+dbg,'libPVSSManager.so')
makeLink(python+'/lib','libpython2.4.so.1.0')
makeLink(python+'/bin','python')
makeLink(root,'libReflex.so')
makeLink(root,'libCore.so')
makeLink(sw+'/lcg/external/RELAX/RELAX_1_1_7/'+systype+'/lib','libSTLRflx.so')
makeLink(inst+'/Online/PVSSInterface/'+systype+dbg,'libPVSSInterface.so')
makeLink(inst+'/Online/PVSSInterface/'+systype+dbg,'libPVSSInterfaceDict.so')
makeLink(inst+'/Online/OnlineKernel/'+systype+dbg,'libOnlineKernel.so')
