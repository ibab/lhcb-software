

if ${?SOURCESHELLALIASLIST} then
  foreach a ( $SOURCESHELLALIASLIST)
    if ( -e $a.csh ) then
      set b=$a:t
      alias $b "source `which $b.csh`"
    endif
  end
  unsetenv SOURCESHELLALIASLIST
  unset a
endif

