"""
LHCb Nightlies settings file.
"""
import os

from LbConfiguration.Project import project_names as listOfProjectNames
projectNamesDict = {}
for p in listOfProjectNames:
    projectNamesDict[p.upper()] = p

#########################################################
# DO NOT USE os.path.join() with os.environ['AFSROOT']. #
# It fails on Windows with 'Z:' as a first argument.    #
# Use os.sep.join([]) instead.                          #
#########################################################

# decide which platform we are on #
if os.environ.get('CMTCONFIG','undefined')[:5] == 'win32' or os.environ.get('CMTCONFIG','undefined')[:5] == 'w_dbg':
    systemType = 'windows'
    os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = os.sep.join(['C:', 'local', 'LCG-nightlies'])
else:
    systemType = 'linux'
    os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'sw', 'lcg', 'app', 'nightlies', 'scripts'])

configurationPath = os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf'])
configurationHistoryPath = os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf', 'history'])

localDirsOnWindows = {}
localDirsOnWindows['Z:\\cern.ch\\user\\k\\kkruzele\\cmtuser\\LBSCRIPTS\\LBSCRIPTS_v99r0\\InstallArea\\win32_vc71\\lib'] = '' # delete from the path
localDirsOnWindows['Z:\\LOCALDRIVE\\C\\'] = 'C:\\'
localDirsOnWindows['Z:\\LOCALDRIVE\\D\\'] = 'D:\\'
localDirsOnWindows['Z:\\LOCALDRIVE\\E\\'] = 'E:\\'
localDirsOnWindows['Z:\\LOCALDRIVE\\F\\'] = 'F:\\'
localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\external'] = 'C:\\local\\external'
localDirsOnWindows['Z:\\cern.ch\\sw\\contrib\\CMT\\v1r20p20070208'] = 'C:\\local\\releases\\CMT\\v1r20p20070208'
localDirsOnWindows['Z:\\cern.ch\\sw\\Gaudi\\releases'] = 'C:\\local\\Gaudi-releases'
localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\app\\releases\\LCGCMT'] = 'C:\\local\\LCGCMT'
localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\app\\nightlies\\scripts'] = 'C:\\local\\LCG-nightlies'

localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\external'] = 'C:\\local\\external;D:\\local\\lib\\lcg\\external'
localDirsOnWindows['Z:\\cern.ch\\lhcb\\scripts'] = 'C:\\local\\LHCb-scripts;D:\\local\\lib\\scripts'
localDirsOnWindows['Z:\\LOCALCOPY\\releases'] = 'C:\\local\\releases;D:\\local\\lib\\lhcb'


cmtCommand = 'cmt -disable_warnings'
getpackCommand = 'getpack --no-config --batch -p anonymous'
python = 'python '
