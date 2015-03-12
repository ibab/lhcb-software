# The class that creates the Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

from Inputs import Hlt2NoPIDsPions

class B2HHCombiner(Hlt2Combiner):
    def __init__(self, name):
        picut = ( "(TRCHI2DOF < %(TrChi2)s )" +
                  " & (PT > %(PionPT)s)" +
                  " & (MIPCHI2DV(PRIMARY) > %(PionIPCHI2)s)" )
        dc =    { 'pi+' : picut }
        cc =    ( "( AM > %(BMassWinLow)s )" +
                  " & ( AM < %(BMassWinHigh)s )" +
                  " & ( ACUTDOCACHI2( %(DOCACHI2)s, '' ) )")
                  #" & ( AMAXDOCACHI2 < %(DOCACHI2)s )")
        mc =    ( "( PT > %(BPT)s )" +
                  " & ( BPVDIRA > %(BDIRA)s )" +
                  " & ( BPVIPCHI2() < %(BIPCHI2)s )" +
                  " & ( BPVVDCHI2 > %(BFDCHI2)s )")
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2NoPIDsPions
        inputs = [ Hlt2NoPIDsPions ]
        Hlt2Combiner.__init__(self, 'B2HH', "B0 -> pi+ pi-", inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

# Combiners
SelB2HH = B2HHCombiner('B2HH')
       

class B2HHLTCombiner(Hlt2Combiner):
    def __init__(self, name):


        picut = ( "( TRCHI2DOF < %(TrChi2)s )" +
                  " & ( PT > %(PionPT)s)" +
                  " & ( PIDK < %(PidCutPions)s )" )
        kcut  = ( "( TRCHI2DOF < %(TrChi2)s )" +
                  " & ( PT > %(PionPT)s)" +
                  " & ( PIDK > %(PidCutKaons)s )" )
        pcut  = ( "( TRCHI2DOF < %(TrChi2)s )" +
                  " & ( PT > %(PionPT)s)" +
                  " & ( PIDp > %(PidCutProtons)s )" )
        dc =    { 'pi+' : picut,
                  'K+'  : kcut,
                  'p+'  : pcut }
        cc =    ( "( (APT1 + APT2) > %(SumPT)s )" +
                  " & ( AM > %(BMassWinLow)s )" +
                  " & ( AM < %(BMassWinHigh)s )" +
                  " & ( ACUTDOCACHI2( %(DOCACHI2)s, '' ) )")
                  #" & ( AMAXDOCACHI2 < %(DOCACHI2)s )" )
        mc =    ( "( BPVDIRA > %(BDIRA)s )" +
                  " & ( BPVIPCHI2() < %(BIPCHI2)s )" +
                  " & ( BPVLTIME() > %(BLT)s )" )
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2RichPions, Hlt2RichKaons, Hlt2RichProtons
        inputs = [ Hlt2RichPions ]
        Hlt2Combiner.__init__(self, 'B2PiPi', "B0 -> pi+ pi-", inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

from Inputs import Hlt2RichPions, Hlt2RichKaons, Hlt2RichProtons
SelB2PiPi = B2HHLTCombiner('B2PiPi') 
SelB2KPi  = SelB2PiPi.clone('B2KPi',decay = '[B0 -> K+ pi-]cc',
                            inputs = [ Hlt2RichKaons, Hlt2RichPions ] )
SelB2KK   = SelB2PiPi.clone('B2KK', decay = 'B0 -> K+ K-',
                            inputs = [ Hlt2RichKaons ] )
SelLb2PK  = SelB2PiPi.clone('Lb2PK',decay = '[Lambda_b0 -> p+ K-]cc',
                            inputs = [ Hlt2RichProtons, Hlt2RichKaons ] )
SelLb2PPi = SelB2PiPi.clone('Lb2PPi',decay = '[Lambda_b0 -> p+ pi-]cc',
                            inputs = [ Hlt2RichKaons, Hlt2RichPions ] )
 
