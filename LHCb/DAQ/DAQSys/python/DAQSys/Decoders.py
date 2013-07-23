"""
Database of standard decoders, can be added to or manipulated in user code.

This database is normally eaten by the DAQSys configurables to organize which data will go where.
"""
from DAQSys.DecoderClass import decoder

decoder_db={}

#===========ODIN===========
decoder("createODIN",active=True,banks=["ODIN"],
        publicTools=["OdinTimeDecoder/ToolSvc.OdinTimeDecoder"],
        conf=decoder_db)

decoder("OdinTimeDecoder/ToolSvc.OdinTimeDecoder",active=False,
        privateTools=["ODINDecodeTool"],
        conf=decoder_db)#tool handle??

decoder("ODINDecodeTool",active=False,#tool handle??
        inputs={"RawEventLocations":["Trigger/RawEvent","DAQ/RawEvent"]},
        outputs={"ODINLocation":"DAQ/ODIN"},
        conf=decoder_db)

#===========VELO===========
decoder("DecodeVeloRawBuffer/createVeloClusters",
        active=True, banks=["Velo"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"VeloClusterLocation" : None},
        properties={"DecodeToVeloClusters": True,"DecodeToVeloLiteClusters":False},
        conf=decoder_db)

decoder("DecodeVeloRawBuffer/createVeloLiteClusters",
        active=True, banks=["Velo"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"VeloLiteClustersLocation" : None},
        properties={"DecodeToVeloClusters": False,"DecodeToVeloLiteClusters":True},
        conf=decoder_db)

#===========TT===========
decoder("RawBankToSTLiteClusterAlg/createTTLiteClusters",
        active=True, banks=["TT","TTPedestal","TTFull","TTProcFull","TTError"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        conf=decoder_db)
#outputs={"clusterLocation":None}, set logically in the code, resetting may not work...

decoder("RawBankToSTClusterAlg/createTTClusters",
        active=True, banks=["TT","TTPedestal","TTFull","TTProcFull","TTError"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        conf=decoder_db)
#outputs={"clusterLocation":None}, set logically in the code, resetting may not work...

#===========RICH===========
#from Chris Jones, get the configurable for the used public tool
from Configurables import RichTools
t=RichTools().smartIDTool()
#ensure it's a public tool...
tname=t.getFullName().replace("/","/ToolSvc.")

decoder("Rich::DAQ::RawBufferToRichDigitsAlg/RichRawEventToDigits",
        active=True, banks=['Rich'],
        outputs={"RichDigitsLocation":None},
        properties={"DecodeBufferOnly":False},
        publicTools=[tname],
        conf=decoder_db)

t2=RichTools().rawDecoder()
t2name=t2.getFullName()

decoder(tname, active=False,
        privateTools=[t2name],
        conf=decoder_db)

decoder(t2name, active=False,
        inputs={"RawEventLocations":["Rich/RawEvent","DAQ/RawEvent"]},
        conf=decoder_db)

#===========IT===========
decoder("RawBankToSTLiteClusterAlg/createITLiteClusters",
        active=True, banks=["IT","ITPedestal","ITFull","ITProcFull","ITError"],
        outputs=["Raw/IT/LiteClusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        properties={"DetType":"IT"},
        conf=decoder_db)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

decoder("RawBankToSTClusterAlg/createITClusters",
        active=True, banks=["IT","ITPedestal","ITFull","ITProcFull","ITError"],
        outputs=["Raw/IT/Clusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},#also set in the C++...
        properties={"DetType":"IT"},
        conf=decoder_db)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

#===========OT===========
decoder("OTTimeCreator",
        active=True,banks=["OT","OTFull","OTError"],
        privateTools=["OTRawBankDecoder/RawBankDecoder"],#tool handle??
        outputs={"OutputLocation": None},
        conf=decoder_db)

decoder("OTRawBankDecoder/RawBankDecoder",#tool handle??
        active=False,
        inputs={"RawEventLocations":["Other/RawEvent","DAQ/RawEvent"]},
        conf=decoder_db)

#===========SPD===========
name="SpdFromRaw" #as in C++
toolname="CaloTriggerBitsFromRaw/"+name+"Tool" #as in C++

decoder("CaloDigitsFromRaw/"+name,
        active=True,banks=["PrsE","PrsPacked"],
        privateTools=[toolname],
        outputs=["Raw/Spd/Digits","Raw/Spd/Adcs"],
        conf=decoder_db)
#outputs={"DigitsContainer" : "Raw/Spd/Digits", "AdcsContainer" : "Raw/Spd/Adcs"} #set logically in code, so overwriting here won't actually work

decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=decoder_db)

#===========PRS===========
name="PrsFromRaw" #as in C++
toolname="CaloEnergyFromRaw/"+name+"Tool" #as in C++

decoder("CaloDigitsFromRaw/"+name,
        active=True,banks=["PrsE","PrsPacked"],
        privateTools=[toolname],
        outputs=["Raw/Prs/Digits","Raw/Prs/Adcs"],
        conf=decoder_db)
#outputs={"DigitsContainer" : "Raw/Prs/Digits", "AdcsContainer" : "Raw/Prs/Adcs"} #set logically in code, so overwriting here won't actually work

decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=decoder_db)

#===========ECAL===========
name="EcalZSup"#as in C++
toolname="CaloEnergyFromRaw/"+name+"Tool" #as in C++

decoder("CaloZSupAlg/"+name, active=True,
        privateTools=[toolname],banks=['EcalE','EcalPacked','EcalPackedError'],
        outputs=["Raw/Ecal/Adcs","Raw/Ecal/Digits"],
        conf=decoder_db)
        #outputs={"OutputADCData": "Raw/Ecal/Adcs","OutputDigitData": "Raw/Ecal/Digits"}#set logically in code, so overwriting here won't actually work

decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=decoder_db)


#===========HCAL===========
name="HcalZSup"#as in C++
toolname="CaloEnergyFromRaw/"+name+"Tool" #as in C++

decoder("CaloZSupAlg/"+name, active=True,
        privateTools=[toolname],banks=['HcalE','HcalPacked','HcalPackedError'],
        outputs=["Raw/Hcal/Adcs","Raw/Hcal/Digits"],
        conf=decoder_db)
#outputs={"OutputADCData": "Raw/Hcal/Adcs","OutputDigitData": "Raw/Hcal/Digits"}#set logically in code, so overwriting here won't actually work

decoder(toolname,active=False,
        inputs={"RawEventLocations" : ["Calo/RawEvent","DAQ/RawEvent"]},
        conf=decoder_db)


#===========MUON===========
decoder("MuonRec",active=True,banks=["Muon"],
        outputs=["Raw/Muon/Coords"],
        inputs=["DAQ/RawEvent"],
        conf=decoder_db)#No way to configure input or output!!!

#TRIGGER ===========L0===========
decoder("L0MuonCandidatesFromRaw/L0MuonFromRaw",
        active=True, banks=["L0Muon","L0MuonProcCand"],
        privateTools=["L0MuonOutputs/OutputTool"],
        inputs={"RawEventLocations" : None},
        conf=decoder_db)

decoder("L0MuonOutputs/OutputTool",
        active=False, 
        outputs=["Trig/L0/MuonCtrl","Trig/L0/MuonBCSU","Trig/L0/MuonData"],
        conf=decoder_db)

decoder("L0CaloCandidatesFromRaw/L0CaloFromRaw",
        active=True, banks=["L0Calo","L0CaloFull"],
        inputs={"RawEventLocations" : None},
        outputs=["Trig/L0/Calo","Trig/L0/FullCalo"],
        conf=decoder_db)

decoder("L0DUFromRawAlg/L0DUFromRaw",
        active=True, banks=["L0DU"],
        privateTools=["L0DUFromRawTool"],
        outputs={"L0DUReportLocation": None, "ProcessorDataLocation": None},
        conf=decoder_db)

decoder("L0DUFromRawTool",
        active=False,
        inputs={"RawLocations" : ["Trigger/RawEvent","DAQ/RawEvent"]},
        conf=decoder_db)


#UPGRADE ===========VP===========
decoder("VPRawBankToLiteCluster/createVPLiteClusters",
        active=True, banks=["VP"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"ClusterLocation": "Raw/VP/LiteClusters"},
        conf=decoder_db)

decoder("VPRawBankToPartialCluster/createVPClusters",
        active=True, banks=["VP"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"ClusterLocation": "Raw/VP/Clusters"},
        conf=decoder_db)

#UPGRADE ===========VL===========
decoder("VLRawBankDecoder/createVLLiteClusters",
        active=True, banks=["VL"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"LiteClusterLocation": None},
        properties={"DecodeToLiteClusters":True,"DecodeToClusters":False},
        conf=decoder_db)

decoder("VLRawBankDecoder/createVLClusters",
        active=True, banks=["VL"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs={"ClusterLocation": None},
        properties={"DecodeToLiteClusters":False,"DecodeToClusters":True},
        conf=decoder_db)



#UPGRADE ===========UT===========
decoder("RawBankToSTLiteClusterAlg/createUTLiteClusters",
        active=True, banks=["UT","UTPedestal","UTFull","UTError"],
        outputs=["Raw/UT/LiteClusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        publicTools=["STOfflinePosition/ToolSvc.UTClusterPosition"],
        properties={"DetType":"UT"},
        conf=decoder_db)
#outputs={"clusterLocation":"Raw/UT/LiteClusters"}, set logically in the code, resetting may not work...

decoder("RawBankToSTClusterAlg/createUTClusters",
        active=True, banks=["UT","UTPedestal","UTFull","UTError"],
        outputs=["Raw/UT/Clusters"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        publicTools=["STOfflinePosition/ToolSvc.UTClusterPosition"],
        properties={"DetType":"UT"},
        conf=decoder_db)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

decoder("STOfflinePosition/ToolSvc.UTClusterPosition",
        active=False,
        properties={"DetType":"UT"},
        conf=decoder_db)

#UPGRADE ===========FT===========
decoder("FTRawBankDecoder/createFTClusters",
        active=True, banks=["FTCluster"],
        inputs={"RawEventLocations" : ["Other/RawEvent","DAQ/RawEvent"]},
        outputs=["Raw/FT/RawClusters"],
        conf=decoder_db)
#no way to steer the output!!
