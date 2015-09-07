#!/bin/bash
#
export PYTHONPATH=${ONLINE_ENV_DIR}:${PYTHONPATH};
#
#cmd=`python <<EOF
#import OnlineEnvBase as Online
#print 'export EVENTSELECTOR_REQ1="EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=%f"'%(Online.AcceptRate*100.,);
#EOF`
#
#
act=${RUN_TYPE}
reqFile="../options/"`echo $act|tr "\|" "-"`".prescaler-requirements"
export PRESCALER_REQUIREMENTS=../options/Default.prescaler-requirements
if [ -f ${reqFile} ];
then
   export PRESCALER_REQUIREMENTS=${reqFile};
fi
export EVENTSELECTOR_REQ1="EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0";
echo [DEBUG] $cmd;
eval $cmd;
export TAN_PORT=YES;
export TAN_NODE=${DATAINTERFACE};
echo [DEBUG] SUBFARM_OPTIONS: ${SUBFARM_OPTIONS};
echo [DEBUG] Eventselector request: ${EVENTSELECTOR_REQ1};
echo [DEBUG] Prescaler Requirement File: ${reqFile};
exec -a ${UTGID} ${Class2_task} -opts=../options/HLT1Sender.opts
