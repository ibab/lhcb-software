############################ ProjectEnv.sh #############################
#---------------------------------------------------------------------------
# set project[s] environment
# for a shell with the Bourne syntax (Bourne shell, Korn shell, BASH, Z shell)
# Usage: . ProjectEnv.sh <project> [v#r#] [+ <plus_project>]
#---------------------------------------------------------------------------
# Checks whether the <project> version entered (if any) is available
# and, if needed, offers the choice of the versions available
# (with the default being the most recent version)
# With + <plus_project>, offers the choice of the <plus_project> versions
# compatible with the <project> version set
#---------------------------------------------------------------------------
# Sept 14 2004
# A. Buckley <Andrew.Buckley@cern.ch> (re-write ~2004)
# F. Ranjard <Florence.Ranjard@cern.ch>
# G. Rybkine <Grigori.Rybkine@cern.ch>
#---------------------------------------------------------------------------
############################################################################


# ---------------------------------------------------------------------------

# Urgh! This is a rather painful work-around for the fact that this script is sourced rather
# than executed in a subshell: user aliases can affect the script and the which command works
# differently from different shells (don't ask me why, it just does!)
function safewhich {
    absprog=$( which -a $1 2> /dev/null | sed -e s/"^[^\/]*"//g | egrep "^\/" | sed -e s/"\ .*"//g -e 1q );
    echo $absprog | sed -e s/"\ .*"//
}

# Util program shorthand names
LS=$( safewhich ls )
GREP=$( safewhich grep )
TR=$( safewhich tr )
SED=$( safewhich sed )
SORT=$( safewhich sort )
FIND=$( safewhich find )
MKTEMP=$( safewhich mktemp )
EXPAND=$( safewhich expand )
CAT=$( safewhich cat )
RM=$( safewhich rm )
WC=$( safewhich wc )
CUT=$( safewhich cut )
EGREP=$( safewhich egrep )


# Decide on the project version, with user input if necessary
function chooseProjectVersion {
    # Set the environment for the current project from the function's positional params list
    echo "Setting $PROJECTname environment"
    # If a CLI version was specified and is valid, use it
    if [[ -z $PROJECTVERSION || -n $( $LS "$releasearea/${PROJECTNAME}_${PROJECTVERSION}" 2>&1 1> /dev/null ) || -z $( echo $* | $GREP $PROJECTVERSION 2> /dev/null ) ]]; then
        if [[ -n $PROJECTVERSION && -z $( echo $* | $GREP $PROJECTVERSION ) ]]; then
            echo "$PROJECTVERSION is not a valid/compatible choice:"
        fi;

        if [[ $# -eq 0 ]]; then
            echo "No valid projects are available"
        elif [[ $# -eq 1 ]]; then
            echo "Only $PROJECTname $1 is available: picking it automatically"
            PROJECTVERSION=$1;
        else
            # Write the selection list
            echo -n "Please enter your choice ("
            lastversion="";
            while [[ $# -gt 0 ]]; do
                echo -n "$1 "
                lastversion=$1;
                shift;
            done
            echo -n "q[uit] [$lastversion]): "
            
            # Read in the choice, handling "quits" and "defaults"
            read CHOSENVERSION;
            case $CHOSENVERSION in
                q|qu|qui|quit) CHOSENVERSION="" ;;
                "") CHOSENVERSION=$lastversion ;;
            esac
            unset lastversion;
            PROJECTVERSION=$CHOSENVERSION;
            unset CHOSENVERSION;
        fi;
    fi;
}


if [[ -z $Gaudi_release_area ]]; then
    export Gaudi_release_area=$GAUDISOFT
fi;


# Function to break up the project handling a bit
function setupProject {
    STARTDIR=`pwd`;
    let projectnumber++;
    PROJECTname=$1;
    PROJECTNAME=$( echo $PROJECTname | $TR '[a-z]' '[A-Z]' );
    PROJECTVERSION=$2;
    shift $#


    # Set up bits of environment and convenience variables for release areas
    releasearea="";

# Gaudi is now sensible and does what everything else does
# so there's no longer any need for this case statement.
# I'll keep it here just in case :-)
#
#    case $PROJECTname in
#        Gaudi) 
#            releasearea=$Gaudi_release_area 
#            envarea=$Gaudi_release_area 
#            releasearea="$envarea/GAUDI";
#            ;;
#        *)
            if eval [ -z "\$${PROJECTname}_release_area" ]; then
                eval "export ${PROJECTname}_release_area=$LHCBRELEASES" 
            fi;
            eval "envarea=\$${PROJECTname}_release_area"
            releasearea="$envarea/$PROJECTNAME";
#            ;;
#    esac;
    

    # Generate the versions string with strict version number sorting
    projectversions=$( \
        $FIND $releasearea -maxdepth 1 \( -type d -o -type l \) | \
        $GREP "${PROJECTNAME}_" | \
        $SED -e s/".*${PROJECTNAME}_\(v[0-9]\+r[0-9]\+.*\)"/"\1blah"/ \
        -e s/"v\([0-9]\)\([^0-9]\)"/"v0\1\2"/g \
        -e s/"r\([0-9][^0-9]\)"/"r0\1"/g \
        -e s/"p\([0-9][^0-9]\)"/"p0\1"/g \
        -e s/"^v"// -e s/"r"/"\."/ -e s/"p"/"\."/  | \
        $SORT -n | \
        $SED -e s/"^"/"v"/ -e s/"\."/r/ -e s/"\."/p/ \
        -e s/"\t"/"\ "/g -e s/"blah"//g \
        -e s/"v0"/"v"/g -e s/"r0"/"r"/g -e s/"p0"/"p"/g | \
        $TR '\012' '\ ' | tr '\011' '\ ' | $EXPAND
    )
    #echo ::$projectversions::


    if [[ $projectnumber -eq 1 ]]; then
        # If the first (main) project


        # Choose the project version, with a dialogue if reqd
        eval "chooseProjectVersion $projectversions"

        if [[ -n $PROJECTVERSION ]]; then
            # This throws up lots of errors!
            . "$envarea/${PROJECTname}Env/$PROJECTVERSION/cmt/setup.sh"
            echo " ---> $PROJECTname environment set for version $PROJECTVERSION <---"
            

            # Get the LHCb version used by the main project
            cd "$envarea/${PROJECTname}Env/$PROJECTVERSION/cmt"
            export lhcbversion=`$CMTEXEC show macro_value LHCBVERS`
            cd $STARTDIR


            export MAINPROJECTname=$PROJECTname;
            export MAINPROJECTNAME=$PROJECTNAME;
            export MAINPROJECTVERSION=$PROJECTVERSION;
        fi;
    else
        # Handle extra projects more carefully: need to make sure they're compatible
        # with the main project's LHCb version


        # Make a list of available compatible versions of the extra project
        TMPFILE=$( $MKTEMP /tmp/$$.XXXXXX );
        for plusversion in `echo $projectversions`; do # NB. it's done like this for zsh compatibility
            lserrs=$( $LS $releasearea/${PROJECTNAME}_$plusversion 1> /dev/null 2>&1 );
            if [[ -z $lserrs ]]; then
                # If the project version exists, check its CMT macros for the LHCb version
                plusprojectcmtdir="$envarea/${PROJECTname}Env/$plusversion/cmt"
                if [[ -d $plusprojectcmtdir ]]; then
                    cd $plusprojectcmtdir
                    pluslhcbversion=$( $CMTEXEC show macro_value LHCBVERS )
                    cd $STARTDIR
                    unset plusprojectcmtdir;

                    # Store the project version if compatible with main project
                    #echo $PROJECTname :: $plusversion $pluslhcbversion vs $lhcbversion
                    if [[ $pluslhcbversion == $lhcbversion ]]; then
                        echo -n "$plusversion " >> $TMPFILE
                    fi;
                    unset pluslhcbversion;
                fi;
            fi;
        done;
        unset projectversions;

        # Put the found compatible versions in the pos params
        plusprojectversions=$( $CAT $TMPFILE );
        $RM $TMPFILE
        unset TMPFILE;


        # Did we find any compatible versions?
        if [[ -z $plusprojectversions ]]; then
            echo " ---? no $PROJECTname versions compatible with $MAINPROJECTname version $MAINPROJECTVERSION ?---"
            PROJECTVERSION=""
        else
            # If there are compatible versions, choose from them if necessary
            eval "chooseProjectVersion $plusprojectversions"
        fi;
        unset plusprojectversions;


        # If there's a chosen version, dismantle and rebuild the CMT path appropriately 
        if [[ -n $PROJECTVERSION ]]; then
            cmtpathelements=$( echo $CMTPATH | $SED -e s/":"/"\ "/g );

            # Re-build the CMT path, inserting the plus projects' paths after the 
            # main project path and before the LHCb path
            pathinserted=0;
            for cmtpathelement in `echo $cmtpathelements`; do
                MYCMTPATH="$MYCMTPATH:$cmtpathelement"
                if [[ $pathinserted -eq 0 ]]; then
                    cmtmatch=$( echo $cmtpathelement | $GREP $MAINPROJECTNAME );
                    if [[ -n $cmtmatch ]]; then
                        MYCMTPATH="$MYCMTPATH:$releasearea/${PROJECTNAME}_$PROJECTVERSION"
                        echo " ---> $PROJECTname environment added for version $PROJECTVERSION <--- "
                        pathinserted=1;
                    fi;
                fi;
            done;

            export CMTPATH=$( echo $MYCMTPATH | $SED -e 's/"^:"//' )
        fi;
    fi;


    # Return to whence we came, just in case...
    cd $STARTDIR;

    unset STARTDIR;
    unset PROJECTname;
    unset PROJECTNAME;
    unset PROJECTVERSION;
}


# ---------------------------------------------------------------------------
# Okay, that's the helper functions (which actually do the work) defined.
# Let's do it for real!


# Fiddle the environment a bit
if [[ -z $LHCBHOME ]]; then
    echo "LHCBHOME parameter not set"
elif [[ -z $* ]]; then
    echo "No project specified"
else
    # Set up CMT
    if [[ -z "$CMTROOT" ]]; then . "$LHCBHOME/scripts/CMT.sh"; fi;
    export CMTEXEC="$CMTROOT/$CMTBIN/cmt.exe";
    MYCMTPATH="";


    # Parse command line arguments into a form suitable for extracting project details
    ARGS=$( echo $* | $EXPAND | $SED -e s/"[^a-zA-Z0-9+]\+"/"\ "/g -e s/"\ +"/"+"/g -e s/"+\ "/"+"/g )
    let NUMPROJS=$( echo $* | $SED -e s/"[^+]"//g -e s/"\ "//g | $WC -L | $EXPAND | $SED -e s/"\ "//g )+1;
    
    
    # Extract project details and check for validity
    validprojects="";
    while [[ -n $ARGS ]]; do
        # Extract the current project
        if [[ -n $( echo $ARGS | $GREP "+" ) ]]; then
            projectstring=$( echo $ARGS | $CUT -d+ -f1 )
        else
            projectstring=$ARGS;
        fi;
        ARGS=$( echo $ARGS | $SED -e s/"$projectstring"// -e s/"^+"// )
        projectstring=$( echo $projectstring | $SED -e s/"^[\ ]*"// -e s/"[\ ]*$"// );
        numwords=$( echo $projectstring | $WC -w | $EXPAND | $SED -e s/"\ "//g );
        
        if [[ $numwords -lt 3 ]]; then
            firstword=$( echo $projectstring | $CUT -d\  -f1 );
            FIRSTWORD=$( echo $firstword | $TR '[a-z]' '[A-Z]' );


            if [[ ($firstword != "Gaudi") && ($firstword != "Vetra") ]]; then
                myreleasearea=$LHCb_release_area;
                projecterror=$( $LS $myreleasearea/$FIRSTWORD 2>&1 1> /dev/null )
                if [[ -n $projecterror ]]; then
                    echo "Project '$firstword' not found in release area $myreleasearea";
                    stophere=1;
                else
                    validprojects="$validprojects $projectstring";
                fi;
                unset projecterror
            else
                validprojects="$validprojects $projectstring";
            fi;


            unset firstword;
        else
            echo "More than 3 words in $projectstring - aborting"
        fi;
        unset projectstring;
        unset numwords;
    done;
    unset NUMPROJS;
    unset ARGS;


    # Produce an error message for invalid input string or continue if input is valid
    if [[ -n $stophere ]]; then
        echo "Usage: . ProjectEnv.sh <project> [v#r#] [+ <plus_project> [v#r#]]"
        echo "Projects available: Bender Boole Brunel DaVinci Gaudi Gauss LHCb Panoramix SICBMC SICBOO"
        unset stophere;
    else
        # Call the functions for each project
        export projectnumber=0;
        validprojects=$( echo "$validprojects" | $SED -e s/"^\ "// )

        while [[ -n $validprojects ]]; do
            shift $#;
            eval "set $validprojects";

            pname=$1;
            shift;

            pver="";
            if [[ -n $( echo $1 | $EGREP "v[0-9]+r[0-9]+" ) ]]; then
                pver=$1;
                shift;
            fi;
            
            # Clear the pos params and do it
            validprojects="$*";
            shift $#;
            eval "setupProject $pname $pver"
        done;
        unset projectnumber;

        # Show the final CMTPATH
        echo " ------------------------------------------------------------------------"
        echo " ---> packages searched for in $CMTPATH <--- "
        echo " ------------------------------------------------------------------------"
    fi;

    # Finally remove the main project variables
    #unset VALIDPROJECTS;
    unset lhcbversion;
    unset CMTEXEC;
    unset MYCMTPATH;
    unset MAINPROJECTname;
    unset MAINPROJECTNAME;
    unset MAINPROJECTVERSION;
fi;

# "Hide" the helper functions
unset setupProject;
unset chooseProjectVersion;

# Clear the util "safe" shortcuts
unset LS; unset GREP; unset TR; unset SED; unset SORT
unset FIND; unset MKTEMP; unset EXPAND; unset CAT
unset RM; unset WC; unset CUT; unset EGREP

# A final clearance of the positional params, just in case
shift $#
