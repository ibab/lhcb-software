


if ${?SOURCESHELLALIASLIST} then
  foreach a ( $SOURCESHELLALIASLIST)
    if ( -e $a.csh ) then
      alias $a:t "source $a.csh"
    endif
  end
  unsetenv SOURCESHELLALIASLIST
  unset a
endif
