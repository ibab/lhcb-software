"""
Database of standard decoders, can be added to or manipulated in user code.

This database is normally eaten by the DAQSys configurables to organize which data will go where.
"""
from DAQSys.DecoderClass import Decoder

DecoderDB={}

#===========ODIN===========
Decoder("createODIN",active=True,banks=["ODIN"],
        publicTools=["OdinTimeDecoder/ToolSvc.OdinTimeDecoder"],
        conf=DecoderDB)

Decoder("OdinTimeDecoder/ToolSvc.OdinTimeDecoder",active=False,
        privateTools=["ODINDecodeTool"],
        conf=DecoderDB)

Decoder("ODINDecodeTool",active=False,#tool handle??
        inputs={"RawEventLocations":["Trigger/RawEvent","DAQ/RawEvent"]},
        outputs={"ODINLocation":"DAQ/ODIN"},
        conf=DecoderDB)

#===========VELO===========

#first create a decoder which can decode either location.
#This is not active by default, but is turned on later
# by RecoTracking.py when rewuired for Brunel, alignment, etc.
vd=Decoder("DecodeVeloRawBuffer/createBothVeloClusters",
        active=False, banks=["Velo"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"VeloClusterLocation" : None, "VeloLiteClustersLocation" : None},
        properties={"DecodeToVeloClusters": True,"DecodeToVeloLiteClusters":True},
        conf=DecoderDB)

#now clone into two algs which can decode these things separately
vdClus=vd.clone("DecodeVeloRawBuffer/createVeloClusters")
vdClus.Active=True
#delete the other unused output location...
del vdClus.Outputs["VeloLiteClustersLocation"]
vdClus.Properties["DecodeToVeloClusters"]=True
vdClus.Properties["DecodeToVeloLiteClusters"]=False

vdLite=vd.clone("DecodeVeloRawBuffer/createVeloLiteClusters")
vdLite.Active=True
#delete the other unused output location...
del vdLite.Outputs["VeloClusterLocation"]
vdLite.Properties["DecodeToVeloClusters"]=False
vdLite.Properties["DecodeToVeloLiteClusters"]=True


#===========PU==========

#actually this is almost two different decoders in one, steered with a property
#unfortunately the location of these two banks are different in S20, so for now I can only declare the L0PU decoder... needs thought!
Decoder("DecodePileUpData",
        active=True, banks=["L0PU"],#,"L0PUFull"],
        inputs={"RawEventLocations" : ["Trigger/RawEvent","Other/RawEvent","DAQ/RawEvent"]},#different from the default!
        outputs={"PUClusterLocation" : None, "PUClusterNZSLocation": None},
        #properties={"NonZeroSupp": False},
        conf=DecoderDB)


#===========TT===========
Decoder("RawBankToSTLiteClusterAlg/createTTLiteClusters",
        active=True, banks=["TT","TTPedestal","TTFull","TTProcFull","TTError"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":None}, set logically in the code, resetting may not work...

Decoder("RawBankToSTClusterAlg/createTTClusters",
        active=True, banks=["TT","TTPedestal","TTFull","TTProcFull","TTError"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":None}, set logically in the code, resetting may not work...

#===========RICH===========
#from Chris Jones, get the configurable for the used public tool
from Configurables import RichTools
t=RichTools().smartIDTool()
#ensure it's a public tool...
tname=t.getFullName().replace("/","/ToolSvc.")

Decoder("Rich::DAQ::RawBufferToRichDigitsAlg/RichRawEventToDigits",
        active=True, banks=['Rich'],
        outputs={"RichDigitsLocation":None},
        properties={"DecodeBufferOnly":False},
        publicTools=[tname],
        conf=DecoderDB)

t2=RichTools().rawDecoder()
t2name=t2.getFullName()

Decoder(tname, active=False,
        privateTools=[t2name],
        conf=DecoderDB)

Decoder(t2name, active=False,
        inputs={"RawEventLocations":["Rich/RawEvent","DAQ/RawEvent"]},
        conf=DecoderDB)

#===========IT===========
Decoder("RawBankToSTLiteClusterAlg/createITLiteClusters",
        active=True, banks=["IT","ITPedestal","ITFull","ITProcFull","ITError"],
        outputs=["Raw/IT/LiteClusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        properties={"DetType":"IT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

Decoder("RawBankToSTClusterAlg/createITClusters",
        active=True, banks=["IT","ITPedestal","ITFull","ITProcFull","ITError"],
        outputs=["Raw/IT/Clusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        properties={"DetType":"IT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

#===========OT===========
ott=Decoder("OTTimeCreator", #the only one which makes a TES location
        active=True,banks=["OT","OTRaw","OTError"],
        #privateTools=["OTRawBankDecoder/RawBankDecoder"],#tool handle??
        #I hope the tool handle actually calls *this* public tool...
        publicTools=["OTRawBankDecoder/ToolSvc.OTRawBankDecoder"],
        outputs={"OutputLocation": None},
        conf=DecoderDB)

from GaudiKernel.SystemOfUnits import ns
rbd=Decoder(ott.PublicTools[0],#tool handle??
        banks=ott.Banks,
        active=False,
        inputs={"RawEventLocations":["Other/RawEvent","DAQ/RawEvent"]},
        properties={"TimeWindow":(-8.0*ns, 56.0*ns)},
        conf=DecoderDB)

#prbd=rbd.clone("OTRawBankDecoder/ToolSvc.OTRawBankDecoder")
# copy into PublicTool, actually it's the same instance used in OTTimeCreator

#===========SPD===========
name="SpdFromRaw" #as in C++
toolname="CaloTriggerBitsFromRaw/"+name+"Tool" #as in C++

Decoder("CaloDigitsFromRaw/"+name,
        active=True,banks=["PrsE","PrsPacked"],
        privateTools=[toolname],
        outputs=["Raw/Spd/Digits","Raw/Spd/Adcs"],
        conf=DecoderDB)
#outputs={"DigitsContainer" : "Raw/Spd/Digits", "AdcsContainer" : "Raw/Spd/Adcs"} #set logically in code, so overwriting here won't actually work

Decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=DecoderDB)

#===========PRS===========
name="PrsFromRaw" #as in C++
toolname="CaloEnergyFromRaw/"+name+"Tool" #as in C++

Decoder("CaloDigitsFromRaw/"+name,
        active=True,banks=["PrsE","PrsPacked"],
        privateTools=[toolname],
        outputs=["Raw/Prs/Digits","Raw/Prs/Adcs"],
        conf=DecoderDB)
#outputs={"DigitsContainer" : "Raw/Prs/Digits", "AdcsContainer" : "Raw/Prs/Adcs"} #set logically in code, so overwriting here won't actually work

Decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=DecoderDB)

#===========ECAL===========
name="EcalZSup"#as in C++
toolname="CaloEnergyFromRaw/"+name+"Tool" #as in C++

Decoder("CaloZSupAlg/"+name, active=True,
        privateTools=[toolname],banks=['EcalE','EcalPacked','EcalPackedError'],
        outputs=["Raw/Ecal/Adcs","Raw/Ecal/Digits"],
        conf=DecoderDB)
        #outputs={"OutputADCData": "Raw/Ecal/Adcs","OutputDigitData": "Raw/Ecal/Digits"}#set logically in code, so overwriting here won't actually work

Decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=DecoderDB)


#===========HCAL===========
name="HcalZSup"#as in C++
toolname="CaloEnergyFromRaw/"+name+"Tool" #as in C++

Decoder("CaloZSupAlg/"+name, active=True,
        privateTools=[toolname],banks=['HcalE','HcalPacked','HcalPackedError'],
        outputs=["Raw/Hcal/Adcs","Raw/Hcal/Digits"],
        conf=DecoderDB)
#outputs={"OutputADCData": "Raw/Hcal/Adcs","OutputDigitData": "Raw/Hcal/Digits"}#set logically in code, so overwriting here won't actually work

Decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=DecoderDB)


#===========MUON===========
Decoder("MuonRec",active=True,banks=["Muon"],
        outputs=["Raw/Muon/Coords"],
        inputs=["DAQ/RawEvent"],
        conf=DecoderDB)#No way to configure input or output!!!

#TRIGGER ===========L0===========
Decoder("L0MuonCandidatesFromRaw/L0MuonFromRaw",
        active=True, banks=["L0Muon","L0MuonProcCand"],
        privateTools=["L0MuonOutputs/OutputTool"],
        inputs={"RawEventLocations" : None},
        conf=DecoderDB)

Decoder("L0MuonOutputs/OutputTool",
        active=False, 
        outputs=["Trig/L0/MuonCtrl","Trig/L0/MuonBCSU","Trig/L0/MuonData"],
        conf=DecoderDB)

Decoder("L0CaloCandidatesFromRaw/L0CaloFromRaw",
        active=True, banks=["L0Calo","L0CaloFull"],
        inputs={"RawEventLocations" : None},
        outputs=["Trig/L0/Calo","Trig/L0/FullCalo"],
        conf=DecoderDB)

Decoder("L0DUFromRawAlg/L0DUFromRaw",
        active=True, banks=["L0DU"],
        privateTools=["L0DUFromRawTool"],
        inputs={"RawEventLocations" : None},
        outputs={"L0DUReportLocation": None, "ProcessorDataLocation": None},
        conf=DecoderDB)

Decoder("L0DUFromRawTool",
        active=False,
        inputs={"RawLocations" : ["Trigger/RawEvent","DAQ/RawEvent"]},
        conf=DecoderDB)

#TRIGGER ==========HLT===========

dec=Decoder("HltSelReportsDecoder",
        active=True, banks=["HltSelReports"],
        inputs=["Trigger/RawEvent","DAQ/RawEvent"],
        #currently cannot be configured, it's a list not present in constructor
        outputs={"OutputHltSelReportsLocation":None},
        conf=DecoderDB)

#split Hlt1/2 scenario
dec2=dec.clone(dec.FullName+"/Hlt2SelReportsDecoder")
dec2.overrideOutputs({"OutputHltSelReportsLocation":"Hlt2/SelReports"})

dec=Decoder("HltDecReportsDecoder",
        active=True, banks=["HltDecReports"],
        inputs=["Trigger/RawEvent","DAQ/RawEvent"],
        #currently cannot be configured, it's a list not present in constructor
        outputs={"OutputHltDecReportsLocation":None},
        conf=DecoderDB)

#split Hlt1/2 scenario
dec1=dec.clone(dec.FullName+"/Hlt1DecReportsDecoder")
dec1.overrideOutputs({"OutputHltDecReportsLocation":"Hlt1/DecReports"})

dec2=dec.clone(dec.FullName+"/Hlt2DecReportsDecoder")
dec2.overrideOutputs({"OutputHltDecReportsLocation":"Hlt2/DecReports"})

Decoder("HltVertexReportsDecoder",
        active=True, banks=["HltVertexReports"],
        inputs=["Trigger/RawEvent","DAQ/RawEvent"],
        #currently cannot be configured properly, it's a list not present in constructor
        outputs={"OutputHltVertexReportsLocation":None},
        conf=DecoderDB)

#is a Routing bits filter really a decoder? it doesn't create output...
Decoder("HltRoutingBitsFilter",
        active=False, banks=["HltRoutingBits"],
        inputs=["Trigger/RawEvent","DAQ/RawEvent"],
        #currently cannot be configured properly, it's a list not present in constructor
        conf=DecoderDB)

Decoder("HltLumiSummaryDecoder",
        active=True, banks=["HltLumiSummary"],
        inputs={"RawEventLocation":None},
        outputs={"OutputContainerName":None},
        conf=DecoderDB)

#UPGRADE ===========VP===========
Decoder("VPRawBankToLiteCluster/createVPLiteClusters",
        active=True, banks=["VP"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"ClusterLocation": "Raw/VP/LiteClusters"},
        conf=DecoderDB)

Decoder("VPRawBankToPartialCluster/createVPClusters",
        active=True, banks=["VP"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"ClusterLocation": "Raw/VP/Clusters"},
        conf=DecoderDB)

#UPGRADE ===========VL===========
Decoder("VLRawBankDecoder/createVLLiteClusters",
        active=True, banks=["VL"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"LiteClusterLocation": None},
        properties={"DecodeToLiteClusters":True,"DecodeToClusters":False},
        conf=DecoderDB)

Decoder("VLRawBankDecoder/createVLClusters",
        active=True, banks=["VL"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"ClusterLocation": None},
        properties={"DecodeToLiteClusters":False,"DecodeToClusters":True},
        conf=DecoderDB)



#UPGRADE ===========UT===========
Decoder("RawBankToSTLiteClusterAlg/createUTLiteClusters",
        active=True, banks=["UT","UTPedestal","UTFull","UTError"],
        outputs=["Raw/UT/LiteClusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        #publicTools=["STOfflinePosition/ToolSvc.UTClusterPosition"],
        properties={"DetType":"UT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/UT/LiteClusters"}, set logically in the code, resetting may not work...
#"STOfflinePosition/ToolSvc.UTClusterPosition" is not part of decoding.

Decoder("RawBankToSTClusterAlg/createUTClusters",
        active=True, banks=["UT","UTPedestal","UTFull","UTError"],
        outputs=["Raw/UT/Clusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        #publicTools=["STOfflinePosition/ToolSvc.UTClusterPosition"],
        properties={"DetType":"UT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...
#"STOfflinePosition/ToolSvc.UTClusterPosition" is not part of decoding.

#Decoder("STOfflinePosition/ToolSvc.UTClusterPosition",
#        active=False,
#        properties={"DetType":"UT"},
#        conf=DecoderDB)
#"STOfflinePosition/ToolSvc.UTClusterPosition" is not part of decoding.


#UPGRADE ===========FT===========
Decoder("FTRawBankDecoder/createFTClusters",
        active=True, banks=["FTCluster"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs=["Raw/FT/RawClusters"],
        conf=DecoderDB)
#no way to steer the output!!
