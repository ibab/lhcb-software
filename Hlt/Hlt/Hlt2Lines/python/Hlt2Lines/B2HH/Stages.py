# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Set of ``stages'' for Hlt2-lines devoted to charmless 2-body B decays.
#  Two stages are present:
#    1) An inclusive selection of all the modes done under the pi+pi- final 
#       state hypothesis. This line use cuts that introduce an acceptance in
#       the decay-time.
#    2) A stage creating exclusive selections for all the modes. This stage
#       don't use cuts that may introduce an acceptance in the decay-time.
#       In order to achieve this the usage of PID variables is necessary.
#
#
#  @author Stefano Perazzini Stefano.Perazzini@cern.ch
#  @date   2015-03-13
#  
#                    $Revision:$
#  Last modification $Date:$
#  by                $Author:$
# =============================================================================
"""  Set of ``stages'' for Hlt2-lines devoted to charmless 2-body B decays.
     Two stages are present:
     1) An inclusive selection of all the modes done under the pi+pi- final 
        state hypothesis. This line use cuts that introduce an acceptance in
        the decay-time.
     2) A stage creating exclusive selections for all the modes. This stage
        don't use cuts that may introduce an acceptance in the decay-time.
        In order to achieve this the usage of PID variables is necessary.
"""
__version__ = "$Revision: $"
__author__  = "Stefano Perazzini Stefano.Perazzini@cern.ch"
__date__    = "2015-03-13"
__all__     = ( 'B2HHCombiner'  ,
                'B2HHLTUnbiasedCombiner'     )

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

from Inputs import Hlt2NoPIDsPions

class B2HHCombiner(Hlt2Combiner):
    def __init__(self, name):

        picut = ( "(TRCHI2DOF < %(TrChi2)s )" +
                  " & (PT > %(PionPT)s)" +
                  " & (MIPCHI2DV(PRIMARY) > %(PionIPCHI2)s)" )
        dc =    { 'pi+' : picut }
        cc =    ( "( (APT1 + APT2) > %(SumPT)s )" +
                  " & ( AM > %(BMassWinLow)s )" +
                  " & ( AM < %(BMassWinHigh)s )" +
                  " & ( ACUTDOCACHI2( %(DOCACHI2)s, '' ) )")
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
       

class B2HHLTUnbiasedCombiner(Hlt2Combiner):
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
        mc =    ( "( BPVDIRA > %(BDIRA)s )" +
                  " & ( BPVIPCHI2() < %(BIPCHI2)s )" +
                  " & ( BPVLTIME() > %(BLT)s )" )

        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2RichPions, Hlt2RichKaons, Hlt2RichProtons
        inputs = [ Hlt2RichPions ]
        Hlt2Combiner.__init__(self, 'B2PiPi', "B0 -> pi+ pi-", inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              tistos = 'TisTosSpec',
                              DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

from Inputs import Hlt2RichPions, Hlt2RichKaons, Hlt2RichProtons
SelB2PiPi = B2HHLTUnbiasedCombiner('B2PiPi') 
SelB2KPi  = SelB2PiPi.clone('B2KPi',decay = '[B0 -> K+ pi-]cc',
                            inputs = [ Hlt2RichKaons, Hlt2RichPions ] )
SelB2KK   = SelB2PiPi.clone('B2KK', decay = 'B0 -> K+ K-',
                            inputs = [ Hlt2RichKaons ] )
SelLb2PK  = SelB2PiPi.clone('Lb2PK',decay = '[Lambda_b0 -> p+ K-]cc',
                            inputs = [ Hlt2RichProtons, Hlt2RichKaons ] )
SelLb2PPi = SelB2PiPi.clone('Lb2PPi',decay = '[Lambda_b0 -> p+ pi-]cc',
                            inputs = [ Hlt2RichProtons, Hlt2RichPions ] )
 
