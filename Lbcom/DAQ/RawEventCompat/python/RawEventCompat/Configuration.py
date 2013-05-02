"""Python module for manipulating the raw event splitting and combining

This module defines both functions and configurables, such that the user can decide which is best.

RecombineWholeEvent: Function. Add up all raw event locations into DAQ/RawEvent

RecombineEventByMap: Function. Add up specific banks from raw event locations


"""
from Gaudi.Configuration import *

####################################################
# Dictionaries
####################################################

#at some point this dictionary will be set from a DBASE package

#dictionary of version: <bank, location>
__locations__={
    #the original raw event, put everything in DAQ/RawEvent
    0.0 : { 
        "ALL" : "DAQ/RawEvent"
        },
    2.0 : {
        'ODIN': "Trigger/RawEvent",
        'L0DU': "Trigger/RawEvent",
        'L0Calo': "Trigger/RawEvent",
        'L0CaloFull': "Trigger/RawEvent",
        'L0DU': "Trigger/RawEvent",
        'L0Muon': "Trigger/RawEvent",
        'L0MuonProcCand': "Trigger/RawEvent",
        'L0PU': "Trigger/RawEvent",
        'HltSelReports': "Trigger/RawEvent",
        'HltDecReports': "Trigger/RawEvent",
        'HltRoutingBits': "Trigger/RawEvent",
        'HltVertexReports': "Trigger/RawEvent",
        'Rich': "Rich/RawEvent",
        'Muon': "Muon/RawEvent",
        'PrsE': "Calo/RawEvent",
        'EcalE': "Calo/RawEvent",
        'HcalE': "Calo/RawEvent",
        'PrsTrig': "Calo/RawEvent",
        'EcalTrig': "Calo/RawEvent",
        'HcalTrig': "Calo/RawEvent",
        'EcalPacked': "Calo/RawEvent",
        'HcalPacked': "Calo/RawEvent",
        'PrsPacked': "Calo/RawEvent",
        'EcalPackedError': "Calo/RawEvent",
        'HcalPackedError': "Calo/RawEvent",
        'PrsPackedError': "Calo/RawEvent"
        }
    }


#at some point this dictionary will be set from a DBASE package

#dictionary of Reco and stripping pass : version
__reco_dict__={ "Reco14" : 2.0, "Strip20" : 2.0 }

####################################################
# Helpers to avoid code duplications
####################################################
def _checkv(version,locations=None,recodict=None):
    "Check the version exists, return the numeric version"
    locations,recodict=_getDict(locations,recodict)
    
    if type(version) is str:
        if version not in recodict:
            raise KeyError("the chosen reconstruction pass is not known "+version)
        else:
            version=recodict[version]
    
    if version not in locations:
        raise KeyError("the chosen version is not known "+str(version))

    return version

def _dod(myCombiner):
    "Add an arbitrary alg to the DOD svc"
    from Configurables import DataOnDemandSvc
    ApplicationMgr().ExtSvc=["DataOnDemandSvc"]+[svc for svc in ApplicationMgr().ExtSvc if svc is not "DataOnDemandSvc"]
    DataOnDemandSvc().AlgMap["DAQ/RawEvent"]=myCombiner
    DataOnDemandSvc().NodeMap[ "DAQ" ]    = "DataObject"

def _getDict(locations=None,recodict=None):
    "Find the latest dictionaries"
    #find the latest dictionaries
    if locations is None:
        locations = RawEventHistory().getProp("Locations")
    if recodict is None:
        recodict = RawEventHistory().getProp("RecoDict")
    return locations,recodict

####################################################
# Simple python functions
####################################################

def ReverseDict(version,locations=None,recodict=None):
    """
    A simple method to return a reversed look up of the location dictionary
    version : which version to look up
    locations: the locations dictionary passed
    """
    #find dictionaries
    locations,recodict=_getDict(locations,recodict)
    #check the options
    version=_checkv(version, locations,recodict)
    
    reversed={}
    for key in locations[version]:
        if locations[version][key] in reversed:
            reversed[locations[version][key]].append(key)
        else:
            reversed[locations[version][key]]=[key]
    
    return reversed

def RecombineEventByMap(version,regex=".*",DoD=True, locations=None, recodict=None):
    """A simple python method to recombine the raw event into DAQ/RawEvent
    Allows fine-tuning of the contents to be copied by specifying a regex of bank names to copy
    
    version: which version to start from, msut be set by user
    regex : regex of known banks to copy, default .*, modify to '(?!(L0)|(Hlt)).*' to ignore all trigger banks, for example
    DoD : Default  True, Ensure data-on-demand is the first in ExtSvc and then add the recombiner to DoD.
    locations: the locations dictionary passed
    recodict: how to link reconstruction/stripping passes to the versions
    
    returns the combiner used
    """
    #find dictionaries
    locations,recodict=_getDict(locations,recodict)
    #regular expressions
    import re
    
    #check the options
    version=_checkv(version, locations)
    
    #configure my combiner
    from Configurables import RawEventMapCombiner
    myCombiner=RawEventMapCombiner("resurectRawEventMap")
    toCopy={}
    for abank in locations[version]:
        if re.match(regex,abank):
            toCopy[abank]=locations[version][abank]
    
    myCombiner.RawBanksToCopy=toCopy
    
    #configure DoD if required
    if DoD:
        _dod(myCombiner)

    return myCombiner

def RecombineWholeEvent(version,DoD=True, regex=".*", locations=None, recodict=None):
    """A simple method to recombine the raw event into DAQ/RawEvent
    Will recombine all contents of all RawEvent locations
    version: which version to start from, must be set by user
    DoD : Default  True, Ensure data-on-demand is the first in ExtSvc and then add the recombiner to DoD.
    regex : regular expression of locations, allowing locations to be ignored
    locations: the locations dictionary passed
    recodict: how to link reconstruction/stripping passes to the versions
    
    returns the combiner used
    """
    #find dictionaries
    locations,recodict=_getDict(locations,recodict)
    import re
    #check the options
    version=_checkv(version, locations)
    
    #configure my combiner
    from Configurables import RawEventSimpleCombiner
    myCombiner=RawEventSimpleCombiner("resurectRawEvent")
    myCombiner.InputRawEventLocations=[aloc for aloc in ReverseDict(version,locations) if re.match(regex,aloc)]
    
    #configure DoD if required
    if DoD:
        _dod(myCombiner)
    
    return myCombiner

####################################################
# Configurables
####################################################
class RawEventHistory(ConfigurableUser):
    "A simple configurable to hold the dictionaries of which event locations are stored where"
    __slots__ = {
        "Locations" : __locations__ #which locations have been where
        , "RecoDict" : __reco_dict__ #which reco version goes to which RawEvent version
        }
    def __apply_configuration__(self):
        pass
     
class RecombineRawEvent(ConfigurableUser):
    "A simple configurable to add the raw event recreation to the DoD service"
    __used_configurables__ = [RawEventHistory]
     
    __slots__ = {
        "Method" : "Simple"  #Simple or Map combiner
        , "Regex" : ".*"  #locations or RawBanks to copy.
        , "Version" : 2.0 #version to copy from
        }
    __known_methods__={
        "Simple": RecombineWholeEvent,
        "Map" : RecombineEventByMap
        }
    
    def __apply_configuration__(self):
        #check arguments
        if self.getProp("Method") not in self.__known_methods__:
            raise KeyError("You have asked for an undefined method ", self.getProp("Method"))
        
        #call correct method
        self.__known_methods__[self.getProp("Method")](
            version=self.getProp("Version"),
            regex=self.getProp("Regex"),
            locations=RawEventHistory().getProp("Locations"),
            recodict=RawEventHistory().getProp("RecoDict")
            )
        
