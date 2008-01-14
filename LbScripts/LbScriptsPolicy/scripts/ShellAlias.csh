

if ${?SHELLALIASLIST} then
  foreach a ( $SHELLALIASLIST)
    alias $a $a.csh
  end
  unsetenv SHELLALIASLIST
  unset a
endif

