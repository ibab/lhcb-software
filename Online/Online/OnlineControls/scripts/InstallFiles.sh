#/bin/sh
#
#
export PVSS_PROJECT_NAME=
export PVSS_INSTALL_NAME=
export PVSS_COMPONENTS_DIR=
#
# Parse arguments
#
vals=`python <<EE_EOF
args = '$*'.split(' ')
i=0
while(i<len(args)):
  a = str(args[i]).upper()[:2]
  if a=='-P':
    print 'export PVSS_PROJECT_NAME='+args[i+1]+'; shift 1; shift 1;'
    i = i + 2
  elif a=='-I':
    print 'export PVSS_INSTALL_NAME='+args[i+1]+'; shift 1; shift 1;'
    i = i + 2
  elif a=='-C':
    print 'export PVSS_COMPONENTS_DIR='+args[i+1]+'; shift 1; shift 1'
    i = i + 2
  else:
    print 'echo "Unknown option:'+args[i]+'";'
    print 'exit 1;'
    break
EE_EOF
`
echo $vals
eval $vals
#
if test -z "$PVSS_PROJECT_NAME";
    then
    echo "No Project name given!";
    echo "usage: Install.sh <project-name> <install_name>"
    echo "  -project <name>: Choose one of: STORAGE MONITORING LBECS"
    echo "  -install <name>: Choose one of: JOBOPTIONS"
    exit 0;
fi;
export PVSS_PROJECT_BASE=/localdisk/pvss/${PVSS_PROJECT_NAME}
export PVSS_II=${PVSS_PROJECT_BASE}/config/config
#
if test -n "$PVSS_INSTALL_NAME";
    then
    echo "PVSS Install name: ${PVSS_INSTALL_NAME}"
fi;
if test -n "$PVSS_COMPONENTS_DIR";
    then
    echo "Install files to directory: ${PVSS_COMPONENTS_DIR}"
else
    export PVSS_COMPONENTS_DIR=${PVSS_PROJECT_BASE}
fi;

. ../cmt/setup.sh
#
echo "Copy files from ${ONLINECONTROLSROOT}/pvss to ${PVSS_COMPONENTS_DIR} ..."
#
cp --recursive --force --symbolic-link ${ONLINECONTROLSROOT}/pvss/* ${PVSS_COMPONENTS_DIR}
