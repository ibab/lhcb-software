#!/usr/bin/env python

import sys
import os
from optparse import OptionParser


from imp import find_module
# bootstraping the location of the file
_this_file = find_module("generateLogin")[1]
_scripts_dir = os.path.dirname(_this_file)
_base_dir = os.path.dirname(_scripts_dir)


def generateLogins(version, targetlocation=None, scripts=None, mysiteroot=None):
    if not targetlocation : 
        targetlocation = os.path.realpath(_base_dir)
    if not scripts :
        scripts = []
        scripts.append(os.path.join(_scripts_dir, "LbLogin.csh"))
        scripts.append(os.path.join(_scripts_dir, "LbLogin.sh"))
        scripts.append(os.path.join(_scripts_dir, "LbLogin.bat"))
    for s in scripts :
        generateLogin(targetlocation, s, version, mysiteroot)

def generateLogin(targetlocation, script, version="", mysiteroot=None):
    knownshells = ["sh", "csh", "tcsh", "zsh", "bat"]
    shell = os.path.splitext(script)[1][1:]
    if not shell in knownshells :
        sys.exit("Unknown %s shell" % shell)
    f = open(script, "w")
    if shell == "sh" or shell == "zsh" :
        if mysiteroot :
            content = """LbLogin_tmpfile=`%s/scripts/LbLogin.py --shell=sh --mktemp --mysiteroot=%s "$@"`
""" % (targetlocation, mysiteroot)
        else :
            content = """LbLogin_tmpfile=`%s/scripts/LbLogin.py --shell=sh --mktemp "$@"`
""" % targetlocation
        content += """LbLoginStatus="$?"
if [ "$LbLoginStatus" = 0 ]; then
    . $LbLogin_tmpfile
fi
rm -f $LbLogin_tmpfile
unset LbLogin_tmpfile
. %s/scripts/SetupProject.sh --disable-CASTOR LbScripts %s --runtime LCGCMT Python -v 2.5

""" % (targetlocation, version)
    elif shell == "csh" or shell == "tcsh" :
        if mysiteroot :
            content = """set LbLogin_tmpfile = `%s/scripts/LbLogin.py --shell=csh --mktemp --mysiteroot=%s ${*:q}`
""" % (targetlocation, mysiteroot)
        else :
            content = """set LbLogin_tmpfile = `%s/scripts/LbLogin.py --shell=csh --mktemp ${*:q}`
""" % targetlocation
        content += """set LbLoginStatus = $?
if ( ! $LbLoginStatus ) then
    source $LbLogin_tmpfile
endif
rm -f $LbLogin_tmpfile
unset LbLogin_tmpfile
source %s/scripts/SetupProject.csh --disable-CASTOR LbScripts %s --runtime LCGCMT Python -v 2.5

""" % (targetlocation, version)
    elif shell == "bat" :
        wintargetlocation = targetlocation
        winmysiteroot = mysiteroot
        if sys.platform != "win32" :
            if targetlocation.startswith("/afs") :
                wintargetlocation = "%AFSROOT%" + targetlocation.replace("/afs","").replace("/","\\")
                if mysiteroot :
                    winmysiteroot = "%AFSROOT%" + mysiteroot.replace("/afs","").replace("/","\\")
                else :
                    winmysiteroot = None
        if winmysiteroot :
            content = """ @echo off
        
set LbLogin_tmpfile="%%TEMP%%\LbLogin_tmpsetup.bat"

python %s\scripts\LbLogin.py --shell=bat --output=%%LbLogin_tmpfile%% --mysiteroot=%s %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9
""" (wintargetlocation, winmysiteroot)
        else :
            content = """ @echo off
        
set LbLogin_tmpfile="%%TEMP%%\LbLogin_tmpsetup.bat"

python %s\scripts\LbLogin.py --shell=bat --output=%%LbLogin_tmpfile%% %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9
""" % wintargetlocation
        content += """set LbLoginStatus=%%ERRORLEVEL%%

if %%LbLoginStatus%% EQU 0 (
    call %%LbLogin_tmpfile%%
)

if exist %%LbLogin_tmpfile%% del %%LbLogin_tmpfile%%
set LbLogin_tmpfile=
call %s\scripts\SetupProject.bat --disable-CASTOR LbScripts %s --runtime LCGCMT Python -v 2.5""" % (wintargetlocation, version)
    
    f.write(content)
    f.close()

if __name__ == '__main__':
    targetlocation = None
    scripts = None
    parser = OptionParser()
    parser.set_defaults(mysiteroot=None)
    parser.add_option("-m", "--mysiteroot", dest="mysiteroot",
                  help="defines the main site location")

    opts, args = parser.parse_args()
    
    if len(args) < 2 :
        sys.exit("Error must have at least 1 argument (the version)")
    if len(args) > 1 :
        version = args[1]
    if len(args) > 2 :
        targetlocation = os.path.realpath(args[2])
    if len(args) > 3 :
        scripts = args[3:]
        
    generateLogins(version, targetlocation, scripts, opts.mysiteroot)


