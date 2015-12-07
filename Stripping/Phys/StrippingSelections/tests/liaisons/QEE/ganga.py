#!/usr/bin/env python

import os

app = DaVinci( version = 'v38r0' )
app.user_release_area = os.path.expandvars('$HOME/cmtuser/')
# app.setupProjectOptions = '--no-user-area'
app.optsfile  = 'TestS21rXp1_settings.py'
app.extraopts = """
from Configurables import DaVinci
DaVinci().EvtMax = 2000
"""

## S24
# ds = LHCbDataset.new('../../data/Reco15a_Run164668.py', catalog=False)[0:100]
# ds.XMLCatalogueSlice = '../../data/Reco15a_Run164668.xml'

## S21rXp1
ds = app.readInputData('TestS21rXp1_settings.py')
ds.XMLCatalogueSlice = '../../data/pool_xml_catalog_Reco14_Run125113.xml'

j = Job()
j.name        = 'TestS21rXp1 Retention'
j.comment     = 'Deletable'
j.backend     = PBS(extraopts='--mem=2400 -t 1-0:0:0')
j.application = app
j.inputdata   = ds
j.splitter    = SplitByFiles( filesPerJob=1 )
j.outputfiles = [ MassStorageFile('*.root'), 'summary.xml' ]

j.submit()
