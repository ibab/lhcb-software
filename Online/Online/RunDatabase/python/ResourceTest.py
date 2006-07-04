class ResourceTest:
  def __init__(self):
    import ResourceDatabase
    self.db = ResourceDatabase.ResourceDatabase()

  def __getattr__(self, name):
    self.call = name
    return self.execute

  def execute(self, *parms, **kws):
    """ Magic method dispatcher """
    import ResourceDatabase
    err = ''
    if self.call is None:
      self.call = 'doc'
    try:
      res = getattr(self.db,self.call)(*parms, **kws)
      s = '      db.'+str(self.call)+'('
      if ( len(parms) > 0 ):
        q = str(parms)
        s = s + q[1:len(q)-1] + ', '
      if ( len(kws) > 0 ):
        for a in kws: s = s + a + '=' + str(kws[a]) + ', '
      if ( s[-2:] == ', ' ): s = s[:-2]
      if ( res[0] == ResourceDatabase.SUCCESS ):
        if ( self.first ):
          print '%-40s    \t returned: %s'%(s+')',str(res))
      else:
        print s+')    \t FAILED.\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n',res[1],'\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n'
      return res
    except Exception, X:
      err = 'Processing Error when calling "'+self.call+'":'+ str(X)
    except:
      err = 'Unknown Processing Error when calling "'+self.call+'"'
    print err
    return {'Status':'Error', 'Result':err } 

if __name__ == "__main__":
  t = ResourceTest()
  #t.uninstall()
  #t.install()
  res = 'RunNumber'
  #t.create(res,-1,0)
  t.create(res,-1,0)
  t.book(res)
  t.book(res)
  t.exists(res)
  t.book(res)
  t.last(res)
  t.release(res,0)
  t.release(res,1)
  res = 'NonSeqresource'
  t.create(res,5,0)
  t.create(res,5,0)
  t.book(res)
  t.book(res)
  t.book(res)
  t.book(res)
  t.book(res)
  t.book(res)
  t.release(res,9)
  t.release(res,0)
  t.release(res,1)
  t.release(res,2)
  t.release(res,3)
  t.release(res,4)
  t.release(res,4)
  t.release(res,5)
  t.release(res,5)
  t.release(res,8)
  res = 'NonSeq2'
  t.create(res,5,111)
  t.book(res)
  t.book(res)
  t.book(res)
  t.book(res)
  t.book(res)
  t.book(res)
  t.release(res,119)
  t.release(res,110)
  t.release(res,111)
  t.release(res,112)
  t.release(res,113)
  t.release(res,114)
  t.release(res,114)
  t.release(res,115)
  t.release(res,116)
  