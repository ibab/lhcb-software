import os
import subprocess
import copy
import pydim
import shlex
import shutil

utgid = 'TEST_ADDER_00'

cmd = ("import GaudiKernel.ProcessJobOptions; " +
       "from Gaudi.Configuration import importOptions; " +
       "GaudiKernel.ProcessJobOptions.printing_level=3; " +
       "from Hlt2Monitoring import Hlt2Adder; Hlt2Adder.configure('top', '/home/raaij/histograms', ports = {'HistoRelay' : {'in' : 41337, 'out' : 41338}, 'InfoRelay' : {'in' : 41339, 'out' : 41340}, 'Adder' : {'out' :41341}, 'InfoSvc' : {'out' : 41342}})")
env = {'LC_ALL' : 'C', 'DIM_DNS_NODE' : 'hlt01',
       'UTGID' : utgid, 'TEMPDIR' : '/tmp/testAdder', 'PARTITION' : 'LHCb2',
       'PARTITION_NAME' : 'LHCb2', 'RUNINFO' : '/home/raaij/cmtuser/MooreOnlineDev_v24r2/scripts/OnlineEnvBase.py'}

## 'LOGFIFO' : "/tmp/test_3K_3g5/logTest_TEST.fifo",
os_env = copy.copy(os.environ)
os_env.update(env)
command = 'GaudiOnlineExe.exe libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -opt=command="%s" -main=/home/raaij/cmtuser/MooreOnlineDev_v23r7/scripts/Main.opts -auto' % cmd

def config():
    pydim.dic_cmnd_service(utgid, ('configure\0',), "C")

def start():
    pydim.dic_cmnd_service(utgid, ('start\0',), "C")

def stop():
    pydim.dic_cmnd_service(utgid, ('stop\0',), "C")

def reset():
    pydim.dic_cmnd_service(utgid, ('reset\0',), "C")

command = os.path.expandvars(command)
p = subprocess.Popen(shlex.split(command), env = os_env)
print p.pid

import atexit
atexit.register(p.terminate)
