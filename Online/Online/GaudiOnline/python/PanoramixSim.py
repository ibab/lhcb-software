import GaudiOnlineTests

def prompt():
  print "Exit the event loop:             0"
  print "Execut next event:               1"
  print "Reinitialize the event selector: 2"
  try:
    return int(input("Issue new command: "))
  except:
    print 'Default: exit the event loop'
    return 0

def reinit(app):
  mgr = app._appmgr
  evt = app.service('EventLoopMgr')
  mgr.stop()
  print 'Starting EventLoopMgr to reinitialize.....'
  evt.reinitialize()
  mgr.start()
  print 'EventLoopMgr reinitialized.....'
  return 1
  
def run(source):
  import time
  try:
    inp = 1
    setup,app = GaudiOnlineTests.runPanoramixSim(source,1,1)
    print setup
    while inp>0:
      inp = prompt()
      if inp==0:
        print 'End event loop and exit.....'
        break
      elif inp==1:
        app.run(1)
      elif inp==2:
        reinit(app)
  except:
    pass
  time.sleep(5)

if __name__ == '__main__':
  run()
  
