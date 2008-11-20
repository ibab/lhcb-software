#!/usr/bin/env python

# this script is for a qmtest
# 
# it monitors a other process
# that runs a Moore instance
# for 50 000 events 
#
# it records the vmsize of the
# Moore process and checks 
# that there is no increase
# after 20 000 events
#
# Author: Stephan Nies

import subprocess, time, sys, os, re

sys.path.append(os.environ['MOOREROOT'] + '/tests/options')
import plotter

# define a pattern to get event number
# from Moore output
event_pat = re.compile('.*?SUCCESS Reading Event record (?P<event>\d+)\.\s.*')

# function to retrieve vmsize via
# reading the /proc/<pid>/stat 
def get_vm_size(pid):
  f = file('/proc/%s/stat' % str(pid))
  stat = f.read()
  f.close()
  stat_list = re.split(' ', stat)
  return int(stat_list[22])

# start the process to be monitored
mem_leak_process = os.environ['MOOREROOT'] + '/tests/options/mem_leak_process.py'
p = subprocess.Popen(mem_leak_process, stdout=subprocess.PIPE )

data_log = []
# monitoring loop 
# while process is not finished
while p.poll() == None:
  line = p.stdout.readline()
  match = event_pat.match(line)
  # check output for event number 
  # and log current vmsize
  if match:
    event  = int(match.groupdict()['event'])
    vmsize = get_vm_size(p.pid)
    vmsize_MB = vmsize / (1024 * 1024)
    print 'pid', p.pid, '  event', event, '  vmsize', vmsize, 'b  =', vmsize_MB, 'MB' 
    data_log.append((event, vmsize_MB))  
  time.sleep(0.05)

result = None
# find the first record
# with an event number > 20000
# and compare it to the last 
for (event, vmsize) in data_log:
  if event > 20000:
    result = [ (event, vmsize), data_log[-1] ]
    break  

print '=== end of mem_leak test'
print '=== result:', result
print '==='  

if result[1][1] > result[0][1]:
  print '=== Warning!!! Memory leak!! ==='
else:    
  print '=== Ok, no leak found. ===' 

print '==='
print '=== writing logs'
print '  ', 'mem_leak.log', 'mem_leak.svg', '...'

f = file('mem_leak.log','w')
for (x,y) in data_log:
  f.write(str(x) + ';' + str(y) + '\n')
f.close()

f = file('mem_leak.svg','w')
f.write(plotter.render(data_log,'# events', 'vmsize in MB'))
f.close()


