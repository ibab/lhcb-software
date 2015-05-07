# =============================================================================
## @file
#  @author Sascha Stahl
#  @date 2015-04-08
#  script to configure Hlt lines for high multiplicity
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt1HighMultLinesConf(HltLinesConfigurableUser):
    
    __slots__ = { 'MinVeloHits'        : 2400,
                  'MaxVeloHits'        : 99999,
                  'nPVs'               :  1 ,
                  'MinVeloHits_PV'     : 2400,
                  'ODIN'               : 'jbit( ODIN_EVTTYP,2 )'
                  }

    def __filterVeloHits(self,name,minVeloHits,maxVeloHits):
        from Configurables import LoKi__VoidFilter
        fltr_veloHits =  LoKi__VoidFilter ( "Hlt1" + name + "Decision"
                                            , Preambulo = ['from LoKiPhys.decorators import *','from LoKiCore.functions import *']
                                            , Code = "in_range( %(MinVeloHits)s , CONTAINS('Raw/Velo/LiteClusters') , %(MaxVeloHits)s )" % {"MinVeloHits":minVeloHits,"MaxVeloHits":maxVeloHits})
        return fltr_veloHits
    
    def __apply_configuration__(self):
        
        from HltLine.HltDecodeRaw import DecodeOT
        from HltLine.HltLine import Hlt1Line
        from Configurables import HltCosmicsOT
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        
        from HltTracking.HltSharedTracking import MinimalVelo, HltHPTTracking
        from HltLine.HltLine import Hlt1Member 
        from HltLine.HltLine import Hlt1Line
        from HltLine.HltDecodeRaw import DecodeVELO
        from Configurables import LoKi__VoidFilter
        from HltTracking.HltPVs import PV3D
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit

        fltr_veloHits = self.__filterVeloHits("HighVeloMult",self.getProp("MinVeloHits"), self.getProp("MaxVeloHits"))
        fltr_veloHits_PV = self.__filterVeloHits("HighVeloMultSinglePV",self.getProp("MinVeloHits_PV"), self.getProp("MaxVeloHits"))
          
        pvReco = PV3D('Hlt1')
        fltr_nPVs =  LoKi__VoidFilter ( 'HighVeloMultSinglePV_NPVsFilter'
                                        , Code = " CONTAINS('%(pvLoc)s')==%(nPVs)s " \
                                        % {"pvLoc" : pvReco.output, "nPVs": self.getProp("nPVs")})

        Hlt1Line('HighVeloMult',
                 prescale  = self.prescale,
                 postscale = self.postscale,
                 ODIN      = self.getProp('ODIN'),
                 algos     = [ Hlt1GECUnit('Activity') # cut on a minimum of hits in the tracking detectors
                               , fltr_veloHits         # select high velo multiplicity events
                               ]
                 )
        
        Hlt1Line('HighVeloMultSinglePV',
                 prescale  = self.prescale,
                 postscale = self.postscale,
                 ODIN      = self.getProp('ODIN'),
                 algos     = [ Hlt1GECUnit('Activity') # cut on a minimum of hits in the tracking detectors
                               , pvReco                # Reconstruct PVs
                               , fltr_nPVs             # Filter on exactly 1 PV
                               , fltr_veloHits_PV      # select high velo multiplicity events
                               ]
                 )

        
