#!/usr/bin/env python

import os

# app = DaVinci( version = 'v39r0p1' ) # S26, no longer compat with Ganga 6.0.44
app = DaVinci( version = 'v38r1p1' ) # need local redirection
app.user_release_area = os.path.expandvars('$HOME/cmtuser/')
# app.setupProjectOptions = '--no-user-area'
app.optsfile  = 'TestS26_Selections.py'
app.extraopts = """
from Configurables import DaVinci
DaVinci().EvtMax = 10000
"""

## S24
# ds = LHCbDataset.new('../../data/Reco15a_Run164668.py', catalog=False)[0:100]
# ds.XMLCatalogueSlice = '../../data/Reco15a_Run164668.xml'

## S21rXp1
# ds = app.readInputData('TestS21rXp1_settings.py')
# ds.XMLCatalogueSlice = '../../data/pool_xml_catalog_Reco14_Run125113.xml'

## S26
ds = app.readInputData('../../data/Reco15a_Run164668.py')
ds.XMLCatalogueSlice = '../../data/Reco15a_Run164668.xml'

j = Job()
j.name        = 'TestS26'
j.comment     = 'Deletable: Test DY2MuMu2LineHltPrescale 0.5'
j.backend     = PBS(extraopts='--mem=2416 -t 1-0:0:0')
# j.backend     = Dirac()
j.application = app
j.inputdata   = ds
j.splitter    = SplitByFiles( filesPerJob=1 )
j.outputfiles = [ 'summary.xml' ] #, LocalFile('*.dst') ]

queues.add(j.submit)
