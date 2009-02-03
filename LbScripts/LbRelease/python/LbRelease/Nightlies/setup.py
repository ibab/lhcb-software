"""
LHCb Nightlies settings file.
"""
import os

listOfProjectNames = ['Gaudi', 'LHCb', 'Lbcom', 'Rec', 'Analysis', 'Phys', 'Hlt', 'Online', 'Gauss', 'Boole', 'Brunel', 'DaVinci', 'Panoramix', 'Bender', 'Moore', 'Geant4', 'Panoptes', 'Alignment']
projectNamesDict = {}
for p in listOfProjectNames:
    projectNamesDict[p.upper()] = p

os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = os.path.join('/afs', 'cern.ch', 'sw', 'lcg', 'app', 'nightlies', 'scripts')
configurationPath = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf')
configurationHistoryPath = os.path.join('/afs', 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf', 'history')

cmtCommand = 'cmt -disable_warnings'
getpackCommand = 'getpack --batch -p anonymous'
python = 'python '
