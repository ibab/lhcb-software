nightly_slot=${slot}
nightly_day=${day}

export CMTPROJECTPATH="${search_path_env}"

if [ -n "$${nightly_slot}" ]; then
    export CMTPROJECTPATH="$${LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}:$${CMTPROJECTPATH}"
fi
