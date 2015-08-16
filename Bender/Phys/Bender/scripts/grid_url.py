#!/usr/bin/env python
# =============================================================================
# @file grid_url.py
#
# Helper module to get Grid access URL 
# Actually it is a bit modified version of dirac-dms-lfn-accessURL
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author VanyaBELYAEV Ivan.Belyaev@itep.ru
# @date   2012-10-23
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""
  Helper module to get Grid access URL 
  Actually it is a bit modified version of dirac-dms-lfn-accessURL script 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2012-10-27"
__version__ = "$Revision$"
__all__     = ( 'accessURLs' , ) 
# =============================================================================
## get the dictionary { 'LFN' : 'access-URL' }
#  Actually it is a bit modified version of dirac-dms-lfn-accessURL script 
#  @author VanyaBELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23
def _accessURLs ( silent  = True ) :  
    """
    Get the dictionary { 'LFN' : 'access-URL' }
    Actually it is a bit modified version of dirac-dms-lfn-accessURL script 
    """

    import DIRAC
    from   LHCbDIRAC.DataManagementSystem.Client.DMScript import DMScript, printDMResult
    from   DIRAC.Core.Base import Script


    dmScript = DMScript()
    dmScript.registerFileSwitches()
    dmScript.registerSiteSwitches()
    Script.setUsageMessage( '\n'.join( [ __doc__.split( '\n' )[1],
                                         'Usage:',
                                         '  %s [option|cfgfile] ... LFN[,LFN2[,LFN3...]] [SE[,SE2]]' % Script.scriptName,
                                         'Arguments:',
                                         '  LFN:      Logical File Name or file containing LFNs',
                                         '  SE:       Valid DIRAC SE' ] ) )


    Script.parseCommandLine( ignoreErrors = True )
    
    seList = dmScript.getOption('SEs',[])

    args = Script.getPositionalArgs()
    if not seList:
        sites = dmScript.getOption( 'Sites', [] )
        from DIRAC import gConfig
        for site in sites:
            res = gConfig.getOptionsDict( '/Resources/Sites/LCG/%s' % site )
            if not res['OK'] and not silent :
                print 'Site %s not known' % site
                Script.showHelp()
            seList.extend( res['Value']['SE'].replace( ' ', '' ).split( ',' ) )

    if not seList:
        if not args :
            if not silent :
                print "Give SE name as last argument or with --SE option"
                Script.showHelp()
            DIRAC.exit(0)
        seList = args.pop(-1).split(',')
    
    if type(seList) == type(''):
        seList = [seList]
        
    # This should be improved, with disk SEs first...
    seList.sort()


    lfns  = dmScript.getOption('LFNs',[])
    lfns += args

    lfnList = lfns 

    from DIRAC.DataManagementSystem.Client.DataManager          import DataManager
    
    dm       = DataManager()
    result   = dm.getReplicas ( lfnList )
    replicas = result.get('Value',{}).get('Successful',{}) 

    from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient   import BookkeepingClient
    bk           = BookkeepingClient()
    results      = {'OK':True, 'Value':{'Successful':{}, 'Failed':{}}}
    notFoundLfns = set( lfnList )

    bkRes = bk.getFileTypeVersion( lfnList )

    mdfFiles = set( [lfn for lfn, fileType in bkRes.get( 'Value', {} ).items() if fileType == 'MDF'] )
    
    from DIRAC.Resources.Storage.StorageElement                 import StorageElement
    
    protocol = None 
    for se in seList:
        lfns = [lfn for lfn in lfnList if se in replicas.get( lfn, [] )]
        if lfns:
            res = StorageElement( se ).getURL( lfns, protocol = protocol )
            success = res.get( 'Value', {} ).get( 'Successful' )
            failed = res.get( 'Value', {} ).get( 'Failed' )
            if res['OK']:
                if success:
                    for lfn in set( success ) & mdfFiles:
                        success[lfn] = 'mdf:' + success[lfn]
                    notFoundLfns -= set( success )
                    results['Value']['Successful'].setdefault( se, {} ).update( success )
                if failed:
                    results['Value']['Failed'].setdefault( se, {} ).update( failed )
            else:
                results['Value']['Failed'].setdefault( se, {} ).update( dict.fromkeys( lfns, res['Message'] ) )
                
    for se, failed in results['Value']['Failed'].items():
        for lfn in list( failed ):
            if lfn not in notFoundLfns:
                failed.pop( lfn )
            else:
                notFoundLfns.remove( lfn )
    if notFoundLfns:
        results['Value']['Failed'] = dict.fromkeys( sorted( notFoundLfns ), 'File not found in required seList' )

    ##print results
    results = results.get('Value',{}).get('Successful',{})
    res = {}
    for se in results :
        access = results[se]
        for lfn in access :
            if res.has_key( lfn ) : res[lfn].append( access[lfn] )
            else                  : res[lfn] = [ access[lfn] ] 
                            
    return res

## def getAccessURL( lfnList, seList, protocol = None ):
##   dm = DataManager()
##   res = dm.getReplicas( lfnList )
##   replicas = res.get( 'Value', {} ).get( 'Successful', {} )
##   if not seList:
##     seList = sorted( set( [se for lfn in lfnList for se in replicas.get( lfn, {} )] ) )
##     if len( seList ) > 1:
##       gLogger.always( "Using the following list of SEs: %s" % str( seList ) )
##   bk = BookkeepingClient()
##   notFoundLfns = set( lfnList )
##   results = {'OK':True, 'Value':{'Successful':{}, 'Failed':{}}}
##   savedLevel = gLogger.getLevel()
##   gLogger.setLevel( 'FATAL' )
##   # Check if files are MDF
##   bkRes = bk.getFileTypeVersion( lfnList )
##   mdfFiles = set( [lfn for lfn, fileType in bkRes.get( 'Value', {} ).items() if fileType == 'MDF'] )
##   for se in seList:
##     lfns = [lfn for lfn in lfnList if se in replicas.get( lfn, [] )]
##     if lfns:
##       res = StorageElement( se ).getURL( lfns, protocol = protocol )
##       success = res.get( 'Value', {} ).get( 'Successful' )
##       failed = res.get( 'Value', {} ).get( 'Failed' )
##       if res['OK']:
##         if success:
##           for lfn in set( success ) & mdfFiles:
##             success[lfn] = 'mdf:' + success[lfn]
##           notFoundLfns -= set( success )
##           results['Value']['Successful'].setdefault( se, {} ).update( success )
##         if failed:
##           results['Value']['Failed'].setdefault( se, {} ).update( failed )
##       else:
##         results['Value']['Failed'].setdefault( se, {} ).update( dict.fromkeys( lfns, res['Message'] ) )
##   gLogger.setLevel( savedLevel )

##   for se, failed in results['Value']['Failed'].items():
##     for lfn in list( failed ):
##       if lfn not in notFoundLfns:
##         failed.pop( lfn )
##       else:
##         notFoundLfns.remove( lfn )
##   if notFoundLfns:
##     results['Value']['Failed'] = dict.fromkeys( sorted( notFoundLfns ), 'File not found in required seList' )

##   printDMResult( results, empty = "File not at SE", script = "dirac-dms-lfn-accessURL" )


# =============================================================================
## get the dictionary { 'LFN' : 'access-URL' }
#  Actually it is a bit modified version of dirac-get-accessURL script 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23
def accessURLs ( silent = True ) :
    """
    Get (optionally silently) the map of access URSL for LFNs
    
    >>> ursl = accessURLs() 
    """
    import sys 
    stdout = sys.stdout
    stderr = sys.stderr

    result = {}
    
    try :
        
        if silent :
            
            sys.stderr = None ## open ( '/dev/null' )
            sys.stdout = None ## open ( '/dev/null' )

        result = _accessURLs ( silent ) ## RETURN 
    
    except:

        if not silent:
            import traceback 
            print  traceback.format_exc() 
            
        result = {}           ## RETURN 
        
    finally:
        
        sys.stderr = stderr
        sys.stdout = stdout
        
    return result

# =============================================================================
if __name__ == '__main__' :

    print accessURLs( True ) 
        
# =============================================================================
# The END 
# =============================================================================
