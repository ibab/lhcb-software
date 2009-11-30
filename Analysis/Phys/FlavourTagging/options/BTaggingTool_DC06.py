# values for DC06 tuning:

taggerKaons.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'8.0' CombDLL(k-p)>'-4.0'"]

taggerMuons.Muon.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'0.0'" ]

taggerElectrons.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'5.0'" ]

dod.AlgMap.update( { hat + kName + '/Particles' : 'CombinedParticleMaker/'+kName ,
                     hat + kName + '/Vertices'  : 'CombinedParticleMaker/'+kName } )
dod.AlgMap.update( { hat + mName + '/Particles' : 'CombinedParticleMaker/'+mName ,
                     hat + mName + '/Vertices'  : 'CombinedParticleMaker/'+mName } )
dod.AlgMap.update( { hat + eName + '/Particles' : 'CombinedParticleMaker/'+eName ,
                     hat + eName + '/Vertices'  : 'CombinedParticleMaker/'+eName } )
dod.AlgMap.update( { hat + pName + '/Particles' : 'CombinedParticleMaker/'+pName ,
                     hat + pName + '/Vertices'  : 'CombinedParticleMaker/'+pName } )
dod.AlgMap.update( { hat + tagName + '/Particles' : 'FilterDesktop/'+tagName ,
                     hat + tagName + '/Vertices'  : 'FilterDesktop/'+tagName } )


from Configurables import TaggerMuonTool, TaggerElectronTool, TaggerKaonSameTool, TaggerKaonOppositeTool, TaggerPionSameTool, TaggerVertexChargeTool

tag.addTool ( TaggerMuonTool )
tag.TaggerMuonTool.Muon_Pt_cut = 1.1 *GeV
tag.TaggerMuonTool.Muon_P_cut  = 0.0 *GeV
tag.TaggerMuonTool.Muon_lcs_cut= 4.0
tag.TaggerMuonTool.NeuralNetName = "NNetTool_DC06"

tag.addTool ( TaggerElectronTool )
tag.TaggerElectronTool.Ele_Pt_cut   = 1.1 * GeV
tag.TaggerElectronTool.Ele_P_cut    = 4.0 * GeV
tag.TaggerElectronTool.Ele_lcs_cut  = 3.0
tag.TaggerElectronTool.Ele_ghost_cut=-25.0
tag.TaggerElectronTool.VeloChargeMin= 0.0 
tag.TaggerElectronTool.VeloChargeMax= 1.7
tag.TaggerElectronTool.EoverP       = 0.9
tag.TaggerElectronTool.NeuralNetName = "NNetTool_DC06"

tag.addTool ( TaggerKaonOppositeTool )
tag.TaggerKaonOppositeTool.Kaon_Pt_cut  = 0.6 *GeV
tag.TaggerKaonOppositeTool.Kaon_P_cut   = 4.0 *GeV
tag.TaggerKaonOppositeTool.Kaon_IPs_cut = 3.5
tag.TaggerKaonOppositeTool.Kaon_IP_cut  = 2.0
tag.TaggerKaonOppositeTool.Kaon_LongTrack_LCS_cut     = 2.5
tag.TaggerKaonOppositeTool.Kaon_upstreamTrack_LCS_cut = 2.0
tag.TaggerKaonOppositeTool.Kaon_PIDkp_extracut   = -1.0
tag.TaggerKaonOppositeTool.Kaon_ghost_cut        =-20.0
tag.TaggerKaonOppositeTool.Kaon_LongTrack_IP_cut = 999.0
tag.TaggerKaonOppositeTool.Kaon_upstreamTrack_IP_cut = 999.0
tag.TaggerKaonOppositeTool.NeuralNetName = "NNetTool_DC06"

tag.addTool ( TaggerKaonSameTool )
tag.TaggerKaonSameTool.KaonSame_Pt_cut = 0.4 *GeV
tag.TaggerKaonSameTool.KaonSame_P_cut  = 4.0 *GeV
tag.TaggerKaonSameTool.KaonSame_IP_cut = 3.0
tag.TaggerKaonSameTool.KaonSame_Phi_cut= 1.1
tag.TaggerKaonSameTool.KaonSame_Eta_cut= 1.0
tag.TaggerKaonSameTool.KaonSame_dQ_cut = 1.4 *GeV
tag.TaggerKaonSameTool.KaonS_LCS_cut   = 4.0
tag.TaggerKaonSameTool.KaonSPID_extracut= 8.0
tag.TaggerKaonSameTool.NeuralNetName = "NNetTool_DC06"

tag.addTool ( TaggerPionSameTool )
tag.TaggerPionSameTool.ProbMin          = 0.55
tag.TaggerPionSameTool.PionSame_P_cut   = 0.2 *GeV
tag.TaggerPionSameTool.PionSame_IPs_cut = 3.0
tag.TaggerPionSameTool.PionS_LCS_cut    = 3.0
tag.TaggerPionSameTool.PionSame_dQ_cut  = 3.0 *GeV
tag.TaggerPionSameTool.PionSame_dQ_extra_cut= 1.5 *GeV
tag.TaggerPionSameTool.Pion_ghost_cut     = -25.0
tag.TaggerPionSameTool.PionSame_PIDNoK_cut= 5.0
tag.TaggerPionSameTool.PionSame_PIDNoP_cut= 10.0
tag.TaggerPionSameTool.NeuralNetName = "NNetTool_DC06"

tag.addTool ( TaggerVertexChargeTool )
tag.TaggerVertexChargeTool.PowerK = 0.35
tag.TaggerVertexChargeTool.MinimumCharge = 0.15
tag.TaggerVertexChargeTool.P0    = 5.259631e-01
tag.TaggerVertexChargeTool.P1    =-3.256825e-01
tag.TaggerVertexChargeTool.Gt075 = 0.35
tag.TaggerVertexChargeTool.TracksEq2 = 0.420701
tag.TaggerVertexChargeTool.TracksGt2 = 0.352641
tag.TaggerVertexChargeTool.ProbMin   = 0.52
