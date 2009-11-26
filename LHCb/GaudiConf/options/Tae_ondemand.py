from Gaudi.Configuration import *
#for triggered slot t0
importOptions('$STDOPTS/DecodeRawEvent.py')

#for the rest (except RICH and L0)
bunch = ["Prev3","Next3","Prev2","Next2","Prev1","Next1"]
dondem = DataOnDemandSvc()

# calos 
from Configurables import CaloDigitsFromRaw,CaloZSupAlg
calo_conf = {}
for d in ["Spd","Prs","Ecal","Hcal"] :
 for b in bunch : 
  if d=="Spd" or d=="Prs" : 
   calo_conf[d+b] = CaloDigitsFromRaw(d+'FromRaw'+b)
  else :
   calo_conf[d+b] = CaloZSupAlg(d+'ZSup'+b)   
  calo_conf[d+b].RootInTES = b
  dondem.AlgMap['/Event/'+b+'/Raw/'+d+'/Digits']=calo_conf[d+b].getFullName()
  
# ot 
from Configurables import OTTimeCreator,OTRawBankDecoder
ot_conf = {}
for b in bunch : 
 ot_conf['oTTimeCreator'+b] = OTTimeCreator('OTTimeCreator'+b)
 ot_conf['oTTimeCreator'+b].OutputLocation = b+'/Raw/OT/Times'
 ot_conf['oTTimeCreator'+b].addTool(OTRawBankDecoder, name='OTRawBankDecoder')
 ot_conf['oTTimeCreator'+b].OTRawBankDecoder.RawEventLocation = b+'/DAQ/RawEvent'
 dondem.AlgMap['/Event/'+b+'/Raw/OT/Times'] = ot_conf['oTTimeCreator'+b].getFullName()
 
from Configurables import MuonRec, MuonRawBuffer
muon_conf = {}
for b in bunch : 
 muon_conf['MuonRec'+b] = MuonRec('MuonRec'+b)
 muon_conf['MuonRec'+b].RootInTES = b
 muon_conf['MuonRec'+b].Context = 'TAE'
 dondem.AlgMap['/Event/'+b+'/Raw/Muon/Coords'] = muon_conf['MuonRec'+b].getFullName()

from Configurables import DecodeVeloRawBuffer
velo_conf = {}
for b in bunch : 
 velo_conf['createVeloClusters'+b] = DecodeVeloRawBuffer('createVeloClusters'+b)
 velo_conf['createVeloClusters'+b].ForceBankVersion=3
 velo_conf['createVeloClusters'+b].RawEventLocation=b+'/DAQ/RawEvent'
 velo_conf['createVeloClusters'+b].VeloClusterLocation=b+'/Raw/Velo/Clusters'
 velo_conf['createVeloClusters'+b].DecodeToVeloClusters = True
 velo_conf['createVeloClusters'+b].DecodeToVeloLiteClusters = False
 velo_conf['createVeloLiteClusters'+b] = DecodeVeloRawBuffer('createVeloLiteClusters'+b)
 velo_conf['createVeloLiteClusters'+b].ForceBankVersion=3
 velo_conf['createVeloLiteClusters'+b].RawEventLocation=b+'/DAQ/RawEvent'
 velo_conf['createVeloLiteClusters'+b].VeloLiteClustersLocation=b+'/Raw/Velo/LiteClusters'
 dondem.AlgMap['/Event/'+b+'/Raw/Velo/LiteClusters'] = velo_conf['createVeloLiteClusters'+b].getFullName()
 dondem.AlgMap['/Event/'+b+'/Raw/Velo/Clusters']     = velo_conf['createVeloClusters'+b].getFullName()
 
from Configurables import RawBankToSTClusterAlg,RawBankToSTLiteClusterAlg
st_conf = {}
for d in ['IT','TT']: 
 for b in bunch : 
  st_conf['create'+d+'Clusters'+b] = RawBankToSTClusterAlg('Create'+d+'Clusters'+b)
  st_conf['create'+d+'Clusters'+b].rawEventLocation = b+'/DAQ/RawEvent'
  st_conf['create'+d+'Clusters'+b].clusterLocation = b+'/Raw/'+d+'/Clusters'
  st_conf['create'+d+'Clusters'+b].summaryLocation = b+'/Rec/'+d+'/Summary'
  st_conf['create'+d+'Clusters'+b].detType  = d
  st_conf['create'+d+'LiteClusters'+b] = RawBankToSTLiteClusterAlg('Create'+d+'LiteClusters'+b)
  st_conf['create'+d+'LiteClusters'+b].rawEventLocation = b+'/DAQ/RawEvent'
  st_conf['create'+d+'LiteClusters'+b].clusterLocation = b+'/Raw/'+d+'/LiteClusters'
  st_conf['create'+d+'LiteClusters'+b].detType  = d
  dondem.AlgMap['/Event/'+b+'/Raw/'+d+'/Clusters']       = st_conf['create'+d+'Clusters'+b].getFullName()
  dondem.AlgMap['/Event/'+b+'/Raw/'+d+'/LiteClusters']   = st_conf['create'+d+'LiteClusters'+b].getFullName()



# to trigger decoding on demand:
#containers = ['Raw/Ecal/Digits','Raw/Hcal/Digits','Raw/Prs/Digits','Raw/Spd/Digits'
#,'Raw/IT/Clusters','Raw/IT/LiteClusters','Raw/Muon/Coords','Raw/OT/Times'
#,'Raw/Rich/Digits','Raw/TT/Clusters','Raw/TT/LiteClusters'
#,'Raw/Velo/Clusters','Raw/Velo/LiteClusters'
#,'Trig/L0/L0DUReport','Trig/L0/MuonData','Trig/L0/Calo']


# check also for tae slots
#for b in ["Prev3/","Next3/","Prev2/","Next2/","Prev1/","Next1/",""]: 
#  if evt[b.replace('/','')]!=None : 
#    for c in containers:
#      sc = evt[b+c]

