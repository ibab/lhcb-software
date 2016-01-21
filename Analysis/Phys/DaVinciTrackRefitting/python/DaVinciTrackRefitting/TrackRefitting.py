## @class TrackRefitting
#  Wrapper to configure tools for the RefitParticleTracks
#  @author Paul Seyfert
#  @data   26/11/2015

__author__ = "Paul Seyfert <Paul.Seyfert@mib.infn.it>"


from LHCbKernel.Configuration import *
from Gaudi.Configuration import appendPostConfigAction
from Gaudi.Configuration import *
from Configurables import TrackInitFit, TrackMasterFitter
from TrackFitter.ConfiguredFitters import ConfiguredMasterFitter
from Configurables import TrackMasterFitter, TrackStateInitTool, FastVeloFitLHCbIDs

from AnalysisPython.Logger import getLogger
log = getLogger ( 'TrackRefitting' )

def ConfigureFitter(fituser,fitAsIn="2015",EnergyLossFactor=1.0):
   '''
   provide a configured TrackInitFit for a DaVinciAlgorithm (or tool)

   Keyword arguments:
   fituser -- the Tool/Algorithm which uses a TrackInitFit
   fitAsIn -- which reconstruction should be mimiced, possible values ['2015','EM2015','Reco14']
   EnergyLossFactor -- modify dE/dx in the material to assess the momentum uncertainty due to the uncertainty on the material budget
   '''
   fituser.addTool(TrackInitFit,"TrackInitFit")
   fituser.TrackInitFit.addTool( TrackMasterFitter, name = "Fit" )
   fituser.TrackInitFit.addTool( TrackStateInitTool, name = "Init")
   fituser.TrackInitFit.Init.UseFastMomentumEstimate = True

   if fitAsIn == 'EM2015':
      ConfiguredMasterFitter( getattr(fituser.TrackInitFit, "Fit"), SimplifiedGeometry = True, LiteClusters = True, MSRossiAndGreisen = False )
      log.warning("cannot compute the early measurements ghost probability. I'll give you the tuning from later in 2015.")
   elif fitAsIn == '2015':
      ConfiguredMasterFitter( getattr(fituser.TrackInitFit, "Fit"), SimplifiedGeometry = True, LiteClusters = True, MSRossiAndGreisen = True )
      from Configurables import Run2GhostId
      fituser.addTool(Run2GhostId("Run2GhostId"),name="Run2GhostId")
      fituser.Run2GhostId.InDaVinci = True
   elif fitAsIn == 'Reco14':
      ConfiguredMasterFitter( getattr(fituser.TrackInitFit, "Fit"), SimplifiedGeometry = False, LiteClusters = False, MSRossiAndGreisen = False )
      from Configurables import TrackNNGhostId
      fituser.addTool(Run2GhostId("TrackNNGhostId"),name="Run2GhostId")
      log.warning("ghost probability computation not tested.")
   else:
      log.warning("no known fit configuration, falling back to TrackFitter.ConfiguredFitters.ConfiguredMasterFitter default")
      ConfiguredMasterFitter( getattr(fituser.TrackInitFit, "Fit") )


   from Configurables import StateDetailedBetheBlochEnergyCorrectionTool
   fituser.TrackInitFit.Fit.MaterialLocator.addTool(StateDetailedBetheBlochEnergyCorrectionTool,name="GeneralDedxTool")
   fituser.TrackInitFit.Fit.MaterialLocator.GeneralDedxTool.EnergyLossFactor = EnergyLossFactor
   fituser.TrackInitFit.Fit.Extrapolator.MaterialLocator.addTool(StateDetailedBetheBlochEnergyCorrectionTool,name="GeneralDedxTool")
   fituser.TrackInitFit.Fit.Extrapolator.MaterialLocator.GeneralDedxTool.EnergyLossFactor = EnergyLossFactor


def ProvideRootInTES(fituser,streamname):
   '''
   set RootInTES for something using a TrackInitFit

   Keyword arguments:
   fituser    -- the Tool/Algorithm which uses a TrackInitFit
   streamname -- the usual RootInTES
   '''
   fituser.TrackInitFit.Init.VeloFitterName = "FastVeloFitLHCbIDs"
   fituser.TrackInitFit.Init.addTool(FastVeloFitLHCbIDs, name = "FastVeloFitLHCbIDs")
   fituser.TrackInitFit.RootInTES = streamname
   fituser.TrackInitFit.Fit.RootInTES = streamname
   fituser.TrackInitFit.Init.RootInTES = streamname
   fituser.TrackInitFit.Init.FastVeloFitLHCbIDs.RootInTES = streamname

from Configurables import RefitParticleTracks
class TrackRefitting(object):
   '''
   wrapper around RefitParticleTracks with tool configuration included and automatic trigger of setting RootInTES for the tool chain

   TODO: determine InputType, Stripping version and RootInTES in a more automatic way
   '''
   def __init__(self,name="TrackRefitting",fitAsIn="2015",RootInTES="",Stripping23=False,EnergyLossFactor=1.0, *args, **kwargs):
      '''
      Keyword arguments:
      name             -- the standard name of an Algorithm in Gaudi
      fitAsIn          -- which reconstruction should be mimiced, possible values ['2015','EM2015','Reco14']
      RootInTES        -- the usual RootInTES (cannot be caught from DaVinci since Stripping23)
      EnergyLossFactor -- to study the impact of the material uncertainty, tune the energy loss when propagating through material
      Stripping23      -- since stripping23, the hit managers need to know about the stream (RootInTES) also on fullDST
      args,kwargs      -- arguments to be forwarded to TrackRefitting
      '''
      self._refitter = RefitParticleTracks(name,*args, **kwargs)
      self._fitAsIn=fitAsIn
      self._EnergyLossFactor = EnergyLossFactor
      ConfigureFitter(self._refitter , fitAsIn=fitAsIn, EnergyLossFactor=EnergyLossFactor)
      self.Stripping23 = Stripping23
      self.RootInTES = RootInTES
      def postconfig():
          self.__apply_configuration()
      appendPostConfigAction(postconfig)

   # allow fitAsIn to be set after instantiation of TrackRefitting
   # fitter gets reconfigured when TrackRefitting.fitAsIn is set by the user
   def get_EnergyLossFactor(self):
      return self._EnergyLossFactor
   def set_EnergyLossFactor(self,value):
      self._EnergyLossFactor = value
      ConfigureFitter(self._refitter, fitAsIn=self._fitAsIn, EnergyLossFactor=self._EnergyLossFactor)
   EnergyLossFactor = property(get_EnergyLossFactor,set_EnergyLossFactor)
   def get_fitAsIn(self):
      return self._fitAsIn
   def set_fitAsIn(self,value):
      self._fitAsIn = value
      ConfigureFitter(self._refitter, fitAsIn=value)
   fitAsIn = property(get_fitAsIn,set_fitAsIn)
   def fitter(self):
      return self._refitter
   def __apply_configuration(self):
       '''
       not to be called by users, PLEASE
       will be automatically called through appendPostConfigAction


       NB: http://stackoverflow.com/questions/610883/how-to-know-if-an-object-has-an-attribute-in-python
       --> i don't use hasattr but catch the exception instead
       '''
       from Configurables import DaVinci
       log.info("calling __apply_configuration")
       log.info("DaVinci().InputType is " + DaVinci().InputType)
       try:
         self._refitter.RootInTES = DaVinci().RootInTES
       except AttributeError:
         pass
       try:
         inputtype = DaVinci().InputType
       except AttributeError:
         log.fatal("DaVinci().InputType is not yet specified, but I need to know it now! (sorry)")
         raise
       if self.Stripping23 or DaVinci().InputType == "MDST":
            rit = self.RootInTES
            if DaVinci().InputType == "MDST":
              try:
                 rit_ref = DaVinci().RootInTES
              except AttributeError:
                 log.warning("DaVinci().InputType is set to MDST, but RootInTES is not specified! (yet?)")
              else:
                if rit != rit_ref:
                 log.warning("TrackRefitting.RootInTES and DaVinci().RootInTES don't agree, although you're running on MDST")
                 log.warning("please check!")
            ProvideRootInTES(self._refitter,rit)
            from Configurables import OTRawBankDecoder, PatSeedFit, Tf__OTHitCreator, FastVeloHitManager, Tf__DefaultVeloPhiHitManager, Tf__DefaultVeloRHitManager
            ToolSvc().addTool(OTRawBankDecoder())
            ToolSvc().addTool(PatSeedFit())
            ToolSvc().addTool(Tf__OTHitCreator("OTHitCreator") )
            ToolSvc().addTool(FastVeloHitManager("FastVeloHitManager"))
            ToolSvc().addTool(Tf__DefaultVeloPhiHitManager("DefaultVeloPhiHitManager"))
            ToolSvc().addTool(Tf__DefaultVeloRHitManager("DefaultVeloRHitManager"))
            ToolSvc().OTRawBankDecoder.RootInTES = rit
            ToolSvc().PatSeedFit.RootInTES = rit
            ToolSvc().OTHitCreator.RootInTES = rit
            ToolSvc().FastVeloHitManager.RootInTES = rit
            ToolSvc().DefaultVeloPhiHitManager.RootInTES = rit
            ToolSvc().DefaultVeloRHitManager.RootInTES = rit

   # using as property is not needed at the moment
   def get_RootInTES(self):
      return self._RootInTES
   def set_RootInTES(self,value):
      self._RootInTES = value
   RootInTES = property(get_RootInTES,set_RootInTES)
   
