"""
High level configuration tools for PhysConf
"""
__version__ = "$Id: Configuration.py,v 1.22 2009-11-06 18:09:53 jonrob Exp $"
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
            
            # Test protos are available
            testrecalib = GaudiSequencer("ProtoParticlePreProcessing")
            init.Members += [ testrecalib ]

            # Run alg to check in ProtoParticles are available
            testrecalib.Members += [ TESCheck( "TESCheckProtoParticles", Inputs = ["Rec/ProtoP"], Stop = False ) ]

            # Check PID info
            pidseq = GaudiSequencer("CheckPID")
            testrecalib.Members += [ pidseq ]

            # Check MuonPIDs
            mseq = GaudiSequencer("CheckMuonSeq")
            testrecalib.Members += [ mseq ]
            mseq.Members += [ MuonPIDsFromProtoParticlesAlg("CheckMuonPIDs") ]
            if self.getProp("AllowPIDRerunning") :
                # Optionally rerun MuonPID here. Eventually this should not be done ever in DV
                # but still needed for DC06/MC09 compatibility ...
                mseq.Members += [ DataObjectVersionFilter( "MuonPIDVersionCheck",
                                                           DataObjectLocation = "Rec/Muon/MuonPID",
                                                           MaxVersion = 0 ) ]
                from MuonID import ConfiguredMuonIDs
                from Configurables import MuonRec
                cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
                mseq.Members += [ MuonRec(), cm.getMuonIDSeq() ]
            
            # Check RichPIDs
            rseq = GaudiSequencer("CheckRichSeq")
            testrecalib.Members += [ rseq ]
            rseq.Members += [ RichPIDsFromProtoParticlesAlg("CheckRichPIDs") ]           

            # Do Combined
            recalib = GaudiSequencer("ProtoParticleCombDLLs")
            recalib.IgnoreFilterPassed = True 
            testrecalib.Members += [ recalib ]
            # Add Rich and Muon PID results to protoparticles
            testrecalib.Members += [ChargedProtoParticleAddMuonInfo("ChargedProtoPAddMuon")]
            testrecalib.Members += [ChargedProtoParticleAddRichInfo("ChargedProtoPAddRich")]
            # Combined DLLs
            testrecalib.Members += [ChargedProtoCombineDLLsAlg("ChargedProtoPCombDLL")]

#            if inputtype != 'RDST' :
#                log.info('Doing MuonID for '+self.getProp( "DataType" )+' and '+inputtype)
#                from MuonID import ConfiguredMuonIDs
#                from Configurables import MuonRec, ChargedProtoParticleAddMuonInfo, ChargedProtoCombineDLLsAlg
#                cm=ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
#                MuonIDSeq=cm.getMuonIDSeq()
#                recalib.Members += [ MuonRec(), MuonIDSeq,
#                                     ChargedProtoParticleAddMuonInfo(),
#                                     ChargedProtoCombineDLLsAlg() ]
#           else: # @todo breaks
            # @todo Should use DoD Svc, but there are some problems
#                from Configurables import (MuonPIDsFromProtoParticlesAlg)
#                recalib.Members += [ MuonPIDsFromProtoParticlesAlg("MuonPIDsFromProtos") ]
#            from Configurables import (RichPIDsFromProtoParticlesAlg)
#            recalib.Members += [ RichPIDsFromProtoParticlesAlg("RichPIDsFromProtos") ]
    
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
        
