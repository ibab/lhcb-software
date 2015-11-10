#!/usr/bin/env python

app = DaVinci( version = 'v38r0' )
app.user_release_area = '~/cmtuser/DaVinciDev_v38r0'
app.optsfile  = 'TestS24.py'
app.extraopts = """
from Configurables import DaVinci
DaVinci().EvtMax = -1
"""

ds = LHCbDataset.new('../../data/Reco15a_Run164668.py', catalog=False)
ds.XMLCatalogueSlice = '../../data/Reco15a_Run164668.xml'

j = Job()
j.name        = 'Test S24'
j.backend     = LSF(extraopts='--mem=2400 -t 1-0:0:0')
j.application = app
j.inputdata   = ds
j.splitter    = SplitByFiles( filesPerJob=1 )
j.outputfiles = [ MassStorageFile('*.root'), 'summary.xml' ]

j.submit()
