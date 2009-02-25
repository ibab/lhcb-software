"""
LHCb Nightlies settings file.
"""
import os

listOfProjectNames = ['Gaudi', 'LHCb', 'Lbcom', 'Rec', 'Analysis', 'Phys', 'Hlt', 'Online', 'Gauss', 'Boole', 'Brunel', 'DaVinci', 'Panoramix', 'Bender', 'Moore', 'Geant4', 'Panoptes', 'Alignment', 'Dirac']
projectNamesDict = {}
for p in listOfProjectNames:
    projectNamesDict[p.upper()] = p

#########################################################
# DO NOT USE os.path.join() with os.environ['AFSROOT']. #
# It fails on Windows with 'Z:' as a first argument.    #
# Use os.sep.join([]) instead.                          #
#########################################################

os.environ['LCG_NIGHTLIES_SCRIPTDIR'] = os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'sw', 'lcg', 'app', 'nightlies', 'scripts'])
configurationPath = os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf'])
configurationHistoryPath = os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf', 'history'])

cmtCommand = 'cmt -disable_warnings'
getpackCommand = 'getpack --batch -p anonymous'
python = 'python '
