


if ${?SOURCESHELLALIASLIST} then
  foreach a ( $SOURCESHELLALIASLIST)
    alias $a "source $SOURCESHELLDIR/$a.csh"
  end
  unsetenv SOURCESHELLALIASLIST
  unset a
endif
