

foreach a ( $SOURCESHELLALIASLIST)
alias $a "source $LbScripts_HOME/InstallArea/scripts/$a.csh"
end


unsetenv SOURCESHELLALIASLIST
