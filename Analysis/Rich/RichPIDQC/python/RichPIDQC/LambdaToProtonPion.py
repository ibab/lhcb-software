
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: LambdaToProtonPion.py,v 1.5 2009-07-06 16:02:19 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import SelDSTWriter
                
## @class LambdaToProtonPionConf
#  Configurable for RICH Lambda -> proton pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Andrew Powell
#  @date   03/05/2009
class LambdaToProtonPionConf(RichConfigurableUser) :

    ## Selection Name
    __sel_name__ = "RichLambdaToPrPiSel"

    ## Possible used Configurables
    __used_configurables__ = [ (SelDSTWriter, __sel_name__+'DST') ]
    
    ## Steering options
    __slots__ = {
         "Context"         : "Offline"  # The context within which to run
        ,"OutputLevel"     : INFO  # The output level to set all algorithms and tools to use
        ,"Sequencer"    : None    # The sequencer to add the calibration algorithms too
        ,"RunSelection" : True
        ,"RunMonitors"  : False
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
        ,"MakeSelDST" : False
        ,"DSTPreScaleFraction" : 0.1
        ,"PlotTools" : [ ]
        ,"Candidates" : [ "/Marco/To/Set/This" ]
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
            from StandardParticles import StdNoPIDsPions, StdNoPIDsProtons
      
            # Filter Pi Tracks
            pionfilterName = self.__sel_name__+"_PiFilter"
            pionfilter = FilterDesktop(pionfilterName)
            pionfilter.Code = "(ISLONG) & (TRCHI2DOF < 3) & (PT > 0.1*GeV) & (MIPCHI2DV(PRIMARY) > 9)"
            self.setOptions(pionfilter)
            pionfilterSel = Selection( pionfilterName+'Sel',
                                       Algorithm = pionfilter,
                                       RequiredSelections = [StdNoPIDsPions] )

            # Filter Proton Tracks
            protonfilterName = self.__sel_name__+"_PrFilter"
            protonfilter = FilterDesktop(protonfilterName)
            protonfilter.Code = "(ISLONG) & (TRCHI2DOF < 3) & (PT > 0.4*GeV) & (MIPCHI2DV(PRIMARY) > 9)"
            self.setOptions(protonfilter)
            protonfilterSel = Selection( protonfilterName+'Sel',
                                         Algorithm = protonfilter,
                                         RequiredSelections = [StdNoPIDsProtons] ) 

            # Make the Lambda
            lambda2ppi = CombineParticles(self.__sel_name__)
            lambda2ppi.DecayDescriptor = "[ Lambda0 -> p+ pi- ]cc"
            lambda2ppi.CombinationCut = "(ADAMASS('Lambda0') < 100*MeV) & (AMAXDOCA('') < 0.2*mm)"
            lambda2ppi.MotherCut = "(ADMASS('Lambda0') < 50.0*MeV) & (PT > 0.5*GeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (MIPDV(PRIMARY) < 0.5) & (BPVVDCHI2 > 750) & (MIPCHI2DV(PRIMARY) < 200) & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 15*MeV ) & (LV01 < 0.98) & (LV02 < 0.98) & (LV01 > -0.98) & (LV02 > -0.98)"
            self.setOptions(lambda2ppi)
            lambda2ppiSel = Selection( self.__sel_name__+'Sel',
                                       Algorithm = lambda2ppi,
                                       RequiredSelections = [pionfilterSel,protonfilterSel] )

            # Selection Sequence
            selSeq = SelectionSequence( self.__sel_name__+'Seq', TopSelection = lambda2ppiSel )

            # Run the selection sequence.
            seq.Members += [selSeq.sequence()]

        # Particle Monitoring plots
        if self.getProp("RunMonitors") :
            
            from Configurables import ParticleMonitor
            plotter = ParticleMonitor(self.__sel_name__+"Plots")
            if self.getProp("RunSelection") : 
                plotter.Inputs  = [ 'Phys/'+self.__sel_name__+'Sel' ]
            else:
                plotter.Inputs = self.getProp("Candidates")
            plotter.PeakCut     = "(ADMASS('Lambda0')<2*MeV)"
            plotter.SideBandCut = "(ADMASS('Lambda0')>2*MeV)"
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
            mcPerf = ParticleEffPurMoni(lambda2ppiName+"MCPerf")
            mcPerf.Inputs = ["Phys/"+lambda2ppiName]
            self.setOptions(mcPerf)
            seq.Members += [mcPerf]
                    
        # Ntuple ?
        if self.getProp("MakeNTuple") : pass
        
