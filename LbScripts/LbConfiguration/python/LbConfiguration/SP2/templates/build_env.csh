set nightly_slot = ${slot}
set nightly_day = ${day}

setenv CMTPROJECTPATH "${search_path_env}"

if ( "$${nightly_slot}" != "" ) then
    setenv CMTPROJECTPATH "$${LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}:$${CMTPROJECTPATH}"
endif
