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