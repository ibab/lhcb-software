if (  -e ${HOME}/.useLHCBLoginscript ) then
   source /home/lhcb/lhcb/LBSCRIPTS/prod/InstallArea/scripts/group_shell.csh
   if ($?loginsh  || $?LB_BANNER) then
      source /home/lhcb/lhcb/LBSCRIPTS/prod/InstallArea/scripts/group_login.csh
   endif
endif
