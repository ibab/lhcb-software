"""
High level configuration tools for PhysConf
"""
__version__ = "$Id: Configuration.py,v 1.25 2009-11-11 15:30:56 jonrob Exp $"
__author__  = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions

class PhysConf(LHCbConfigurableUser) :

    __slots__ = {
        "DataType"          : 'MC09'      # Data type, can be ['DC06','2008']
        ,  "Simulation"        : True        # set to True to use SimCond
        ,  "InputType"         : 'DST'       # Hopefully irrelevant
        ,  "AllowPIDRerunning" : True        # Allow, under the correct circumstances, PID reconstruction to be rerun (e.g. MuonID)
        ,  "EnableUnpack"      : None# Enable unpacking of DST.
        }
    
    __used_configurables__ = (
        'CaloDstUnPackConf'   ,
        'OffLineCaloRecoConf' ,
        'OffLineCaloPIDsConf' 
        )
    
#
# configure reconstruction to be redone
#
    def initSequence(self):
        """
        Init Sequence. Called by master application.
        """
        # only one initialisiation do far
        from Configurables import GaudiSequencer
        init = GaudiSequencer("PhysInitSeq")
        self.configureReco(init)
        return init
        
#
# configure reconstruction to be redone
#
    def configureReco(self,init):
        """
        Configure Reconstruction to be redone
        """

        from Configurables  import OffLineCaloRecoConf
        from Configurables  import OffLineCaloPIDsConf 

        ## CaloPIDs 
        caloPIDs = OffLineCaloPIDsConf (
            EnablePIDsOnDemand = True   , ## enable PIDs-On-Demand
            DataType           = self.getProp('DataType')
            )

        ## Calo reco

        caloReco = OffLineCaloRecoConf (
            EnableRecoOnDemand = True     ## enable Reco-On-Demand
            )
        
        ## unpack Calo (?)
        from Configurables import CaloDstUnPackConf 
        unpack = CaloDstUnPackConf()

        if self.isPropertySet('EnableUnpack') :
            unpack.setProp('Enable', self.getProp('EnableUnpack') )

        if unpack.getProp( 'Enable' ) : 
            hypos    = unpack   .getProp( 'Hypos'   )
            reco_old = caloReco .getProp( 'RecList' )
            reco_new = [ h for h in reco_old if h not in hypos ]
            caloReco.RecList = reco_new
            log.warning("PhysConf: CaloReco.RecList is redefined: %s:" %  reco_new )

        #
        # ProtoParticle pre-processing
        #
        inputtype =  self.getProp('InputType').upper()
        if inputtype != 'MDST'  :
            
            from Configurables import ( GaudiSequencer, TESCheck,
                                        MuonPIDsFromProtoParticlesAlg, RichPIDsFromProtoParticlesAlg,
                                        ChargedProtoParticleAddRichInfo, ChargedProtoParticleAddMuonInfo,
                                        ChargedProtoCombineDLLsAlg, DataObjectVersionFilter )

            # Print out TES contents at the start of each event
            #from Configurables import StoreExplorerAlg
            #init.Members += [ StoreExplorerAlg() ]
            
            # Test protos are available
            protoPartPreProcess = GaudiSequencer("ProtoParticlePreProcessing")
            init.Members += [ protoPartPreProcess ]

            # Run alg to check in ProtoParticles are available
            protoPartPreProcess.Members += [ TESCheck( "TESCheckProtoParticles", Inputs = ["Rec/ProtoP"], Stop = False ) ]

            # Check PID info
            pidseq = GaudiSequencer("CheckPID")
            pidseq.IgnoreFilterPassed = True
            protoPartPreProcess.Members += [ pidseq ]

            # Check MuonPIDs
            mseq = GaudiSequencer("CheckMuonSeq")
            pidseq.Members += [ mseq ]
            mseq.Members += [ MuonPIDsFromProtoParticlesAlg("CheckMuonPIDs") ]
            if self.getProp("AllowPIDRerunning") and inputtype != 'RDST' :
                # Optionally rerun MuonPID here. Eventually this really should not be done in DV
                # but still needed for DC06/MC09 compatibility for a while ...
                mseq.Members += [ DataObjectVersionFilter( "MuonPIDVersionCheck",
                                                           DataObjectLocation = "Rec/Muon/MuonPID",
                                                           MaxVersion = 0 ) ]
                # Check raw event is available
                mseq.Members += [ TESCheck( "TESCheckRawEvent", Inputs = ["DAQ/RawEvent"], Stop = False ) ]
                # Run Muon PID
                from MuonID import ConfiguredMuonIDs
                from Configurables import MuonRec
                cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
                mseq.Members += [ MuonRec(), cm.getMuonIDSeq() ]
            
            # Check RichPIDs
            rseq = GaudiSequencer("CheckRichSeq")
            pidseq.Members += [ rseq ]
            rseq.Members += [ RichPIDsFromProtoParticlesAlg("CheckRichPIDs") ]           

            # Do Combined
            recalib = GaudiSequencer("ProtoParticleCombDLLs")
            recalib.IgnoreFilterPassed = True 
            protoPartPreProcess.Members += [ recalib ]
            # Add Rich and Muon PID results to protoparticles
            recalib.Members += [ChargedProtoParticleAddMuonInfo("ChargedProtoPAddMuon")]
            recalib.Members += [ChargedProtoParticleAddRichInfo("ChargedProtoPAddRich")]
            # Combined DLLs
            recalib.Members += [ChargedProtoCombineDLLsAlg("ChargedProtoPCombDLL")]

#
# Data on demand
#
    def dataOnDemand(self):
        """
        dataOnDemand service
        """
        DataOnDemandSvc().NodeMap['/Event/Rec']            = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Rec/Muon']       = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Rec/Rich']       = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Phys']           = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Relations/Phys'] = 'DataObject'
        
        # raw event, not for DC06
        importOptions("$STDOPTS/DecodeRawEvent.py")

        DataOnDemandSvc().OutputLevel = 1

#
# LoKi
#
    def loki(self) :
        """
        Define loki service
        """
        from Configurables import LoKiSvc
        lokiService = LoKiSvc()
        ApplicationMgr().ExtSvc +=  [ lokiService ]
        
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        Apply configuration for Phys
        """
        log.info("Applying Phys configuration")
        self.dataOnDemand()
        self.loki()
        
