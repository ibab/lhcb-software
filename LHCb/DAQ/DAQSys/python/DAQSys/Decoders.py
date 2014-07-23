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
        inputs={"RawEventLocations":None},
        outputs={"ODINLocation":None},
        conf=DecoderDB)

#===========VELO===========

#first create a decoder which can decode either location.
#This is not active by default, but is turned on later
# by RecoTracking.py when rewuired for Brunel, alignment, etc.
vd=Decoder("DecodeVeloRawBuffer/createBothVeloClusters",
        active=False, banks=["Velo"],
        inputs={"RawEventLocations" : None},
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
        inputs={"RawEventLocations" : None},
        outputs={"PUClusterLocation" : None, "PUClusterNZSLocation": None},
        #properties={"NonZeroSupp": False},
        conf=DecoderDB)


#===========TT===========
Decoder("RawBankToSTLiteClusterAlg/createTTLiteClusters",
        active=True, banks=["TT"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : None},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":None}, set logically in the code, resetting may not work...

Decoder("RawBankToSTLiteClusterAlg/createTTLiteClustersExpert",
        active=False, banks=["TTPedestal","TTFull","TTProcFull","TTError"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : None},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":None}, set logically in the code, resetting may not work...

Decoder("RawBankToSTClusterAlg/createTTClusters",
        active=True, banks=["TT"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : None},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":None}, set logically in the code, resetting may not work...

Decoder("RawBankToSTClusterAlg/createTTClustersExpert",
        active=False, banks=["TTPedestal","TTFull","TTProcFull","TTError"],
        outputs={"clusterLocation":None},
        inputs={"RawEventLocations" : None},
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
        inputs={"RawEventLocations":None},
        conf=DecoderDB)

#===========IT===========
Decoder("RawBankToSTLiteClusterAlg/createITLiteClusters",
        active=True, banks=["IT"],
        outputs=["Raw/IT/LiteClusters"],
        inputs={"RawEventLocations" : None},
        properties={"DetType":"IT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

Decoder("RawBankToSTLiteClusterAlg/createITLiteClustersExpert",
        active=False, banks=["ITPedestal","ITFull","ITProcFull","ITError"],
        outputs=["Raw/IT/LiteClusters"],
        inputs={"RawEventLocations" : None},
        properties={"DetType":"IT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

Decoder("RawBankToSTClusterAlg/createITClusters",
        active=True, banks=["IT"],
        outputs=["Raw/IT/Clusters"],
        inputs={"RawEventLocations" : None},
        properties={"DetType":"IT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

Decoder("RawBankToSTClusterAlg/createITClustersExpert",
        active=False, banks=["ITPedestal","ITFull","ITProcFull","ITError"],
        outputs=["Raw/IT/Clusters"],
        inputs={"RawEventLocations" : None},
        properties={"DetType":"IT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...

#===========OT===========
ott=Decoder("OTTimeCreator", #the only one which makes a TES location
        active=True,banks=["OT"],
        #privateTools=["OTRawBankDecoder/RawBankDecoder"],#tool handle??
        #I hope the tool handle actually calls *this* public tool...
        publicTools=["OTRawBankDecoder/ToolSvc.OTRawBankDecoder"],
        outputs={"OutputLocation": None},
        conf=DecoderDB)

ott=Decoder("OTTimeCreator/OTTimeCreatorExpert", #the only one which makes a TES location
        active=False,banks=["OTRaw", "OTError"],
        #privateTools=["OTRawBankDecoder/RawBankDecoder"],#tool handle??
        #I hope the tool handle actually calls *this* public tool...
        publicTools=["OTRawBankDecoder/ToolSvc.OTRawBankDecoder"],
        outputs={"OutputLocation": None},
        conf=DecoderDB)

from GaudiKernel.SystemOfUnits import ns
rbd=Decoder(ott.PublicTools[0],#tool handle??
        banks=ott.Banks,
        active=False,
        inputs={"RawEventLocations":None},
        properties={"TimeWindow":(-8.0*ns, 56.0*ns)},
        conf=DecoderDB)


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
        inputs={"RawEventLocations" : None},
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
        inputs={"RawEventLocations" : None},
        conf=DecoderDB)

#===========ECAL and HCAL===========

#ECAL and HCAL use the same algorithms, where the name of the alg
#decides which detector is decoded!
#Two versions are needed, one with zero suppression for Brunel and up, one without zero suppression for Boole

dec_calo_zs=[]
dec_calo_nzs=[]

for cal in ["Ecal","Hcal"]:
    #first       Zero suppressed (Brunel),      Non-zero suppressed (Boole)
    for algs in [("CaloZSupAlg","ZSup",True),("CaloDigitsFromRaw","FromRaw",False)]:
        alg,nametag,zs=algs
        # \/ e.g. EcalZSup (Brunel) or EcalFromRaw (Boole)
        name=cal+nametag
        algname=alg+"/"+name
        #push into a list for my helper function
        if zs:
            dec_calo_zs.append(algname)
        else:
            dec_calo_nzs.append(algname)
        toolname="CaloEnergyFromRaw/"+name+"Tool" #as in C++
        # Zero suppressed is "active", NZS is "not active", only activated in Boole
        Decoder(algname, active=zs,
                privateTools=[toolname],banks=[cal+'E',cal+'Packed'],
                outputs=["Raw/"+cal+"/Adcs","Raw/"+cal+"/Digits"],
                conf=DecoderDB)
        #outputs={"OutputADCData": "Raw/Ecal/Adcs","OutputDigitData": "Raw/Ecal/Digits"}#set logically in code, so overwriting here won't actually work
        
        Decoder(algname+"Expert", active=False,
                privateTools=[toolname],banks=[cal+'PackedError'],
                outputs=["Raw/"+cal+"/Adcs","Raw/"+cal+"/Digits"],
                conf=DecoderDB)
        #outputs={"OutputADCData": "Raw/Ecal/Adcs","OutputDigitData": "Raw/Ecal/Digits"}#set logically in code, so overwriting here won't actually work
        
        Decoder(toolname,active=False,
                inputs={"RawEventLocations" : None},
                conf=DecoderDB)

def caloSetZeroSuppressed(ddb, zerosuppression=True,dec_calo_zs=dec_calo_zs,dec_calo_nzs=dec_calo_nzs):
    """
    helper function to switch between zero-suppressed and NSZ-banks
    """
    for calos,zs in [(dec_calo_zs, True), (dec_calo_nzs, False)]:
        for adec in calos:
            ddb[adec].Active=(zerosuppression==zs)


#===========MUON===========
tname="MuonRawBuffer"

Decoder("MuonRec",active=True,banks=["Muon"],
        outputs={"OutputLocation": None},
        privateTools=[tname], #used in TAE mode
        publicTools=[tname+"/ToolSvc."+tname], #used in TAE mode
        conf=DecoderDB)

tool=Decoder("MuonRawBuffer",active=False,
             inputs={"RawEventLocations":None},
             conf=DecoderDB)

tool2=tool.clone(tname+"/ToolSvc."+tname)


#TRIGGER ===========L0===========
Decoder("L0MuonCandidatesFromRaw/L0MuonFromRaw",
        active=True, banks=["L0Muon","L0MuonProcCand"],
        privateTools=["L0MuonOutputs/OutputTool"],
        required=["createODIN"], #needed in TAE mode
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

#this is new, really I should let the banks be determined from the sum of lower banks so that I could in the future split up the calo banks
Decoder("L0DUFromRawTool",
        active=False,
#        banks=['EcalE','EcalTrig','EcalPacked','EcalPackedError',
#               'HcalE','HcalTrig','HcalPacked','HcalPackedError',
#               "PrsE",'PrsTrig',"PrsPacked"],
        inputs={"RawEventLocations" : None},
        conf=DecoderDB)

Decoder("L0CaloAlg/L0Calo",
        active=False, #false by default, does not run usually, only in Boole?
        privateTools=["CaloTriggerAdcsFromRaw/EcalTriggerAdcTool","CaloTriggerAdcsFromRaw/HcalTriggerAdcTool","CaloTriggerBitsFromRaw"],
        conf=DecoderDB)

Decoder("CaloTriggerAdcsFromRaw/EcalTriggerAdcTool",
        active=False,
        banks=['EcalE','EcalTrig','EcalPacked'],
        inputs={"RawEventLocations":None},
        conf=DecoderDB)

Decoder("CaloTriggerAdcsFromRaw/HcalTriggerAdcTool",
        active=False,
        banks=['HcalE','HcalTrig','HcalPacked'],
        inputs={"RawEventLocations":None},
        conf=DecoderDB)

Decoder("CaloTriggerBitsFromRaw",
        active=False,
        banks=["PrsE",'PrsTrig',"PrsPacked"],
        inputs={"RawEventLocations":None},
        conf=DecoderDB)

Decoder("CaloTriggerAdcsFromRaw/EcalTriggerAdcToolExpert",
        active=False,
        banks=['EcalPackedError'],
        inputs={"RawEventLocations":None},
        conf=DecoderDB)

Decoder("CaloTriggerAdcsFromRaw/HcalTriggerAdcToolExpert",
        active=False,
        banks=['HcalPackedError'],
        inputs={"RawEventLocations":None},
        conf=DecoderDB)

        

#TRIGGER ==========HLT===========

#firstly the Dec/Sel/Vertex reports,
#these need separate versions for Hlt1/2 split scenario,
#decoding into different locations

#create them all in a similar way, since they have similar options
#for each decoder we have an HLT1, HLT2 and combined version.
#Decoders look like "Hlt(''/1/2)(Sel/Dec/Vertex)ReportsDecoder"

#report, the type of report
for report in ["Dec","Sel","Vertex"]:
    #hlt, which HLT to decode? None=both, 1=Hlt1, 2=Hlt2
    for hlt in [None,1,2]:
        hltname="Hlt"
        algtype="Hlt"+report+"ReportsDecoder"
        algname=algtype
        if hlt is not None:
            hltname=hltname+str(hlt)
            algname=algtype+"/"+hltname+report+"ReportsDecoder"
        #create the decoder
        dec=Decoder(
            #\/ e.g. HltSelReportsDecoder/Hlt1SelReportsDecoder
            algname,
            active=True,
            #\/ e.g. HltSelReports
            banks=["Hlt"+report+"Reports"],
            inputs={"RawEventLocations":None},
            #\/ e.g. OutputHltSelReportsLocation: Hlt1/SelReports
            outputs={"OutputHlt"+report+"ReportsLocation" : hltname+"/"+report+"Reports"},
            properties={"SourceID" : hlt}, #None=default(0)
            conf=DecoderDB
            )

#Also TrackingDecoder, but don't make it active, it's only used during HLT2 stand-alone!
Decoder("HltTrackReportsDecoder",
        active=False, banks=["HltTrackReports"],
        inputs = {"RawEventLocations":None},
        outputs=["Hlt/Track/Velo","Hlt1/Track/PestiForward"],
        conf=DecoderDB
        )
#outputs={"Output2SourceId": { "Hlt/Track/Velo" : 1, "Hlt1/Track/PestiForward" : 3 }}, set logically in the code, resetting may not work...


#is a Routing bits filter really a decoder? it doesn't create output...
Decoder("HltRoutingBitsFilter",
        active=False, banks=["HltRoutingBits"],
        inputs={"RawEventLocations":None},
        conf=DecoderDB)

Decoder("HltLumiSummaryDecoder",
        active=True, banks=["HltLumiSummary"],
        inputs={"RawEventLocations":None},
        outputs={"OutputContainerName":None},
        conf=DecoderDB)

#UPGRADE ===========UT===========
Decoder("RawBankToSTLiteClusterAlg/createUTLiteClusters",
        active=True, banks=["UT"],
        outputs=["Raw/UT/LiteClusters"],
        inputs={"RawEventLocations" : None},
        #publicTools=["STOfflinePosition/ToolSvc.UTClusterPosition"],
        properties={"DetType":"UT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/UT/LiteClusters"}, set logically in the code, resetting may not work...
#"STOfflinePosition/ToolSvc.UTClusterPosition" is not part of decoding.

Decoder("RawBankToSTLiteClusterAlg/createUTLiteClustersExpert",
        active=False, banks=["UTPedestal","UTFull","UTError"],
        outputs=["Raw/UT/LiteClusters"],
        inputs={"RawEventLocations" : None},
        #publicTools=["STOfflinePosition/ToolSvc.UTClusterPosition"],
        properties={"DetType":"UT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/UT/LiteClusters"}, set logically in the code, resetting may not work...
#"STOfflinePosition/ToolSvc.UTClusterPosition" is not part of decoding.

Decoder("RawBankToSTClusterAlg/createUTClusters",
        active=True, banks=["UT"],
        outputs=["Raw/UT/Clusters"],
        inputs={"RawEventLocations" : None},
        #publicTools=["STOfflinePosition/ToolSvc.UTClusterPosition"],
        properties={"DetType":"UT"},
        required=["createODIN"],
        conf=DecoderDB)
#outputs={"clusterLocation":"Raw/IT/LiteClusters"}, set logically in the code, resetting may not work...
#"STOfflinePosition/ToolSvc.UTClusterPosition" is not part of decoding.

Decoder("RawBankToSTClusterAlg/createUTClustersExpert",
        active=False, banks=["UTPedestal","UTFull","UTError"],
        outputs=["Raw/UT/Clusters"],
        inputs={"RawEventLocations" : None},
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
        inputs={"RawEventLocations" : None},
        outputs={"OutputLocation":None},
        conf=DecoderDB)
