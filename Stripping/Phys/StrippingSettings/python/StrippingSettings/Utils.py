"""
Helpers to obtain configuration parameter dicitonaries for stripping line builders.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import shelve, os

def dbaseDir() :
    return os.getenv('STRIPPINGSETTINGSROOT')+'/dbase/'

def dbase(stripping) :
    return dbaseDir() + stripping.lower()


def strippingConfiguration(stripping) :
    """
    Get read access to configuration dictionaries for a given stripping
    Usage:
    conf = getConfigurationDicitonary('Stripping12')
    """
    return shelve.open( dbase(stripping), 'r' )

def lineBuilderConfiguration(stripping, lineBuilderName) :
    """
    Get a copy of a line builder configuration dictionary
    for a given stripping.
    Usage:
    conf = getLineBuilderConfiguration('Stripping12', 'Bd2KstarMuMuConfig')
    """
    db = shelve.open( dbase(stripping), 'r' )
    dbdict = dict( db[lineBuilderName] )
    db.close()
    return dbdict

def registerNewLineBuilderConfiguration(stripping, lineBuilderName, config) :
    """
    Register a new line builder configuration dictionary for a given stripping.
    Only allows to add new entries, not overwrite existing ones.
    """
    db = shelve.open( dbase(stripping) )
    if not lineBuilderName in db.keys() :
        db[lineBuilderName] = config
        db.close()
    else :
        db.close()
        raise KeyError(lineBuilderName+' is already in '+ dbase(stripping) )
    
def dbaseFromModule(stripping, confModule, verbose = False) :
    """
    Create a database with all the dictionaries in a given module.
    Writes file to $PWD/<stripping>.tmp
    """
    dbName = stripping.lower()+'.tmp'
    db = shelve.open( dbName,  'n')

    if verbose :
        print 'Creating database for module', confModule
        
    goodNames = filter(lambda x : x[0]!='_',
                       confModule.__dict__.keys())

    if verbose :
        print 'Storing', goodNames, 'in database',  dbName
    for k in goodNames :
        if verbose : print 'Store', k
        db[k]=confModule.__dict__[k]

    db.close()

    
