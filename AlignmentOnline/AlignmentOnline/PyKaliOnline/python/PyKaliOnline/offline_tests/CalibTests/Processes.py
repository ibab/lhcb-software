import os, shutil
import pydim
from OfflineOnline.Processes import *

class Iterator(GaudiTestProcess):
    def __init__(self, logger, launcher, **kwargs):
        GaudiTestProcess.__init__(self, logger, launcher, kwargs.pop('priority'))
        self.__processes = []
        self.__dim_svc = []
        self.__python = kwargs.pop('python')
        
        # Stuff for synchronisation
        from multiprocessing import Lock
        self.__callback_lock = Lock()

        logger.info("Using temporary directory %s" % get_tmpdir())
                
    def utgid(self):
        return TestProcess.utgid(self, "ALIGNITER")
                
    def initialize(self):
        utgid = self.utgid()
        svc = utgid + '/status'
        self.logger().debug("Subscribing to Iterator status service.")
        self.__dim_svc.append(pydim.dic_info_service(svc, "C", self.__callback, pydim.MONITORED, 0, 0, None))

        if type(self.__python) == str:
            p = self.__python
            gaudi = True
        else:
            p = self.__python()
            gaudi = self.__python.gaudi()

        if gaudi:
            python_cmd = ("import GaudiKernel.ProcessJobOptions; " +
                          "from Gaudi.Configuration import importOptions; " +
                          "GaudiKernel.ProcessJobOptions.printing_level=999; ")
            command = ('GaudiOnlineExe.exe libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task' +
                       ' -msgsvc=LHCb::FmcMessageSvc -opt=command="{0}"' +
                       ' -main={1}').format(python_cmd + p, self.main_opts())
        else:
            command = 'python -c "{0}"'.format(p)
            
        ## Add this directory to the python path to get our OnlineEnvBase.py
        import inspect
        module = inspect.getfile(type(self))
        pd = os.path.dirname(module)
        env = {'LOGFIFO' : self.log_fifo(), 'PYTHONPATH' : '%s:%s' % (pd, os.environ['PYTHONPATH']),
               'TEMPDIR' : get_tmpdir(), 'PARTITION' : self.partition()}
        self.condition().acquire()
        self.launch(utgid, command, env)
        self.condition().wait()
        self.condition().release()
        self.logger().info("Iterator launched.")

    def run(self):
        self.configure()
                
    def configure(self):
        self.__send_command(self.utgid(), "configure")
        self.logger().info("Iterator initialized.")

        origin = "/group/online/alignment"
        tempdir = get_tmpdir() + "/OnlineXmlDir"
        v0s = ['Velo/VeloGlobal/v0.xml', 'Velo/VeloModules/v0.xml', 'TT/TTGlobal/v0.xml',
               'TT/TTModules/v0.xml', 'IT/ITGlobal/v0.xml', 'IT/ITModules/v0.xml',
               'OT/OTGlobal/v0.xml', 'OT/OTModules/v0.xml']
        for v0 in v0s:
            dest = os.path.join(tempdir, v0)
            dest_dir = os.path.dirname(dest)
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            shutil.copy(os.path.join(origin, v0), dest)
        asd_dir = get_tmpdir() + "/ASDDir"
        if not os.path.exists(asd_dir):
            os.makedirs(asd_dir)

    def start(self):
        # Send start to the iterator task and wait for it to go running.
        self.__send_command(self.utgid(), "start")

    def pause(self):
        self.condition().acquire()
        self.__send_command(self.utgid(), "pause", False)
        ## Wait for paused
        self.condition().wait()
        ## Wait for ready or running
        self.condition().wait()
        self.condition().release()
        if self.state() == State.RUNNING:
            return "continue"
        elif self.state() == State.READY:
            return "stop"
        else:
            raise RuntimeError("Bad state when pauseing: %s" % self.state())
        
    def stop(self):
        if self.state() == State.READY:
            return
        self.__send_command(self.utgid(), "stop")
    
    def finalize(self):
        if self.state() != State.READY:
            return
        # Send reset to all moore tasks and wait for them to finalize.
        self.__send_command(self.utgid(), "reset")
        self.set_state(State.NOT_READY)
        # Disconnect from DIM services
        for svc in self.__dim_svc:
            pydim.dic_release_service(svc)
            self.__dim_svc = []
                                    
    def terminate(self):
        # Kill self
        self.__send_command(self.utgid(), "unload", False)
        TestProcess.terminate(self)

    def launch(self, utgid, command, env, do_exec = False, node = 0):
        p = self.launcher().launch(utgid, command, env, do_exec, node)
        self.__processes.append(p)
        return p
            
    def __send_command(self, utgid, command, wait = True):
        if hasattr(pydim, "version") and float(pydim.version()) >= 0.4:
            command = (command,)
        else:
            command = (command + '\0',)
        if wait:
            self.condition().acquire()
            pydim.dic_cmnd_service(utgid, command, "C")
            self.condition().wait()
            self.condition().release()
        else:
            pydim.dic_cmnd_service(utgid, command, "C")

    def __callback(self, tag, val):
        if val == None:
            return
        self.__callback_lock.acquire()
        def __msg(tag, state):
            return "Iterator went to %s." % state
        try:
            if self.state() == State.UNKNOWN and val.strip().startswith("NOT_READY"):
                self.logger().info(__msg(tag, "NOT_READY"))
                self.set_state(State.NOT_READY)
                self._notify()
            if self.state() == State.NOT_READY and val.strip().startswith("READY"):
                self.logger().info(__msg(tag, "READY"))
                self.set_state(State.READY)
                self._notify()
            if self.state() in [State.READY, State.PAUSED] and val.strip().startswith("RUNNING"):
                self.logger().info(__msg(tag, "RUNNING"))
                self.set_state(State.RUNNING)
                self._notify()
            elif self.state() == State.PAUSED and val.strip().startswith("READY"):
                self.logger().info(__msg(tag, "READY"))
                self.set_state(State.READY)
                self._notify()
            if self.state() == State.RUNNING and val.strip().startswith("PAUSED"):
                self.logger().info(__msg(tag, "PAUSED"))
                self.set_state(State.PAUSED)
                self._notify()
            elif self.state() == State.READY and val.strip().startswith("NOT_READY"):
                self.logger().info(__msg(tag, "NOT_READY"))
                self.set_state(State.NOT_READY)
                self._notify()
            elif not val.strip().startswith(self.state()):
                self.logger().debug(__msg(tag, val.strip()))
        except Exception as e:
            self.logger().debug("exception while waiting for Iterator to start up ...", exc_info = True)
        self.__callback_lock.release()

class Analyser(GaudiTestProcess):
    def __init__(self, logger, launcher, **kwargs):
        GaudiTestProcess.__init__(self, logger, launcher, kwargs.pop('priority'))
        self.__n_workers = kwargs.pop('n_workers')
        self.__n_nodes = kwargs.pop('n_nodes')
        self.__processes = []
        self.__dim_svc = []
        self.__python = kwargs.pop('python')
        
        self.__states = [State.UNKNOWN] * self.__n_workers
        
        # Stuff for synchronisation
        from multiprocessing import Lock
        self.__callback_lock = Lock()

    def utgid(self, n):
        return TestProcess.utgid(self, "ALIGNWORK_%02d" % n)

    def state(self, n = -1):
        if n == -1:
            return GaudiTestProcess.state(self)
        else:
            return self.__states[n]
    
    def set_state(self, state, n = -1):
        if n == -1:
            GaudiTestProcess.set_state(self, state)
        else:
            self.__states[n] = state
                    
    def initialize(self):
        for n in range(self.__n_workers):
            utgid = self.utgid(n)
            svc = utgid + '/status'
            self.logger().debug("Subscribing to Alignment worker %d status service." % n)
            self.__dim_svc.append(pydim.dic_info_service(svc, "C", self.__callback, pydim.MONITORED, 0, n, None))

        ## Add this directory to the python path to get our OnlineEnvBase.py
        import inspect
        module = inspect.getfile(type(self))
        pd = os.path.dirname(module)
        env = {'LOGFIFO' : self.log_fifo(), 'PYTHONPATH' : '%s:%s' % (pd, os.environ['PYTHONPATH']),
               'TEMPDIR' : get_tmpdir(), 'PARTITION' : self.partition()}
        
        if type(self.__python) == str:
            gaudi = True
        else:
            gaudi = self.__python.gaudi()
        
        self.condition().acquire()
        for n in range(self.__n_workers):
            utgid = self.utgid(n)
            if type(self.__python) == str:
                if '{0}' in command:
                    p = self.__python.format(n)
                else:
                    p = self.__python
            else:
                p = self.__python(n)
            if gaudi:
                python_cmd = ("import GaudiKernel.ProcessJobOptions; " +
                              "from Gaudi.Configuration import importOptions; " +
                              "GaudiKernel.ProcessJobOptions.printing_level=999;")
                command = ('GaudiOnlineExe.exe libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task' +
                           ' -msgsvc=LHCb::FmcMessageSvc -opt=command="{0}" -main={1}')
                command = command.format(python_cmd + ' ' + p, self.main_opts())
            else:
                command = 'python -c "{0}"'.format(p)
            self.launch(utgid, command, env, node = n % self.__n_nodes)

        # Wait for not ready
        self.__wait_all(State.NOT_READY)
        self.condition().release()

    def run(self):
        self.configure()
        
    def configure(self):
        self.condition().acquire()
        # Send Configure
        for i in xrange(self.__n_workers):
            self.__send_command(self.utgid(i), "configure", False)
        # Wait for ready
        self.__wait_all(State.READY)
        self.logger().info("All Alignment workers ready.")
        self.condition().release()

    def start(self):
        # Send start to all tasks and wait for them to be ready
        for i in xrange(self.__n_workers):
            self.__send_command(self.utgid(i), "start")
        # Wait for running and paused
        self.condition().acquire()
        self.__wait_all(State.PAUSED)
        self.condition().release()
                
    def stop(self):
        if self.state() == State.READY:
            return
        # Stop process
        # Send stop to all tasks and wait for them to be ready.
        self.condition().acquire()
        for i in xrange(self.__n_workers):
            self.__send_command(self.utgid(i), "stop", False)
        self.__wait_all(State.READY)
        self.condition().release()
                
    def finalize(self):
        if self.state() != State.READY:
            return
        # Send reset to all moore tasks and wait for them to finalize.
        for i in xrange(self.__n_workers):
            self.__send_command(self.utgid(i), "reset")
        self.set_state(State.NOT_READY)
        # Disconnect from DIM services
        for svc in self.__dim_svc:
            pydim.dic_release_service(svc)
            self.__dim_svc = []
                                    
    def terminate(self):
        # Kill workers
        for i in xrange(self.__n_workers):
            self.__send_command(self.utgid(i), "unload", False)

        for p in self.__processes:
            if p == None:
                continue
            self._terminate(p)

    def results(self):
        return {}

    def launch(self, utgid, command, env, do_exec = False, node = 0):
        p = self.launcher().launch(utgid, command, env, do_exec, node)
        self.__processes.append(p)
        return p

    def __wait_all(self, state, callback = None, cb_args = (), ):
        def __count_state(s):
            return len(filter(lambda i: self.state(i) == s, xrange(self.__n_workers)))
        n_ready = __count_state(state)
        while n_ready < self.__n_workers:
            self.condition().wait()
            if callback:
                args = (i,) if not cb_args else args + tuple(cb_args)
                callback(*args)
            n_ready = __count_state(state)
        self.set_state(state)

    def __send_command(self, utgid, command, wait = True):
        if hasattr(pydim, "version") and float(pydim.version()) >= 0.4:
            command = (command,)
        else:
            command = (command + '\0',)
        if wait:
            self.condition().acquire()
            pydim.dic_cmnd_service(utgid, command, "C")
            self.condition().wait()
            self.condition().release()
        else:
            pydim.dic_cmnd_service(utgid, command, "C")

    def __callback(self, tag, val):
        print tag, val
        if val == None:
            return
        self.__callback_lock.acquire()
        def __msg(tag, state):
            return "Alignment worker %02d went to %s." % (tag, state)
        try:
            if self.state(tag) == State.UNKNOWN and val.startswith("NOT_READY"):
                self.logger().info(__msg(tag, "NOT_READY"))
                self.set_state(State.NOT_READY, tag)
                self._notify()
            if self.state(tag) == State.NOT_READY and val.startswith("READY"):
                self.logger().info(__msg(tag, "READY"))
                self.set_state(State.READY, tag)
                self._notify()
            if self.state(tag) == State.READY and val.startswith("RUNNING"):
                self.logger().info(__msg(tag, "RUNNING"))
                self.set_state(State.RUNNING, tag)
                self._notify()
            if self.state(tag) == State.RUNNING and val.startswith("PAUSED"):
                self.logger().info(__msg(tag, "PAUSED"))
                self.set_state(State.PAUSED, tag)
                self._notify()
            elif self.state(tag) == State.PAUSED and val.startswith("READY"):
                self.logger().info(__msg(tag, "READY"))
                self.set_state(State.READY, tag)
                self._notify()
            elif self.state(tag) == State.READY and val.startswith("NOT_READY"):
                self.logger().info(__msg(tag, "NOT_READY"))
                self.set_state(State.NOT_READY, tag)
                self._notify()
            elif not val.strip().startswith(self.state(tag)):
                self.logger().info("Unhandled transtion of Alignment worker %d from %s to %s" % (tag, self.state(tag), val.strip()))
        except Exception as e:
            self.logger().info("exception while waiting for Alignment worker %d to start up ..." % tag, exc_info = True)
        self.__callback_lock.release()
