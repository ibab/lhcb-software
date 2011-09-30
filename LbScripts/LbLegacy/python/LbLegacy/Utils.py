""" utilities to convert old behaviour into new python style """


from subprocess import Popen, PIPE, STDOUT

def getStatusOutput(cmdline):
    p = Popen(cmdline, shell=True, stdout=PIPE, stderr=STDOUT, close_fds=True)
    output = p.communicate()[0].rstrip("\n")
    status = p.returncode
    return status, output

def createStdMD5File(filename):
    
    
    pass

