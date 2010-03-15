#!/usr/bin/env python
# =============================================================================
## @file LoKiCore/doxygenurl.py
#
# Simple module to provide access to Doxygen documentation for the various
# classes and objects.
# Actually the lines are stallen from T.Ruf's (Thomas.Ruf@cern.ch)
# module gaudigadgets.py
#
# Esentially it provides tho major  public functions:
#
#  -  getURL ( object , projects = None )
#  -  browse ( object , projects = None )
#
# and the minor helper function
#
#  - httpExists ( url )
# 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-07-22
# =============================================================================
"""
Simple module to provide access to Doxygen documentation for the various
classes and objects.
Actually the lines are stallen from T.Ruf's (Thomas.Ruf@cern.ch)
module gaudigadgets.py

Esentially it provides tho major  public functions:

  -  getURL ( object , projects = None )
  -  browse ( object , projects = None )

and the minor helper function

  - httpExists ( url )
  
"""
# =============================================================================
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ "
__author__  = " Vanya BELYAEV ibelyaev@physics.syr.edu "
__date__    = " 2007-07-22"
__all__     = (
    'getURL'     , ## get (doxygen) url for class/symbol/object
    'browse'     , ## browse (doxygen) for the class/symbol/object
    'httpExists' , ## check the existance of the certain URL
    'searchURL'  , ## search form for the symbol 
    'lbglimpse'    ## use Lbglimpse command
    )
# =============================================================================
## URL-s patterns
# =============================================================================
_ROOTURL    = 'http://root.cern.ch/root/html/%s.html'
_GAUDIURL   = 'http://cern.ch/proj-gaudi/releases/GAUDI/%s/doc/html/%s'
_LHCBURL    = 'http://cern.ch/LHCb-release-area/DOC/%s/releases/%s/doxygen/'

## the default list of projects (if not specified)
_PROJECTS = ( "ROOT"      ,
              "MOORE"     ,
              "PANORAMIX" ,
              "BENDER"    ,
              "ERASMUS"   ,
              "DAVINCI"   ,
              "ORWELL"    ,
              "PANOPTES"  ,
              "BRUNEL"    ,
              "ANALYSIS"  ,
              "PHYS"      ,
              "HLT"       ,
              "REC"       ,
              "LBCOM"     ,
              "LHCB"      ,
              "LCGCMT"    ,
              "GAUDI"     )

# =============================================================================
## check the existence(validity) of the URL
def httpExists( url ) :
    """ Check the existence(validity) of the url  """ 
    
    import urlparse
    import httplib
    
    host, path = urlparse.urlsplit(url)[1:3]
    ## Make HTTPConnection Object
    connection = httplib.HTTPConnection(host)
    connection.request("HEAD", path)
    ## Grab HTTPResponse Object
    response = connection.getresponse()

    if response.status in ( 302 , 300 ) : return True    ## RETURN
    ##
    return False                               ## RETURN
# =============================================================================
## Get the project version
# =============================================================================
def _projVersion ( project = None ) :
    """
    get the project version
    """
    import os

    if not project :
        for p in _PROJECTS :
            version = _projVersion ( p )
            if version : return (p,version)    ## RETURN 
        return None


    ##  "/PROJECT/PROJECT_" or "\PROJECT\PROJECT_"    
    PROJECT     = project.upper()
    PROJECTROOT = PROJECT + 'ROOT'
    if not os.environ.has_key( PROJECTROOT ) : return None   ## RETURN
    
    root   = os.environ[ PROJECTROOT ]
    target = os.sep + PROJECT + os.sep + PROJECT + "_"
    l      = root.rfind ( target )
    if 0 > l :                                return None    ## RETURN 

    l = l + 2 *len(PROJECT) + 3 
    root = root[l:]
    
    ## "PROJECT_vers1:"
    vers1 = root.split ( os.pathsep ) [0]
    ## "PROJECT_vers1/"
    vers2 = root.split ( os.sep     ) [0]
    ## 
    return min(vers1,vers2)

# =============================================================================
## Convert C++ class name into the name of Doxygen file 
# =============================================================================
def _classDoxName ( klassname ) :
    """
    Convert C++ class name into the name of Doxygen file
    """
    result = 'class'
    ##
    if '::' == klassname[-2:] :
        result = 'namespace'
        klassname = klassname[:-2]
    for c in klassname :
        if c.isupper() : result += '_' + c.lower()
        elif '<' == c  : break  
        else           : result +=       c
    result = result.replace('::','_1_1')
    ##
    return result + '.html'
# =============================================================================
## Convert C++ namespace name into the name of Doxygen file 
# =============================================================================
def _namspDoxName ( klassname ) :
    """
    Convert C++ class name into the name of Doxygen file
    """
    result = 'namespace'
    ##
    if '::' == klassname[-2:] :
        klassname = klassname[:-2]
    for c in klassname :
        if c.isupper() : result += '_' + c.lower()
        elif '<' == c  : break  
        else           : result +=       c
    result = result.replace('::','_1_1')
    ##
    return result + '.html'
# =============================================================================
## Helper method to loop over projects and find the proper pages
# =============================================================================
def _getURL ( klass , dox , projects , latest = False ) :
    """
    Helper method to loop over projects and find the proper pages
    """
    import urllib
    
    ## loop over all projects
    for project in projects :
        project = project.upper()
        if   'ROOT'  == project :
            url = _ROOTURL % klass 
            if httpExists( url ) : return url                   ## RETURN 
        else :
            version = _projVersion ( project )
            if not version :
                if not latest : continue                         ## CONTINUE 
                ## try to use the latest version 
                version = 'latest'
            if 'GAUDI' == project :
                dox = dox.replace('classi','class')
                url = _GAUDIURL %(version,dox)
                if httpExists ( url ) : return url               ## RETURN
                continue                                         ## CONTINUE
            else :
                urlbase    = _LHCBURL  %(project.lower(),version)
                urlclasses = urlbase + 'classes.html'
                if not httpExists ( urlclasses ) : continue       ## CONTINUE
                xxx = urllib.urlopen(urlclasses)
                for line in xxx.readlines() :
                    p1 = line.find( dox )
                    if p1 < 0 : continue  
                    p2 = line[:p1].rfind('href')+6
                    url = urlbase + line [p2:p1] + dox
                    if httpExists ( url )  : return url           ## RETURN 
   
    return None
# =============================================================================
## get the URL with proper Doxygen documentation for the object
# =============================================================================
def getURL ( o , projects = None , latest = False ) :
    """
    Get the URL with the proper Doxygen documentation for the object/class
    
    The object could be:
    
    - type or namespace name (string)
    - the type (including the templated type)
    - the namespace 
    - the instance

    It tries to get the projecc version from the current environment.
    For 'latest=True', it also tries 'the latest' releases
    
    """
    if not projects : projects = list( _PROJECTS ) 
        
    ## get the proper class name 
    klass = o 
    if not str is type( klass ) :
        if not hasattr    ( klass , '__class__'    ) : return None
        if not issubclass ( klass.__class__ , type ) : ## class instance
            ## for LoKi's types often better to see the low-level class
            if hasattr ( klass , 'objType' ) : klass = klass.objType()
            else : klass = klass.__class__.__name__ 
        else : klass = klass.__name__                  ## class type
    
    if not str is type( klass ) : return None
    ## check for the proper projects:
    if 2 < len(klass) and 'T' == klass[0] and \
       klass[1].isupper()         and not 'ROOT'  in projects : projects += [ 'ROOT' ]
    if 0 <= klass.find ( 'Gaudi') and not 'GAUDI' in projects : projects += [ 'GAUDI']
    if 0 <= klass.find ( 'LHCb' ) and not 'LHCB'  in projects : projects += [ 'LHCB' ]
    if 0 <= klass.find ( 'LoKi' ) and not 'LHCB'  in projects : projects += [ 'LHCB' ]
    if 0 <= klass.find ( 'LoKi' ) and not 'PHYS'  in projects : projects += [ 'PHYS' ]
    ## transform it into doxygen file name
    dox = _classDoxName ( klass )

    ## loop over all projects
    url = _getURL ( klass , dox , projects , latest  )
    if not url :  ## is it namespace ?
        from PyCintex import gbl
        if type(o) is gbl.PyRootType and not type(o.__init__) is gbl.MethodProxy :
            ## it is namespace !!!
            dox = _namspDoxName ( klass )
            url = _getURL ( klass , dox , projects , latest )
    return url

# =============================================================================
def searchURL ( o ) :
    
    project,version = _projVersion()
    
    url  = _LHCBURL  %(project.lower(),version)
    url += 'search.php?query=%s' % o
    
    if httpExists ( url ) : return url               ## RETURN
    return None

# =============================================================================
## browse the doxygen documentation for the class/object
def browse ( o , projects = None , latest = False ) :
    """
    Browse the doxygen documentation for the class/object
    """
    ## find the proper URL 
    url = getURL ( o , projects , latest )
    if not url : return None                                 ## RETURN
    import webbrowser
    print "Trying to open URL = '%s' " % url 
    webbrowser.open ( url , new = True ) 
    return url                                           
# =============================================================================
### Use Lblimpse tool to find the files with the symbol
def lbglimpse ( o , project = None , version = None ) :
    """
    
    Use Lblimpse tool to find the files with the symbol
    
    """
    if       project and not version : version         = _projVersion ( project )
    elif not project and not version : project,version = _projVersion (         )
        
    if not project or not version : 
        project,version = _projVersion()
        
    import popen2

    
    if issubclass ( type(o) , ( list , tuple ) ) :
        result = []
        for i in o :
            result += lbglimpse ( i , project , version )
        return result
        
    what = None 
    if       issubclass ( type(o) , str ) : what = o
    elif not issubclass ( o .__class__ , type ) : ## class instance
        ## for LoKi's types often better to see the low-level class
        if hasattr ( o , 'objType' ) : what = o.objType()
        else : what = o.__class__.__name__ 
    else : what = o.__name__                  ## class type

    command =  'Lbglimpse %s %s %s' % ( what    ,
                                        project ,
                                        version )

    stdout,stdin,stderr = popen2.popen3 ( command ) 

    result = [ line[:-1] for line in stdout ]
    
    if 0 > what.find ('LoKi::') : 
        result += lbglimpse ( 'LoKi::Cuts::' + what , project , version )
    if issubclass ( type(o), str ) and 0 > o.find('LoKi::Cuts::') : 
        result += lbglimpse ( 'LoKi::Cuts::' + o    , project , version )

    result = result[1:]
    result.sort() 
    return result

# =============================================================================
if __name__ == "__main__" :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120
   

# =============================================================================
# The END
# =============================================================================
