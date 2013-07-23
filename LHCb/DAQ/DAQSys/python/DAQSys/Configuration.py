from Configurables import RawEventFormatConf
from Gaudi.Configuration import ConfigurableUser
from Configurables import DataOnDemandSvc

class DecodeRawEvent(ConfigurableUser):
    """Complicated beast to handle raw event decoding
    
    set DataOnDemand=True to simulate old configuration style
    
    Nominally this will only be used to overwrite inputs with different inputs in the case that the
    property 'OverrideInputRawEventLocations' is required
    
    """
    __queried_configurables__ = [RawEventFormatConf]
    
    __slots__ = {
        "DataOnDemand" : False #Add decoding into DoD?
        , "Sequencer" : None #Add decoding to this sequence?
        , "DecoderDB" : None #if none, then decoder_db is used
        , "OverrideInputs" : None #use raw event version to override input locations
        }
    def __db__(self):
        db=None
        if self.isPropertySet("DecoderDB"):
            db=self.getProp("DecoderDB")
        if db is None:
            from DAQSys.Decoders import decoder_db
            db=decoder_db
        return db
        
    def allBanks(self):
        """
        Return a list of all raw banks I know how to decode
        """
        retlist=[]
        for k,v in self.__db__().items():
            if not v.Active:
                continue
            retlist=retlist+[b for b in v.Banks if b not in retlist]
        return retlist
    
    def overrideIfRequired(self, adecoder=None,setup=False):
        """
        Override inputs to the decoders, the RawEventLocation,
        with a different location as specified in RawEventFormat DB.
        
        Will complain if what you're trying to go from/to is not possible.
        """
        if (not self.isPropertySet("OverrideInputs")) or (self.getProp("OverrideInputs") is None):
            return
        #load dictionary of possible raw event locations
        RawEventFormatConf().loadIfRequired()
        from RawEventCompat.Configuration import WhereBest, WhereAll
        v=self.getProp("OverrideInputs")
        if adecoder is None:
            #which banks, and which decoders will be overwritten
            banks=self.allBanks()
            b_locs_toset={}
            for b in banks:
                b_locs_toset[b]=WhereBest(b,v)
            reset_list={}
            for b in banks:
                from DAQSys.DecoderClass import decodersForBank
                for d in decodersForBank(self.__db__(),b):
                    if d.listInputs()[0]!=b_locs_toset[b]:
                        if b not in reset_list:
                            reset_list[b]=[d]
                        elif d not in reset_list[b]:
                            reset_list[b].append(d)
                if not len(reset_list):
                    return
            for k,ds in reset_list.iteritems():
                for d in ds:
                    self.overrideIfRequired(d)
            return
        #when a decoder is passed...
        d=adecoder
        dest=WhereBest(d.Banks[0],v)
        
        #does it make sense to overwrite these banks?
        for b in d.Banks:
            if dest!=WhereBest(b,v):
                raise ValueError("The algorithm "+d.FullName+" decodes multiple banks, so they need to all be in the same location!")
        
        #am I able to overwrite these banks?
        if not d.isInputSettable():
            flag=False
            whereall=WhereAll(d.Banks[0],v)
            for d in adecoder.listInputs():
                if d in whereall:
                    flag = True
                    break
            if not flag:
                raise ValueError("I cannot overwrite to this raw version, because the input for the alg "+d.FullName+" is not configurable")
            print "# WARNING: Re-configuration of inputs for "+d.FullName+" was not done, but it might be OK since at least one location is in the default decode list"
            return
        
        #overwrite them if able
        #print "Overriding "+d.FullName+" from "+d.listInputs().__str__()+" to " +str(dest)
        d.overrideInputs(dest)
        if setup:
            d.setup(onlyInputs=True)
    
    def __apply_configuration__(self):
        #only override input locations (if I was asked to)
        self.overrideIfRequired(setup=True)
        #if I was asked to override the locations, I must also propagate this to the configurables, not just the db
        #so I must setup the configurables. If someone else wants different properties,
        #they should have edited the database, not edited the configurables directly, or they should
        #add a postConfigAction to do what they want to change
        
        if not self.isPropertySet("Sequencer") and not self.getProp("DataOnDemand"):
            #then I'm not doing anything
            return
        if self.isPropertySet("Sequencer") and self.getProp("DataOnDemand"):
            raise ValueError("You asked me to do the DoD service *and* a sequencer, but it only make sense to do one of these")
        #if DoD, check that no active algs want to write to the same location...
        foundSoFar=[]
        for k,v in self.__db__().items():
            if not v.Active:
                continue
            thedecoder=v.setup()
            if self.isPropertySet("Sequencer"):
                self.getProp("Sequencer").Members+=[thedecoder]
            #or DoD
            if self.getProp("DataOnDemand"):
                locs=v.listOutputs()
                for loc in locs:
                    if loc in foundSoFar:
                        raise AttributeError("At least two active algs want to write to the same location. Check your DecoderDB! "+loc)
                    DataOnDemandSvc().AlgMap[loc]=thedecoder
        #Done :)
