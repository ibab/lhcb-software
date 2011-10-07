#@PydevCodeAnalysisIgnore
#----------------------------------#
# -- Author: V.Garonne
# -- Mail: garonne@lal.in2p3.fr
# -- Date: 08/25/2006 
# -- Name: tbroadcast
# -- Description: main class
#----------------------------------#

import os
import sys
import time
import exceptions
from threading import BoundedSemaphore

from LbConfiguration.External import tbroadcast_version
_tbroadcast_install_dir = os.path.join(os.path.dirname(os.environ["CMTROOT"]), "tbroadcast", tbroadcast_version)
sys.path.insert(0, os.path.join(_tbroadcast_install_dir, "python"))


from threadpool import WorkRequest
from threadpool import ThreadPool
from threadpool import NoResultsPending
from threadpool import NoWorkersAvailable
from threadpool import makeRequests
from executer   import exeCommand
from executer   import getstatusoutput

class Scheduler:

    def __init__(self, num_workers=20, filename=None, ignore_cycles=False, local=False,
                 output=None, error=None, silent=False, perf=False, keep_going=True):
        self.pool = ThreadPool(num_workers=num_workers)
        self.current_package = self.get_current_package()
        self.current_project = {'name': None, 'path': None, 'version': None}
        self.packages = {}
        self.counter = 0
        self.semaphore = BoundedSemaphore(1)
        self.local = local
        self.ignore_cycles = ignore_cycles
        self.output = output
        self.error = error
        self.silent = silent
        self.perf = perf
        self.keep_going = keep_going
        if self.perf is not False:
            f = open (self.perf, 'w+')
            f.close()
        if output is not None:
            if not os.path.exists (output):
                print "path", output, "no exits"
                sys.exit(-1)
            if not os.path.isdir(output):
                print "path", output, "no a valid directory"
                sys.exit(-1)

        self.get_current_project()
        self.instanciate_packages (filename)
        if self.local: self.get_local_graph()
        self.check_cycles()


    def get_current_project(self):
        cmd = 'cmt show projects | grep current'
        status, output = getstatusoutput (cmd)
        if status != 0:
            print output
            sys.exit(-1)
        lines = output.split('\n')
        for line in lines:
            if line != '' and line [0] != '#':
                item = line.split(' ')
                self.current_project ['name'] = item[0]
                self.current_project ['version'] = item[1]
                self.current_project ['path'] = item[3][:-1]
                version = self.current_project ['path'][len(self.current_project ['path']) - len(self.current_project ['version']):]
                if self.current_project ['version'] and self.current_project ['version'] == version:
                    self.current_project ['path'] = self.current_project ['path'][:-len(self.current_project ['version'])]
                self.current_project ['path'] = os.path.normpath(self.current_project ['path'])
                return
                #print self.current_project     

    def get_counter(self):
        self.semaphore.acquire ()
        self.counter = self.counter + 1
        value = self.counter
        self.semaphore.release()
        return value

    def check_cycles (self):
        cmd = 'cmt -private show cycles'
        status, output = getstatusoutput (cmd)
        if status != 0:
            print output
            sys.exit(-1)
        lines = output.split('\n')
        cycles = list ()
        for line in lines:
            if line != '' and line [0] != '#':
                cycles.append (line.split())
        cercles = list()
        for cycle in cycles:
            cycleInProject = True
            for package in cycle:
                if not self.packages.has_key(package):
                    cycleInProject = False
            if cycleInProject:
                cercles.append(cycle)
        if len(cercles):
            if not self.ignore_cycles:
                print "# Error: cycles found, not possible to execute broadcast with threads. Please correct the following cycles:"
                for cycle in cercles:
                    loop = ""
                    for package in cycle:
                        loop = loop + package + ' -> '
                    print loop + '...'
                sys.exit(-1)
            else:
                print "# Warning: There are cycles and you have selected the automatic suppress cycles mode"
                for cycle in cercles:
                    loop = ""
                    for package in cycle:
                        loop = loop + package + ' -> '
                    if cycle[0] in self.packages[cycle[len(cycle) - 1]]['uses']:
                        print '## In cycle: ' + loop + '..., we suppress the dependency ' + cycle[len(cycle) - 1] + '->' + cycle[0]
                        self.packages[cycle[len(cycle) - 1]]['uses'].remove(cycle[0])

    def format_uses (self, content):
        # format variables
        lignes = content.split('\n')
        lines = list()
        for ligne in lignes:
            if ligne [0] == '#' and ligne[:5] != "#CMT>" and ligne[:10] != "# Required" and ligne not in ['# Selection :', '#']:
                lines.append(ligne)
        lines.reverse()
        return lines

    def format_paths (self, content):
        # format variables
        lignes = content.split('\n')
        lines = list()
        for ligne in lignes:
            if ligne[:4] == "use ":
                lines.append(ligne)
        return lines

    def get_paths (self, content):
        lines = self.format_paths(content)
        for line in lines:
            result = line[4:len(line)].split(' ')
            if  self.packages.has_key(result[0]):
                if len(result) == 4:
                    name, version, offset, path = line[4:len(line)].split(" ")
                    #print name, version, offset, path
                    #print path[1:-1] + '/' + offset + '/' +name + '/' + version + '/cmt'
                    if path == '(no_auto_imports)':
                        path = offset
                        offset = ''
                    if os.path.exists(path[1:-1] + '/' + offset + '/' + name + '/' + version + '/cmt'):
                        full_path = path[1:-1] + '/' + offset + '/' + name + '/' + version + '/cmt'
                    elif os.path.exists(path[1:-1] + '/' + offset + '/' + name + '/cmt'):
                        full_path = path[1:-1] + '/' + offset + '/' + name + '/cmt'
                    else:
                        print '# error path not found for', name
                        sys.exit(-1)
                elif len(result) == 5:
                    name, version, offset, path, importation = line[4:len(line)].split(" ")
                    if os.path.exists(path[1:-1] + '/' + offset + '/' + name + '/' + version + '/cmt'):
                        full_path = path[1:-1] + '/' + offset + '/' + name + '/' + version + '/cmt'
                    elif os.path.exists(path[1:-1] + '/' + offset + '/' + name + '/cmt'):
                        full_path = path[1:-1] + '/' + offset + '/' + name + '/cmt'
                    else:
                        print '# error path not found for', name
                        sys.exit(-1)
                elif len(result) == 3:
                    name, version, path = line[4:len(line)].split(" ")
                    if os.path.exists(path[1:-1] + '/' + name + '/' + version + '/cmt'):
                        full_path = path[1:-1] + '/' + name + '/' + version + '/cmt'
                    elif os.path.exists(path[1:-1] + '/' + name + +'/cmt'):
                        full_path = path[1:-1] + '/' + name + +'/cmt'
                    else:
                        print '# error path not found for', name
                        sys.exit(-1)
                else:
                    print "error:", line
                    print str(result)
                    sys.exit(-1)
                self.packages[result[0]]['path'] = os.path.normpath(full_path)
                commonprefix = os.path.commonprefix([self.packages[result[0]]['path'], self.current_project ['path']])
                if os.path.normpath(commonprefix) == self.current_project ['path']:
                    #print result[0], ' belong to project', self.current_project ['name']
                    self.packages[result[0]]['current_project'] = True

    def get_uses(self, content):
        # initiates variables
        lignes = self.format_uses(content)
        if not len(lignes): return
        self.packages [self.current_package] = {'version': '*', 'client': list(),
                                                'uses': list(), 'status': 'waiting',
                                                'current_project': True, 'path': os.getcwd()}
        previous_client = self.current_package
        previous_level = 0
        level_stack = [{'name':previous_client, 'level':previous_level}, ]
        ligne = lignes.pop()
        while len(lignes) != 0:
            current_level = ligne.find('use')
            while current_level > previous_level:
                name = ligne.split()[2]
                version = ligne.split()[3]
                if not self.packages.has_key (name):
                    self.packages [name] = {'version': version, 'uses': list(),
                                            'client': list(), 'status': 'waiting',
                                            'current_project': False, 'path': None}
                if name not in self.packages[previous_client]['uses']:# and name != previous_client:
                    self.packages[previous_client]['uses'].append (name)
                level_stack.append({'name':previous_client, 'level':previous_level})
                previous_client = name
                previous_level = current_level
                if len(lignes):
                    ligne = lignes.pop()
                    current_level = ligne.find('use')

            # restore the level
            if len(lignes):
                if len(level_stack):
                    item = level_stack.pop()
                    while item['level'] >= current_level and len(level_stack):
                        item = level_stack.pop()
                    previous_client = item['name']
                    previous_level = item['level']

    def instanciate_packages(self, filename=None):
        # We create the schedule of the work units
        print '# First, we initialize the DAG by parsing "cmt show uses"'
        if filename is None:
            cmd = 'cmt show uses'
        else:
            cmd = 'cat ' + filename
        status, output = getstatusoutput (cmd)
        if status != 0:
            print output
            sys.exit(-1)
        self.get_uses(output)
        self.get_paths(output)
        #self.check_execution (package=self.current_package)
        #self.simulate_execution()

    def get_local_graph(self):
        To_remove = list()
        for key in self.packages:
            if self.packages[key]['current_project'] == False:
                for selected in self.packages:
                    if key in self.packages[selected]['uses']:
                        self.packages[selected]['uses'].remove(key)
                To_remove.append (key)
        for item in To_remove:
            del self.packages[item]

    def simulate_execution(self):
        ok = True
        indice = 1
        while ok:
            runnable = list()
            for key in self.packages:
                if  self.packages[key]['status'] != 'done':
                    if len(self.packages[key]['uses']) == 0:
                        runnable.append(key)
            if len(runnable):
                print '\n#--------------------------------------------------------------'
                print "# Execute parallel actions within packages " + str(runnable)
            for selected in runnable:
                print '#--------------------------------------------------------------'
                print '# (' + str(indice) + '/' + str(len(self.packages)) + ') Now trying [] in ' + self.packages[selected]['path']
                print '#--------------------------------------------------------------'
                self.packages[selected]['status'] = 'done'
                indice = indice + 1
                for key in self.packages:
                    if selected in self.packages[key]['uses']:
                        self.packages[key]['uses'].remove(selected)
            if len(runnable) == 0:
                ok = False

    def check_execution(self, package, path=list(), cycles=list()):
        #print package,'-->',self.packages[package]['uses']
        #print path
        if package in path:
            if path[path.index(package):] not in cycles:
                print 'Cycles:', path[path.index(package):], package
                cycles = cycles + path[path.index(package):]
                sys.exit(-1)
        path.append(package)
        for item in self.packages[package]['uses']:
            self.check_execution(package=item, path=path, cycles=cycles)
            path.pop()

    def get_current_package(self):
        cmd = 'cmt show macro package'
        status, output = getstatusoutput (cmd)
        if status != 0:
            print output
            sys.exit(-1)
        lines = output.split('\n')
        for line in lines:
            if line [0] != '#':
                start = line.find("'")
                end = line[start + 1:len(line)].find("'")
                return line [start + 1:start + end + 1]

    def get_work_area_path (self, name):
        return self.packages [name]['path']

    def get_package_path (self, name):
        #return os.getcwd ()
        cmd = 'cmt -use=' + name + ' run pwd'
        status, output = getstatusoutput (cmd)
        if status != 0:
            print output
            sys.exit(-1)
        lines = output.split('\n')
        for line in lines:
            if line [0] != '#' and line[:5] != "#CMT>":
                print line
                return line

    def print_dependencies(self):
        print '# ------------------------'
        print '# package --> dependencies'
        print '# ------------------------'
        for key in self.packages.keys():
            print key, '-->', self.packages[key] ['uses'], ',', self.packages[key] ['status']

    def print_status(self, status):
        print '# ------------------------'
        print '# package --> dependencies'
        print '# ------------------------'
        i = 1
        for key in self.packages.keys():
            if self.packages[key] ['status'] == status:
                print i , key, '-->', self.packages[key] ['uses'], ',', self.packages[key] ['status']
                i = i + 1

    def is_work_unit_waiting (self, name):
        return self.packages[name] ['status'] == 'waiting'

    def set_work_unit_status (self, name, status):
        self.packages[name] ['status'] = status

    def get_dependencies (self, name):
        return self.packages[name] ['uses']

    def get_next_work_units (self):
        result = list ()
        for key in self.packages.keys():
            if len(self.get_dependencies (key)) == 0 and self.is_work_unit_waiting(key) :
                result.append(key)
        return result

    def is_work_units (self):
        for key in self.packages.keys():
            if self.is_work_unit_waiting(key) :
                return True
        return False

    def suppress_work_unit (self, name):
        #print '# remove', name, 'from schedule' 
        for key in self.packages.keys():
            if name in self.packages[key]['uses']:
                self.packages[key]['uses'].remove(name)

    def add_work_unit (self, name, cmd):
        if self.is_work_unit_waiting (name):
            # we create requests
            arg = {'cmd': cmd , 'package':name}
            req = WorkRequest(self.do_execute, [arg] , None, callback=self.result_callback, exc_callback=self.handle_exception)
            # then we put the work request in the queue...
            self.set_work_unit_status (name, 'queued')
            self.pool.putRequest(req)
            #print "# Work request #%s added on %s." % (req.requestID, str(arg['package']))

    def execute (self, command):
        #self.print_dependencies ()
        packages = self.get_next_work_units()
        if len(packages) != 0:
            print '\n#--------------------------------------------------------------'
            print '# Execute parallel actions within packages', packages
            for package in packages:
                self.add_work_unit (package, command)

    def execute_all(self, command):
        #self.print_dependencies ()
        self.execute (command)
        self.wait()
        #self.print_dependencies ()
        #self.print_status (status='waiting')        
        #while self.is_work_units(): 
        #self.wait()            

    def wait (self):
        self.pool.wait()

    # this will be called each time a result is available
    def result_callback(self, request, result):
        self.execute (result['cmd'])

    # the work the threads will have to do 
    def do_execute(self, arg):
        path = self.get_work_area_path (arg['package'])
        if path == None or not os.path.exists(path):
            raise RuntimeError('Path to package ' + arg['package'] + ' not found')
        self.set_work_unit_status (arg['package'], 'running')
        header = '#--------------------------------------------------------------\n'
        header = header + '# (' + str(self.get_counter()) + '/' + str(len(self.packages)) + ') Now trying [' + arg['cmd'] + '] in ' + path + '\n'
        header = header + '#--------------------------------------------------------------\n'
        print header
        project_path = self.current_project['path'] + '/' + self.current_project['version'] + '/'
        log_name = path.replace(project_path, '')
        log_name = log_name.replace('/cmt', '')
        log_name = log_name.replace('/', '_')
        log_name = log_name + '.loglog'
        arg['log'] = log_name
        cmd = "cd " + path + ";" + arg['cmd']
        # init output file
        if self.output is not None:
            f = open (self.output + '/' + log_name, 'w+')
            f.write (header)
            f.close()
            if self.error is not None:
                f = open (self.error + '/error' + log_name, 'w+')
                f.close()
        self.packages[arg['package']] ['startTime'] = time.time ()
        status, output, error, pythonError = exeCommand(sCmd=cmd, oLineCallback=self.redirectOutput, arg=arg)#,iTimeout = 3600)
        if not self.keep_going and status > 0:
            sys.exit(status)

        self.packages[arg['package']] ['endTime'] = time.time ()
        if self.perf:
            self.semaphore.acquire ()
            f = open (self.perf, 'a')
            f.write (arg['package'] + " " + str(self.packages[arg['package']] ['startTime']) + " " + str(self.packages[arg['package']] ['endTime']) + '\n')
            f.close()
            self.semaphore.release()
        self.suppress_work_unit (arg['package'])
        self.set_work_unit_status (arg['package'], 'done')
        return {'output':output, 'cmd': arg['cmd'], 'package':arg['package']}

    def redirectOutput(self, index, buf, arg):
        """Filter function to redirect the std output and error of the job
           executable for real-time debugging 
        """
        if self.output is not None:
            if index == 0:
                f = open (self.output + '/' + arg['log'], 'a')
                f.write (buf + '\n')
                f.close()
            elif index == 1:
                if self.error is not None:
                    f = open (self.error + '/error' + arg['log'], 'a')
                else:
                    f = open (self.output + '/' + arg['log'], 'a')
                f.write (buf + '\n')
                f.close()
        if not self.silent:
            print buf

    # this will be called when an exception occurs within a thread
    def handle_exception(self, request, exc_info):
        print '#--------------------------------------------------------------'
        if exc_info[0] == exceptions.SystemExit:
            print "Stop execution (No_keep_going option enabled): exit code == %s " % (exc_info[1])
            print '#--------------------------------------------------------------'
            sys.exit(exc_info[1])
        print "# Exception occured: %s" % (exc_info[1])
        print exc_info
        print '#--------------------------------------------------------------'


    def generate_make (self, filename, command):
        makefile = open (filename, 'w+')
        makefile.write ('MAKE=make\n')
        #MFLAGS= -j10
        self.counter = len(self.packages)
        self.recursive_make (self.current_package, command, makefile, len(self.packages))
        makefile.close ()

    def recursive_make (self, package, command, makefile, indice, actions=list()):
        lines = self.generate_action_make (package, command)
        makefile.write (lines)
        #print lines                
        for pkg in self.packages[package] ['uses']:
            if pkg not in actions:
                actions.append(pkg)
                indice = indice - 1
                self.counter = self.counter - 1
                self.recursive_make(pkg, command, makefile, indice, actions)

    def generate_action_make (self, package, command):
        lines = package + ' :: '
        # add dependencies
        for pkg in self.packages[package] ['uses']:
            lines = lines + ' ' + pkg

        # add the action itself
        newcommand = command.replace('<package>', package)
        if command == '':
            newcommand = '$(MAKE)'
        lines = lines + '\n'
        lines = lines + '\t@echo "#--------------------------------------------------------------"\n'
        lines = lines + '\t@echo "# (' + str(self.counter) + '/' + str(len(self.packages)) + ') Now trying [' + newcommand + '] in ' + self.packages[package]['path'] + '"\n'
        lines = lines + '\t@echo "#--------------------------------------------------------------"\n'
        lines = lines + 'ifdef LOCATION\n'
        lines = lines + '\t@echo "#--------------------------------------------------------------"> $(LOCATION)/' + package + '.loglog\n'
        lines = lines + '\t@echo "# (' + str(self.counter) + '/' + str(len(self.packages)) + ') Now trying [' + newcommand + '] in ' + self.packages[package]['path'] + '">> $(LOCATION)/' + package + '.loglog\n'
        lines = lines + '\t@echo "#--------------------------------------------------------------">> $(LOCATION)/' + package + '.loglog\n'
        lines = lines + '\t+@cd ' + self.packages[package]['path']
        lines = lines + ' && ' + newcommand + ' >> $(LOCATION)/' + package + '.loglog 2>&1\n'
        lines = lines + 'else\n'
        lines = lines + '\t+@cd ' + self.packages[package]['path']
        lines = lines + ' && ' + newcommand + '\n'
        lines = lines + 'endif\n\n'
        return lines

#--------- EoF --------#
