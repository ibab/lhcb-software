
## @package RichPIDQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: DstarToDzeroPi.py,v 1.5 2009-10-30 17:54:36 powell Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import SelDSTWriter
              
## @class DstarToDzeroPiConf
#  Configurable for RICH D* -> D0(Kpi) pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Andrew Powell
#  @date   15/08/2008
class DstarToDzeroPiConf(RichConfigurableUser) :

    ## Selection Name
    __sel_name__ = "RichDstarToD0PiSel"

    ## Possible used Configurables
    __used_configurables__ = [ (SelDSTWriter, __sel_name__+'DST') ]
    
    ## Steering options
    __slots__ = {
         "Context"         : "Offline"  # The context within which to run
        ,"OutputLevel"     : INFO  # The output level to set all algorithms and tools to use
        ,"Sequencer"    : None     # The sequencer to add the calibration algorithms too
        ,"RunSelection" : True
        ,"RunMonitors"  : False
        ,"MCChecks"     : False
        ,"MakeNTuple"   : False
        ,"MakeSelDST"   : False
        ,"DSTPreScaleFraction" : 1.0
        ,"PlotTools"    : [ ]
        ,"Candidates" : [ "/Marco/To/Set/this" ]
        }

    ## Set general job options
    def setOptions(self,alg):
        if self.isPropertySet("OutputLevel") : alg.OutputLevel = self.getProp("OutputLevel")

    ## Configure Ds -> Phi Pi selection
    def __apply_configuration__(self) :

        from Configurables import ( GaudiSequencer,
                                    CombineParticles, FilterDesktop )
        from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
        
        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequence not set")
        seq = self.getProp("Sequencer")

        if self.getProp("RunSelection") : 

            # STD particles
            from StandardParticles import StdNoPIDsPions,StdNoPIDsKaons
            
            # Filter Pi/K Tracks
            trackfilterName = self.__sel_name__+"_TrackFilter"
            trackfilter = FilterDesktop(trackfilterName)
            trackfilter.Code = "(PT > 0.4*GeV) & (P > 2.0*GeV) & (TRCHI2DOF < 10) & (MIPCHI2DV(PRIMARY) > 6)"
            self.setOptions(trackfilter)
            trackfilterSel = Selection( trackfilterName+'Sel',
                                        Algorithm = trackfilter,
                                        RequiredSelections = [StdNoPIDsPions,StdNoPIDsKaons] )

            # Filter Pi Tracks
            pionfilterName = self.__sel_name__+"_PiFilter"
            pionfilter = FilterDesktop(pionfilterName)
            pionfilter.Code = "(PT > 110*MeV) & (MIPCHI2DV(PRIMARY) > 2)"
            self.setOptions(pionfilter)
            pionfilterSel = Selection( pionfilterName+'Sel',
                                       Algorithm = pionfilter,
                                       RequiredSelections = [StdNoPIDsPions] )

            # Make the D0
            d02kpiName = self.__sel_name__+"_D0ToKPiSel"
            d02kpi = CombineParticles(d02kpiName)
            d02kpi.DecayDescriptor = "[ D0 -> K- pi+ ]cc"
            d02kpi.CombinationCut = "(ADAMASS('D0') < 100*MeV)"
            d02kpi.MotherCut = "(ADMASS('D0') < 50.0*MeV)"\
                               "& (PT > 1.00*GeV)"\
                               "& (VFASPF(VCHI2/VDOF)< 10.0)"\
                               "& (BPVDIRA > 0.9999)"\
                               "& (BPVVDCHI2 > 12)"
            self.setOptions(d02kpi)
            d02kpiSel = Selection( d02kpiName+'Sel',
                                   Algorithm = d02kpi,
                                   RequiredSelections = [trackfilterSel] )

            # Make the D*
            dstar2d0piName = self.__sel_name__
            dstar2d0pi = CombineParticles(dstar2d0piName)
            dstar2d0pi.DecayDescriptor = "[ D*(2010)+ -> D0 pi+ ]cc"
            dstar2d0pi.CombinationCut = "(ADAMASS('D*(2010)+') < 100*MeV)"
            dstar2d0pi.MotherCut = "(ADMASS('D*(2010)+') < 50.0*MeV)"\
                                   "& (PT > 2.2*GeV)"\
                                   "& (VFASPF(VCHI2/VDOF)< 15.0)"\
                                   "& (M-MAXTREE('D0'==ABSID,M)<155.5)"
            self.setOptions(dstar2d0pi)
            dstar2d0piSel = Selection( dstar2d0piName+'Sel',
                                       Algorithm = dstar2d0pi,
                                       RequiredSelections = [d02kpiSel,pionfilterSel] )

            # Selection Sequence
            selSeq = SelectionSequence( self.__sel_name__+'Seq', TopSelection = dstar2d0piSel )

            # Run the selection sequence.
            seq.Members += [selSeq.sequence()]

        # Particle Monitoring plots
        if self.getProp("RunMonitors") :

            from Configurables import ParticleMonitor
            plotter =  ParticleMonitor(self.__sel_name__+"Plots")
            if self.getProp("RunSelection") : 
                plotter.Inputs = [ 'Phys/'+self.__sel_name__+'Sel' ]
            else:
                plotter.Inputs = self.getProp("Candidates")
            plotter.PeakCut     = "(M-MAXTREE('D0'==ABSID,M)<147.43) "\
                                  "& (M-MAXTREE('D0'==ABSID,M)>143.43) "\
                                  "& (INTREE((ABSID=='D0') & (ADMASS('D0') < 15*MeV) ))"
            plotter.SideBandCut = "(M-MAXTREE('D0'==ABSID,M)>147.43) "\
                                  "& (M-MAXTREE('D0'==ABSID,M)<143.43) "\
                                  "& (INTREE((ABSID=='D0') & (ADMASS('D0') > 15*MeV) ))"
            plotter.PlotTools   = self.getProp("PlotTools") 
            self.setOptions(plotter)
            seq.Members += [ plotter ]

        # Make a DST ?
        if self.getProp("MakeSelDST"):

            # Prescale
            from Configurables import DeterministicPrescaler
            scaler = DeterministicPrescaler( self.__sel_name__+'PreScale',
                                             AcceptFraction = self.getProp("DSTPreScaleFraction") )
            seq.Members += [scaler]
            # Write the DST
            MyDSTWriter = SelDSTWriter( self.__sel_name__+"DST",
                                        SelectionSequences = [ selSeq ],
                                        OutputPrefix = self.__sel_name__ )
            seq.Members += [MyDSTWriter.sequence()]
        
        # MC Performance checking ?
        if self.getProp("MCChecks") :

            from Configurables import ParticleEffPurMoni
            mcPerf = ParticleEffPurMoni(dstar2d0piName+"MCPerf")
            mcPerf.Inputs = ["Phys/"+dstar2d0piName]
            self.setOptions(mcPerf)
            seq.Members += [mcPerf]
    
        # Ntuple ?
        if self.getProp("MakeNTuple") : pass
