#!/usr/bin/env python

import sys
import os

from imp import find_module
# bootstraping the location of the file
_this_file = find_module("generateLogin")[1]
_scripts_dir = os.path.dirname(_this_file)
_base_dir = os.path.dirname(_scripts_dir)


def generateLogins(version, targetlocation=None, scripts=None):
    if not targetlocation : 
        targetlocation = os.path.realpath(_base_dir)
    if not scripts :
        scripts = []
        scripts.append(os.path.join(_scripts_dir, "LbLogin.csh"))
        scripts.append(os.path.join(_scripts_dir, "LbLogin.sh"))
        scripts.append(os.path.join(_scripts_dir, "LbLogin.bat"))
    for s in scripts :
        generateLogin(targetlocation, s, version)

def generateLogin(targetlocation, script, version=""):
    knownshells = ["sh", "csh", "tcsh", "zsh", "bat"]
    shell = os.path.splitext(script)[1][1:]
    if not shell in knownshells :
        sys.exit("Unknown %s shell" % shell)
    f = open(script, "w")
    if shell == "sh" or shell == "zsh" :
        content = """LbLogin_tmpfile=`%s/scripts/LbLogin.py --shell=sh --mktemp "$@"`
LbLoginStatus="$?"
if [ "$LbLoginStatus" = 0 ]; then
    . $LbLogin_tmpfile
fi
rm -f $LbLogin_tmpfile
unset LbLogin_tmpfile
. %s/scripts/SetupProject.sh --disable-CASTOR LbScripts %s --runtime LCGCMT Python -v 2.5

""" % (targetlocation, targetlocation, version)
    elif shell == "csh" or shell == "tcsh" :
        content = """set LbLogin_tmpfile = `%s/scripts/LbLogin.py --shell=csh --mktemp ${*:q}`
set LbLoginStatus = $?
if ( ! $LbLoginStatus ) then
    source $LbLogin_tmpfile
endif
rm -f $LbLogin_tmpfile
unset LbLogin_tmpfile
source %s/scripts/SetupProject.csh --disable-CASTOR LbScripts %s --runtime LCGCMT Python -v 2.5

""" % (targetlocation, targetlocation, version)
    elif shell == "bat" :
        if sys.platform != "win32" :
            if targetlocation.startswith("/afs") :
                wintargetlocation = "Z:" + targetlocation.replace("/afs","").replace("/","\\")
        content = """ @echo off
        
set LbLogin_tmpfile="%%TEMP%%\LbLogin_tmpsetup.bat"

python %s\LbLogin.py --shell=bat --output=%%LbLogin_tmpfile%% %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9

set LbLoginStatus=%%ERRORLEVEL%%

if %%LbLoginStatus%% EQU 0 (
    call %%LbLogin_tmpfile%%
)

if exist %%LbLogin_tmpfile%% del %%LbLogin_tmpfile%%
set LbLogin_tmpfile=
call %s\SetupProject.bat --disable-CASTOR LbScripts %s --runtime LCGCMT Python -v 2.5""" % (wintargetlocation, wintargetlocation, version)
    
    f.write(content)
    f.close()

if __name__ == '__main__':
    targetlocation = None
    scripts = None
    
    if len(sys.argv) < 2 :
        sys.exit("Error must have at least 1 argument (the version)")
    if len(sys.argv) > 1 :
        version = sys.argv[1]
    if len(sys.argv) > 2 :
        targetlocation = os.path.realpath(sys.argv[2])
    if len(sys.argv) > 3 :
        scripts = sys.argv[3:]
        

    generateLogins(version, targetlocation, scripts)
