
## @package RichPIDQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: KshortPiPi.py,v 1.6 2009-07-06 16:02:19 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import SelDSTWriter
   
## @class KshortPiPiConf
#  Configurable for RICH Ds -> phi(KK) pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Andrew Powell
#  @date   03/05/2009
class KshortPiPiConf(RichConfigurableUser) :

    ## Selection Name
    __sel_name__ = "RichKsToPiPiSel"
    
    ## Possible used Configurables
    __used_configurables__ = [ (SelDSTWriter, __sel_name__+'DST') ]
    
    ## Steering options
    __slots__ = {
         "Context"             : "Offline"  # The context within which to run
        ,"OutputLevel"         : INFO  # The output level to set all algorithms and tools to use
        ,"Sequencer"           : None    # The sequencer to add the calibration algorithms too
        ,"RunSelection"        : True
        ,"RunMonitors"         : True
        ,"MCChecks"            : False
        ,"MakeNTuple"          : False
        ,"MakeSelDST"          : False
        ,"DSTPreScaleFraction" : 0.01
        ,"PlotTools"           : [ ]
        ,"Candidates"          : [ ]
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
            from StandardParticles import StdNoPIDsPions

            # Filter Pi Tracks
            pionFilterName = self.__sel_name__+"_PiFilter"
            pionfilter = FilterDesktop(pionFilterName)
            pionfilter.Code = "(ISLONG) & (TRCHI2DOF < 5) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 30)"
            self.setOptions(pionfilter)
            pionfilterSel = Selection( pionFilterName+'Sel',
                                       Algorithm = pionfilter,
                                       RequiredSelections = [StdNoPIDsPions] )

            # Make the KS0
            ks02pipi = CombineParticles(self.__sel_name__)
            ks02pipi.DecayDescriptor = "KS0 -> pi+ pi-"
            ks02pipi.CombinationCut = "(ADAMASS('KS0') < 200*MeV) & (AMAXDOCA('') < 0.6*mm)"
            ks02pipi.MotherCut = "(ADMASS('KS0') < 100*MeV) & (VFASPF(VCHI2/VDOF) < 10) & (MIPDV(PRIMARY) < 0.75) & (BPVVDCHI2 > 150) & (MIPCHI2DV(PRIMARY) < 100) & ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > 8*MeV ) & ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > 8*MeV )"
            self.setOptions(ks02pipi)
            ks02pipiSel = Selection( self.__sel_name__+'Sel',
                                     Algorithm = ks02pipi,
                                     RequiredSelections = [pionfilterSel] )

            # Selection Sequence
            selSeq = SelectionSequence( self.__sel_name__+'Seq', TopSelection = ks02pipiSel )

            # Run the selection sequence.
            seq.Members += [selSeq.sequence()]

        # Particle Monitoring plots
        if self.getProp("RunMonitors") :

            from Configurables import ParticleMonitor
            plotter = ParticleMonitor(self.__sel_name__+"Plots")
            if self.getProp("RunSelection") : 
                plotter.Inputs = [ 'Phys/'+self.__sel_name__+'Sel' ]
            else:
                plotter.Inputs = self.getProp("Candidates")
            plotter.PeakCut     = "(ADMASS('KS0')<7*MeV)"
            plotter.SideBandCut = "(ADMASS('KS0')>7*MeV)"
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

            #mcPerfPi = ParticleEffPurMoni("StdNoPIDsPionsMCPerf")
            #mcPerfPi.Inputs = ["Phys/StdNoPIDsPions"]
            #mcPerfPi.OutputLevel = DEBUG
            #seq.Members += [mcPerfPi]
 
            #mcPerfPiFilt = ParticleEffPurMoni(pionFilterName+"MCPerf")
            #mcPerfPiFilt.Inputs = ["Phys/"+pionFilterName]
            #seq.Members += [mcPerfPiFilt]
             
            mcPerfD = ParticleEffPurMoni(ks02pipiName+"MCPerf")
            mcPerfD.Inputs = ["Phys/"+ks02pipiName]
            self.setOptions(mcPerfD)
            seq.Members += [mcPerfD]
                    
        # Ntuple ?
        if self.getProp("MakeNTuple") : pass
        
