#!/bin/bash
#
# Simple script to update the LbUtils copy of the script env.py in Gaudi.
#

git_root=/afs/cern.ch/sw/Gaudi/git/Gaudi.git

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

# Get the id of the latest commit in the repository.
if [ -n "$1" ] ; then
    remote_id=$(cd $git_root ; git log -1 --format=%H "$1")
else
    remote_id=$(cd $git_root ; git log -1 --format=%H master)
fi

# Check if we do have a commit hash of a local copy.
if [ ! -r $rootdir/env_script.version ] ; then
    echo "Importing the files from commit ${remote_id}"
    # clean checkout
    git archive --remote=$git_root ${remote_id} cmake/env.py | \
	tar -x -v -C $rootdir/../scripts --strip-components=1 -f -
    git archive --remote=$git_root ${remote_id} cmake/EnvConfig | \
	tar -x -v -C $rootdir/../python --strip-components=1 -f -
    # create release notes
    (
	cd /afs/cern.ch/sw/Gaudi/git/Gaudi.git
	git log ${remote_id} -- cmake/env.py cmake/EnvConfig
    ) > $rootdir/../doc/env_script.notes
    # Remember the version of the latest update
    echo ${remote_id} > $rootdir/env_script.version
else
    local_id=$(cat $rootdir/env_script.version)
    if [ "${remote_id}" == "${local_id}" ] ; then
	echo "Already at the latest version."
    else
	echo "Applying differences for ${local_id}..${remote_id}"
        # apply patches
	(cd $git_root ; git diff ${local_id}..${remote_id} -- cmake/env.py ) | \
	    (cd $rootdir/../scripts ; patch -p2)
	(cd $git_root ; git diff ${local_id}..${remote_id} -- cmake/EnvConfig ) | \
	    (cd $rootdir/../python ; patch -p2)
	# update release notes
	mv $rootdir/../doc/env_script.notes $rootdir/../doc/env_script.notes.tmp
	(
	    cd /afs/cern.ch/sw/Gaudi/git/Gaudi.git
	    git log ${local_id}..${remote_id} -- cmake/env.py cmake/EnvConfig
	) > $rootdir/../doc/env_script.notes
	cat $rootdir/../doc/env_script.notes.tmp >> $rootdir/../doc/env_script.notes
	rm $rootdir/../doc/env_script.notes.tmp
    # Remember the version of the latest update
    echo ${remote_id} > $rootdir/env_script.version
    fi
fi

