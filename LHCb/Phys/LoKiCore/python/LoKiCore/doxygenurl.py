#!/usr/bin/env python
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
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu"
__date__    = "2007-07-22"

# =============================================================================
## URL-s patterns
# =============================================================================
_ROOTURL    = 'http://root.cern.ch/root/html/%s.html'
_GAUDIURL   = 'http://proj-gaudi.web.cern.ch/proj-gaudi/releases/%s/doxygen/%s'
_LHCBURL    = 'http://LHCb-release-area.web.cern.ch/LHCb-release-area/DOC/%s/releases/%s/doxygen/%s'

## the default list of projects (if not specified)
_PROJECTS = [ "ROOT" , "GAUDI" , "LHCB" , "HLT" , "ANALYSIS" ]

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
    if 200 == response.status : return True    ## RETURN
    ##
    return False                               ## RETURN
# =============================================================================
## Get the project version
# =============================================================================
def _projVersion ( project ) :
    """ get the project version """
    import os
    ##  "/PROJECT/PROJECT_" or "\PROJECT\PROJECT_"
    target = os.sep + project.upper() + os.sep + project.upper() + "_"
    for value in os.environ.values() :
        l =  value.rfind(target)
        if 0 >= l : continue
        ## "PROJECT_vers1:"
        vers1 = value[l+2*len(project)+3:].split(os.pathsep)[0]
        ## "PROJECT_vers1/"
        vers2 = value[l+2*len(project)+3:].split(os.sep)[0]
        return min(vers1,vers2)
    return None 
# =============================================================================
## Convert C++ class name into the name of Doxygen file 
# =============================================================================
def _classDoxName ( klassname ) :
    """ convert C++ class name into the name of Doxygen file """
    result = 'class'
    if '::' == klassname[-2:] :
        result = 'namespace'
        klassname = klassname[:-2]
    for c in klassname :
        if c.isupper() : result += '_' + c.lower()
        elif '<' == c  : break  
        else           : result +=       c
    result = result.replace('::','_1_1')
    return result + '.html'
# =============================================================================
## Convert C++ namespace name into the name of Doxygen file 
# =============================================================================
def _namspDoxName ( klassname ) :
    """ convert C++ class name into the name of Doxygen file """
    result = 'namespace'
    if '::' == klassname[-2:] :
        klassname = klassname[:-2]
    for c in klassname :
        if c.isupper() : result += '_' + c.lower()
        elif '<' == c  : break  
        else           : result +=       c
    result = result.replace('::','_1_1')
    return result + '.html'
# =============================================================================
## Helper method to loop over projects and find the proper pages
# =============================================================================
def _getURL ( klass , dox , projects , latest = False ) :
    """
    Helper method to loop over projects and find the proper pages
    """ 
    ## loop over all projects
    for project in projects :
        project = project.upper()
        if   'ROOT'  == project :
            url = _ROOTURL % klass 
            if httpExists( url ) : return url                ## RETURN 
        else :
            version = _projVersion ( project )
            if not version :
                if not latest : continue                  ## continue 
                ## try to use the latest version 
                version = 'latest'
            if 'GAUDI' == project : url = _GAUDIURL %(version,dox)
            else                  : url = _LHCBURL  %(project.lower(),version,dox)
            if httpExists ( url ) : return url               ## RETURN            
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
    if not projects : projects = _PROJECTS
        
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
       klass[1].isupper()        and not 'ROOT'  in projects : projects += [ 'ROOT'  ]
    if 0 <= klass.find( 'Gaudi') and not 'GAUDI' in projects : projects += [ 'GAUDI' ]
    if 0 <= klass.find( 'LHCb' ) and not 'LHCb'  in projects : projects += [ 'LHCb'  ]
    if 0 <= klass.find( 'LoKi' ) and not 'LHCb'  in projects : projects += [ 'LHCb'  ]
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
## browse the doxygen documentation for the class/object
def browse ( o , projects = None , latest = False ) :
    """
    Browse the doxygen documentation for the class/object
    """
    ## find the proper URL 
    url = getURL ( o , projects , latest )
    if not url : return None                                 ## RETURN
    import webbrowser
    print 'Trying to open URL=%s'%url 
    webbrowser.open ( url , new = True ) 
    return url                                           
# =============================================================================

if __name__ == "__main__" :
    print __doc__
    dir() 

# =============================================================================
# The END
# =============================================================================
