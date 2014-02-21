from Configurables import RawEventFormatConf
from Gaudi.Configuration import ConfigurableUser
from Configurables import DataOnDemandSvc


def SetEvtClock(bank,db=None):
    """
    Add specific decoder to EvtClockSvc, replace disparate code
    """
    if db is None:
        from DAQSys.Decoders import DecoderDB
        db=DecoderDB
    from DAQSys.DecoderClass import decodersForBank
    odinconfs=decodersForBank(db,bank)
    if len(odinconfs):
        #force to take the same public tool
        publicTool=odinconfs[0].PublicTools[0]
        from Configurables import EventClockSvc
        #force it to use the same public tool as the rest of the ODIN decoding
        EventClockSvc(EventTimeDecoder = publicTool.replace("ToolSvc.","")+":PUBLIC")


class DecodeRawEvent(ConfigurableUser):
    """Complicated beast to handle raw event decoding
    
    set DataOnDemand=True to simulate old configuration style
    
    Nominally this will only be used to overwrite inputs with different inputs in the case that the
    property 'OverrideInputs' is required
    
    """
    __queried_configurables__ = [RawEventFormatConf]
    
    __slots__ = {
        "DataOnDemand" : False #Add decoding into DoD?
        , "Sequencer" : None #Add decoding to this sequence?
        , "DecoderDB" : None #if none, then decoder_db is used
        , "OverrideInputs" : None #use raw event version to override input locations
        , "EvtClockBank" : "ODIN" #use the ODIN bank tool to configure the EvtClock Svc, needs to be an option such that we can override it with RecHeader info
        }
    def __db__(self):
        db=None
        if self.isPropertySet("DecoderDB"):
            db=self.getProp("DecoderDB")
        if db is None:
            from DAQSys.Decoders import DecoderDB
            db=DecoderDB
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

        Will override any decoder which has either been set to active *or* was 'used'
        
        """
        #print "IN OVERRIDE IF REQUIRED!!"
        if (not self.isPropertySet("OverrideInputs")) or (self.getProp("OverrideInputs") is None):
            #print "RETURNING FOR SOME STUPID REASON"
            return
        #load dictionary of possible raw event locations
        RawEventFormatConf().loadIfRequired()
        from RawEventCompat.Configuration import WhereBest, WhereAll, KnownBanks
        v=self.getProp("OverrideInputs")
        
        if adecoder is None:
            #print "Finding the decoders!"
            #which banks, and which decoders will be overwritten
            from DAQSys.DecoderClass import decodersForBank, usedDecoders
            banks=self.allBanks()
            for d in usedDecoders(self.__db__()):
                for b in d.Banks:
                    if b not in banks:
                        banks.append(b)
            b_locs_toset={}
            known=KnownBanks(v)
            for b in banks:
                if b not in known:
                    #I can't reconfigure it if the bank type is not known!
                    from DAQSys.DecoderClass import decodersForBank
                    d=decodersForBank(self.__db__(),b)
                    if len(d):
                        names=[di.FullName for di in d]
                        print "# WARNING: Re-configuration of inputs for "+names.__str__()+" could not be done, because the bank it decodes '"+b+"' does not appear in the list of available banks for target version '"+str(v)+"' try setting this decoder to active=False and configuring yourself if really needed, or manually editing the contents of the RawEventFormat/Compat dictionaries"
                    else:
                        #there are no decoders for this bank, just ignore it
                        pass
                else:
                    #the bank type is known
                    b_locs_toset[b]=WhereBest(b,v)
            reset_list={}
            for b in b_locs_toset:
                from DAQSys.DecoderClass import decodersForBank, usedDecoders
                for d in decodersForBank(self.__db__(),b)+usedDecoders(self.__db__(),b):
                    if d.listInputs()[0]!=b_locs_toset[b]:
                        if b not in reset_list:
                            reset_list[b]=[d]
                        elif d not in reset_list[b]:
                            reset_list[b].append(d)
                if not len(reset_list):
                    return
            for k,ds in reset_list.iteritems():
                for d in ds:
                    self.overrideIfRequired(d, setup=setup)
            return
        #when a decoder is passed...
        d=adecoder
        known=KnownBanks(v)
        #have to skip it if it decodes an unknown bank!
        for b in d.Banks:
            if b not in known:
                raise ValueError("The algorithm "+d.FullName+" should not have been passed into the overrideInputs method, because it decodes '"+b+"' banks which do not exist in the target format version '"+str(v)+"' try setting this decoder to active=False and configuring yourself if really needed, or manually editing the contents of the RawEventFormat/Compat dictionaries")
        dest=WhereBest(d.Banks[0],v)
        
        #does it make sense to overwrite these banks?
        for b in d.Banks:
            if dest!=WhereBest(b,v):
                raise ValueError("The algorithm "+d.FullName+" decodes multiple banks, so they need to all be in the same location!")
        
        #am I able to overwrite these banks?
        if not d.isInputSettable():
            if not d.Active:
                #ignore other not-Active things..., pah, not sure, new to enable overriding all "used" items...
                return
            flag=False
            whereall=WhereAll(d.Banks[0],v)
            for l in adecoder.listInputs():
                if l in whereall:
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
        #check the DB is self-consistent!
        from DAQSys.DecoderClass import validate
        validate(self.__db__())
        #only override input locations (if I was asked to)
        self.overrideIfRequired(setup=True)
        #if I was asked to override the locations, I must also propagate this to the configurables, not just the db
        #so I must setup the configurables. If someone else wants different properties,
        #they should have edited the database, not edited the configurables directly, or they should
        #add a postConfigAction to do what they want to change
        
        #if ODIN is active, then configure the EventTimeDecoder
        from DAQSys.DecoderClass import decodersForBank
        if self.getProp("EvtClockBank") is not None and len(self.getProp("EvtClockBank")):
            SetEvtClock(self.getProp("EvtClockBank"),self.__db__())
        
        if not self.isPropertySet("Sequencer") and not self.getProp("DataOnDemand"):
            #then I'm not doing anything else
            return
        if self.isPropertySet("Sequencer") and self.getProp("DataOnDemand"):
            raise ValueError("You asked me to do the DoD service *and* a sequencer, but it only make sense to do one of these")
        #if DoD, check that no active algs want to write to the same location...
        for k,v in self.__db__().items():
            if not v.Active:
                continue
            thedecoder=v.setup()
            
            #either add to a sequence, respecting dependencies
            if self.isPropertySet("Sequencer"):
                if self.getProp("Sequencer").Members is None:
                    self.getProp("Sequencer").Members=[]
                if thedecoder in self.getProp("Sequencer").Members:
                    continue
                
                #add any requirements first!
                for alg in v.listRequired():
                    depdecoder=self.__db__()[alg].setup()
                    if depdecoder not in self.getProp("Sequencer").Members:
                        self.getProp("Sequencer").Members.append(depdecoder)
                
                self.getProp("Sequencer").Members.append(thedecoder)
            #or DoD
            if self.getProp("DataOnDemand"):
                if DataOnDemandSvc().AlgMap is None or type(DataOnDemandSvc().AlgMap) is not dict:
                    DataOnDemandSvc().AlgMap={}
                locs=v.listOutputs()
                for loc in locs:
                    if loc in DataOnDemandSvc().AlgMap:
                        testname=DataOnDemandSvc().AlgMap[loc]
                        if type(testname) is not str:
                            testname=testname.getFullName()
                        #handle default names!
                        if testname==v.FullName or (
                            testname.split("/")[0]==testname.split("/")[-1]
                            and v.getFullName().split("/")[0]==v.getFullName().split("/")[-1]
                            and testname.split("/")[0]==v.getFullName().split("/")[0]):
                            print "# WARNING: something else configured a decoder already, "+loc+" "+testname
                        else:
                            raise AttributeError("At least two different active algs want to write to the same location. Check your DecoderDB! "+loc+": "+testname+" & "+v.FullName)
                    DataOnDemandSvc().AlgMap[loc]=thedecoder
        
        #Done :)

