#!/usr/bin/env python
# =============================================================================
# $Id: summary.py,v 1.6 2009-09-24 09:28:48 rlambert Exp $
# =============================================================================
"""
*******************************************************************************
*        Class to contain the python xmlsummary object                        *
*        this simplifies the interface to the schema parser                   *
*        it can also be taken as an example of how to interact                *
*        with the xml schema/VTree object                                     *
*                                                                             * 
*        By necessity, the summary object hard-codes the name of several      *
*        of the tags and attributes of the LHCb schema.                       * 
*        This is required for merging and intelligent handling of the objects * 
*                                                                             * 
*                                                                             * 
* Print: returns an xml string                                                * 
* write: sends output to a file                                               * 
* set_step: is used to set 'initialize'/'finalize' etc.                       * 
* fill_file:    used to add information about a file                          * 
* fill_counter: used to create a counter in the xml                           * 
* fill_lumi:    used to create a counter to be stored under lumiCounters      * 
* fill_memory:  used to add memory usage information                          * 
* file_dict:    returns dictionary of input/output files by name and GUID     * 
* count_dict:   returns dictionary of counters/lumiCounters by name           * 
*******************************************************************************
"""
# =============================================================================
__author__ = 'Rob Lambert Rob.Lambert@cern.ch'
# =============================================================================

import os as __os__

#import the schema-handling
try:
    from   XMLSummaryBase.schema import *
except ImportError:
    from schema import *
    #finally fail here if module cannot be found!

__default_schema__='$XMLSUMMARYBASEROOT/xml/XMLSummary.xsd'
__file_tag__ = 'file'
__count_tag__= 'counter'

# =============================================================================
# Simple algorithm which manipulates with counters 
# =============================================================================
class Summary(VTree):
    """ Simple helper functions for writing the xml summary in python
    This class inherits from the VTree object, and has some hard coded element names
    merging might also be done here"""
    #a dictionary of name:object
    #__count_dict__={}
    #a dictionary of name:object
    #__file_dict__={}
    def __init__ ( self, schemafile=__default_schema__ ) :
        """ Constructor. Variables in schemafile are expanded """
        self.__schema__=Schema(schemafile)
        self.__element__=self.__schema__.create_default(self.__schema__.root()).__element__
        self.__count_dict__={}
        for mother in self.__schema__.Tag_mothers(__count_tag__):
            self.__count_dict__[mother]={}
        self.__file_dict__={}
        for mother in self.__schema__.Tag_mothers(__file_tag__):
            self.__file_dict__[mother]={}
        VTree.__init__(self,self.__element__,self.__schema__,False)

    def __file_exists__(self, GUID, filename, isOutput=False):
        '''internal method, return the file if it already exists'''
        mothers=self.__schema__.Tag_mothers(__file_tag__)
        self.file_dict()
        mother=mothers[0]
        if isOutput: mother=mothers[1]
        
        #if it exists, return it
        if (GUID is not None) and (GUID in self.__file_dict__[mother].keys()):
            return self.__file_dict__[mother][GUID]
        elif (filename is not None) and (filename in self.__file_dict__[mother].keys()):
            return self.__file_dict__[mother][filename]

        return None
    
    def __file_merger__(self, destination, filename=None, GUID=None, status=None, addevents=0, isOutput=False):
        '''merge two file objects'''
        mothers=self.__schema__.Tag_mothers(__file_tag__)
        mother=mothers[0]
        if isOutput: mother=mothers[1]
        
        #update filename or GUID if only one is present
        if (GUID is not None) and (filename is not None):
            if ( destination.attrib('name')==""):
                destination.attrib('name',filename)
                self.__file_dict__[mother][filename]=destination
            if ( destination.attrib('GUID')==""):
                destination.attrib('GUID',GUID)
                self.__file_dict__[mother][GUID]=destination
        
        #because it's a reference I can do this:
        if status is not None:
            if destination.attrib('status')=='fail':
                pass
                #a fail is a fail
            elif status=='fail':
                #a fail is a fail
                destination.attrib('status',status)
            elif destination.attrib('status')=='mult':
                #mult is mult!
                pass
            elif destination.attrib('status')=='full':# and status!='full':
                #more than one lot of this file!
                destination.attrib('status','mult')
            elif status=='none':
                #none is default, don't overwrite anything
                pass
                #destination.attrib('status',status)
            else:
                destination.attrib('status',status)
        #print 'at end of merge for file', filename, 'adding', addevents, 'to', destination.value()
        #destination.value(destination.value()+addevents)
        #print destination.value()
        return destination.value(destination.value()+addevents)
                
    def __fill_file__(self, filename=None, GUID=None, status=None, addevents=0, isOutput=False):
        '''Internal common method to fill the file processed information
        filename and GUID are the file to find/create
        status is a new status to set
        the number of events is taken from addevents'''
        if filename is "":
            filename=None
        if GUID is "":
            GUID=None
        if status is "":
            status=None
            
        if filename is None and GUID is None:
            return False
        
        mothers=self.__schema__.Tag_mothers(__file_tag__)
        mother=mothers[0]
        if isOutput: mother=mothers[1]
        
        if filename is not None:
            if "LFN:" not in filename.upper():
                if "PFN:" not in filename.upper():
                    filename="PFN:"+filename
        open_file=self.__file_exists__(GUID,filename,isOutput)

        #hardcoded tag for file entry :S
        
        if open_file is None:
                #file needs to be created
                open_file=self.__schema__.create_default(__file_tag__)
                #print 'default created'
                if (status==None): open_file.attrib('status','none')
                else: open_file.attrib('status',status)
                open_file.value(addevents)
                files=self.children('input')[0]
                if isOutput: files=self.children('output')[0]
                
                if filename is not None:
                    open_file.attrib('name',filename)
                    self.__file_dict__[mother][filename]=open_file
                if GUID is not None:
                    open_file.attrib('GUID',GUID)
                    self.__file_dict__[mother][GUID]=open_file
                files.__append_element__(open_file)
                return True
        
        return self.__file_merger__(open_file, filename,
                            GUID,status,
                            addevents, isOutput)
                  
    def __buildcounter__(self, name, flag, nEntries=None, flag2=None, min=None,  max=None):
        '''method to build a VTree counter'''
        #pick the daughter to fill
        type=__count_tag__
        attrib="name"
        
        #if isLumi:
        #    if nEntries is not None: type="lumiStatEntity"
        #    else: type="lumiCounter"
        if nEntries is not None:
            type="statEntity"
        
        #create the xml object to fill
        cnt=self.__schema__.create_default(type)
        cnt.attrib(attrib,name)
        #if it's a simple counter...
        if nEntries is None: 
            cnt.value(flag)
            return cnt
        #else it's a statEntity
        val=[]
        for f in cnt.attrib('format'):
            if f=='Flag':
                val.append(flag)
            elif f=='Flag2':
                val.append(flag2)
            elif f=='Entries':
                val.append(nEntries)
        cnt.value(val)
        cnt.attrib('max',max)
        cnt.attrib('min',min)
        
        return cnt
    
    def fill_VTree_file(self, file, isOutput=False):
        '''append or merge a vtree file into the tree'''

        if file.attrib('name')!="":
            if "LFN:" not in file.attrib('name').upper():
                if "PFN:" not in file.attrib('name').upper():
                    file.attrib('name',"PFN:"+file.attrib('name'))

        filename=file.attrib('name')
        GUID=file.attrib('GUID')
        open_file=self.__file_exists__(file.attrib('GUID'),file.attrib('name'), isOutput)
        
        
        #if it doesn't exist, just add it
        if open_file is None:
                files=self.children('input')[0]
                if isOutput: files=self.children('output')[0]
                
                files.__append_element__(open_file)
                return True

        #else merge:
        return self.__file_merger__(open_file, file.attrib('name'),
                            file.attrib('GUID'),file.attrib('status'),
                            file.value(), isOutput)
        
    
    def isFailure(self):
        """the opposite of isSuccess, negates what is stored by the success tag"""
        return (self.children('success')[0].value()==False)
    
    def set_step( self, step="initialize", success=False ) :
        """ Set the step entry, and the success entry """
        test=True
        istep=self.children('step')[0]
        test= (istep.value(step) and test)
        
        #example setting success
        isuccess=self.children('success')[0]
        test= (isuccess.value(success) and test)
        
        return test
    
    def parse(self, xmlfile):
        """ parse an existing xml file, and load it into THIS object """
        self.__element__=self.__schema__.parse(xmlfile).__element__
    
    def fill_output(self, filename=None, GUID=None, status=None, addevents=0):
        '''Method to fill the file processed information
        filename and GUID are the file to find/create
        status is a new status to set
        the number of events is taken from addevents'''
        return self.__fill_file__(filename,GUID,status,addevents,True)
    
    def fill_input(self, filename=None, GUID=None, status=None, addevents=0):
        '''Method to fill the file processed information
        filename and GUID are the file to find/create
        status is a new status to set
        the number of events is taken from addevents'''
        return self.__fill_file__(filename,GUID,status,addevents,False)
    
    def fill_VTree_counter(self, counter, isLumi=False):
        ''' fill a basic or complex stat counter, or lumi counter, from a VTree
        will merge the counter if it already exists.
        If you want to add, but not merge use the add method'''
        if 'VTree' not in str(type(counter)):
            raise TypeError, 'expected VTree, got '+str(type(counter))+' instead'
        if counter.tag() not in ['statEntity', __count_tag__]:
            raise TypeError, 'expected counter, got '+counter.tag()+' instead'
        mother=self.__schema__.Tag_mothers(counter.tag())[0]
        if isLumi:
            mother=self.__schema__.Tag_mothers(counter.tag())[1]
        
        self.counter_dict(True)
        
        #hard coded, no way around it :S
        attrib='name'
        name=counter.attrib(attrib)
        
        #check if it exists. If not, just add it
        #print self.__count_dict__
        #print self.__count_dict__[mother]
        if name not in self.__count_dict__[mother].keys():
            counters=self.children(mother)[0]
            #counter=counter.clone()
            counters.__append_element__(counter)
            self.__count_dict__[name]=counter
            return True
        
        #counters=self.children(mother)[0]
        #check if it exists. If not, just add it
        #cnt=counters.children(counter.tag(),attrib={attrib:name})
        #if cnt is None or len(cnt)==0:
        #    counters.add(counter)
        #    return True
        cnt=self.__count_dict__[mother][name]
        #else, merge the two
        if 'statentity' not in counter.tag().lower():
            cnt.value(cnt.value()+counter.value())
            return True
        #else be clever about the format
        val=cnt.value()
        for i in range(len(cnt.attrib('format'))):
            for j in range(len(counter.attrib('format'))):
                if cnt.attrib('format')[i]==counter.attrib('format')[j]:
                    #all entries are additive
                    val[i]=val[i]+counter.value()[j]
                    break
        cnt.value(val)
        if cnt.attrib('max')<counter.attrib('max'): cnt.attrib('max',counter.attrib('max'))
        if cnt.attrib('min')>counter.attrib('min'): cnt.attrib('min',counter.attrib('min'))
        
    def fill_counter(self, name, flag, nEntries=None, flag2=None, min=None, max=None):
        ''' fill a basic or complex stat counter '''
        cnt=self.__buildcounter__(name, flag, nEntries, flag2, min, max)
        return self.fill_VTree_counter(cnt, False)
    
    def fill_lumi(self, name, flag, nEntries=None, flag2=None, min=None, max=None):
        ''' fill a basic or complex Lumi stat counter '''
        cnt=self.__buildcounter__(name, flag, nEntries, flag2, min, max)
        return self.fill_VTree_counter(cnt, True)

    def fill_memory(self, memory, unit='b'):
        '''Method to fill the memory usage information'''
        usage=self.children('usage')[0]
        astat=usage.children('stat', attrib={'useOf':'MemoryMaximum'})
        if not astat or len(astat)==0:
            astat=self.__schema__.create_default('stat')
            astat.attrib('useOf','MemoryMaximum')
            astat.attrib('unit',unit)
            astat.value(memory)
            usage.add(astat)
        else:
            astat=astat[0]
            if astat.attrib('unit') != unit:
                raise AttributeError, 'I cannot compare two MemoryMaxima when they have different units! '+ astat.attrib('unit')+ " " +unit
            if astat.value()<memory:
                astat.value(memory)
    
    def counter_dict(self, recache=False):
        '''return a name:VTree dictionary of counters'''
        mothers=self.__schema__.Tag_mothers(__count_tag__)
        empty=True
        if self.__count_dict__!={}:
            for mother in mothers:
                if self.__count_dict__[mother]!={}:
                    empty=False
        if empty or recache:
            for mother in mothers:
                self.__count_dict__[mother].clear()
                for amother in self.children(mother):
                    for count in amother.children():
                        self.__count_dict__[mother][count.attrib('name')]=count
        return self.__count_dict__
    
    def file_dict(self, recache=False):
        '''return a name:VTree dictionary of all input and output files'''
        mothers=self.__schema__.Tag_mothers(__file_tag__)
        empty=True
        if self.__file_dict__!={}:
            for mother in mothers:
                if self.__file_dict__[mother]!={}:
                    empty=False
        if empty or recache:
            for mother in mothers:
                self.__file_dict__[mother].clear()
                for amother in self.children(mother):
                    for file in amother.children():
                        if(file.attrib('name')!=""): self.__file_dict__[mother][file.attrib('name')]=file
                        if(file.attrib('GUID')!=""): self.__file_dict__[mother][file.attrib('GUID')]=file
        return self.__file_dict__
    
def Merge(summaries, schema=__default_schema__):
    '''Merge a list of summaries, return a new summary
    summaries can be a list of xml files to be parsed, or a list of summary objects'''
    if type([]) != type(summaries): 
        raise TypeError, 'you should send a list into the merger, I got a ' + str(type(summaries)) + ' object instead'
    sum_objects=[]
    if type("")==type(summaries[0]):
        for asummary in summaries:
            #print asummary
            sum_object=Summary(schema)
            sum_object.parse(asummary)
            #print sum_object
            sum_objects.append(sum_object)
    elif str(type(Summary()))== str(type(summaries[0])):
        sum_objects=summaries
        #check they all have the same schema
        schema=sum_objects[0].__schema__.__schemafile_short__
        for asummary in sum_objects:
            if asummary.__schema__.__schemafile_short__ != schema:
                raise AttributeError, 'these xml files have different schema so cannot be merged'
    else:
        raise TypeError, 'you should send strings or Summaries into the merger, I got a ' + str(type(summaries[0])) + ' object instead'
    #make default object
    merged=Summary(schema)
    #merge success
    flag = True
    for asummary in sum_objects:
        if asummary.isFailure():
            flag =False
            break
    merged.children('success')[0].value(flag)
    #merge step
    #enum will be in order
    steps=merged.__schema__.Tag_enumeration(merged.children('step')[0].tag())
    flag=''
    for asummary in sum_objects:
        if flag==asummary.children('step')[0].value(): continue
        for step in steps:
            #retain the lowest possible step
            if flag==step: break
            if step==asummary.children('step')[0].value(): 
                flag=step
                break
    merged.children('step')[0].value(flag)
    #merge input/output, simple counters, usage
    for asummary in sum_objects:
        for stat in asummary.children('usage')[0].children('stat'):
            merged.fill_memory(stat.value(),stat.attrib('unit'))
        for file in asummary.children('input')[0].children():
            merged.fill_input(file.attrib('name'), file.attrib('GUID'), file.attrib('status'), file.value())
        for file in asummary.children('output')[0].children():
            merged.fill_output(file.attrib('name'), file.attrib('GUID'), file.attrib('status'), file.value())
        #merge counters
        for cnt in asummary.children('counters')[0].children(__count_tag__):
            merged.fill_VTree_counter(cnt.clone())
        #merge counters
        for cnt in asummary.children('lumiCounters')[0].children(__count_tag__):
            merged.fill_VTree_counter(cnt.clone(),isLumi=True)
    #merge statCounters
    for asummary in sum_objects:
        #merge statCounters
        for cnt in asummary.children('counters')[0].children('statEntity'):
            merged.fill_VTree_counter(cnt.clone())
        #merge counters
        for cnt in asummary.children('lumiCounters')[0].children('statEntity'):
            merged.fill_VTree_counter(cnt.clone(), isLumi=True)
    return merged
    
    
    
