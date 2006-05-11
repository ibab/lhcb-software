import RunDatabase

db=RunDatabase.RunDatabase("RunDatabase")

def execRunDb(s):
  q = 'db.'+s
  res=eval(q)
  print q , ' = ',res
  return str(res)


import RunDb
wr = RunDb.RunDbWrapper(execRunDb,"TESTRPC/RunDB")



import time, random, math
def executeCmd(s):
  print 's=',s
  return 'hello'

import RunDb
cl = RunDb.RunDbClient(executeCmd,"TESTRPC/RunDB")
run = 12

cmds = []
cmds.append("")
cmds.append("""dump(RunNumber=123456)""")
cmds.append("""deleteRun(RunNumber=123456)""")
cmds.append("""createRun(StartDate='2001-05-11 14:32:46', PartitionID=103, BeamEnergy=6952.21664362, StartLuminosity=8930.56725429, RunNumber=123456, FillNumber=13345, SomeParam='Hello run 123456')""")
cmds.append("""addRunParam(123456, 'BeamEnergy', 6952.2166436229681, 'PARAM')""")
cmds.append("""addRunParam(123456, 'OtherParam', 1234567, 'ENV')""")
cmds.append("""runParam(123456, 'OtherParam')""")
cmds.append("""modifyRunParam(RunNumber=123456, Typ='SOME_TYPE', Name='OtherParam', Val=7654444456)""")
cmds.append("""runParam(123456, 'OtherParam')""")
cmds.append("""runParams(RunNumber=123456)""")
cmds.append("""runParams(RunNumber=123456, Typ=('LIKE', "'SOME%'"))""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/somefile_0.mdf', 'BB inclusive', Status='Opened', Type='MDF', ToCastor='YES')""")
cmds.append("""addFileParam(RunNumber=123456, Val=1234567890, Typ="INTEGER", Name="Option", File='/castor/cern.ch/lhcb/Run.00123456/somefile_0.mdf')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/somefile_0.mif', 'BB inclusive',     Status='Opened', Type='MIF', ToCastor='YES')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/somefile_1.mdf', 'J/Psi to Leptons', Status='Opened', Type='MDF', ToCastor='YES')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/somefile_1.mif', 'J/Psi to Leptons', Status='Opened', Type='MIF', ToCastor='YES')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/signal.mdf', 'Exclusive Signals', Status='Opened',          Type='MDF', ToCastor='YES')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/Junk_0.mdf', '2 kHz Junk', Status='Opened', Reconstruct='NO', Type='MDF', ToCastor='YES')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/Junk_1.mdf', '2 kHz Junk', Status='Opened', Reconstruct='NO', Type='MDF', ToCastor='YES')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb/Run.00123456/Junk_2.mdf', '2 kHz Junk', Status='Opened', Reconstruct='NO', Type='MDF', ToCastor='YES')""")
cmds.append("""addFile(123456, '/castor/cern.ch/lhcb.somefile_1.root', 'J/Psi to Leptons')""")
cmds.append("""modifyRun(RunNumber=123456, EndLumi=8635.34639147)""")
cmds.append("""modifyRun(123456,EndDate='2001-05-11 16:44:49')""")

for i in cmds:
  print i
  cl.executeCmd(i)
  time.sleep(5)
