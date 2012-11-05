#!/bin/bash
#
# Simple script to update the LbUtils copy of the script env.py in Gaudi.
#

git_root=/afs/cern.ch/sw/Gaudi/git/Gaudi.git
git_root=$WORK/workspace/Gaudi  

# Check if we have all the commands we need.
for c in git patch ; do
    if which $c >/dev/null 2>&1 ; then
	# good
	true
    else
	echo "Cannot find required command '$c'."
	exit 1
    fi
done

# Chech if we can access the Gaudi GIT repository from AFS.
if [ ! -d ${git_root} ] ; then
    echo "This script must be run from a machine with access to AFS."
    exit 1
fi

# Find ourselves (for the destination location)
rootdir=$(dirname $0)

# Branch to use.
if [ -n "$1" ] ; then
    remote_id=$1
else
    remote_id=master
fi

notes_file=$rootdir/../doc/gaudi_cmake.notes

echo "Clean destination directory"
# we do not remove '.svn' directories (or directories either)
find $rootdir/../cmake -noleaf -not -wholename "*.svn" -type f -exec rm -f \{} \;

echo "Importing the files from ${remote_id}"
git archive --remote=$git_root ${remote_id} cmake | \
    tar -x -v -C $rootdir/.. -f -

echo "Creating dummy __init__.py"
touch $rootdir/../cmake/cmt2cmake/__init__.py

# create release notes
(
    cd $git_root
    git log --date=short --pretty=format:'! %ad - %an (%h)%n%n - %s%n%n%w(80,3,3)%b%n' ${remote_id} -- cmake
) > $notes_file
