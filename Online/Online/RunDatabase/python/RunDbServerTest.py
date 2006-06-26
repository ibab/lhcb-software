import time, RunDatabase

class RunDbTest:
  def __init__(self):
    self.first = 1
    self.tim = time.time() - 5*365*3600*24
    self.db = RunDatabase.RunDatabase("RunDatabase")
    self.fill=1000
    self.runno = 0

  def __getattr__(self, name):
    self.call = name
    return self.execute

  def execute(self, *parms, **kws):
    """ Magic method dispatcher """
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
      if ( res[0] == RunDatabase.SUCCESS ):
        if ( self.first ):
          print s+')    \t returned: ',str(res)
      else:
        print ' FAILED:',s+') \n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n',res[1],'\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n'
      return res
    except Exception, X:
      err = 'Processing Error when calling "'+self.call+'":'+ str(X)
    except:
      err = 'Unknown Processing Error when calling "'+self.call+'"'
    print err
    return {'Status':'Error', 'Result':err } 

  def addRun(self):
    import random, math
    start = int(self.tim)
    stop  = int(self.tim + 3000*2)
    self.tim = self.tim + 3600*2
    energy = 7000. + 50*(random.random()-1.0)
    lumi   = math.pow(10,4) * ( 1 + 0.2*(random.random()-1.0))
    self.fill =int(energy)
    if ( self.first ):
      self.existsRun(RunNumber=3)

    fill = self.fill
    self.runno = self.runno+1
    if ( self.first ):
      run = self.createRun(FillNumber=fill,Partition=0x103,Activity='Physics',StartDate=start,ProgramName='Moore',ProgramVersion='v1r23',SomeParam='Hello from fill '+str(fill)+' run '+str(self.runno))
      run = run[1]
      self.existsRun(RunNumber=run)
      self.deleteRun(RunNumber=run)
      self.existsRun(RunNumber=run)
      self.addRunParam(150000,'OtherParam',1234567,'ENV')
    run = self.createRun(FillNumber=fill,Partition=0x103,Activity='Physics',StartDate=start,ProgramName='Moore',ProgramVersion='v1r23',SomeParam='Hello from fill '+str(fill)+' run '+str(self.runno))
    run = run[1]

    self.addRunParam(run,'BeamEnergy',energy,'PARAM')
    self.addRunParam(run,'RunNumber',run,'PARAM')
    self.addRunParam(run,'OtherParam',1234567,'ENV')
    self.runParam(run,'OtherParam')
    self.modifyRunParam(RunNumber=run,Name='OtherParam',Typ='SOME_TYPE',Val=7654321000+run)
    self.runParam(run,'OtherParam')
    self.addRunParam(run,"OtherParam2","OtherValue 2")
    self.addRunParam(run,"OtherParam3","OtherValue 3")
    self.runParam(run,'OtherParam2')
    self.runParams(RunNumber=run)
    self.runParams(RunNumber=run,Typ=('LIKE',"'SOME%'"))
    self.runParam(run,'SomeParam')
    
    dir = '/castor/cern.ch/lhcb/Run.%08d/'%run
    self.addFile(run,dir+'somefile_0.mdf','BB inclusive',Type='MDF',FileStatus='Opened',ToCastor='YES')
    self.addFileParam(RunNumber=run,FileName=dir+'somefile_0.mdf',Name='Option',Val=1234567890,Typ='INTEGER')
    self.addFile(run,dir+'somefile_0.mif','BB inclusive',Type='MIF',FileStatus='Opened',ToCastor='YES')
    self.addFile(run,dir+'somefile_1.mdf','J/Psi to Leptons',Type='MDF',FileStatus='Opened',ToCastor='YES')
    self.addFile(run,dir+'somefile_1.mif','J/Psi to Leptons',Type='MIF',FileStatus='Opened',ToCastor='YES')
    self.addFile(run,dir+'signal.mdf','Exclusive Signals',Type='MDF',FileStatus='Opened',ToCastor='YES')
    self.addFile(run,dir+'Junk_0.mdf','2 kHz Junk',Type='MDF',FileStatus='Opened',ToCastor='YES',Reconstruct='NO')
    self.addFile(run,dir+'Junk_1.mdf','2 kHz Junk',Type='MDF',FileStatus='Opened',ToCastor='YES',Reconstruct='NO')
    self.addFile(run,dir+'Junk_2.mdf','2 kHz Junk',Type='MDF',FileStatus='Opened',ToCastor='YES',Reconstruct='NO')
    # add same file again: should give an error
    if ( self.first ):
      self.runParams(RunNumber=150000)
      self.runParam(run,'dasfjhgdahfgdas')
      self.addFile(run,'/castor/cern.ch/lhcb.somefile_1.root','J/Psi to Leptons')
    lumi   = math.pow(10,4) * ( 1 + 0.5*(random.random()-1.0))
    self.modifyRun(RunNumber=run,IntegratedLumi=lumi)
    self.modifyRun(run,EndDate=stop)
    self.first = 0
    return run

  def test2(self):
    self.nextRunNumber()
    self.nextFileID()
    self.nextSequenceID('Bla')

if __name__ == "__main__":
  RunDatabase.Installer('RunDatabase').uninstall(1)
  RunDatabase.Installer('RunDatabase').install(1)
  t = RunDbTest()
  #for i in xrange(1000):
  #  if ( i > 13 ): t.addRun(i)
  #t.test2()
  for i in xrange(200):
    t.first = None
    no = t.addRun()
    print 'Run ',no,' installed....'
  t.dump(RunNumber=10)  
