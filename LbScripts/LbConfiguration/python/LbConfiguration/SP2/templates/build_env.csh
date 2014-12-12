# load build configuration (nightly slot)
if ( -e build.conf ) then
    eval `sed -n '/^[^#]/{s/^/set /;s/$$/ ;/p}' build.conf`
endif

setenv CMTPROJECTPATH "${search_path_env}"

if ( $${?nightly_slot} ) then
    if ( "$${nightly_slot}" != "" ) then
        setenv CMTPROJECTPATH "$${LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}:$${CMTPROJECTPATH}"
    endif
endif
