import os, sys, string
batch = False
exe = os.popen('cmt show macro_value GAUDI_install_scripts').readlines()[0][:-1]
exe = exe + os.sep + '..' + os.sep + os.environ['CMTCONFIG']+os.sep+'bin'+os.sep+'Gaudi.exe'
mdf = os.environ['MDFROOT'].replace(os.sep,'/')
if sys.platform=='win32' and mdf[1]==':': mdf = mdf[2:]
os.environ['MDFROOT'] = mdf
print 'MDFROOT:',os.environ['MDFROOT']
def nextTest(msg):
  print msg
  val = ''
  try:
    val=str(input('Continue [Y=CR/N]'))
  except NameError, X:
    val = 'N'
  except SyntaxError, X:
    return 1
  except Exception, X:
    print X.__class__, str(X)
    val = 'N'
  if len(val) > 0:
    sys.exit(0)
    return 0
  return 1

def execute(cmd):
  c = cmd
  c=c.replace('/',os.sep)
  print '-->',c
  os.system(exe+" "+c)

if len(sys.argv)>1:
  batch=True
print 'gaudi executable is:',exe
tests = []
tests.append(mdf+"/options/CreateMDF.opts")
tests.append(mdf+"/options/ReadMDF.opts")
tests.append(mdf+"/options/CreateMIF.opts")
tests.append(mdf+"/options/ReadMIF.opts")
tests.append(mdf+"/options/CreatePOOL.opts")
tests.append(mdf+"/options/ReadPOOL.opts")
tests.append(mdf+"/options/CreateTAE.opts")
tests.append(mdf+"/options/ReadTAE.opts")
tests.append(mdf+"/options/ReadTAEPOOL.opts")
for i in tests:
  if batch:
    execute(i)
  elif nextTest(i)>0:
    execute(i)
