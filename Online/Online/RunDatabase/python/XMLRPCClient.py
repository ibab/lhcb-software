"""
 -----------------------------------------------------------------------------

 Generic client for the LHCb XML RPC server @author  M.Frank @version 1.0

 -----------------------------------------------------------------------------
 """
class Client:
  """
      Client class

      @author  M.Frank
      @version 1.0
  """

  #============================================================================
  def __init__(self, url='http://lbnts1.cern.ch:8102'):
    """ Default constructor: Takes basic nod-URL"""
    self.url  = url + '/RPC/FileCatalog'
    self.call = 'doc'
    
  #============================================================================
  def __getattr__(self, name):
    self.call = name
    return self.execute

  #============================================================================
  def execute(self, *parms, **kws):
    """ Magic method dispatcher """
    import xmlrpclib
    err = ''
    if self.call is None:
      self.call = 'doc'
    try:
      return getattr(xmlrpclib.Server(self.url),self.call)(*parms, **kws)
    except Exception, X:
      err = 'Processing Error when calling "'+self.call+'":'+ str(X)
    except:
      err = 'Unknown Processing Error when calling "'+self.call+'"'
    print err
    return (0, err)
