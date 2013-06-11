"""Python module for manipulating the raw event splitting and combining

This module defines both functions and configurables, such that the user can decide which is best.

RecombineWholeEvent: Function. Add up all raw event locations into DAQ/RawEvent

RecombineEventByMap: Function. Add up specific banks from raw event locations

Uses dictionaries from RawEventFormat, which is in DBASE

"""
from Gaudi.Configuration import *

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
    "Find the latest dictionaries, using the configurable"
    #find the latest dictionaries
    if locations is None:
        if not RawEventFormatConf().isPropertySet("Locations"):
            raise ValueError("RawEventFormatConf Locations not yet specified.To force the loading from a dictionary use RawEventFormatConf().forceLoad()")
        locations = RawEventFormatConf().getProp("Locations")
    if recodict is None:
        if not RawEventFormatConf().isPropertySet("RecoDict"):
            raise ValueError("RawEventFormatConf RecoDict not yet specified.To force the loading from a dictionary use RawEventFormatConf().forceLoad()")
        recodict = RawEventFormatConf().getProp("RecoDict")
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
# Lowest level configurable, interrogates the DB
####################################################
class RawEventFormatConf(ConfigurableUser):
    "A simple configurable to hold the dictionaries of which event locations are stored where. A configurable is one way to do this, allowing users to modify things, or maybe some python shelved dictionary with a method to get at it"
    __slots__ = {
        "Locations" : None #which locations have been where
        , "RecoDict" : None #which reco version goes to which RawEvent version
        }
    _propertyDocDct={
        "Locations" : "which banks have been where, loaded from a databse in RawEventFormat in DBASE." 
        , "RecoDict" : "which reco version goes to which RawEvent version, loaded from a database in RawEventFormat in DBASE."
        }
    def forceLoad(self):
        """
        Force to get the dictionary from the database when called
        """
        try:
            import RawEventFormat
        except ImportError:
            raise ImportError("No RawEventFormat module found, needed for RawEventFormatConf, unless you specify the raw event location dictionaries explicitly")
        self.setProp("Locations",RawEventFormat.Raw_location_db)
        self.setProp("RecoDict",RawEventFormat.Reco_dict_db)
        
    def loadIfRequired(self):
        """
        Only take the dictionary from the DB if it hasn't been explicitly set
        """
        if self.isPropertySet("Locations") and self.isPropertySet("RecoDict"):
            return
        try:
            import RawEventFormat
        except ImportError:
            raise ImportError("No RawEventFormat module found, needed for RawEventFormatConf, unless you specify the raw event location dictionaries explicitly")
        if not self.isPropertySet("Locations"):
            self.setProp("Locations",RawEventFormat.Raw_location_db)
        if not self.isPropertySet("RecoDict"):
            self.setProp("RecoDict",RawEventFormat.Reco_dict_db)
        
    def __apply_configuration__(self):
        #don't do anything if explicitly configured
        print "############ CONFIGURING RawEventFormatConf!! ###############"
        self.loadIfRequired()
        


####################################################
# Higher level configurables, for setting up actions
####################################################
class RecombineRawEvent(ConfigurableUser):
    """A simple configurable to add the raw event recreation to the DoD service
    Only used to recreate the original raw event location for the trigger."""
    #not sure if this is the correct specification here... 
    #__used_configurables__ = [RawEventFormat]
    __queried_configurables__ = [RawEventFormatConf]
    
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

class RawEventJuggler(ConfigurableUser):
    """
    A complex configurable ...
    Juggle around raw event locations, will split up and/or combine anything from anywhere to anywhere else using maps.
    
    Aimed for use during the juggling currently performed in Brunel, and for future use in MC between different Moore versions

    Can configure writers, sequecers and/or DoD
    
    """
    __queried_configurables__ = [RawEventFormatConf]
    
    __slots__ = {
        "Input" : 2.0  #Raw event location to start with
        , "Output" : 0.0 #Raw Event location to go to
        , "KillInputBanksBefore" : None #Regex of banks to kill BEFORE copying around, from input locations
        , "KillInputBanksAfter" : None #Regex of banks to kill AFTER copying around, from input locations
        , "KillExtraNodes" : False #Bool, whether to remove nodes which don't exist in target format
        , "WriterOptItemList" : None #append a writer here to add output locations as OptItemList
        , "WriterItemList" : None #append a writer here to add output locations as ItemList
        , "Sequencer" : None #send in a sequencer to add the juggling into
        , "DataOnDemand" : False #Add juggling into DoD?
        , "TCK" : None
        , "Depth" : "#1"
        , "TCKReplacePattern" : "#TCK#"
        }
    
    def __TCKWrap__(self, aloc):
        if self.getProp("TCKReplacePattern") in aloc:
            return aloc.replace(self.getProp("TCKReplacePattern"),self.getProp("TCK"))
        return aloc
    
    def __opWrap__(self,loc):
        return [self.__TCKWrap__(aloc) +self.getProp("Depth") for aloc in loc]
    
    def __apply_configuration__(self):
        #make sure the dictionaries are there... Hack! %TODO -> Remove this!
        RawEventFormatConf().loadIfRequired()
        #then get them ...
        locs,rec=_getDict()
        added_locations=[]
        output_locations=ReverseDict(self.getProp("Output"),locs,rec)
        input_locations=ReverseDict(self.getProp("Input"),locs,rec)
        #check if a Trigger TCK is needed
        
        if len([loc for loc in input_locations if self.getProp("TCKReplacePattern") in loc]) or len([loc for loc in output_locations if self.getProp("TCKReplacePattern") in loc]):
            if not self.isPropertySet("TCK"):
                raise AttributeError("The raw event version you specified requires a TCK to be given. Set RawEventJuggler().TCK")

        killBefore=[]
        loc_to_alg_dict={}
        killAfter=[]
        
        ######################################
        # Stage 1: KillBefore if requested
        ######################################
        if self.isPropertySet("KillInputBanksBefore"):
            from Configurables import bankKiller
            #if KillBanks or KillNodes... has been requested, I can't use DoD, it's unsafe. I don't know what's "before" and "after"
            if self.getProp("DataOnDemand"):
                raise AttributeError("You have asked for some killing of banks, and asked for DoD, which is not a safe way to run. Either cope with the extra banks, or use a sequencer instead of DoD")
            import re
            for loc in input_locations:
                #see if this location has banks to kill
                killBanks=[abank for abank in input_locations[loc] if re.match(self.getProp("KillInputBanksBefore"),abank)]
                if not len(killBanks):
                    continue
                loc=self.__TCKWrap__(loc)
                bk=bankKiller("killBefore"+loc.replace("/",""))
                killBefore.append(bk)
                bk.RawEventLocations=[loc]
                bk.BankTypes=killBanks
        
        ##################################
        # The part which does the juggling
        ##################################
        if self.getProp("Input")!=self.getProp("Output"):
            ######################################
            # Stage 2: Check options
            ######################################
            #if a bank is needed to be added to an existing location, I can't do that.
            for loc in input_locations:
                if loc in output_locations:
                    extra=[abank for abank in output_locations[loc] if abank not in input_locations[loc]]
                    if len(extra):
                        raise ValueError("I can't add banks to an already existing location right now. Think of another way to do it for "+loc)
            #if banks need to be juggled, tell me how to do it, sequencer or DoD
            if not (self.getProp("DataOnDemand") or self.isPropertySet("Sequencer")):
                raise AttributeError("You have asked for some juggling, but not told me where to put the algorithms. Set either DataOnDemand or pass a Sequencer")
            #if banks need to be juggled, tell me how to do it, sequencer or DoD
            if self.getProp("DataOnDemand") and self.isPropertySet("Sequencer"):
                raise AttributeError("You asked for DoD and also gave a sequencer, I can't do both at the same time")
            
            #if KillBanks or KillNodes... has been requested, I can't use DoD, it's unsafe. I don't know what's "before" and "after"
            if self.getProp("DataOnDemand")  and (self.getProp("KillExtraNodes") or self.getProp("KillInputBanksBefore") or self.getProp("KillInputBanksAfter")):
                raise AttributeError("You have asked for some killing of banks, and asked for DoD, which is not a safe way to run. Either cope with the extra banks, ro use a sequencer instead of DoD")
            
            #raise import error if you don't have the correct requirements
            from Configurables import RawEventMapCombiner, EventNodeKiller, bankKiller
            
            ###############################################
            # Stage 3: Recombine banks from given locations
            ###############################################
            
            #for each new location, add a RawEventMapCombiner to the DoD or to the sequencer
            loc_to_alg_dict={}
            for loc in output_locations:
                if loc not in input_locations:
                    wloc=self.__TCKWrap__(loc)
                    loc_to_alg_dict[wloc]=RawEventMapCombiner("create"+wloc.replace("/",""))
                    loc_to_alg_dict[wloc].OutputRawEventLocation=wloc
                    loc_to_alg_dict[wloc].RawBanksToCopy={}
                    
                    added_locations.append(wloc)
                    
                    for abank in output_locations[loc]:
                        loc_to_alg_dict[wloc].RawBanksToCopy[abank]=WhereBest(abank,self.getProp("Input"),locs,rec)
            
            
            ###############################################
            # Stage 4: Clean up, kill after if requested
            ###############################################
            
            
            killAfter=[]
            if self.isPropertySet("KillInputBanksAfter"):
                import re
                for loc in input_locations:
                    #see if this location has banks to kill
                    killBanks=[abank for abank in input_locations[loc] if re.match(self.getProp("KillInputBanksAfter"),abank)]
                    if not len(killBanks):
                        continue
                    loc=self.__TCKWrap__(loc)
                    bk=bankKiller("killAfter"+loc.replace("/",""))
                    killAfter.append(bk)
                    bk.RawEventLocations=[loc]
                    bk.BankTypes=killBanks
            
            #for each missing location, kill that location?
            if self.getProp("KillExtraNodes"):
                if len([loc for loc in input_locations if loc not in output_locations])>0:
                    enk=EventNodeKiller("KillRawEventNodes")
                    enk.Nodes=[]
                    killAfter.append(enk)
                    for loc in input_locations:
                        if loc not in output_locations:
                            enk.Nodes.append(self.__TCKWrap__(loc))
            
        elif self.isPropertySet("KillInputBanksAfter") or self.getProp("KillExtraNodes"):
            raise AttributeError("You've asked to kill something from the input *after* copying, but you aren't actually doing any copying. Please kill them yourself without this configurable, or use KillInputBanksBefore!")
        
        ###############################################
        # Stage 5: Add to sequence or DoD
        ###############################################
        if self.getProp("DataOnDemand"):
            if len(loc_to_alg_dict):
                from Configurables import DataOnDemandSvc
                ApplicationMgr().ExtSvc=["DataOnDemandSvc"]+[svc for svc in ApplicationMgr().ExtSvc if svc is not "DataOnDemandSvc"]
                for loc in loc_to_alg_dict:
                    DataOnDemandSvc().AlgMap[loc]=loc_to_alg_dict[loc]
        else:
            for alg in killBefore:
                self.getProp("Sequencer").Members.append(alg)
            for loc in loc_to_alg_dict:
                self.getProp("Sequencer").Members.append(loc_to_alg_dict[loc])
            for alg in killAfter:
                self.getProp("Sequencer").Members.append(alg)
            
        ###############################################
        # Stage 6: now handle the writers if appended
        ###############################################
        if self.isPropertySet("WriterOptItemList"):
            self.getProp("WriterOptItemList").OptItemList+=self.__opWrap__(output_locations.keys())
        if self.isPropertySet("WriterItemList"):
            self.getProp("WriterItemList").ItemList+=self.__opWrap__(output_locations.keys())
        
        
