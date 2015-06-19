#!/bin/bash
#
# Simple script to update the LbUtils copy of the script env.py in Gaudi.
#

git_url=https://gitlab.cern.ch/gaudi/Gaudi.git

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

# Find ourselves (for the destination location)
rootdir=$(dirname $(cd $(dirname $0); pwd))

# Branch to use.
if [ -n "$1" ] ; then
    remote_id=$1
else
    remote_id=master
fi

notes_file=$rootdir/doc/gaudi_cmake.notes

echo "Clean destination directory"
# we do not remove '.svn' directories (or directories either)
find $rootdir/cmake -noleaf -not -wholename "*.svn" -type f -exec rm -f \{} \;

echo "Importing the files from ${remote_id}"
git clone --mirror $git_url gaudi_tmp
(
    cd gaudi_tmp
    git archive ${remote_id} cmake | \
        tar -x -v -C $rootdir -f -

    git archive ${remote_id} Makefile-cmake.mk | \
        tar -x -v -C $rootdir/data -f -

    echo "Creating dummy __init__.py"
    touch $rootdir/cmake/cmt2cmake/__init__.py

    # create release notes
    git log --date=short --pretty=format:'! %ad - %an (%h)%n%n - %s%n%n%w(80,3,3)%b%n' ${remote_id} -- cmake Makefile-cmake.mk > $notes_file
)
rm -rf gaudi_tmp

# update package release notes
echo "Updating $rootdir/doc/release.notes"
mv $rootdir/doc/release.notes $rootdir/doc/release.notes.tmp
cat > $rootdir/doc/release.notes <<EOF
$(date -I)  Marco Clemencic  <Marco.Clemencic@cern.ch>

	* cmake: updated CMake support modules

EOF
cat $rootdir/doc/release.notes.tmp >> $rootdir/doc/release.notes
rm $rootdir/doc/release.notes.tmp
