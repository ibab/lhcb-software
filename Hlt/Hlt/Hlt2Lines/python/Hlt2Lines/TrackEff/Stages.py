__author__  = 'Mika Vesterinen'
__date__    = '$Date: 2015-02-23$'
__version__ = '$Revision: 0.0 $'
# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Preambulo import D0Preambulo, DstPreambulo

linePrefix = ''

class TrackGEC(Hlt2VoidFilter):
    def __init__(self, name):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules =  Factory().Modules
        for i in [ 'LoKiTrigger.decorators' ] : 
            if i not in modules : modules.append(i)
                                                           
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        Hlt2VoidFilter.__init__(self, linePrefix + 'Track', 'GEC', code, [tracks])

class TrackEffTagFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        ### fixme. need the tag PID cut.
        cut = "(PT > %(Tag_MinPT)s*MeV) & (MIPCHI2DV(PRIMARY) > %(Tag_MinIPCHI2)s) & (TRCHI2DOF <%(Tag_MaxTrchi2)s)"
        Hlt2ParticleFilter.__init__(self, linePrefix+"__TagFilter", name, cut, inputs)

class TrackEffProbeFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = ("(ETA > %(Probe_MinETA)s) & (ETA < %(Probe_MaxETA)s)" 
               #+ " & (TRCHI2DOF <%(Probe_MaxTrchi2)s)" 
               #+ " & (MIPDV(PRIMARY) > %(Probe_MinIP)s *mm)" 
               + " & (MIPCHI2DV(PRIMARY) > %(Probe_MinIPCHI2)s)")
        Hlt2ParticleFilter.__init__(self, linePrefix+"__ProbeFilter", name, cut, inputs)

class TrackEffSlowPionFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = ("(PT > %(Slowpi_MinPt)s)")
        Hlt2ParticleFilter.__init__(self, linePrefix+"__SlowPionFilter", name, cut, inputs)

class D0ToKpiCombiner(Hlt2Combiner):
    def __init__(self, name,inputs):
        cuts = dict(CombinationCut = ("(in_range( %(D0_MinAM)s,AM, %(D0_MaxAM)s ))"
                                      +"& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(D0_MaxDOCA)s)"),
                    MotherCut = ("(BPVVDZ > %(D0_MinVDZ)s*mm) & ((acos(BPVDIRA)*180./3.142) < %(D0_MaxACosDIRADeg)s) & (VFASPF(VCHI2/VDOF)< %(D0_MaxVCHI2NDF)s)"))
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, linePrefix+"D0Combiner", name,"[D0 -> K- pi+]cc", cuts, inputs,
                              dependencies = [TrackGEC(linePrefix + '__TrackGEC'), PV3D('Hlt2')])
        

class D0Filter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax
        cut = "(in_range( %(D0_MinSimpleFitM)s,D0_M, %(D0_MaxSimpleFitM)s ))"
        from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax
        args = {'PreMonitor'  : (Hlt2MonitorMinMax("D0_M","D0 fast fit mass [MeV]", 500,3500,"D0_FastFit_M_in",nbins=50)),
                'PostMonitor'  : (Hlt2MonitorMinMax("D0_M","D0 fast fit mass [MeV]", 500,3500,"D0_FastFit_M_out",nbins=50))
                }
        Hlt2ParticleFilter.__init__(self, linePrefix+"__D0Filter", name, cut, inputs,Preambulo = D0Preambulo(),**args)
        
class DstTagger(Hlt2Combiner):
    def __init__(self, name,inputs):
    ## opening angle between slow pion and D0 flight
        dot_prod = "(ACHILD( VFASPF( VX ),1)*ACHILD(PX,2) + ACHILD( VFASPF( VY ),1)*ACHILD(PY,2) + ACHILD( VFASPF( VZ ),1)*ACHILD(PZ,2))"
        mag = "math.sqrt(ACHILD( VFASPF( VX ),1)**2+ACHILD( VFASPF( VY ),1)**2+ACHILD( VFASPF( VZ ),1)**2)"
        formula = "(180./3.1415)*acos( %(dot_prod)s / (ACHILD(P,2)*%(mag)s) )" %{"dot_prod":dot_prod,
                                                                                 "mag":mag}
        DstCombinationCut = "("+formula+" < %(Dst_MaxAOA)s) & (APT > %(Dst_MinAPT)s *MeV) & (AALLSAMEBPV)"
        DstMotherCut = "(Dst_M - D0_M < %(Dst_MaxSimpleFitDeltaMass)s)" 
        cuts = dict(DaughtersCuts  = {'pi+' : "(PT > %(Slowpi_MinPt)s*MeV)"}, # & (MIPCHI2DV(PRIMARY) < %(Slowpi_MaxIPCHI2)s)"},
                    CombinationCut = (DstCombinationCut),
                    MotherCut =     (DstMotherCut))
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, linePrefix+"__DstTagger",name,"[D*(2010)+ -> D0 pi+]cc", cuts, inputs,
                              dependencies = [TrackGEC(linePrefix + '__TrackGEC'), PV3D('Hlt2')],
                              Preambulo = DstPreambulo())

class DstDTFFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax
        cut = "(DTF_FUN ( M , True ,  'D0' , %(Dst_MaxDTFCHI2NDF)s , %(Dst_MaxDTFM)s+1.0 ) < %(Dst_MaxDTFM)s *MeV)"
        args = {'PreMonitor'  : (Hlt2MonitorMinMax("DTF_FUN(M,True,'D0')","D^{*} M_{DTF}(D^0,PV) [MeV]", 2000,2040,"Dst_DTF_M_in",nbins=50)
                                 +"&"+Hlt2MonitorMinMax("DTF_CHI2NDOF(True,'D0')","DecayTreeFitter D^{*} #chi^{2}/ndf", 0,20,"Dst_DTF_CHI2_in",nbins=50)),
                'PostMonitor'  : (Hlt2MonitorMinMax("DTF_FUN(M,True,'D0')","DecayTreeFitter D^{*} mass [MeV]", 2000,2040,"Dst_DTF_M_out",nbins=50)
                                  +"&"+Hlt2MonitorMinMax("DTF_CHI2NDOF(True,'D0')","DecayTreeFitter D^{*} #chi^{2}/ndf", 0,20,"Dst_DTF_CHI2_out",nbins=50))
                }
        Hlt2ParticleFilter.__init__(self, linePrefix+"__DstDTFFilter", name, cut, inputs,**args)

# Combiners
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2ProbeVeloKaons, Hlt2ProbeVeloPions , Hlt2SlowPions
#SlowPions = TrackEffSlowPionFilter('D0ToKpiPionProbe',inputs = [Hlt2SlowPions])

#### pion probe
ProbePions = TrackEffProbeFilter('D0ToKpiPionProbe',inputs = [Hlt2ProbeVeloPions])
TagKaons   = TrackEffTagFilter('D0ToKpiPionProbe',inputs = [Hlt2LooseKaons])
D0ToKpiPionProbe  = D0ToKpiCombiner('D0ToKpiPionProbe',inputs = [TagKaons,ProbePions])
FilteredD0ToKpiPionProbe  = D0Filter('D0ToKpiPionProbe',inputs = [D0ToKpiPionProbe])
DstD0ToKpiPionProbe  = DstTagger('D0ToKpiPionProbe',inputs=[FilteredD0ToKpiPionProbe,Hlt2LoosePions])
FilteredDstD0ToKpiPionProbe  = DstDTFFilter('D0ToKpiPionProbe',inputs=[DstD0ToKpiPionProbe])

##### kaon probe
ProbeKaons = TrackEffProbeFilter('D0ToKpiKaonProbe',inputs = [Hlt2ProbeVeloKaons])
TagPions   = TrackEffTagFilter('D0ToKpiKaonProbe',inputs = [Hlt2LoosePions])
D0ToKpiKaonProbe   = D0ToKpiPionProbe.clone('D0ToKpiKaonProbe', decay = "[D0 -> pi+ K-]cc",
                                            inputs = [TagPions,ProbeKaons])
FilteredD0ToKpiKaonProbe  = D0Filter('D0ToKpiKaonProbe',inputs = [D0ToKpiKaonProbe])
DstD0ToKpiKaonProbe  = DstTagger('D0ToKpiKaonProbe',inputs=[FilteredD0ToKpiKaonProbe,Hlt2LoosePions])
FilteredDstD0ToKpiKaonProbe  = DstDTFFilter('D0ToKpiKaonProbe',inputs=[DstD0ToKpiKaonProbe])

