#!/usr/bin/env python
import os, sys, re
import argparse
import pydim
import atexit

def is_valid_dir(parser, arg):
    if arg != None and (not arg or not os.path.exists(arg) or not os.path.isdir(arg)):
       parser.error("The file %s does not exist!" % arg)
    else:
       return arg

parser = argparse.ArgumentParser(usage = 'usage: %(prog)s TestModule',
                                 description = "Standalone test of the online alignment framework.")

parser.add_argument("test_module", type = str, help = "Which module to import from "
                    +"CalibrationTests.TestModules to start the iterator and analyzers.")
parser.add_argument("--numa", action = 'store_true', dest = 'numa', default = False,
                    help = "Use NUMA binding to bind instances of all tasks to "
                    "specific CPU and memory nodes.")
parser.add_argument("--directory", type = lambda x: is_valid_dir(parser, x),
                    dest = "directory", default = None)
parser.add_argument("--dim-dns-node", type = str, dest = "dim_dns_node", default = "localhost",
                    help = "Hostname or IP address of the DIM dns node.")
parser.add_argument("--nodes", type = int, dest = "nodes", default = -1, choices = [-1] + range(1, 10),
                    help = "How many NUMA nodes to run on, default is all.")
parser.add_argument("--workers", type = int, dest = "workers", default = -1,
                    help = "Number of tasks to run per node.")
parser.add_argument("--log-file", type = str, dest = "log", default = 'KaliOnlineTest.log',
                    help = "Log file to use. If --task-log contains a directory, the log file " +
                    " will be put in that directory.")
parser.add_argument("--task-log", dest = "task_log", type = str,
                    default = "log", help = "Write Iterator and Worker tasks output to (split) log file;" +
                    " may contain a directory.")
parser.add_argument("--viewers", action = "store_true", dest = "viewers",
                    help = "Run the LogViewer in an xterm.")
args = parser.parse_args()

os.environ["DIM_DNS_NODE"] = args.dim_dns_node
if not pydim.dis_get_dns_node():
    print "No Dim DNS node found, problem with DIM_DNS_NODE??"
    sys.exit(1)

# if the default fifo isn't there, stuff doesn't work. Create it in that case.
if not os.path.exists("/tmp/logSrv.fifo"):
    os.mkfifo("/tmp/logSrv.fifo")

# Write log in directory
if os.path.sep in args.task_log:
    d, f = args.task_log.rsplit(os.path.sep, 1)
    if not os.path.exists(d):
        os.makedirs(d)
    log_file = os.path.join(d, args.log)
else:
    log_file = args.log
        
# logging
import logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
file_handler = logging.FileHandler(log_file)
formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
file_handler.setFormatter(formatter)
file_handler.setLevel(logging.DEBUG)
stream_handler = logging.StreamHandler(sys.stdout)
stream_handler.setLevel(logging.INFO)
logger.addHandler(file_handler)
logger.addHandler(stream_handler)

input_dir = args.directory
if not input_dir:
    print "WARNING: no input directory given, make sure this is what you want!"
    input_files = []
else:
    files = [f for f in os.listdir(input_dir) if re.match(r'.*\.(mdf|dst|fmDST|root)$', f)]
    # Check that there are mdf files
    if not files:
        print 'The input directory must contain RAW or DST files to run over.'
        sys.exit(-1)

    prefixes = set(fn[ : 4] for fn in files)
    if len(prefixes) != 1:
        print 'All files in the input directory must have filenames with the same first 4 characters.'
        sys.exit(-1)

    input_files = [os.path.join(input_dir, i) for i in sorted(list(files))]
    if len(input_files) == 1:
        input_files *= args.workers

# Instantiate the benchmark processes.
from CalibTests.CalibSteering import CalibrationTest
test_args = dict(viewers = args.viewers, n_workers = args.workers,
                 input_files = input_files, log_file = args.task_log, logger = logger,
                 n_nodes = args.nodes, numa = args.numa, partition = "TEST",
                 test_module = args.test_module)

with CalibrationTest(**test_args) as test:
    test.start()

    logger.info("Starting test")
    test.run()
    logger.info("Test completed")

    results = test.results()
