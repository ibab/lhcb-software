""" Common tools for the LHCb configuration """

from LbConfiguration.Platform import supported_shells
from LbConfiguration.SetupProject import SetupProject

import logging
import os

def getTargetShell(filename):
    """ get the target shell according to the filename extension """
    shell = None
    ext = os.path.splitext(filename)[1]
    if ext :
        ext = ext[1:] 
        if ext in supported_shells :
            shell = ext

    return shell


def StripPath(path):
    collected = []
    for p in path.split(os.pathsep):
        rp = os.path.realpath(p)
        if os.path.exists(rp) and os.path.isdir(rp):
            if len(os.listdir(rp)) != 0:
                collected.append(p)
    return os.pathsep.join(collected)

def appendVariable(scriptfile, shell, varname, varcont):
    log = logging.getLogger()
    if varcont :
        log.debug("Appending %s to %s" % (varname, scriptfile))
        outf = open(scriptfile, "a")
        if shell == "sh" :
            outf.write("export %s=%s\n" % (varname, varcont) )
        elif shell == "csh" :
            outf.write("setenv %s %s\n" % (varname, varcont) )
        elif shell == "bat" :
            outf.write("set %s=%s\n" % (varname, varcont) )
        outf.close()
    else :
        log.warning("Content of %s is empty" % varname)

def getVariable(scriptfile, shell, varname):
    log = logging.getLogger()
    log.debug("Extracting %s from %s" % (varname, scriptfile))
    inf = open(scriptfile, "r")
    data = inf.readlines()
    inf.close()
    value = ""
    for l in data :
        words = l.split()
        if len(words) > 1 :
            if shell == "sh" :
                if words[0] == "export" and words[1].startswith("%s=" % varname) :
                    value = words[1].split("=")[1]
                    continue
            elif shell == "csh" and len(words)>2 :
                if words[0] == "setenv" and words[1] == varname :
                    value = words[2]
                    continue
            elif shell == "bat" :
                if words[0] == "set" and words[1].startswith("%s=" % varname) :
                    value = words[1].split("=")[1]
                    continue
    return value

def fixPath(scriptfile, shell):
    # clean up the variables
    varlist = ["PATH", "LD_LIBRARY_PATH", "PYTHONPATH",
               "JOBOPTSEARCHPATH", "HPATH", "MANPATH"]
    for v in varlist :
        varvalue = StripPath(getVariable(scriptfile, shell, v))
        appendVariable(scriptfile, shell, v, varvalue)


def cleanStripPath(scriptfile, shell):
    # remove the call to StripPath
    inf = open(scriptfile, "r")
    data = inf.readlines()
    inf.close()
    outf = open(scriptfile, "w")
    found = 0
    for l in data :
        words = l.split()
        if len(words)>0 and words[0] == "if" and l.find("StripPath") != -1 :
            found = 1
        else :
            if found :
                if found < 4 :
                    found += 1
                else :
                    found = 0
        if found :
            if shell == "bat" :
                outf.write("REM " + l )
            else :
                outf.write("# " + l)
        else :
            outf.write(l)
    outf.close()



def postProcessSetupScript(scriptfile, shell, strip_path=False, fix_path=True):
    log = logging.getLogger()
    log.debug("Post processing of %s" % scriptfile)
    if not strip_path :
        cleanStripPath(scriptfile, shell)
        if fix_path :
            fixPath(scriptfile, shell)

def genProjectSetup(output_file, project, version, cmtconfig, 
                    shell="csh", setup_project_extra_args=None,
                    strip_path=False, fix_path=True):
    """ generates the setup script for the CMT project """
    
    os.environ["CMTCONFIG"] = cmtconfig
    log = logging.getLogger()
    setuprojargs = []
    setuprojargs.append("--shell=%s" % shell)
    setuprojargs.append("--output=%s" % output_file)
    setuprojargs.append("--no-user-area")
    if setup_project_extra_args :
        setuprojargs += setup_project_extra_args
    setuprojargs.append(project)
    setuprojargs.append(version)
    log.debug("Running SetupProject %s" % " ".join(setuprojargs))
    SetupProject().main(setuprojargs)
    if os.path.exists(output_file) :
        postProcessSetupScript(output_file, shell, strip_path, fix_path)
    else :
        log.error("%s doesn't exist" % output_file)
