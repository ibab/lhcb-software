#!/usr/bin/env python
# =============================================================================
# $Id: summary.py,v 1.3 2009-09-15 14:29:52 rlambert Exp $
# =============================================================================
"""
*******************************************************************************
*        Class to contain the python xmlsummary object                        *
*        this simplifies the interface to the schema parser                   *
*        it can also be taken as an example of how to interact                *
*        with the xml schema/VTree object                                     *
*                                                                             * 
*        This class may be expanded with getter functions later               *
*                                                                             * 
*                                                                             * 
* Print: returns an xml string                                                * 
* write: sends output to a file                                               * 
* set_step: is used to set 'initialize'/'finalize' etc.                       * 
* fillfile: used to add information about a file                              * 
* fillcounter: used to create a counter in the xml                            * 
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

# =============================================================================
# Simple algorithm which manipulates with counters 
# =============================================================================
class Summary(VTree):
    """ Simple helper functions for writing the xml summary in python
    This class inherits from the VTree object, and has some hard coded element names
    merging might also be done here"""
    def __init__ ( self, schemafile=__default_schema__ ) :
        """ Constructor. Variables in schemafile are expanded """
        self.__schema__=Schema(schemafile)
        self.__element__=self.__schema__.create_default('summary').__element__
        VTree.__init__(self,self.__element__,self.__schema__,False)
    
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
        
        open_file=None

        #hardcoded tag for file entry :S
        tag='file'

        #no hard coded structure apart from that
        mother=self.__schema__.Tag_mothers(tag)[0]
        #find the new file, or use the existing one
        if isOutput: mother=self.__schema__.Tag_mothers(tag)[1]
        #else: files=self.children('input')[0]
        files=self.find(mother)[0]
        
        alist1={}
        alist2={}
        #use GUID if provided, else use filename
        if GUID is not None:
            alist1['GUID']=GUID
        if len(alist1.keys())==0:
            alist1=None
        if filename is not None:
            if "lfn:" not in filename.lower():
                if "pfn:" not in filename.lower():
                    filename="PFN:"+filename
            alist2['name']=filename
        if len(alist2.keys())==0:
            alist1=None
            
        for alist in [alist1, alist2]:
            if alist is not None:
                open_file=files.children(tag,alist)
                if open_file is not None and len(open_file):
                    #file exists
                    open_file=open_file[0]
                    break
                else: open_file=None
            
        if open_file is None:
                #file needs to be created
                open_file=self.__schema__.create_default('file')
                files.add(open_file)
                if filename is not None:
                    open_file.attrib('name',filename)
                if GUID is not None:
                    open_file.attrib('GUID',GUID)
        #update filename or GUID if only one is present
        elif (GUID is not None) and (filename is not None):
            if ( open_file.attrib('name')==""):
                open_file.attrib('name',filename)
            if ( open_file.attrib('GUID')==""):
                open_file.attrib('GUID',GUID)

                
        #because it's a reference I can do this:
        if status is not None:
            if open_file.attrib('status')=='fail':
                pass
                #a fail is a fail
            elif status=='fail':
                #a fail is a fail
                open_file.attrib('status',status)
            elif open_file.attrib('status')=='mult':
                #mult is mult!
                pass
            elif open_file.attrib('status')=='full':# and status!='full':
                #more than one lot of this file!
                open_file.attrib('status','mult')
            elif status=='none':
                #none is default, don't overwrite anything
                pass
                #open_file.attrib('status',status)
            else:
                open_file.attrib('status',status)
        
        return open_file.value(open_file.value()+addevents)
                  
    def __buildcounter__(self, name, flag, isLumi=False, nEntries=None, flag2=None, min=None,  max=None):
        '''method to build a VTree counter'''
        #pick the daughter to fill
        type="counter"
        attrib="name"
        
        if isLumi:
            if nEntries is not None: type="lumiStatEntity"
            else: type="lumiCounter"
        elif nEntries is not None:
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
        for f in cnt.format():
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
    
    def isFailure(self):
        """the opposite of isSuccess, negates what is stored by the success tag"""
        return (self.success()[0].value()==False)
    
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
    
    def fill_VTree_counter(self, counter):
        ''' fill a basic or complex stat counter, or lumi counter, from a VTree
        will merge the counter if it already exists.
        If you want to add, but not merge use the add method'''
        if 'VTree' not in str(type(counter)):
            raise TypeError, 'expected VTree, got '+str(type(counter))+' instead'
        if counter.tag() not in ['statEntity', 'counter', 'lumiCounter', 'lumiStatEntity']:
            raise TypeError, 'expected counter, got '+counter.tag()+' instead'
        mother=self.__schema__.Tag_mothers(counter.tag())[0]

        
        #hard coded, no way around it :S
        attrib='name'
        name=counter.attrib(attrib)
        
        counters=self.children(tag=mother)[0]
        #check if it exists. If not, just add it
        cnt=counters.children(tag=counter.tag(),attrib={attrib:name})
        if cnt is None or len(cnt)==0:
            counters.add(counter)
            return True
        cnt=cnt[0]
        #else, merge the two
        if 'statentity' not in counter.tag().lower():
            cnt.value(cnt.value()+counter.value())
            return True
        #else be clever about the format
        val=cnt.value()
        for i in range(len(cnt.format())):
            for j in range(len(counter.format())):
                if cnt.format()[i]==counter.format()[j]:
                    #all entries are additive
                    val[i]=val[i]+counter.value()[j]
                    break
        cnt.value(val)
        if cnt.max()<counter.max(): cnt.max(counter.max())
        if cnt.min()>counter.min(): cnt.min(counter.min())
        
    def fill_counter(self, name, flag, nEntries=None, flag2=None, min=None, max=None):
        ''' fill a basic or complex stat counter '''
        cnt=self.__buildcounter__(name, flag, False, nEntries, flag2, min, max)
        return self.fill_VTree_counter(cnt)
    
    def fill_lumi(self, name, flag, nEntries=None, flag2=None, min=None, max=None):
        ''' fill a basic or complex Lumi stat counter '''
        cnt=self.__buildcounter__(name, flag, True, nEntries, flag2, min, max)
        return self.fill_VTree_counter(cnt)

    def fill_memory(self, memory, unit='b'):
        '''Method to fill the memory usage information'''
        usage=self.usage()[0]
        astat=usage.children(tag='stat', attrib={'useOf':'MemoryMaximum'})
        if not astat or len(astat)==0:
            astat=self.__schema__.create_default('stat')
            astat.useOf('MemoryMaximum')
            astat.unit(unit)
            astat.value(memory)
            usage.add(astat)
        else:
            astat=astat[0]
            if astat.unit() != unit:
                raise AttributeError, 'I cannot compare two MemoryMaxima when they have different units!'+ (astat.unit())+unit
            if astat.value()<memory:
                astat.value(memory)
    
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
    merged.success()[0].value(flag)
    #merge step
    #enum will be in order
    steps=merged.__schema__.Tag_enumeration(merged.step()[0].tag())
    flag=''
    for asummary in sum_objects:
        if flag==asummary.step()[0].value(): continue
        for step in steps:
            #retain the lowest possible step
            if flag==step: break
            if step==asummary.step()[0].value(): 
                flag=step
                break
    merged.step()[0].value(flag)
    #merge input/output, simple counters, usage
    for asummary in sum_objects:
        for stat in asummary.usage()[0].stat():
            merged.fill_memory(stat.value(),stat.unit())
        for file in asummary.input()[0].file():
            merged.fill_input(file.name(), file.GUID(), file.status(), file.value())
        for file in asummary.output()[0].file():
            merged.fill_output(file.name(), file.GUID(), file.status(), file.value())
        #merge counters
        for cnt in asummary.counters()[0].counter():
            merged.fill_VTree_counter(cnt)
        #merge counters
        for cnt in asummary.lumiCounters()[0].lumiCounter():
            merged.fill_VTree_counter(cnt)
    #merge statCounters
    for asummary in sum_objects:
        #merge statCounters
        for cnt in asummary.counters()[0].statEntity():
            merged.fill_VTree_counter(cnt)
        for cnt in asummary.lumiCounters()[0].lumiStatEntity():
            merged.fill_VTree_counter(cnt)
    return merged
    
    
    
