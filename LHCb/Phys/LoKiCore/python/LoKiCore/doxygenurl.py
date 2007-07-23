#!/usr/bin/env python
# =============================================================================
"""
Simple module to provide access to Doxygen documentation for the various
classes and objects.
Actually the lines are stallen from T.Ruf's (Thomas.Ruf@cern.ch)
module gaudigadgets.py
"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu"
__date__    = "2007-07-22"

# =============================================================================
## URL-s patterns
# =============================================================================
_ROOTURL  = 'http://root.cern.ch/root/html/%s.html'
_GAUDIURL = 'http://proj-gaudi.web.cern.ch/proj-gaudi/releases/%s/doxygen/%s'
_LHCBURL  = 'http://LHCb-release-area.web.cern.ch/LHCb-release-area/DOC/%s/releases/%s/doxygen/%s'

# =============================================================================
## check the existence(validity) of the URL
def httpExists( url ) :
    """ Check the existence(validity) of the URL """ 
    
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
    target = os.sep + project.upper() + os.sep + project.upper() + "_"
    for value in os.environ.values() :
        l =  value.rfind(target)
        if 0 >= l : continue
        vers1 = value[l+2*len(project)+3:].split(os.pathsep)[0]
        vers2 = value[l+2*len(project)+3:].split(os.sep)[0]
        return min(vers1,vers2)
    return None 
# =============================================================================
## Convert C++ class name into the name of Doxygen file 
# =============================================================================
def _doxName ( klassname ) :
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
## get the URL with proper Doxygen documentation for the object
# =============================================================================
def getURL ( o , browse = False , projects = None ) :
    """
    Get the URL with the proper Doxygen documentation for the object/class
    The object could be:
    - type or namespace name (string)
    - the type
    - the instance 
    """
    if not projects :
        projects = [ "ROOT"     , "GAUDI" , "LHCB"  , "PHYS"   , "HLT"   ,
                     "ANALYSIS" , "REC"   , "BOOLE" , "BRUNEL" , "GAUSS" ,
                     "PANORAMIX" ]
        
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
    ## transform it into doxygen file name
    dox = _doxName ( klass )
    ## loop over all projects
    for project in projects :
        project = project.upper()
        if   'ROOT'  == project :
            url = _ROOTURL % klass 
            if httpExists( url ) :
                if browse :
                    import webbrowser
                    print url 
                    webbrowser.open ( url , new = True ) 
                return url 
        else :
            version = _projVersion ( project )
            print version 
            if not version : continue 
            if 'GAUDI' == project : url = _GAUDIURL %(version,dox)
            else                  : url = _LHCBURL  %(project.lower(),version,dox)
            print url 
            if httpExists ( url ) :
                if browse :
                    import webbrowser
                    print url 
                    webbrowser.open ( url , new = True ) 
                    return url 
                break   
    return None 

if __name__ == "__main__" :
    print __doc__
    dir() 

# =============================================================================
# The END
# =============================================================================
