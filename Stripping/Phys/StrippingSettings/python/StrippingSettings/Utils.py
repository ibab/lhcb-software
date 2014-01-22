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

def registerNewLineBuilderConfiguration(stripping,
                                        lineBuilderName,
                                        lineBuilderType,
                                        config) :
    """
    Register a new line builder configuration dictionary for a given stripping.
    Only allows to add new entries, not overwrite existing ones.
    """
    db = shelve.open( dbase(stripping) )
    if not lineBuilderName in db.keys() :
        db[lineBuilderName] = {'BUILDERTYPE' : str(lineBuilderType),
                               'CONFIG' : config}
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
        
    goodNames = filter(lambda x : x[0]!='_' and not isUnit(x),
                       confModule.__dict__.keys())

    if verbose :
        print 'Storing', goodNames, 'in database',  dbName
    for k in goodNames :
        if verbose : print 'Store', k
        db[k]=confModule.__dict__[k]

    db.close()

def isUnit(name) :
    from GaudiKernel import SystemOfUnits as units
    return hasattr(units, name)

def addModuleToDbase(stripping, confModule, verbose = False) :
    dbName = stripping.lower()
    db = shelve.open( dbName )

    if verbose :
        print 'Adding module', confModule, 'to database', dbName
        
    goodNames = filter(lambda x : x[0]!='_' and not isUnit(x),
                       confModule.__dict__.keys())

    if verbose :
        print 'Storing', goodNames, 'in database',  dbName
    for k in goodNames :
        if not k in db.keys() :
            config_dict = confModule.__dict__[k]
            if isinstance(config_dict, dict) : 
        	if verbose : print 'Store', k, 'in', dbName
        	print config_dict
        	db[k]=config_dict
        else :
            print 'Key', k, 'already in database', dbName

    db.close()

def groupFromDBase( db, keytype ):
    '''
    Utility function that returns db keys
    gropued by keytype (WG,Stream)
    '''
    lines = {}
    
    for key in db.keys():
        lineWGs = db[key][keytype]
        for WG in lineWGs:
            if WG in lines: lines[WG].append( key )
            else: lines[WG] = [ key ]
    return lines
            
        
def printBuildersByWG( stripping='stripping13' ):
    '''
    Utility function that prints all line
    builders grouped by WG
    '''
    db = strippingConfiguration( stripping )
    lines = groupFromDBase( db,'WGs')

    for key in lines:
        print '\nWorking Group: ', key
        for iline in range(0,len(lines[key])):
            dbkey = lines[key][iline]
            print iline+1, lines[key][iline], db[dbkey]['STREAMS']
    return


def printBuildersByStream( stripping ):
    '''
    Utility function that prints all line
    builders grouped by stream
    '''
    db = strippingConfiguration( stripping )
    lines = groupFromDBase( db,'STREAMS')
    
    for key in lines:
        print '\nStream: ', key
        for iline in range(0,len(lines[key])):
            dbkey = lines[key][iline]
            print iline+1, lines[key][iline], db[dbkey]['WGs']
    return

                        

def printBuilderProperties( stripping, dbkey ):
    '''
    Print the configuration of a stripping line builder
    Usage:
    >> printBuilderProperties("stripping13","B2XGamma")
    '''
    db = strippingConfiguration( stripping )

    if dbkey not in db.keys():
        print dbkey, 'not in dbase ' + stripping
        return

    WGs     = db[dbkey]['WGs']
    config  = db[dbkey]['CONFIG']
    builder = db[dbkey]['BUILDERTYPE']
    streams = db[dbkey]['STREAMS']
    
    print '\nLine: ', dbkey
    print '===============================================\n'
    print 'WGs: ', WGs
    print 'Appears in streams:', streams, '\n'
    print 'Will be generated using the line builder ' + builder
    print 'with the config dictionary: \n'
    for key in config:
        print '  ' + key, ':', config[key]
    return
