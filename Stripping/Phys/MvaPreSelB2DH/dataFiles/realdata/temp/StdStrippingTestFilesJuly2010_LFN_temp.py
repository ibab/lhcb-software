#-- GAUDI jobOptions generated on Tue Jun 15 17:55:13 2010
#-- Contains event types : 
#--  for stripping selection tests

#from Gaudi.Configuration import * 

#EventSelector().Input   = [
ds=LHCbDataset(files=[
'LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002181_1.sdst',
'LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002182_1.sdst',
'LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002183_1.sdst',
'LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002199_1.sdst',
'LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002294_1.sdst',
'LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002308_1.sdst',
'LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002347_1.sdst'
])
export(ds,'StdStrippingTest.ganga')

