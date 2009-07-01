from SelectionSummary import Selection
'''
Bs2JpsiPhiSel = Selection('SeqBs2Jpsi2MuMuPhi2KK',
                          'Bs2Jpsi2MuMuPhi2KK',
                          '$MICRODSTEXAMPLEROOT/options/selections/SeqBs2Jpsi2MuMuPhi2KK.opts',
                          '$MICRODSTEXAMPLEROOT/options/JpsiPhiDataPFN.py')
'''

Bs2JpsiPhiSel = Selection('SeqBs2Jpsi2MuMuPhi2KK',
                          'Bs2Jpsi2MuMuPhi2KK',
                          '$MICRODSTEXAMPLEROOT/options/selections/SeqBs2Jpsi2MuMuPhi2KK.py',
                          '$MICRODSTEXAMPLEROOT/options/JpsiPhiDataPFN.py')

Jpsi2MuMuSel = Bs2JpsiPhiSel.clone()
Jpsi2MuMuSel.mainSelector = "Jpsi2MuMu_forDC06selBs2JpsiPhi_unbiased"

Phi2KKSel = Bs2JpsiPhiSel.clone()
Phi2KKSel.mainSelector = "Phi2KK_forDC06selBs2JpsiPhi_unbiased"

BdJpsiKstarSel = Selection('SeqB2Jpsihh',
                           'DC06SelBd2JpsiKstar_lifetime_unbiased',
                           '$CCBARROOT/options/DoDC06selBd2Jpsi2MuMuKst2Kpi_lifetime_unbiased.opts',
                           '$MICRODSTEXAMPLEROOT/options/JpsiKstar_MCDC06_recosimV30_strippingV31.py')

KstarSel = BdJpsiKstarSel.clone()
KstarSel.mainSelector = 'Kst2Kpi_forDC06selBd2Jpsi2MuMuKst2Kpi_lifetime_unbiased'

Bs2DsDsSel = Selection('SeqPreselBs2DsDs',
                       'PreselBs2DsDs',
                       '$B2DSXROOT/options/DoPreselBs2DsDs.opts',
                       'Bs2DsXCock.py')
