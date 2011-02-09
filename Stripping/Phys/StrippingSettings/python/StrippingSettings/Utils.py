"""
Helpers to obtain configuration parameter dicitonaries for stripping line builders.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import shelve, os

def dbaseDir() :
    return os.getenv('STRIPPINGSETTINGSROOT')+'/dbase/'

def dbase(stripping) :
    return dbaseDir() + stripping.lower()

def getStrippingConfiguration(stripping) :
    """
    Get all the configuration dictionaries for a given stripping.
    Usage:
    conf = getConfigurationDicitonary('Stripping12')
    """
    db = shelve.open( dbase(stripping), 'r' )
    dbdict = dict( db )
    db.close()
    return dbdict

def getLineBuilderConfiguration(stripping, lineBuilderName) :
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
    
def dbaseFromModule(stripping, confModule) :
    """
    Create a database with all the dictionaries in a given module.
    Writes file to $PWD/<stripping>.tmp
    """
    db = shelve.open( stripping.lower()+'.tmp',  'n')

    goodNames = filter(lambda x : x[0]!='_',
                       confModule.__dict__.keys())

    for k in goodNames :
        db[k]=confModule.__dict__[k]

    db.close()

    
