"""
Configuration of the Velo Monitoring Histograms
"""
__version__ = "$Id: Configuration.py,v 1.4 2010-02-23 17:04:13 erodrigu Exp $"
__author__  = "Rob Lambert"

from Gaudi.Configuration import *

class VeloRecMonitors(ConfigurableUser):
    __slots__ = {
        "Histograms"   : "OfflineFull"
        , "MoniSequence": None
        , "KnownHistograms" : ['OfflineFull', 'OfflineExpress', 'Expert', 'Online', 'None']
        , "OutputLevel": INFO
        }
    
    _propertyDocDct = { 
       'Histograms'       : """ What level of Histogramming to write """
       , "MoniSequence"   : """ The sequence to which to append the monitors"""
       , "KnownHistograms": """what Histograms settings are known """
       , "OutputLevel"    : """The output level of algs"""
       }
    
    def __apply_configuration__(self):
        '''choose to add the monitors to the sequence or not'''
        if hasattr( self, "Histograms" ):
            
            if self.getProp("Histograms") not in self.getProp("KnownHistograms") :
                RuntimeError("Unknown Histograms option '%s'"%self.getProp("Histograms"))
                #this is an error, perhaps it should throw an exception
                return
            
            #no histograms to be added in this mode
            if self.getProp("Histograms") in ['Online', 'None']: return
            
            #must be 'OfflineFull', 'OfflineExpress' or 'Expert'
            #histograms to be added in this mode            
            if hasattr( self, "MoniSequence" ):
                # set by RecMoniConf to GaudiSequencer('MoniVELOSeq')
                theseq=self.getProp("MoniSequence")
                if theseq is None:
                    #this is an error, I should throw an exception
                    RuntimeError("Monitoring sequence not set")
                    return
                importOptions( '$VELORECMONITORSROOT/options/BrunelMoni_Velo.py' )
        
        return
