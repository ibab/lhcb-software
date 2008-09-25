#!/usr/bin/env python

import sys
import os

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
        content = """ @echo off
        
set LbLogin_tmpfile="%TEMP%\LbLogin_tmpsetup.bat"

python %~d0\%~p0\LbLogin.py --shell=bat --output=%LbLogin_tmpfile% %1 %2 %3 %4 %5 %6 %7 %8 %9

set LbLoginStatus=%ERRORLEVEL%

if %LbLoginStatus% EQU 0 (
    call %LbLogin_tmpfile%
)

if exist %LbLogin_tmpfile% del %LbLogin_tmpfile%
set LbLogin_tmpfile=
call %~d0\%~p0\SetupProject.bat --disable-CASTOR LbScripts """ + version + " --runtime LCGCMT Python -v 2.5"
    
    f.write(content)
    f.close()

if __name__ == '__main__':
    if len(sys.argv) < 4 :
        sys.exit("Error must have at least 3 arguments")
    else :
        targetlocation = sys.argv[1]
        version = sys.argv[2]
        scripts = sys.argv[3:]
        

    for s in scripts :
        generateLogin(targetlocation, s, version)
