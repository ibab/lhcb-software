import os, sys, string

mystripping=sys.argv[1]
mydataset="Reco15a_Run164668.py"
mydatasetxml="Reco15a_Run164668.xml"

#linebuilders list to be updated with latest stripping!
linebuilders=['B2threebody', 'BetaSPi0', 'Ccbar2PpbarExclusive', 'ChargedHyperons', 'D2XMuMuSS', 'DstarPromptWithD02HHLL', 'LFV', 'Lambdac2V0H', 'SigmacForPID', 'TaggedJets', 'We', 'Xibc', 'Lb2pMuNuVub', 'Bc2JpsiMu', 'Beauty2XGamma', 'BetaS', 'Bu2hhh', 'D2HHHPi0', 'D2hhhFTCalib', 'DY2MuMu', 'DstarD2XGamma', 'ZVTOP', 'Kshort2eePiPi', 'BadEvents', 'Bd2JpsieeKS', 'Bu2MuNu', 'CharmAssociative', 'DstarD02xx', 'HeavyBaryon', 'HighPtTau', 'MiniBias', 'TrackEffD0ToK3Pi', 'DstarD2HHHH', 'Z02ee', 'B2CharmlessQ2B', 'CharmFromBSemiForProtonPID', 'D02KPiPi0', 'DY2ee', 'DstarD02KShhForTrackingEff', 'DstarPromptWithD02HHMuMuControl', 'Kshort2MuMuMuMu', 'Tau2LambdaMu', 'TrackEffDownMuon', 'Xicc', 'D2HMuNu', 'B2XuMuNu', 'B2fourbody', 'B2pphh', 'CC2DD', 'D2HHHKs', 'DstarD2KShh', 'LowMultINC', 'RareStrange', 'B23Mu', 'B2DDphi', 'B2Lambda0Mu', 'Bd2DstarMuNu', 'Ccbar2PPPiPi', 'D2EtaPrimeH', 'DiMuonInherit', 'DisplVertices', 'Ditau', 'Tau23Mu', 'B23MuNu', 'B2JpsiHH', 'B2KShh', 'B2KstTauTau', 'B2Ksthh', 'Bc2Ds1Gamma', 'Ccbar2Phi', 'D2EtaH', 'Lb2EtacKp', 'B2DMuNuX', 'B2HHPi0', 'B2Kpi0Lines', 'B2XTauNuAllLines', 'Bc2hhh', 'Beauty2Charm', 'Beauty2XGammaNoBias', 'Bs2GammaGamma', 'D2Pi0H', 'D2hhhLines', 'DarkBoson', 'DstarD2KSHHPi0', 'Kshort2PiPiMuMu', 'Lb2L0Gamma', 'PromptCharm', 'SingleTrackTIS', 'TrackEffVeloMuon', 'WmuAKTJets', 'Xb23ph', 'D23Mu', 'B24p', 'B2DstMuNuIncl', 'B2XMuMu', 'BLV', 'Ccbar2Ppbar', 'D2KS0H', 'Lc23Mu', 'WeAKTJets', 'XicHHH', 'B2DMuForTauMu', 'Bc2JpsiHBDT', 'D2EtaEEGH', 'D2PhiH', 'TrackEffD0ToKPi', 'B2KShhh', 'Bs2MuMuLines', 'Bs2st2KKMuX', 'Bu2LLK', 'Ccbar2PpbarDetached', 'D02KSKS', 'Hb2Charged2Body', 'StrangeBaryons', 'B2ppipiSigmacmm_Lcpi', 'Beauty2XGammaExclusive', 'BetaSBs2JpsieePhi', 'Bs2EtacPhiBDT', 'Bu2Ksthh', 'CC2DDcontrol', 'CharmFromBSemi', 'RnS', 'TrackEffMuonTT', 'B2XLL', 'Bc3h', 'BetaSBs2PhiPhi', 'ChiCJPsiGammaConv', 'H24Mu', 'Hb2V0V0h', 'LLP2MuX', 'Z02MuMu', 'Bd2eeKstarBDT', 'Bs2K0stK0st', 'CharmForVub', 'CharmFromBSemiForHadronAsy', 'Dijets', 'Dst2PiD0EEG', 'WMu', 'B2HHBDT', 'B2XTau', 'B2twobody', 'D2hh', 'DstarD0ToHHPi0', 'InclbJets', 'Lb2V0h', 'MuMuSS', 'PhiToKSKS', 'ProtonAsym', 'SbarSCorrelations', 'StrangeBaryonsNoPID']


#linebuilders=['DstarD02KShhForTrackingEff', 'DstarPromptWithD02HHMuMuControl', 'Kshort2MuMuMuMu', 'Tau2LambdaMu', 'TrackEffDownMuon', 'Xicc', 'D2HMuNu', 'B2XuMuNu', 'B2fourbody', 'B2pphh', 'CC2DD', 'D2HHHKs', 'DstarD2KShh', 'LowMultINC', 'RareStrange', 'noPIDDstar', 'B23Mu', 'B2DDphi', 'B2Lambda0Mu', 'Bd2DstarMuNu', 'Ccbar2PPPiPi', 'D2EtaPrimeH', 'DiMuonInherit', 'DisplVertices', 'Ditau', 'Tau23Mu', 'B23MuNu', 'B2JpsiHH', 'B2KShh', 'B2KstTauTau', 'B2Ksthh', 'Bc2Ds1Gamma', 'Ccbar2Phi', 'D2EtaH', 'Lb2EtacKp', 'B2DMuNuX', 'B2HHPi0', 'B2Kpi0Lines', 'B2XTauNuAllLines', 'Bc2hhh', 'Beauty2Charm', 'Beauty2XGammaNoBias', 'Bs2GammaGamma', 'D2Pi0H', 'D2hhhLines', 'DarkBoson', 'DstarD2KSHHPi0', 'Kshort2PiPiMuMu', 'Lb2L0Gamma', 'PromptCharm', 'SingleTrackTIS', 'TrackEffVeloMuon', 'WmuAKTJets', 'Xb23ph', 'D23Mu', 'B24p', 'B2DstMuNuIncl', 'B2XMuMu', 'BLV', 'Ccbar2Ppbar', 'D2KS0H', 'Lc23Mu', 'PIDCalib', 'WeAKTJets', 'XicHHH', 'B2DMuForTauMu', 'Bc2JpsiHBDT', 'D2EtaEEGH', 'D2PhiH', 'TrackEffD0ToKPi', 'B2KShhh', 'Bs2MuMuLines', 'Bs2st2KKMuX', 'Bu2LLK', 'Ccbar2PpbarDetached', 'D02KSKS', 'Hb2Charged2Body', 'StrangeBaryons', 'B2ppipiSigmacmm_Lcpi', 'Beauty2XGammaExclusive', 'BetaSBs2JpsieePhi', 'Bs2EtacPhiBDT', 'Bu2Ksthh', 'CC2DDcontrol', 'CharmFromBSemi', 'RnS', 'TrackEffMuonTT', 'B2XLL', 'Bc3h', 'BetaSBs2PhiPhi', 'ChiCJPsiGammaConv', 'H24Mu', 'Hb2V0V0h', 'LLP2MuX', 'Z02MuMu', 'Bd2eeKstarBDT', 'Bs2K0stK0st', 'CharmForVub', 'CharmFromBSemiForHadronAsy', 'Dijets', 'Dst2PiD0EEG', 'WMu', 'B2HHBDT', 'B2XTau', 'B2twobody', 'D2hh', 'DstarD0ToHHPi0', 'InclbJets', 'Lb2V0h', 'MuMuSS', 'PhiToKSKS', 'ProtonAsym', 'SbarSCorrelations', 'StrangeBaryonsNoPID']

#linebuilders=['B2Kpi0Lines', 'B2XTauNuAllLines', 'Bc2hhh', 'Beauty2Charm', 'Beauty2XGammaNoBias', 'Bs2GammaGamma', 'D2Pi0H', 'D2hhhLines', 'DarkBoson', 'DstarD2KSHHPi0', 'Kshort2PiPiMuMu', 'Lb2L0Gamma', 'PromptCharm', 'SingleTrackTIS', 'TrackEffVeloMuon', 'WmuAKTJets', 'Xb23ph', 'D23Mu', 'B24p', 'B2DstMuNuIncl', 'B2XMuMu', 'BLV', 'Ccbar2Ppbar', 'D2KS0H', 'Lc23Mu', 'PIDCalib', 'WeAKTJets', 'XicHHH', 'B2DMuForTauMu', 'Bc2JpsiHBDT', 'D2EtaEEGH', 'D2PhiH', 'TrackEffD0ToKPi', 'B2KShhh', 'Bs2MuMuLines', 'Bs2st2KKMuX', 'Bu2LLK', 'Ccbar2PpbarDetached', 'D02KSKS', 'Hb2Charged2Body', 'StrangeBaryons', 'B2ppipiSigmacmm_Lcpi', 'Beauty2XGammaExclusive', 'BetaSBs2JpsieePhi', 'Bs2EtacPhiBDT', 'Bu2Ksthh', 'CC2DDcontrol', 'CharmFromBSemi', 'RnS', 'TrackEffMuonTT', 'B2XLL', 'Bc3h', 'BetaSBs2PhiPhi', 'ChiCJPsiGammaConv', 'H24Mu', 'Hb2V0V0h', 'LLP2MuX', 'Z02MuMu', 'Bd2eeKstarBDT', 'Bs2K0stK0st', 'CharmForVub', 'CharmFromBSemiForHadronAsy', 'Dijets', 'Dst2PiD0EEG', 'WMu', 'B2HHBDT', 'B2XTau', 'B2twobody', 'D2hh', 'DstarD0ToHHPi0', 'InclbJets', 'Lb2V0h', 'MuMuSS', 'PhiToKSKS', 'ProtonAsym', 'SbarSCorrelations', 'StrangeBaryonsNoPID']

#linebuilders=['B2threebody']


print "running for "+mystripping

dv = DaVinci(version='v38r1',
             setupProjectOptions = '--nightly lhcb-prerelease Wed')

#ds = dv.readInputData(mydataset)
#dv = DaVinci(version='v38r0')
job_name = 'Str24_all'
f = open('TestFromSettings_all.py','r')
opts = f.read()
#opts = opts.replace('___lb___',lb)
opts = opts.replace('__mystripping__',mystripping)
f.close()
dv.extraopts = opts
#help(dv)
ds = dv.readInputData(mydataset)
ds.XMLCatalogueSlice = mydatasetxml
#help(ds)
#print ds
  #ds.readInputData(mydataset)
#help(Job)
j = Job(name = job_name,
        application = dv,
        inputdata = ds,
        inputsandbox = ["DV-RawEventJuggler-0_3-to-4_2.py"],
        outputfiles = [LocalFile(namePattern='*.root'),
                                              MassStorageFile(namePattern='*.dst'),
                                              MassStorageFile(namePattern='*.mdst'),
                                              MassStorageFile(namePattern='*.xml')],
        splitter = SplitByFiles(filesPerJob =15, maxFiles = 15, ignoremissing = True),
##       backend    = Dirac()
##       j.backend.settings['CPUTime'] = 3600
        backend = LSF(queue = '1nw')
        )
##j.backend    = Dirac()
##j.backend.settings['CPUTime'] = 3600*24
j.application.args = ["-T"]
  ##j.application.readInputData(LocalFile('TestFromSettings_perline.py'))
j.submit()
#print str(j.id) +" AAA "+str(j.name)


for lb in linebuilders:
 #dv = DaVinci(version='v38r0')
 dv = DaVinci(version='v38r1',
             setupProjectOptions = '--nightly lhcb-prerelease Wed')
 job_name = 'Str24_'+str(lb)
 f = open('TestFromSettings_perline.py','r')
 opts = f.read()
 opts = opts.replace('___lb___',lb)
 opts = opts.replace('__mystripping__',mystripping)
 f.close()
 dv.extraopts = opts
 ds = dv.readInputData(mydataset)
 ds.XMLCatalogueSlice = mydatasetxml
 #ds.readInputData(mydataset)
 j = Job(name = job_name,
       application = dv,
       inputdata = ds,
       inputsandbox = ["DV-RawEventJuggler-0_3-to-4_2.py"],
       outputfiles = [LocalFile(namePattern='*.root'),
                                             MassStorageFile(namePattern='*.dst'),
                                             MassStorageFile(namePattern='*.mdst'),
                                             MassStorageFile(namePattern='*.xml')],
       splitter = SplitByFiles(filesPerJob =25, maxFiles = 25, ignoremissing = True),
     backend = LSF(queue = '1nd')
	)
 j.application.args = ["-T"]
 j.submit()
 print str(j.id) +" AAA "+str(j.name)
