"""
Module to hold the configurabloid python class for decoder-like objects.
class: Decoder
and also simple manipulators of the database it creates:
validate(db), decodersForBank(db,bank,ignoreActive=False), decoderToLocation(db,location,ignoreActive=False)
"""
from GaudiConf.Manipulations import configurableInstanceFromString, addPrivateToolFromString


class Decoder(object):
    """
    A sort of configurabloid which adds itself to a database of possible decoders to configure.
    
    Gets around the annoying problem that all decoders seem to be written completely differently from each other
    
    Members:
        FullName="" #the full Gaudi name of the low-level configurable
        Active=False # Flags this as an alg to be configured, which somehow writes something on the TES
        Banks={} # list of banks I decode
        Inputs={} # list of input locations or {Property : value dict}
        Outputs={} # list of output locations or {Property: value} dict
        Properties={} # {Property: value dict of misc properties
        PrivateTools=[] #related private toolsmust also be in the DB at configuration time

    Private member:
        __db__ reference to the databse in which I am stored. Can be overwritten if required, or used to validate the database. Used to find the public/private tools.
    """
    FullName="" #the full Gaudi name of the low-level configurable
    Active=False # Flags this as an alg to be configured, which somehow writes something on the TES
    Banks={} # list of banks I decode
    Inputs={} # list of input locations or {Property : value dict}
    Outputs={} # list of output locations or {Property: value} dict
    Properties={} # {Property: value dict of misc properties
    PrivateTools=[] #related private toolsmust also be in the DB at configuration time
    PublicTools=[] #related public tools, must also be in the DB at configuration time
    def __init__(self,fullname,active,banks=[],inputs={},outputs={},properties={},privateTools=[],publicTools=[],conf=None):
        """
        Create a decoder configurabloid. Options:
        fullname: The full Gaudi name for this algorithm, Type/InstanceName
        active: flag in the DB to prompt auto configuration
        banks: list of banks this decoder can work on
        Inputs: dictionary of property->location
        Outputs: dictionary of property->location
        Properties: {Attribute: value}
        PrivateTools: list of possible decoder private tools, which should also be defined in the DB
        PublicTools: list of possible decoder public tools, which should also be defined in the DB
        conf: a database of decoders, a dictionary to add myself to, keyed by my name, it's where I expect to find my tools
        
        PrivateTools and PublicTools are configured at the same time as this decoder, providing that they exist in the DB.
        """
        self.FullName=fullname
        self.Active=active
        self.Banks=banks
        self.Inputs=inputs
        self.Outputs=outputs
        self.Properties=properties
        self.PrivateTools=privateTools
        self.PublicTools=publicTools
        if conf is not None:
            conf[self.FullName]=self
            self.__db__=conf
    def activate(self):
        self.Active=True
    def deactivate(self):
        self.Active=False
    def clone(self, newname):
        """return another copy of this guy with a new name
        deep copy dictionaries,  but not the DB!!"""
        ip=self.Inputs
        op=self.Outputs
        pr=self.Properties
        #construct new dictionaries to avoid having the same objects
        if type(ip) is dict:
            ip=dict(ip)
        if type(op) is dict:
            op=dict(op)
        if type(pr) is dict:
            pr=dict(pr)
        return Decoder(newname,self.Active,self.Banks,ip,op,pr,self.PrivateTools,self.PublicTools,self.__db__)
    def __setprop__(self,top,prop,val):
        """
        Handle tool handles? not 100% sure...
        """
        if "." in prop:
            anattr=prop.split('.')[0]
            newprop=prop.replace(anattr+'.','')
            self.__propagate__(getattr(top,anattr),newprop,val)
        else:
            if hasattr(top,"setProp"):
                top.setProp(prop,val)
            else:
                setattr(top,prop,val)
    
    def __getprop__(self,top,prop):
        """
        Handle tool handles? not 100% sure...
        """
        if "." in prop:
            anattr=prop.split('.')[0]
            newprop=prop.replace(anattr+'.','')
            return self.__getprop__(getattr(top,anattr),newprop)
        else:
            if hasattr(top,"getProp"):
                return top.getProp(prop)
            else:
                return getattr(top,prop)
    
    def overrideInputs(self,input):
        """
        List of input locations to search, set to all daughters
        """
        if not self.isInputSettable():
            raise AttributeError("My input is not settable "+self.FullName)
        if type(self.Inputs) is list and len(self.Inputs):
            self.Inputs=input
        elif type(self.Inputs) is dict and len(self.Inputs):
            for k,ip in self.Inputs.items():
                ensuretype=list
                if ip is None:
                    #determine default type, first get my configurable
                    thedecoder=configurableInstanceFromString(self.FullName)
                    ensuretype=type(self.__getprop__(thedecoder,k))
                else:
                    ensuretype=type(ip)
                if type(input)==ensuretype:
                    self.Inputs[k]=input
                elif type(input) is list and len(input)>0 and ensuretype is str:
                    raise TypeError(self.FullName+" cannot set property of type list to this string, "+self.FullName+" "+input.__str__())
                elif ensuretype is list and type(input) is str:
                    self.Inputs[k]=[input]
                else:
                    raise TypeError(self.FullName+" cannot convert input from type "+ str(type(input)) +" to "+ str(ensuretype))
        #then cascade downwards
        for tool in self.PublicTools+self.PrivateTools:
            if tool in self.__db__:
                if self.__db__[tool].isInputSettable():
                    self.__db__[tool].overrideInputs(input)
    
    def overrideOutputs(self,output):
        """
        List of output locations to search, set to all daughters
        """
        if not self.isOutputSettable():
            raise AttributeError("My output is not settable "+self.FullName)
        if type(self.Outputs) is list and len(self.Outputs):
            self.Outputs=output
        elif type(self.Outputs) is dict and len(self.Outputs):
            for k,op in self.Outputs.items():
                ensuretype=list
                if op is None:
                    #determine default type, first get my configurable
                    thedecoder=configurableInstanceFromString(self.FullName)
                    ensuretype=type(self.__getprop__(thedecoder,k))
                else:
                    ensuretype=type(op)
                if type(output)==ensuretype:
                    self.Outputs[k]=output
                elif type(output) is list and len(output)>0 and ensuretype is str:
                    raise TypeError("Cannot set property of type list to this string, "+self.FullName+" "+output.__str__())
                elif ensuretype is list and type(output) is str:
                    self.Outputs[k]=[output]
                else:
                    raise TypeError(self.FullName+": Cannot convert from type "+ str(type(output)) +" to "+ str(ensuretype))
        #then cascade downwards
        for tool in self.PublicTools+self.PrivateTools:
            if tool in self.__db__:
                if self.__db__[tool].isOutputSettable():
                    self.__db__[tool].overrideOutputs(output)
    
    def listOutputs(self):
        """
        Cascade down the database and find all output locations
        """
        outputs=[]
        if type(self.Outputs) is list:
            outputs=outputs+self.Outputs
        elif type(self.Outputs) is dict:
            for key in self.Outputs:
                if self.Outputs[key] is None:
                    ops=self.__getprop__(configurableInstanceFromString(self.FullName),key)
                    if ops is not None and type(ops) is str:
                        ops=[ops]
                    for op in ops:
                        if op not in outputs:
                            outputs.append(op)
                else:
                    ops=self.Outputs[key]
                    if ops is not None and type(ops) is str:
                        ops=[ops]
                    for op in ops:
                        if op not in outputs:
                            outputs.append(op)
        
        for tool in self.PrivateTools+self.PublicTools:
            if tool in self.__db__:
                thieroutputs=self.__db__[tool].listOutputs()
                outputs=outputs+[val for val in thieroutputs if val not in outputs]
        return outputs
    
    def listInputs(self):
        """
        Cascade down the database and find all input locations
        """
        inputs=[]
        #maybe I'm just a list
        if type(self.Inputs) is list:
            inputs=inputs+self.Inputs
        #or I'm a dict
        elif type(self.Inputs) is dict:
            for key in self.Inputs:
                #do I have to be found from the configurable?
                if self.Inputs[key] is None:
                    #OK, find me then
                    ips=self.__getprop__(configurableInstanceFromString(self.FullName),key)
                    #only add if not already in the list
                    if ips is not None and type(ips) is str:
                        ips=[ips]
                    for ip in ips:
                        if ip not in inputs:
                            inputs.append(ip)
                else:
                    #no? well add me to the list
                    ips=self.Inputs[key]
                    if ips is not None and type(ips) is str:
                        ips=[ips]
                    for ip in ips:
                        if ip not in inputs:
                            inputs.append(ip)
        #cascade down the tools
        for tool in self.PrivateTools+self.PublicTools:
            if tool in self.__db__:
                thierinputs=self.__db__[tool].listInputs()
                inputs=inputs+[val for val in thierinputs if val not in inputs]
        return inputs
    
    def isInputSettable(self):
        """
        Cascade down the database and work out if you can set my inputs
        """
        if (type(self.Inputs) is dict and len(self.Inputs)):
            return True
        for tool in self.PrivateTools+self.PublicTools:
            if tool in self.__db__:
                if self.__db__[tool].isInputSettable():
                    return True
        return False
    
    def isOutputSettable(self):
        """
        Cascade down the database and work out if you can set my outputs
        """
        if (type(self.Outputs) is dict and len(self.Outputs)):
            return True
        if self.__db__ is None:
            return False
        for tool in self.PrivateTools+self.PublicTools:
            if tool in self.__db__:
                if self.__db__[tool].isOutputSettable():
                    return True
        return False
    
    def setup(self,cascade=True,configured=None,onlyInputs=False):
        """
        Return the configured configurable of this algorithm or public tool. Options:
        cascade: whether to follow down and configure the private tools of this alg also.
        configured: internal use only for configuring private tools
        """
        thedecoder=configured
        if thedecoder is None:
            thedecoder=configurableInstanceFromString(self.FullName)
        #configure me
        if type(self.Inputs) is dict:
            for prop in self.Inputs:
                if self.Inputs[prop] is not None:
                    self.__setprop__(thedecoder,prop, self.Inputs[prop])
        if not onlyInputs:
            for prop in self.Properties:
                self.__setprop__(thedecoder,prop, self.Properties[prop])
            if type(self.Outputs) is dict:
                for prop in self.Outputs:
                    if self.Outputs[prop] is not None:
                        self.__setprop__(thedecoder,prop, self.Outputs[prop])
        if not cascade or self.__db__ is None:
            return thedecoder
        #configure public tools
        for atool in self.PublicTools:
            if atool in self.__db__:
                self.__db__[atool].setup(True,onlyInputs=onlyInputs)
            else:
                raise KeyError("Error: "+tool+" not found in DB, set cascade=False, remove this from the list, or re-validate the db")
        #configure private tools
        for atool in self.PrivateTools:
            if atool in self.__db__:
                thetool=addPrivateToolFromString(thedecoder,atool)
                self.__db__[atool].setup(True,thetool,onlyInputs=onlyInputs)
            else:
                raise KeyError("Error: "+tool+" not found in DB, set cascade=False, remove this from the list, or re-validate the db")
        return thedecoder

# =============== Database tools =======================
        
def validate(db):
    for k,v in db.items():
        if k!=v.FullName:
            raise KeyError("A decoder has had its name changed, but wasn't updated in the DB..."+k+" -> "+v.FullName)
        if v.__db__!=db:
            raise AttributeError("A decoder in this database thinks it is not in this database... try again! "+k)
        for tool in v.PublicTools+v.PrivateTools:
            if tool not in db:
                raise KeyError("A decoder is asking for a tool which hasn't been put in this database... try again! "+adecoder.FullName+"-> "+tool)
    return True

def decodersForBank(db,bank,ignoreActive=False):
    """
    Obtain a list of decoders for a given bank
    """
    retlist=[]
    for k,v in db.items():
        if not ignoreActive and not v.Active:
            continue
        if bank in v.Banks:
            retlist.append(v)
    return retlist

def decoderToLocation(db,location,ignoreActive=False):
    """
    Obtain the decoder whose output is going *to* the location specified
    """
    retlist=[]
    for k,v in db.items():
        if not ignoreActive and not v.Active:
            continue
        if location in v.listOutputs():
            retlist.append(v)
    if len(retlist)>1:
        raise ValueError("More than one active decoder for this location "+[d.FullName for d in retlist].__str__())
    if not len(retlist):
        return None
    return retlist[0]
