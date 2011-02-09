"""
Helpers to obtain configuration parameter dicitonaries for stripping line builders.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import shelve, os

def getStrippingConfiguration(stripping) :
    """
    Get all the configuration dictionaries for a given stripping.
    Usage:
    conf = getConfigurationDicitonary('Stripping12')
    """
    dbasedir = os.getenv('STRIPPINGSETTINGSROOT')+'/dbase/'
    dbase = dbasedir + stripping.lower()
    db = shelve.open(dbase, 'r')
    dbdict = dict(db)
    db.close()
    return dbdict

def getLineBuilderConfiguration(stripping, lineBuilderName) :
    """
    Get a copy of a line builder configuration dictionary
    for a given stripping.
    Usage:
    conf = getLineBuilderConfiguration('Stripping12', 'Bd2KstarMuMuConfig')
    """
    dbasedir = os.getenv('STRIPPINGSETTINGSROOT')+'/dbase/'
    dbase = dbasedir + stripping.lower()
    print 'Trying to open dbase', dbase
    db = shelve.open(dbase, 'r')
    _dict = dict(db[lineBuilderName])
    db.close()
    return _dict
