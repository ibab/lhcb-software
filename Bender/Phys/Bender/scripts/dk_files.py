#!/usr/bin/env python
# =============================================================================
## @file
#  Simple script to get some infromation on decay-files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-02-18
#
#                     $Revision:$
#  Last modifgication $Date:$
#                  by $Author:$
# =============================================================================
"""

Simple script to find some information on decay files
It scans for data base of decay-files and seeks for
decays that match certain pattern either in
event type or in nickname or in decay-descriptor

e.g.

dec-files  '123*'
dec-files  '*DD,TightCut'
dec-files  '.*DD,TightCut' -r 

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-02-18"
__version__ = "$Revision:$"
__usage__   = 'dec-files PATTERN  [-r] [ -d DATABASE] \n' + __doc__ 
# =============================================================================
## default name of DB file
db_name     = "$DECFILESROOT/doc/table_event.txt"
#


## parse DB 
def parse_db ( db_file ) :

    import os
    
    ## expand the actual file name 
    db_file  = os.path.expandvars ( db_file )
    db_file  = os.path.expanduser ( db_file )
    db_file  = os.path.expandvars ( db_file )
    db_file  = os.path.expanduser ( db_file )
    db_file  = os.path.expandvars ( db_file )

    dct = {}
    
    with open ( db_file , 'r' ) as f :
        
        for line in f :
            
            r1         = line.find ( '|' )
            r2         = line.find ( '|' , r1+1 )
            #
            try    :
                evttype    = line [      0 : r1 ] . strip ()
                evttype    = int ( evttype ) 
            except :
                continue
            #
            nick       = line [ r1 + 1 : r2 ] . strip ()
            descriptor = line [ r2 + 1 :    ] . strip () 

            dct [ evttype ] = nick,descriptor

    return dct


def get_matched (  dct , match ) :

    result = {}

    keys = dct.keys()
    keys.sort()

    for k in keys :

        p1 = str ( k )
        p2 = dct [k] [0]
        p3 = dct [k] [1]

        if match ( p1 ) or match ( p2 ) or match ( p3 ) : 
            result [k] = dct[k]

    return result 
            
def get_entries ( dct , pattern , regex = False ) :

    if regex :
        
        import re
        return get_matched ( dct , lambda s : re.match ( pattern , s ) )
        
    import fnmatch
    return get_matched ( dct , lambda s : fnmatch.fnmatch ( s , pattern ) )

def print_entries ( dct ) :

    keys = dct.keys()
    keys.sort()

    for k in dct :

        line = '%8d | %60s | %s ' % ( k , dct[k][0] , dct[k][1] )
        
        print line
        
        
        
    
    
# =============================================================================
if '__main__' == __name__ :

    
    #
    ## make parser
    #
    
    from optparse import OptionParser
    parser = OptionParser (
        usage   = __usage__   ,
        version = __version__ )
    
    parser.add_option (
        '-r'                   ,
        '--re'                 ,
        action  = "store_true" ,
        dest    = 'Regex'      ,
        help    = "Use re-library for string matching" ,
        default = False        )
    
    parser.add_option (
        '-d'         , 
        '--database' ,
        type    = 'str'       ,
        dest    = 'DataBase'  ,
        help    = 'The name of data-base file' ,
        default = db_name
        )
    
    #
    ## parse it! 
    # 
    options , arguments = parser.parse_args()
    
    ## Files must be specfied are mandatory!
    if not arguments : parser.error ( 'No patterns are specified' ) 
    elif 1 != len ( arguments )  :
        parser.error ( 'Only one pattern is needed' )
        
    dct = parse_db    ( options.DataBase    )
        
    dct = get_entries ( dct , arguments[0]  , options.Regex ) 

    print_entries ( dct ) 
    
# =============================================================================
# The END 
# =============================================================================
