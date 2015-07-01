__author__  = 'Mika Vesterinen'
__date__    = '$Date: 2015-02-23$'
__version__ = '$Revision: 0.0 $'

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

class TrackGEC(Hlt2VoidFilter):
    def __init__(self, name):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules =  Factory().Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        Hlt2VoidFilter.__init__(self, "TrackEff" + name, code, [tracks], shared = True, nickname = 'TrackGEC')

class TrackEffTagFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        ### fixme. need the tag PID cut.
        cut = "(PT > %(Tag_MinPT)s) & (MIPCHI2DV(PRIMARY) > %(Tag_MinIPCHI2)s) & (TRCHI2DOF <%(Tag_MaxTrchi2)s)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs)

class D0ToKpiCombiner(Hlt2Combiner):
    def __init__(self, name,inputs):
        combCut   = ("(in_range( %(D0_MinAM)s,AM, %(D0_MaxAM)s ))"
                     + "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(D0_MaxDOCA)s)")
        motherCut = ("(BPVVDZ > %(D0_MinVDZ)s) & ((acos(BPVDIRA)*180./3.142) < %(D0_MaxACosDIRADeg)s) "
                     + "& (VFASPF(VCHI2/VDOF)< %(D0_MaxVCHI2NDF)s)"
                     +"&(in_range( %(D0_MinSimpleFitM)s,D0_M, %(D0_MaxSimpleFitM)s ))")
        from HltTracking.HltPVs import PV3D
        from Preambulo import D0Preambulo
        Hlt2Combiner.__init__(self, name,["[D0 -> K- pi+]cc","[D0 -> K- pi-]cc"], inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec',
                              CombinationCut = combCut, MotherCut = motherCut,
                              Preambulo = D0Preambulo())

class DstTagger(Hlt2Combiner):
    def __init__(self, name,inputs):
        ## opening angle between slow pion and D0 flight
        dot_prod = "(ACHILD( VFASPF( VX ),1)*ACHILD(PX,2) + ACHILD( VFASPF( VY ),1)*ACHILD(PY,2) + ACHILD( VFASPF( VZ ),1)*ACHILD(PZ,2))"
        mag = "math.sqrt(ACHILD( VFASPF( VX ),1)**2+ACHILD( VFASPF( VY ),1)**2+ACHILD( VFASPF( VZ ),1)**2)"
        formula = "(180./3.1415)*acos( %(dot_prod)s / (ACHILD(P,2)*%(mag)s) )" %{"dot_prod":dot_prod,
                                                                                 "mag":mag}

        DstCombinationCut = "("+formula+" < %(Dst_MaxAOA)s) & (APT > %(Dst_MinAPT)s ) & (AALLSAMEBPV)"
        DstMotherCut      = "(Dst_M - D0_M < %(Dst_MaxSimpleFitDeltaMass)s)"
        DaughtersCuts     = {'pi+' : "(PT > %(Slowpi_MinPt)s)"}
        from HltTracking.HltPVs import PV3D
        from Preambulo import DstPreambulo
        Hlt2Combiner.__init__(self, name, "[D*(2010)+ -> D0 pi+]cc", inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              CombinationCut = DstCombinationCut, MotherCut = DstMotherCut,
                              DaughtersCuts = DaughtersCuts, Preambulo = DstPreambulo())

class DstDTFFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax
        cut = "(DTF_FUN ( M , True ,  'D0' , %(Dst_MaxDTFCHI2NDF)s , %(Dst_MaxDTFM)s+1.0 ) < %(Dst_MaxDTFM)s)"
        args = {'PreMonitor'  : (Hlt2MonitorMinMax("DTF_FUN(M,True,'D0')","D^{*} M_{DTF}(D^0,PV) [MeV]", 2000,2040,"Dst_DTF_M_in",nbins=50)
                                 +"&"+Hlt2MonitorMinMax("DTF_CHI2NDOF(True,'D0')","DecayTreeFitter D^{*} #chi^{2}/ndf", 0,20,"Dst_DTF_CHI2_in",nbins=50)),
                'PostMonitor'  : (Hlt2MonitorMinMax("DTF_FUN(M,True,'D0')","DecayTreeFitter D^{*} mass [MeV]", 2000,2040,"Dst_DTF_M_out",nbins=50)
                                  +"&"+Hlt2MonitorMinMax("DTF_CHI2NDOF(True,'D0')","DecayTreeFitter D^{*} #chi^{2}/ndf", 0,20,"Dst_DTF_CHI2_out",nbins=50))
                }
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,**args)

from Inputs import GoodPions, GoodKaons, VeloKaons, VeloPions , SlowPions

#### pion probe
TagKaons   = TrackEffTagFilter('TagK',inputs = [GoodKaons])
D0ToKpiPionProbe  = D0ToKpiCombiner('D0', inputs = [TagKaons,VeloPions])
DstD0ToKpiPionProbe  = DstTagger('Dst',inputs=[D0ToKpiPionProbe,SlowPions])
FilteredDstD0ToKpiPionProbe  = DstDTFFilter('DstDTF',inputs=[DstD0ToKpiPionProbe])

##### kaon probe
TagPions   = TrackEffTagFilter('TagPi',inputs = [GoodPions])
D0ToKpiKaonProbe   = D0ToKpiPionProbe.clone('D0', decay = ["[D0 -> pi+ K-]cc","[D0 -> pi+ K+]cc"],
                                            inputs = [TagPions,VeloKaons])
DstD0ToKpiKaonProbe  = DstTagger('Dst',inputs=[D0ToKpiKaonProbe,SlowPions])
FilteredDstD0ToKpiKaonProbe  = DstDTFFilter('DstDTF',inputs=[DstD0ToKpiKaonProbe])
