#  @date 2007-11-28
#  @author Hugo RUIZ hugo.ruiz@cern.ch
# =============================================================================
""" Simple counter class and some functions to initialize and print a dictionary of counters """
# =============================================================================
__author__ = "Hugo RUIZ hugo.ruiz@cern.ch"
# =============================================================================


#---------------------------------------------------
class counter:
    """ Simple counter class
    Author: Hugo Ruiz"""

    def __init__( self, initValue = 0):
        """ Initializes, setting to zero by default """
        self.value = initValue
        return
    
    def up (self, n = 1):
        """ Increases counter, by 1 by default"""
        self.value = self.value + n
        return
    
    def value (self):
        """ Returns counter value"""
        return self.value

    def setValue (self, n):
        """ Sets counterValue """
        self.value = n
        return

    def __repr__( self ):
        return str(self.value)
    

    def __int__( self):
        return self.value

    def __float__( self):
        return float(self.value)


#---------------------------------------------------
def printCounterDict (countDic, keyList = False, referenceKey = False):
    """ Prints a dictionnary of counters.
    List of keys can be provided as keyList to force an order.
    A reference counter key to compute %'s can be provided.
    See example at bottom!
    Author: Hugo Ruiz, hugo.ruiz@cern.ch
    """
    referenceKeyIndex = 0
    if not keyList: keyList = countDic.keys()
    for iKey in keyList:
        out = iKey+': '+str(countDic[iKey])
        if referenceKey and iKey!=referenceKey:
            if countDic[referenceKey]>0 and countDic[iKey]>0:
                try:
                    perc = 100.*float(int(countDic[iKey]))/float(int(countDic[referenceKey]))
                    out += ' ('+str(perc)+'% of '+referenceKey+')'
                except:
                    print 'COULDNT DO DIVISION WITH: ', countDic[iKey], countDic[referenceKey]
                    raise RuntimeError
    print out
    return
                                                      

#---------------------------------------------------
def prepareCounterDict( dict, keyList ):
    """ Fills a dictionnary of counters with initial value 0 from a list of counter keys
    Author: Hugo Ruiz"""
    for iKey in keyList:
        dict[iKey] = counter( 0 )
    return


#---------------------------------------------------
def resetCounterDict( dict):
    """ Resets to 0 a dictionnary of counters.
    Author: Hugo Ruiz"""
    for iKey in dict.keys():
        dict[iKey].setValue( 0 )
    return



#---------------------------------------------------
def countExample():
    """ To ilustrate behaviour of counter class and functions.
    Author: Hugo Ruiz"""
    myKeyList1 = ['Events','PassL0']
    myKeyList2 = ['PassL0Mu','PassL0Had']
    myKeyList = myKeyList1 + myKeyList2
    counters = {}
    prepareCounterDict( counters, myKeyList)
    counters['Events'].up(40000000)
    counters['PassL0'].up(1000000)
    counters['PassL0Mu'].up()
    counters['PassL0Had'].up()
    print 'Counters:'
    print '========='
    printCounterDict (counters, keyList = myKeyList1, referenceKey = 'Events') 
    print
    printCounterDict (counters, keyList = myKeyList2, referenceKey = 'PassL0')
    return
