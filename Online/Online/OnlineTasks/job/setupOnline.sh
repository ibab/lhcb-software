#!/bin/bash
#  Generic script to setup the environment for an Online Gaudi/Gaucho task run via the TM
# no arguments: only set HLTOPTs and gaudi executables
# $1: node above this DU in FSM; for jobs running on controls pc prefixed by NOTAN 
# $2: partition name
# $3: runtype; for tae events prefixed with TAE_
# $4: acceptrate in percent

export ARCH=`uname -i`;
echo "Architecture "${ARCH}

if test -n "$1" ;
  then 
    export PARENT=$1 
    # TAN - do not touche; need to protect this as it doesn't work on controls pcs
    if [[ ${PARENT:0:5} == "NOTAN" ]]
      then export PARENT=${PARENT:5}
      else export TAN_PORT=YES;export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`;export TAN_NODE=${DATAINTERFACE}
    fi
fi
  
if test -n "$2" ; 
  then export PARTNAME=$2;export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts;export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${PARENT}_HLT.opts
fi
  
if test -n "$3" ; 
  then export RUNTYPE=$3
fi  

#if test -n "$4" ; 
#  then export PERCENTPASS="Presc.PercentPass                = "$4";" ; export PRESCALEROPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}_Prescaler.opts 
#fi

if  [[ ${RUNTYPE:0:4} == "TAE_" ]]
  then echo "TAE run";export IS_TAE_RUN="_TAE";export RUNTYPE=${RUNTYPE:4}
  else echo "Normal run";export IS_TAE_RUN=""
fi   


if [[ ${ARCH} == "x86_64" ]]
  then 
     if test -z "${DEBUGGING}";
      then echo running normal sw;export CMTCONFIG=slc4_amd64_gcc34
      else echo running debug sw;export CMTCONFIG=slc4_amd64_gcc34_dbg   
     fi        
  else
    if test -z "${DEBUGGING}";
      then echo running normal sw;export CMTCONFIG=slc4_ia32_gcc34
      else echo running debug sw;export CMTCONFIG=slc4_ia32_gcc34_dbg   
    fi
fi
# now in INFOOPTIONS
#if test -z "${PRESCALEROPTIONS}";
#   then echo Using default acceptrate.
#   else 
#      if test -f ${PRESCALEROPTIONS};
#         then echo "removing old file "${PRESCALEROPTIONS}; rm ${PRESCALEROPTIONS}
#      fi  
#      echo ${PERCENTPASS} >  ${PRESCALEROPTIONS}; more ${PRESCALEROPTIONS}
#fi


# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done
export HOME=/home/$(/usr/bin/whoami)

echo Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE and home $HOME
 
export MYSITEROOT=/sw/lib
export LHCBHOME=/sw/lib
. $MYSITEROOT/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH

#. ../cmt/setup.sh
if [[ ${CMTCONFIG} == "slc4_ia32_gcc34" ]]
  then . ./pathsetup
  else 
     if  [[ ${CMTCONFIG} == "slc4_ia32_gcc34_dbg" ]]
       then . ./pathsetup_dbg
     fi
fi
if [[ ${CMTCONFIG} == "slc4_amd64_gcc34" ]]
  then . ./pathsetup64
  else 
     if  [[ ${CMTCONFIG} == "slc4_amd64_gcc34_dbg" ]]
       then . ./pathsetup64_dbg
     fi
fi

export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc"  
export HLTOPTS=${ONLINETASKSROOT}/hltopts
export CLASS1_TASK="${gaudi_exe} -tasktype=LHCb::Class1Task -main=${HLTOPTS}/Main.opts"
export CLASS0_TASK="${gaudi_exe} -tasktype=LHCb::Class0Task -opt=${GAUDIONLINEROOT}/options/Daemon.opts"

export gaudi_exe3="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline"  

