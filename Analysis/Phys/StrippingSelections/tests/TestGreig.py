#! /usr/bin/env gaudirun.py

from Gaudi.Configuration import *
from Configurables import DaVinci

##################
# Stripping Code #
##################
importOptions("$STRIPPINGSELECTIONSROOT/options/Stripping.py")
importOptions("$STRIPPINGSELECTIONSROOT/options/StrippingSelections.py")

##########################
# Set up the application #
##########################
DaVinci().EvtMax = 100
DaVinci().DataType = 'DC06'
DaVinci().Simulation = True
DaVinci().HltType = 'NONE'
DaVinci().TupleFile = 'ntuples.root'
DaVinci().HistogramFile = 'histos.root'
DaVinci().ETCFile = 'DVetc.root'
DaVinci().ReplaceL0BanksWithEmulated = True # enable to rerun L0

##############
# Input Data #
##############
data = '/afs/cern.ch/user/g/gcowan/cmtuser/options/data/'
importOptions( data + 'dc06_jpsiPhi_data_phys-v4-lumi2_PFNs_subset.py')

