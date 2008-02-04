


if ${?SOURCESHELLALIASLIST} then
  foreach a ( $SOURCESHELLALIASLIST)
    if ( -e $SOURCESHELLDIR/$a.csh ) then
      alias $a "source $SOURCESHELLDIR/$a.csh"
    endif
  end
  unsetenv SOURCESHELLALIASLIST
  unset a
endif
