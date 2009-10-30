
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: DstarToDzeroPi.py,v 1.5 2009-10-30 17:54:36 powell Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
              
## @class DstarToDzeroPiConf
#  Configurable for RICH D* -> D0(Kpi) pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Andrew Powell
#  @date   15/08/2008
class DstarToDzeroPiConf(LHCbConfigurableUser) :

    ## Possible used Configurables
    __used_configurables__ = [ ]
    
    ## Steering options
    __slots__ = {
        "Sequencer"   : None    # The sequencer to add the calibration algorithms too
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
        }

    ## Configure Ds -> Phi Pi selection
    def __apply_configuration__(self) :

        from Configurables import ( GaudiSequencer,
                                    CombineParticles, FilterDesktop )

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequence not set")
        seq = self.getProp("Sequencer")
      
        # Filter Pi/K Tracks
        trackfilterName = "RichDStSelTrackFilter"
        trackfilter = FilterDesktop(trackfilterName)
        trackfilter.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsKaons" ]
        trackfilter.Code = "(PT > 0.4*GeV) & (P > 2.0*GeV) & (MIPCHI2DV(PRIMARY) > 6) & (TRCHI2DOF < 10)"

        # Filter Pi Tracks
        pionfilterName = "RichDStSelPiFilter"
        pionfilter = FilterDesktop(pionfilterName)
        pionfilter.InputLocations = [ "StdNoPIDsPions" ]
        pionfilter.Code = "(PT > 110*MeV) & (MIPCHI2DV(PRIMARY) > 2)"

        # Make the D0
        d02kpiName = "RichD0ToKPiSel"
        d02kpi = CombineParticles(d02kpiName)
        d02kpi.InputLocations = [ trackfilterName ]
        d02kpi.DecayDescriptor = "[ D0 -> K- pi+ ]cc"
        d02kpi.CombinationCut = "(ADAMASS('D0') < 100*MeV)"
        d02kpi.MotherCut = "(ADMASS('D0') < 50.0*MeV)"\
                           "& (PT > 1.00*GeV)"\
                           "& (VFASPF(VCHI2/VDOF)< 10.0)"\
                           "& (BPVDIRA > 0.9999)"\
                           "& (BPVVDCHI2 > 12)"

        # Make the D*
        dstar2d0piName = "RichDstarToD0PiSel"
        dstar2d0pi = CombineParticles(dstar2d0piName)
        dstar2d0pi.InputLocations = [ d02kpiName, pionfilterName ]
        dstar2d0pi.DecayDescriptor = "[ D*(2010)+ -> D0 pi+ ]cc"
        dstar2d0pi.CombinationCut = "(ADAMASS('D*(2010)+') < 100*MeV)"
        dstar2d0pi.MotherCut = "(ADMASS('D*(2010)+') < 50.0*MeV)"\
                               "& (PT > 2.2*GeV)"\
                               "& (VFASPF(VCHI2/VDOF)< 15.0)"\
                               "& (M-MAXTREE('D0'==ABSID,M)<155.5)"

        # Add selection algs to the sequence
        seq.Members += [trackfilter,pionfilter,d02kpi,dstar2d0pi]

        # Particle Monitoring plots
        from Configurables import ( ParticleMonitor, PhysDesktop )
        plotter =  ParticleMonitor(dstar2d0piName+"Plots")
        plotter.InputLocations = [ "Phys/"+dstar2d0piName ]
        plotter.PeakCut     = "(M-MAXTREE('D0'==ABSID,M)<147.43) "\
                              "& (M-MAXTREE('D0'==ABSID,M)>143.43) "\
                              "& (INTREE((ABSID=='D0') & (ADMASS('D0') < 15*MeV) ))"
        plotter.SideBandCut = "(M-MAXTREE('D0'==ABSID,M)>147.43) "\
                              "& (M-MAXTREE('D0'==ABSID,M)<143.43) "\
                              "& (INTREE((ABSID=='D0') & (ADMASS('D0') > 15*MeV) ))"
        plotter.PlotTools = [ "MassPlotTool","MomentumPlotTool",
                              "CombinedPidPlotTool",
                              "RichPlotTool","CaloPlotTool","MuonPlotTool" ]
        seq.Members += [ plotter ]
        
        # MC Performance checking ?
        if self.getProp("MCChecks") :

            from Configurables import ParticleEffPurMoni
            mcPerf = ParticleEffPurMoni(dstar2d0piName+"MCPerf")
            mcPerf.InputLocations = ["Phys/"+dstar2d0piName]
            seq.Members += [mcPerf]
    
        # Ntuple ?
        if self.getProp("MakeNTuple") : pass
