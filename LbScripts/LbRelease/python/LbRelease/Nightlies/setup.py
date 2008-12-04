"""
LHCb Nightlies settings file.
"""
import os

listOfProjectNames = ['LHCb', 'Lbcom', 'Rec', 'Analysis', 'Phys', 'Hlt', 'Online', 'Gauss', 'Boole', 'Brunel', 'DaVinci', 'Panoramix', 'Bender', 'Moore', 'Geant4', 'Panoptes']
cmtCommand = 'cmt -disable_warnings'
#os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = '/afs/cern.ch/user/k/kkruzele/workspace/nightlies-devel-sandbox'
os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = os.path.join('/afs', 'cern.ch', 'sw', 'lcg', 'app', 'nightlies', 'scripts')
os.environ['LCG_XMLCONFIGDIR'] = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf')
configurationHistoryPath = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf', 'history')
getpackCommand = 'python /afs/cern.ch/lhcb/scripts/python/getpack.py'
nightliesAFSPath = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies')

pathLbLogin = '/afs/cern.ch/lhcb/software/releases/LBSCRIPTS/prod/InstallArea/scripts/LbLogin.sh'
cmtVersion = None # == default
#cmtVersion = 'v1r20p20080222'
#cmtVersion = 'v1r20p20070208'

pathCmtSetup = None
#pathCmtSetup = '/afs/cern.ch/sw/contrib/CMT/v1r20p20080222/mgr/setup.sh' #use: --cmtvers== instead
#pathCmtSetup = '/afs/cern.ch/sw/contrib/CMT/v1r20p20070208/mgr/setup.sh'
