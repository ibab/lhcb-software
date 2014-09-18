import sys

fname = sys.argv[1]
input = open(fname,"r")

ignore = ['stat',
          'read',
          'write',
          'lseek',
          'poll',
          'ioctl',
          'fcntl',
          'getdents',
          'getpid',
          'lstat',
          'fstat',
          'sendto',
          'recv(',
          'recvfrom(',
          'mprotect(',
          'getrusage(',
          'gettid(',
          'tgkill(',
          'tkill(',
          'clone(',
          'fork()',
          'wait',
          'pread',
          'nanosleep',
          'socket',
          'setsockopt(',
          'listen(',
          'bind(',
          'connect(',
          'times(',
          'futex(',
          'rt_sigaction(',
          'rt_sigpending(',
          'rt_sigprocmask(',
          'uname(',
          'arch_prctl(',
          'mtcp',
          'brk(']

filter = [' = -1 ENOENT']
select = ['open(','close(','mmap(','munmap(']

line = input.readline()
lib_name = ''
lib_full_name = ''
size = 0
root_size = 0
gaudi_size = 0
heap_size = 0
other_lib_size = 0

other = {}
heap = []

while len(line):
  line = line[:-1]
  use = True
  for i in ignore:
    if line.find(i) == 0:
      use = False
      break;
  for f in filter:
    if line.find(f) >= 0:
      use = False
      break;
  use = False
  for s in select:
    if line.find(s) == 0:
      use = True
      break

  if use:
    items = line[line.find('(')+1:line.find(')')-1].replace(' ','').split(',')
    if line.find('open(')==0:
      lib_name = items[0][1:-1].replace('open("','')
      lib_full_name = lib_name
      lib_name = lib_name[lib_name.rfind('/')+1:]
      if lib_name.find(".so")<=0:
          lib_name = ''
    elif line.find('close(')==0:
      lib_name = ''
      lib_full_name = ''
      pass
    elif line.find('mmap(')==0:
      lib_addr = line[line.find(' = ')+3:]
      lib_addr = int(lib_addr,16)
      lib_size = int(items[1])
      if len(lib_name) and lib_size>4096: 
          size = size + lib_size
      if len(lib_name) == 0:
          heap_size = heap_size + lib_size
      if lib_full_name.find('/ROOT/')>=0: 
          root_size = root_size + lib_size
      elif lib_full_name.find('/InstallArea/')>=0: 
          gaudi_size = gaudi_size + lib_size
      elif len(lib_name) and lib_size>4096:
          other_lib_size = other_lib_size + lib_size
          if other.has_key(lib_name):
              other[lib_name][0] = other[lib_name][0] + lib_size
          else:
              other[lib_name] = [lib_size]
      print 'MMAP:  %-64s %10d %016X'%('"'+lib_name+'"', lib_size, lib_addr,)
    else:
      print line
    pass
  line = input.readline()

print 100*'='
print '| Total Memory    size: %6d MByte'%(size/1024/1024,)
print '| HEAP/STACK      size: %6d MByte'%(heap_size/1024/1024,)
print '| ROOT  Libraries size: %6d MByte'%(root_size/1024/1024,)
print '| GAUDI Libraries size: %6d MByte'%(gaudi_size/1024/1024,)
print '| OTHER Libraries size: %6d MByte'%(other_lib_size/1024/1024,)
print 100*'='

for i in other.keys():
  print 'MMAP:  %-64s %10d'%(i, other[i][0],)
