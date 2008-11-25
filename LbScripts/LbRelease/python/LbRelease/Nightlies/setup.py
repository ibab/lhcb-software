"""
LHCb Nightlies settings file.
"""
import os

listOfProjectNames = ['LHCb', 'Lbcom', 'Rec', 'Analysis', 'Phys', 'Hlt', 'Online', 'Gauss', 'Boole', 'Brunel', 'DaVinci', 'Panoramix', 'Bender', 'Moore', 'Geant4']
cmtCommand = 'cmt -disable_warnings'
#os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = '/afs/cern.ch/user/k/kkruzele/workspace/nightlies-devel-sandbox'
os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = os.path.join('/afs', 'cern.ch', 'sw', 'lcg', 'app', 'nightlies', 'scripts')
os.environ['LCG_XMLCONFIGDIR'] = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf')
configurationHistoryPath = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf', 'history')
getpackCommand = 'python /afs/cern.ch/lhcb/scripts/python/getpack.py'
nightliesAFSPath = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies')

pathLbLogin = '/afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v1r10/InstallArea/scripts/LbLogin.sh'
#cmtVersion = 'v1r20p20080222' # or None if default
cmtVersion = 'v1r20p20070208' # or None if default
#pathCmtSetup = '/afs/cern.ch/sw/contrib/CMT/v1r20p20080222/mgr/setup.sh' #because --cmtvers== seems not to work
pathCmtSetup = '/afs/cern.ch/sw/contrib/CMT/v1r20p20070208/mgr/setup.sh'