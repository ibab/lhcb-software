if [[ -e ${HOME}/.useLHCBLoginscript ]]; then
   . /home/lhcb/lhcb/LBSCRIPTS/prod/InstallArea/scripts/group_shell.sh
   if [[ -o login || -n "$LB_BANNER" ]] ; then
      . /home/lhcb/lhcb/LBSCRIPTS/prod/InstallArea/scripts/group_login.sh
   fi
fi
