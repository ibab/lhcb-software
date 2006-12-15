. $GAUDIONLINEROOT/tests/cmt/preamble.sh
python -c "from hlt_evacuate import *;submit(2,15);"

# ddd --command=./gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe &
sleep 1
export UTGID=Prod_0
# $gaudi_exe -opt=$OPTS/MDF2MBM.opts  -msgsvc=$msg_svc
# $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so mdf_producer -n=prod_0 -p=333 -s=500 -b=OUT_14d -f=./mdfOutput.dat
/usr/bin/tail -n 3 ./hlt_evacuate.sh
