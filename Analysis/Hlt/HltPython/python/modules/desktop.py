#! /usr/bin/env python
# =============================================================================
""" @namespace desktop
@brief Deals with counters and root persistency.

Module that deals with counters and root persistency.
Allows initializing and printing all counters in a single command.
Allows writting all registered ROOT objects in a file in a single command.
See desktopExample() for examples on how to use this module.

@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@author Hugo Ruiz hugo.ruiz@cern.ch
@date 2007-11-28
"""
# =============================================================================


#---------------------------------------------------

from ROOT import *
from PyTree import PyTree
import datetime
import shutil
import cPickle
import commands

DEBUG = False

ROOTOBJECTS = {}
COUNTERS = {}
TOOLS = {}

ALGOS = []

#---------------------------------------------------
# Counters
#---------------------------------------------------
#---------------------------------------------------
def initCounter(name, n = 0):
    """ Initialize counter
    @param name Counter name
    @param n Initial counter value
    @author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
    """
    COUNTERS[name] = n
    return

#---------------------------------------------------
def initCounters(nameList, n = 0):
    """ Initialize counter list
    @param nameList List of counter names
    @param n Initial counter value
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """

    for iName in nameList:
        COUNTERS[iName] = n
    return

#---------------------------------------------------
def increaseCounter(name,n=1):
    """ Increases counter
    @param name Counter name
    @param n Value to be increased
    @author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
    """
    COUNTERS[name] = COUNTERS[name] + n

#---------------------------------------------------
def showCounter(name, refName = False):
    """ Prints counter value in a nice format
    @param name Counter name
    @param refName Name of counter to be used as reference for computation of percentage
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    out =  name+': '+ str(COUNTERS[name])
    if refName and name!= refName:
        if COUNTERS[name] > 0 and COUNTERS[refName] > 0:
            perCent = 100*float(COUNTERS[name])/float(COUNTERS[refName])
            out += '( '+str(perCent)+'% of '+refName+' )'
    print out
    return

#---------------------------------------------------
def showCounters(nameList, refName = False):
    """ Prints list of counter values in a nice format
    @param nameList List of names of counters to be printed
    @param refName Name of counter to be used as reference for computation of percentage
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    for iName in nameList:
        showCounter(iName, refName)
    return

#---------------------------------------------------
def counterValue(name):
    """ Value of the counter
    @param name Counter name
    @returns Value of the counter
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    return COUNTERS[name]

#---------------------------------------------------
def setCounterValue(name, n):
    """ Set the value of a counter
    @param name Counter name
    @returns n Value to be set
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    COUNTERS[name] = n
    return




#---------------------------------------------------
# Tools
#---------------------------------------------------

#---------------------------------------------------
def tool(gaudi,toolname,toolinterface):
    """ retrieve a tool
    @author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
    """
    if (TOOLS.has_key(toolname)): return TOOLS[toolname]
    TOOLS[toolname] = gaudi.toolsvc().create(toolname,interface=toolinterface)
    return TOOLS[toolname]


#---------------------------------------------------
# ROOT Objects
#---------------------------------------------------
#---------------------------------------------------
def register( rootObject ):
    """ Register a ROOT object in main dictionary.
    @param rootOjbect Root object to be registered
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    objectName = rootObject.GetName()

    if objectName in ROOTOBJECTS.keys():
        print 'Already registered object named: ',objectName
        raise RuntimeError

    rootObject.lock = False  
    ROOTOBJECTS[ objectName ] = rootObject

    return

def unlockHistos():
    """ If the histos are loock, unlock them
    """
    for key in ROOTOBJECTS.keys(): ROOTOBJECTS[key].look = False
    
def hfill(name,value,weight=1.,lock=True):
    """ fill a histogram, and lock it for this event (not able to be filled anymore)
    @param name of the histogram
    @param value of the quantity to be histogramed
    @param weight weight of the entry
    @param lock lock this histogram, not be able to filled anymore till unlocked
    @author Jose A. Hernando jose.hernando@cern.ch
    """
    h = my(name)
    if (not h.lock):
        h.Fill(value,weight)
        if (lock): h.lock = True
        
   
def hisnorma(title,norma):
    """ normalize a histogram to a value
    @param title title of the histogram
    @param norma value to normalize
    @author jose.hernando@cern.ch
    """
    h = my(title)
    h.Scale(norma) 


#---------------------------------------------------
def showRegistered():
    """ Print registered objects in an organized way.
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """

    objectList = {}
    for iObjectName in ROOTOBJECTS.keys():
        iClassName = ROOTOBJECTS[iObjectName].ClassName()
        try:
            objectList[iClassName].append(iObjectName)
        except:
            objectList[iClassName] = [iObjectName]
    print objectList
            
#---------------------------------------------------
def deleteRegistered():
    """  Delete all registered objects.
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """

    if not ROOTOBJECTS.keys():
        print 'No objects registered'
        return
    
    for iObjectName in ROOTOBJECTS.keys():
        ROOTOBJECTS[iObjectName].Delete()
    ROOTOBJECTS = 0
    ROOTOBJECTS = {}
    return


#---------------------------------------------------
def saveRegistered(fileName = 'desktop', addDate = True, keepOld = True):
    """ Write all registered ROOT objects in a root file
    @param fileName Name of the root file (without .root extension)
    @param addDate Add _[current date] to file name, to allow keeping different versions
    @param keepOld Rename the existing version of the root file to .old
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    
    # Add _date to avoid loosing data    
    if addDate:
        fileName += '_'+str(datetime.datetime.today().date())+'.root'

    # Keep old version as .old
    if keepOld:
        try:
            shutil.move( fileName, fileName+'.old')
        except:
            pass

    # Done with paranoia. Now just write!    
    outFile = TFile( fileName ,'new','write')
    for iKey in ROOTOBJECTS.keys():
        ROOTOBJECTS[iKey].Write(iKey)
    outFile.Close()    

    return


#---------------------------------------------------
def my( objectName ):
    """ Return a registered object.
    @param objectName Name of the object
    @returns The registered object
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """

    return ROOTOBJECTS[ objectName ]
    

#---------------------------------------------------
def pickleSave( myObject, fileName):
    """ A shortcut to save an object with pickle.
    @param myObject Object to save
    @param fileName File to use
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    print commands.getoutput('mv '+fileName+' '+fileName+'.old')
    f = open(fileName, 'w')
    cPickle.dump(myObject, f)
    f.close()
    return

#---------------------------------------------------
def pickleLoad(fileName):
    """ A shortcut to load an object from a pickle file.
    @param fileName File to use
    @returns The object in the file
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    f = open(fileName, 'r')
    return cPickle.load(f)

#---------------------------------------------------
def addAlgorithm(gaudi,algotype,algoname):
    """ Create an algorithm and add it to gaudi, keep the object in a list ALGOS
    @param gaudi application
    @param algotype the type of the algorithm
    @param algoname the name of the algotithm
    @returns the created algo
    @author jose.hernando@cern.ch
    """
    alg = algotype(algoname)
    gaudi.addAlgorithm(alg)
    ALGOS.append(alg)
    return alg
    

#---------------------------------------------------
def desktopExample():
    """ Example of the functionality of this module.
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """

    # Initialize counters
    counterNameList = ['nEvents','nParticles','nTracks']
    initCounters( counterNameList )
        
    # Register ROOT objects
    register(TH1F('titi','titi title',100,0.,10.))
    register(TH1F('tete','tete title',100,0.,10.))
    register(TH2F('toto','toto title',100,0.,10.,100,0.,10.))

    # Register a pytree (from Gabriel) also
    register(PyTree('myTree', 'myTree')) 
    tree_vars = ['n/I', 'a/F', 'x[2]/F', 'y[n]/F', 'z<>/F']
    my('myTree').book(tree_vars)

    # Some inspection
    print '\nSome inspection:'
    showRegistered()
    print 'Contents of nEvents:',counterValue('nEvents')

    # Event loop
    for i in range(10):

        # In case something is not filled
        my('myTree').setAll0()
        
        increaseCounter('nEvents')

        for j in range(5):

            # Fill histos
            my('titi').Fill(j)
            my('tete').Fill(float(j)/2)
            my('toto').Fill(j,sqrt(float(j)))

            # Increase counters
            increaseCounter('nParticles',j)
            increaseCounter('nTracks')

            # Fill PyTree
            my('myTree').set('n', 3)
            my('myTree').set('a', 3.141592)
            my('myTree').set('x', [0, 1])
            my('myTree').set('y', [0, 1, 2])
            my('myTree').set('z', [0, 1, 2, 3, 4, 5])
            my('myTree').Fill() # always fill in the end of the event loop


    # Draw a histo
    my('toto').Draw()

    # Show counters individually
    print '\nIndividually:'
    showCounter('nEvents')
    showCounter('nParticles', 'nEvents') # nEvents sets the reference for percentages
        
    # Or colectively
    print '\nColectively:'
    showCounters( counterNameList, 'nEvents')
        
    # Save root file
    saveRegistered()
    # saveRegistered('miFile') # if want to specify file name. The '.root' is appended
    
