__author__  = 'Mika Vesterinen'
__date__    = '$Date: 2015-02-23$'
__version__ = '$Revision: 0.0 $'

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class TrackEffLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale' : {},
                 'GEC' : {'NTRACK_MAX'           : 180},
                 'Common' : {'Slowpi_MinPt' : 120. #MeV
                             , 'D0_MinAM' : 900. #MeV
                             , 'D0_MaxAM' : 2200. #MeV
                             , 'D0_MinSumIP' : 0.2 #0.5 #mm 
                             , 'D0_MinVDZ' : 4.0 #mm
                             , 'D0_MaxVCHI2NDF' : 3.5 #
                             , 'D0_MaxACosDIRADeg' : 1.2 # degrees
                             , 'D0_MaxDOCA' : 0.1 # #mm
                             , 'D0_MinSimpleFitM' : 1865.-450. # MeV
                             , 'D0_MaxSimpleFitM' : 1865.+450. # MeV
                             , 'Dst_MaxAOA'  : 2.5 # degrees
                             , 'Dst_MinAPT'  : 1500. #*MeV 
                             , 'Dst_MaxDTFM' : 2030. #2025. #*MeV 
                             , 'Dst_MaxDTFCHI2NDF' : 3.0 #2.0 #
                             , 'Dst_MaxSimpleFitDeltaMass' : 250. # MeV
                             , 'Tag_MinPT' : 1300. # MeV
                             , 'Tag_MinIPCHI2' : 25. #
                             , 'Tag_MaxTrchi2' : 2.0 #
                             , 'TagK_MinPIDK' : 0. #
                             , 'TagPi_MaxPIDK' : 10. #
                             , 'Probe_MinETA' : 1.9 #
                             , 'Probe_MaxETA' : 5.1 #
                             #, 'Probe_MinIP' : 0.1 #mm
                             , 'Probe_MinIPCHI2' : 9. #
                             #, 'Probe_MaxTrchi2' : 2.0 #
                             }
                 }
    
    def __apply_configuration__(self):
        from Stages import FilteredDstD0ToKpiPionProbe
        from Stages import FilteredDstD0ToKpiKaonProbe
        
        stages = {'D0ToKpiPionProbe'  : [FilteredDstD0ToKpiPionProbe],
                  'D0ToKpiKaonProbe'  : [FilteredDstD0ToKpiKaonProbe]}
        
        from HltLine.HltLine import Hlt2Line
        for (nickname, algos) in self.algorithms(stages).iteritems():
            Hlt2Line('TrackEff_' + nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
