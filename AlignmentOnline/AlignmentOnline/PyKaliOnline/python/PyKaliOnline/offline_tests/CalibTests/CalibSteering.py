from OfflineOnline.Processes import *
from OfflineOnline.BenchmarkProcess import *
from CalibTests.Processes import Iterator, Analyser

class CalibrationSteering(BenchmarkProcessBase):
    def __init__(self, process_id, logger, components):
        BenchmarkProcessBase.__init__(self, logger, self.run)

        self.__process_id = process_id
        self.__components = components
        self.__processes = []
        for process, kwargs in components:
            launcher = kwargs.pop('launcher')
            p = process(logger, launcher, **kwargs)
            if process == Iterator:
                self.__iterator = p
            elif process == Analyser:
                self.__analyser = p
            self.__processes.append(p)

    def process_id(self):
        return self.__process_id
            
    def initialize(self):
        for p in self.processes():
            p.initialize()
        self.put_status("initialized")

    def processes(self, reverse = False):
        return sorted(self.__processes, key = lambda p: p.priority(), reverse = reverse)

    def run( self ):
        self.initialize()

        command = self.get_command()
        if command == "run":
            # This autostarts all the other components, for the iterator and
            # analyser, it calls configure.
            for p in self.processes():
                p.run()
            # Start the iterator
            self.__iterator.start()
        else:
            self.logger().error("Bad command from queue: %s" % command)

        while True:
            ## Run workers
            self.__analyser.start()
            self.__analyser.stop()
            ## Signal iterator
            status = self.__iterator.pause()
            if status == 'stop':
                break
        
        for p in self.processes():
            p.stop()
        for p in sorted(self.__processes, key = lambda p: p.priority(), reverse = True):
            p.finalize()
        self.logger().info("%s is done" % self.process_id())
        self.put_status("done")
        self.close_status()
        self.terminate()

    def terminate(self):
        for p in self.processes(True):
            p.terminate()

class CalibrationTest(object):
    def __init__(self, **kwargs):
        test_module = kwargs.pop('test_module')
        viewers = kwargs.pop('viewers', False)
        n_nodes = kwargs.pop('n_nodes')
        n_workers = kwargs.pop('n_workers')
        partition = kwargs.pop('partition')
        do_numa = kwargs.pop('numa')
        log_file = kwargs.pop('log_file')
        input_files = kwargs.pop('input_files')
        self.__logger = kwargs.pop('logger')

        from collections import defaultdict
        self.__results = defaultdict(list)

        import numa
        max_nodes = numa.get_max_node() + 1        
        if do_numa:
            if n_nodes > max_nodes:
                print ('WARNING: %d NUMA nodes have been requested to be used, ' +
                       'but only %d are available and will be used.')
                n_nodes = max_nodes
            n_nodes = max_nodes if n_nodes == -1 else n_nodes
            CalibLauncher = NumaLauncher
        else:
            n_nodes = 1 if n_nodes == -1 else n_nodes
            CalibLauncher = Launcher

        if n_workers != -1:
            n_work = n_workers
        else:
            import multiprocessing
            n_work = int(float(multiprocessing.cpu_count()) / float(max_nodes) + 0.5)

        tms = __import__("CalibTests.TestModules" , globals(), locals(), [test_module])
        tm = getattr(tms, test_module)

        import os
        directory = os.path.dirname(input_files[0])
            
        self.__logger.info("Will start %d workers on %d nodes." % (n_work, n_nodes))
        launcher = Launcher(partition, self.__logger)
        self.__components = [(LogServer, dict(launcher = launcher, priority = 1)),
                             (Iterator, dict(launcher = launcher, priority = 3,
                                             python = tm.Iterator(directory))),
                             (Analyser, dict(n_nodes = n_nodes, n_workers = n_work,
                                             python = tm.Analyzer(input_files),
                                             launcher = CalibLauncher(partition, self.__logger),
                                             priority = 4))]
        if log_file:
            self.__components.append((LogViewer, dict(launcher = launcher, priority = 2,
                                                      output_file = log_file, timing_width = 90,
                                                      split_regex = 'CALIBWORK_(\d+)')))
        if viewers: 
            self.__components.append((LogViewer, dict(launcher = launcher, priority = 2)))

        self.__calib_process = CalibrationSteering("CalibrationTest", self.__logger, self.__components)

        # To get an idea of overall CPU usage
        from OfflineOnline.BenchmarkProcess import CPUMeasurer
        self.__cpu_process = CPUMeasurer(self.__logger)

    def start(self):
        ps = [self.__cpu_process, self.__calib_process]
        for bp in ps:
            bp.start()

        for bp in ps:
            status = bp.get_status()
            if status != 'initialized':
                self.__logger.warning("Got unexpected data from benchmark processes %s" % data)

    def run(self):
        self.__cpu_process.put_command("run")
        self.__calib_process.put_command("run")

        ## Wait for iterations to complete
        r = self.__calib_process.get_status()
        if r != "done":
            self.__logger.warning("Got unexpected status from calibment process %s" % r)
        self.__cpu_process.put_command("stop")
        try:
            while True:
                r = self.__cpu_process.get_status()
                if r == "done":
                    break
                else:
                    self.__results[self.__cpu_process.process_id()].append(r)
        except EOFError:
            pass

        self.__cpu_process.join()
        self.__calib_process.join()

    def results(self):
        return self.__results

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.__cpu_process.terminate()
        self.__calib_process.terminate()

