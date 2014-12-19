# load build configuration (nightly slot)
if [ -e build.conf ] ; then
    . ./build.conf
fi

export CMTPROJECTPATH="${search_path_env}"

if [ -n "$${nightly_slot}" ]; then
    export CMTPROJECTPATH="$${LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}:$${CMTPROJECTPATH}"
fi
