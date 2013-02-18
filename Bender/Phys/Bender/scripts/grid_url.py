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
#                    $Revision: 146621 $
#  Last modification $Date: 2012-10-08 14:47:42 +0200 (Mon, 08 Oct 2012) $
#                 by $Author: ibelyaev $ 
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
__version__ = "$Revision: 146621 $"
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
    
    from DIRAC.DataManagementSystem.Client.ReplicaManager import ReplicaManager
    from DIRAC import gLogger
  
    if len(seList) > 1 and not silent :
        gLogger.info( "Using the following list of SEs: %s" %str(seList))
    
    rm = ReplicaManager()
    gLogger.setLevel("FATAL")
  
    #
    ## return result in a form of list
    # 
    result = {}

    for lfn in lfns :
        #
        result[lfn] = []
        # 
        for se in seList : 
            res = rm.getReplicaAccessUrl( lfn , se )
            if not res ['OK']                       : continue 
            result[ lfn ].append ( res['Value']['Successful'][ lfn ] )
            
    return result

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
