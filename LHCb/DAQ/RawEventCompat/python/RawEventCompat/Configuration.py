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
# if a list of locations is given, treat as a search path, <bank, [location1, location2...] >
__locations__={
    #the original raw event, as in the pit, and Boole, put everything in DAQ/RawEvent, input for Trigger.
    0.0 : {
        'ODIN': "DAQ/RawEvent",
        'L0DU': "DAQ/RawEvent",
        'L0Calo': "DAQ/RawEvent",
        'L0CaloFull': "DAQ/RawEvent",
        'L0DU': "DAQ/RawEvent",
        'L0Muon': "DAQ/RawEvent",
        'L0MuonProcCand': "DAQ/RawEvent",
        'L0PU': "DAQ/RawEvent",
        'HltSelReports': "DAQ/RawEvent",
        'HltDecReports': "DAQ/RawEvent",
        'HltRoutingBits': "DAQ/RawEvent",
        'HltVertexReports': "DAQ/RawEvent",
        'Rich': "DAQ/RawEvent",
        'Muon': "DAQ/RawEvent",
        'PrsE': "DAQ/RawEvent",
        'EcalE': "DAQ/RawEvent",
        'HcalE': "DAQ/RawEvent",
        'PrsTrig': "DAQ/RawEvent",
        'EcalTrig': "DAQ/RawEvent",
        'HcalTrig': "DAQ/RawEvent",
        'EcalPacked': "DAQ/RawEvent",
        'HcalPacked': "DAQ/RawEvent",
        'PrsPacked': "DAQ/RawEvent",
        'EcalPackedError': "DAQ/RawEvent",
        'HcalPackedError': "DAQ/RawEvent",
        'PrsPackedError': "DAQ/RawEvent"
        },
    #stripping 17-like, all in DAQ/RawEvent, and some things also in Trigger/RawEvent
    1.0 : {
        'ODIN': ["Trigger/RawEvent","DAQ/RawEvent"],
        'L0DU': ["Trigger/RawEvent","DAQ/RawEvent"],
        'L0Calo': ["Trigger/RawEvent","DAQ/RawEvent"],
        'L0CaloFull': ["Trigger/RawEvent","DAQ/RawEvent"],
        'L0DU': ["Trigger/RawEvent","DAQ/RawEvent"],
        'L0Muon': ["Trigger/RawEvent","DAQ/RawEvent"],
        'L0MuonProcCand': ["Trigger/RawEvent","DAQ/RawEvent"],
        'L0PU': ["Trigger/RawEvent","DAQ/RawEvent"],
        'HltSelReports': ["Trigger/RawEvent","DAQ/RawEvent"],
        'HltDecReports': ["Trigger/RawEvent","DAQ/RawEvent"],
        'HltRoutingBits': ["Trigger/RawEvent","DAQ/RawEvent"],
        'HltVertexReports': ["Trigger/RawEvent","DAQ/RawEvent"],
        'Rich': "DAQ/RawEvent",
        'Muon': "DAQ/RawEvent",
        'PrsE': "DAQ/RawEvent",
        'EcalE': "DAQ/RawEvent",
        'HcalE': "DAQ/RawEvent",
        'PrsTrig': "DAQ/RawEvent",
        'EcalTrig': "DAQ/RawEvent",
        'HcalTrig': "DAQ/RawEvent",
        'EcalPacked': "DAQ/RawEvent",
        'HcalPacked': "DAQ/RawEvent",
        'PrsPacked': "DAQ/RawEvent",
        'EcalPackedError': "DAQ/RawEvent",
        'HcalPackedError': "DAQ/RawEvent",
        'PrsPackedError': "DAQ/RawEvent"
        },
    #stripping 20-like, NO DAQ/RawEvent, everything split up into different places
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
__reco_dict__={ "Reco14" : 2.0, "Strip20" : 2.0, "Reco12" : 1.0, "Strip17" :1.0, "Pit" : 0.0 , "LHCb" : 0.0 , "Moore" : 0.0}

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
        locations = RawEventFormat().getProp("Locations")
    if recodict is None:
        recodict = RawEventFormat().getProp("RecoDict")
    return locations,recodict

####################################################
# Simple python functions
####################################################
def WhereAll(bank,version,locations=None,recodict=None):
    """Return a search path, all possible places to find a bank in a given version"""
    #find dictionaries
    locations,recodict=_getDict(locations,recodict)
    #check the options
    version=_checkv(version, locations,recodict)
    loc=locations[version][bank]
    if type(loc) is str:
        return [loc]
    else:
        return loc
    
def WhereBest(bank,version,locations=None,recodict=None):
    """Return one location, where best to find a given bank in a given processing"""
    return WhereAll(bank,version,locations,recodict)[0]

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
        #if it's a list, treat as a search path, add both
        loc=locations[version][key]
        if type(loc) is not list:
            loc=[loc]
        for aloc in loc:
            if aloc in reversed:
                reversed[aloc].append(key)
            else:
                reversed[aloc]=[key]
    
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
    try:
        from Configurables import RawEventMapCombiner
    except ImportError:
        raise ImportError("You don't have RawEventMapCombiner available, choose another method, or getpack the correct version of DAQ/DAQUtils (v1r5p1, v1r7b, head)")
    myCombiner=RawEventMapCombiner("resurectRawEventMap")
    toCopy={}
    for abank in locations[version]:
        if re.match(regex,abank):
            toCopy[abank]=WhereBest(abank,version,locations,recodict)
    
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
    try:
        from Configurables import RawEventSimpleCombiner
    except ImportError:
        raise ImportError("You don't have RawEventSimpleCombiner available, choose another method, or getpack the correct version of DAQ/DAQUtils (v1r5p1, v1r7b, head)")
    myCombiner=RawEventSimpleCombiner("resurectRawEvent")
    myCombiner.InputRawEventLocations=[aloc for aloc in ReverseDict(version,locations) if re.match(regex,aloc)]

    #configure DoD if required
    if DoD:
        _dod(myCombiner)

    return myCombiner

####################################################
# Configurables
####################################################
class RawEventFormat(ConfigurableUser):
    "A simple configurable to hold the dictionaries of which event locations are stored where. A configurable is one way to do this, allowing users to modify things, or maybe some python shelved dictionary with a method to get at it"
    __slots__ = {
        "Locations" : __locations__ #which locations have been where
        , "RecoDict" : __reco_dict__ #which reco version goes to which RawEvent version
        }
    def __apply_configuration__(self):
        pass

class RecombineRawEvent(ConfigurableUser):
    """A simple configurable to add the raw event recreation to the DoD service
    Only used to recreate the original raw event location for the trigger."""
    #not sure if this is the correct specification here... 
    __used_configurables__ = [RawEventFormat]
    
    __slots__ = {
        "Method" : "Simple"  #Simple or Map combiner
        , "Regex" : ".*"  #locations or RawBanks to copy.
        , "Version" : 2.0 #version to copy from
        }
    _propertyDocDct={
        "Method" : "Simple or Map combiner, see the documentation of the methods in __known_methods__, default Simple"  #Simple or Map combiner
        , "Regex" : "locations or RawBanks to copy. A regular expression such that you can ignore certain locations or bank names. Default .*"  #locations or RawBanks to copy.
        , "Version" : "Version to copy from, float. Default 2.0" #version to copy from
        }
    
    __known_methods__={
        "Simple": RecombineWholeEvent,
        "Map" : RecombineEventByMap
        }
    
    def __apply_configuration__(self):
        #check arguments
        if self.getProp("Method") not in self.__known_methods__:
            raise KeyError("You have asked for an undefined method ", self.getProp("Method"))
        if self.getProp("Version") in [0.0,1.0]:
            raise KeyError("Versions 0.0 and 1.0 anyway have a combined raw event... why would you want to do this? ", self.getProp("Version"))
        loc,rec=_getDict()
        #call correct method
        self.__known_methods__[self.getProp("Method")](
            version=self.getProp("Version"),
            regex=self.getProp("Regex"),
            locations=loc,
            recodict=rec
            )

