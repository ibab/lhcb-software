#!/bin/python

def saneHex(string):
	#cut away leading 0x
	hex = string.replace('0x','')
	return '0x%(h)08d'% {'h':int(hex)}

import sys

# guess that the only given argument is "--help"
if (1<len(sys.argv)) :
   print "USAGE:"
   print "  SetupProject Moore"
   print "  python makeZooList.py "
   print ""
   print "NB: * only use this if you make a trigger list for a new release."
   print "    * this is a script for maintainers, not for users"
   sys.exit()


from TCKUtils.utils import *

# until sept 2012
tcklist_2012 = [
0x561710,
0x641710,
0x7E0039,
0x7E003A,
0x7F0040,
0x860040,
0x8C0040,# fine
0x8E0040,
0x94003D,# fine
0x95003D,# fine
#0x97003C, not fine
0x97003D,
0x990042,
0x990043,
0x990044,
0x9A0042
]
tcklist_2011 = [
0x360032,
0x480032,
0x4A0033,
0x561710,
0x5A0032,
0x5B0032,
0x5D0033, # last of last release
0x6D0032,
0x700034,
0x710035,
0x730035,
0x740036,
0x760037,
0x790037,
0x790038
]
tcklist_2010 = [
0x10001F,
0x101710,
0x12001F,
0x13001F,
0x14001E,
0x14001F,
0x17001F,
0x18001F,
0x19001F,
0x190023,
0x190024,
0x1A001F,
0x1D0030,
0x1E0030,
0x1F0029,
0x1F0031,
0x24002A,
0x24002C,
0x25002A,
0x25002C,
0x2A002A,
0x2A002C,
0x2B002A,
0x2D002B,
0x2E002A,
0x2E002C,
0x91710 ,
0xE001F ,
0xE2710 ,
0xF001F ,
0xF2710 
]


oldhlt1list = ['Hlt::Line/Hlt1DiMuonHighMass',
'Hlt::Line/Hlt1DiMuonLowMass',
'Hlt::Line/Hlt1SingleMuonNoIP',
'Hlt::Line/Hlt1SingleMuonHighPT',
'Hlt::Line/Hlt1TrackAllL0',
'Hlt::Line/Hlt1TrackMuon',
'Hlt::Line/Hlt1TrackPhoton',
'Hlt::Line/Hlt1Lumi',
'Hlt::Line/Hlt1LumiMidBeamCrossing',
'Hlt::Line/Hlt1MBNoBias',
'Hlt::Line/Hlt1L0Any',
'Hlt::Line/Hlt1L0AnyRateLimited',
'Hlt::Line/Hlt1L0AnyNoSPD',
'Hlt::Line/Hlt1L0AnyNoSPDRateLimited',
'Hlt::Line/Hlt1NoPVPassThrough',
'Hlt::Line/Hlt1DiProtonLowMult',
'Hlt::Line/Hlt1BeamGasNoBeamBeam1',
'Hlt::Line/Hlt1BeamGasNoBeamBeam2',
'Hlt::Line/Hlt1BeamGasBeam1',
'Hlt::Line/Hlt1BeamGasBeam2',
'Hlt::Line/Hlt1BeamGasCrossingEnhancedBeam1',
'Hlt::Line/Hlt1BeamGasCrossingEnhancedBeam2',
'Hlt::Line/Hlt1BeamGasCrossingForcedReco',
'Hlt::Line/Hlt1ODINTechnical',
'Hlt::Line/Hlt1Tell1Error',
'Hlt::Line/Hlt1BeamGasCrossingParasitic',
'Hlt::Line/Hlt1ErrorEvent',
'Hlt::Line/Hlt1Global',
'Hlt::Line/Hlt1MBMicroBiasVelo',
'Hlt::Line/Hlt1MBMicroBiasVeloRateLimited',
'Hlt::Line/Hlt1MBMicroBiasTStation',
'Hlt::Line/Hlt1MBMicroBiasTStationRateLimited',
'Hlt::Line/Hlt1DiProton',
'Hlt::Line/Hlt1VeloClosingMicroBias']
oldhlt2list =['Hlt::Line/Hlt2SingleElectronTFLowPt',
'Hlt::Line/Hlt2SingleElectronTFHighPt',
'Hlt::Line/Hlt2DiElectronHighMass',
'Hlt::Line/Hlt2DiElectronB',
'Hlt::Line/Hlt2Topo2BodySimple',
'Hlt::Line/Hlt2Topo3BodySimple',
'Hlt::Line/Hlt2Topo4BodySimple',
'Hlt::Line/Hlt2Topo2BodyBBDT',
'Hlt::Line/Hlt2Topo3BodyBBDT',
'Hlt::Line/Hlt2Topo4BodyBBDT',
'Hlt::Line/Hlt2TopoMu2BodyBBDT',
'Hlt::Line/Hlt2TopoMu3BodyBBDT',
'Hlt::Line/Hlt2TopoMu4BodyBBDT',
'Hlt::Line/Hlt2TopoE2BodyBBDT',
'Hlt::Line/Hlt2TopoE3BodyBBDT',
'Hlt::Line/Hlt2TopoE4BodyBBDT',
'Hlt::Line/Hlt2IncPhi',
'Hlt::Line/Hlt2IncPhiSidebands',
'Hlt::Line/Hlt2CharmHadD02HHKsLL',
'Hlt::Line/Hlt2Dst2PiD02PiPi',
'Hlt::Line/Hlt2Dst2PiD02MuMu',
'Hlt::Line/Hlt2Dst2PiD02KMu',
'Hlt::Line/Hlt2Dst2PiD02KPi',
'Hlt::Line/Hlt2PassThrough',
'Hlt::Line/Hlt2Transparent',
'Hlt::Line/Hlt2Forward',
'Hlt::Line/Hlt2DebugEvent',
'Hlt::Line/Hlt2CharmHadD02KK',
'Hlt::Line/Hlt2CharmHadD02KKWideMass',
'Hlt::Line/Hlt2CharmHadD02KPi',
'Hlt::Line/Hlt2CharmHadD02KPiWideMass',
'Hlt::Line/Hlt2CharmHadD02PiPi',
'Hlt::Line/Hlt2CharmHadD02PiPiWideMass',
'Hlt::Line/Hlt2ExpressJPsi',
'Hlt::Line/Hlt2ExpressJPsiTagProbe',
'Hlt::Line/Hlt2ExpressLambda',
'Hlt::Line/Hlt2ExpressKS',
'Hlt::Line/Hlt2ExpressDs2PhiPi',
'Hlt::Line/Hlt2ExpressBeamHalo',
'Hlt::Line/Hlt2ExpressDStar2D0Pi',
'Hlt::Line/Hlt2ExpressHLT1Physics',
'Hlt::Line/Hlt2Bs2PhiGamma',
'Hlt::Line/Hlt2Bs2PhiGammaWideBMass',
'Hlt::Line/Hlt2Bd2KstGamma',
'Hlt::Line/Hlt2Bd2KstGammaWideKMass',
'Hlt::Line/Hlt2Bd2KstGammaWideBMass',
'Hlt::Line/Hlt2CharmHadD2KS0Pi',
'Hlt::Line/Hlt2CharmHadD2KS0K',
'Hlt::Line/Hlt2CharmRareDecayD02MuMu',
'Hlt::Line/Hlt2B2HH',
'Hlt::Line/Hlt2MuonFromHLT1',
'Hlt::Line/Hlt2SingleMuon',
'Hlt::Line/Hlt2SingleMuonHighPT',
'Hlt::Line/Hlt2SingleMuonLowPT',
'Hlt::Line/Hlt2TFBc2JpsiMuX',
'Hlt::Line/Hlt2TFBc2JpsiMuXSignal',
'Hlt::Line/Hlt2DisplVerticesLowMassSingle',
'Hlt::Line/Hlt2DisplVerticesHighMassSingle',
'Hlt::Line/Hlt2DisplVerticesDouble',
'Hlt::Line/Hlt2DisplVerticesSinglePostScaled',
'Hlt::Line/Hlt2DisplVerticesHighFDSingle',
'Hlt::Line/Hlt2DisplVerticesSingleDown',
'Hlt::Line/Hlt2B2HHPi0_Merged',
'Hlt::Line/Hlt2CharmHadD2HHH',
'Hlt::Line/Hlt2CharmHadD2HHHWideMass',
'Hlt::Line/Hlt2DiMuon',
'Hlt::Line/Hlt2DiMuonLowMass',
'Hlt::Line/Hlt2DiMuonJPsi',
'Hlt::Line/Hlt2DiMuonJPsiHighPT',
'Hlt::Line/Hlt2DiMuonPsi2S',
'Hlt::Line/Hlt2DiMuonB',
'Hlt::Line/Hlt2DiMuonZ',
'Hlt::Line/Hlt2DiMuonDY1',
'Hlt::Line/Hlt2DiMuonDY2',
'Hlt::Line/Hlt2DiMuonDY3',
'Hlt::Line/Hlt2DiMuonDY4',
'Hlt::Line/Hlt2DiMuonDetached',
'Hlt::Line/Hlt2DiMuonDetachedHeavy',
'Hlt::Line/Hlt2DiMuonDetachedJPsi',
'Hlt::Line/Hlt2DiMuonNoPV',
'Hlt::Line/Hlt2TriMuonDetached',
'Hlt::Line/Hlt2TriMuonTau',
'Hlt::Line/Hlt2CharmHadD02HHHH',
'Hlt::Line/Hlt2CharmHadD02HHHHWideMass',
'Hlt::Line/Hlt2ErrorEvent',
'Hlt::Line/Hlt2Global',
'Hlt::Line/Hlt2B2HHLTUnbiased',
'Hlt::Line/Hlt2CharmHadD02HH_D02PiPi',
'Hlt::Line/Hlt2CharmHadD02HH_D02PiPiWideMass',
'Hlt::Line/Hlt2CharmHadD02HH_D02KK',
'Hlt::Line/Hlt2CharmHadD02HH_D02KKWideMass',
'Hlt::Line/Hlt2CharmHadD02HH_D02KPi',
'Hlt::Line/Hlt2CharmHadD02HH_D02KPiWideMass',
'Hlt::Line/Hlt2CharmHadD2KS0H_D2KS0Pi',
'Hlt::Line/Hlt2CharmHadD2KS0H_D2KS0K',
'Hlt::Line/Hlt2DiProton',
'Hlt::Line/Hlt2DiProtonTF',
'Hlt::Line/Hlt2DiProtonLowMult',
'Hlt::Line/Hlt2DiProtonLowMultTF',
'Hlt::Line/Hlt2CharmSemilepD02HMuNu_D02KMuNuWS',
'Hlt::Line/Hlt2CharmSemilepD02HMuNu_D02PiMuNuWS',
'Hlt::Line/Hlt2CharmSemilepD02HMuNu_D02KMuNu',
'Hlt::Line/Hlt2CharmSemilepD02HMuNu_D02PiMuNu',
'Hlt::Line/Hlt2CharmSemilepD2HMuMu',
'Hlt::Line/Hlt2CharmSemilepD2HMuMuWideMass',
'Hlt::Line/Hlt2CharmSemilepD02HHMuMu',
'Hlt::Line/Hlt2CharmSemilepD02HHMuMuWideMass']


tcklist = [0x2e002a, 0x2a002a, 0x2e002c, 0x2b002a, 0x25002c, 0x1f0029, 0x1e0030, 0x190024, 0x19001f]
tcklist = tcklist_2011 + tcklist_2012 #+ tcklist_2010
hlt1list = []
hlt2list = []

for tck in tcklist:
	tmphlt1 = getHlt1Lines(tck)
	for line in tmphlt1:
		if not hlt1list.count(line):
		 #if not oldhlt1list.count(line):
			hlt1list += [line]
	tmphlt2 = getHlt2Lines(tck)
	for line in tmphlt2:
		if not hlt2list.count(line):
		 #if not oldhlt2list.count(line):
			hlt2list += [line]

word = 1
byte = 0
byte_count = 0
print '//HLT1 Lines\n\n'
for i in hlt1list:
	byte = pow(2, byte_count)
	byte_count += 1
	name = i.replace('Hlt::Line/','')
	if name.find('Global') == -1:  
		#string = 'MakeTriggerDec(' + name + '\t\t\t' +repr(word) + ',' + hex(byte) + ',"' + name + 'Decision);'
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)sDecision");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	else:
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)s");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	

	print string
	
	if byte_count == 32:
		byte_count = 0
		word +=1
		print '\n'

print '\n//HLT2 Lines\n\n'
word += 1
byte = 0
byte_count = 0
for i in hlt2list:
	byte = pow(2, byte_count)
	byte_count += 1
	name = i.replace('Hlt::Line/','')
	if name.find('Global') == -1:  
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)sDecision");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	else:
		string = 'MakeTriggerDec(%(n)-40s,%(w)d,%(h)s,"%(n)s");' % {"n":name, "w":word, "h":saneHex(hex(byte))}
	print string
	
	if byte_count == 32:
		byte_count = 0
		word +=1
		print '\n'

