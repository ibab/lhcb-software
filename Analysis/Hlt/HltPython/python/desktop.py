#! /usr/bin/env python
#  @date 2007-11-28
#  @author Hugo RUIZ hugo.ruiz@cern.ch
#  @author Jose Angel HERNANDO jose.angel.hernando-morata@cern.ch
# =============================================================================
""" Module that deals with counters and root persistency"""
# =============================================================================
__author__ = "Hugo RUIZ hugo.ruiz@cern.ch"
__author__ = "Jose Angel HERNANDO jose.angel.hernando-morata@cern.ch"
# =============================================================================


#---------------------------------------------------

from math import *
from ROOT import *
from PyTree import *
import datetime
import shutil

DEBUG = False

ROOTOBJECTS = {}
COUNTERS = {}
TOOLS = {}

#---------------------------------------------------
# Counters
#---------------------------------------------------
#---------------------------------------------------
def initCounter(name, n = 0):
    """ init counter
    """
    COUNTERS[name] = n
    return

#---------------------------------------------------
def initCounters(nameList, n = 0):
    """ init counter list
    Author: Hugo Ruiz, hugo.ruiz@cern.ch
    """
    for iName in nameList:
        COUNTERS[iName] = n
    return

#---------------------------------------------------
def increaseCounter(name,n=1):
    """ increase counter
    """
    COUNTERS[name] = COUNTERS[name] + n

#---------------------------------------------------
def showCounter(name, refName = False):
    out =  name+': '+ str(COUNTERS[name])
    if refName and name!= refName:
        if COUNTERS[name] > 0 and COUNTERS[refName] > 0:
            perCent = 100*float(COUNTERS[name])/float(COUNTERS[refName])
            out += '( '+str(perCent)+'% of '+refName+' )'
    print out
    return

#---------------------------------------------------
def showCounters(nameList, refName = False):
    for iName in nameList:
        showCounter(iName, refName)
    return

#---------------------------------------------------
def counterValue(name):
    return COUNTERS[name]

#---------------------------------------------------
def setCounterValue(name, n):
    COUNTERS[name] = n
    return




#---------------------------------------------------
# Tools
#---------------------------------------------------

#---------------------------------------------------
def tool(gaudi,toolname,toolinterface):
    """ retrieve a tool
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
    Author: Hugo Ruiz, hugo.ruiz@cern.ch
    """
    objectName = rootObject.GetName()

    if objectName in ROOTOBJECTS.keys():
        print 'ALREADY REGISTERED A ROOT OBJECT WITH THAT NAME!'
        raise RuntimeError

    ROOTOBJECTS[ objectName ] = rootObject

    return

#---------------------------------------------------
def showRegistered():
    """ Show registered objects in an organized way.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch
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
def saveRegistered(fileName = False, addDate = True, keepOld = True):
    """ Write ROOT OBJECTS in a root file
    Author: Hugo Ruiz, hugo.ruiz@cern.ch
    """
    
    # Default file naname
    if not fileName:
        fileName = 'desktop'
        
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
    Author: Hugo Ruiz, hugo.ruiz@cern.ch.
    """
    return ROOTOBJECTS[ objectName ]
    


#---------------------------------------------------
def desktopExample():
    """ Example of the functionality of this module.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch
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
    map(lambda a:my('myTree').book(a), tree_vars)

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
    
