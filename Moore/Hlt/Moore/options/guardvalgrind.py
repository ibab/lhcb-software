if __name__ == "__main__":
    import GaudiPython
    app_mgr = GaudiPython.AppMgr()
    import os
    import time

    for n in xrange(5):
        if n == 1:
            os.system("callgrind_control -i on")
            time.sleep(1)

        print "run(%i)"%n
        app_mgr.run(1)
