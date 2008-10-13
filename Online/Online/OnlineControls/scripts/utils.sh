installSubfarm()
{
    if test -z "${1}";
	then
	echo "No project name given.";
	elif test -z "${2}";
	then
	echo "No subfarm number given.";
	else
	echo Executing: ssh -X ${1} bash ${ONLINECONTROLSROOT}/scripts/InstallSubfarm.sh REC${1} ${2}
	ssh -q -f -Y ${1} "bash ${ONLINECONTROLSROOT}/scripts/InstallSubfarm.sh REC${1} ${2}" &
	fi;
}
