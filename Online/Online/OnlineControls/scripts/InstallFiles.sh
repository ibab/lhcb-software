#/bin/sh
#
#
if test -z "$1";
    then
    echo "No Project name given!";
    echo "usage: Install.sh <project-name> <install_name>"
    echo "  project-name: Choose one of: STORAGE MONITORING LBECS"
    echo "  install-name: Choose one of: JOBOPTIONS"
    exit 0;
fi;
export PVSS_PROJECT_NAME=$1
shift 1;
if test -n "$1";
    then
    export PVSS_INSTALL_NAME=$1
    echo "PVSS Install name: ${PVSS_INSTALL_NAME}"
    shift 1;
fi;

. ../cmt/setup.sh
#
export PVSS_PROJECT_BASE=/localdisk/pvss/${PVSS_PROJECT_NAME}
export PVSS_II=${PVSS_PROJECT_BASE}/config/config
echo "Copy files from ${ONLINECONTROLSROOT}/pvss to ${PVSS_PROJECT_BASE} ..."
cp --recursive --force --symbolic-link ${ONLINECONTROLSROOT}/pvss/* ${PVSS_PROJECT_BASE}
