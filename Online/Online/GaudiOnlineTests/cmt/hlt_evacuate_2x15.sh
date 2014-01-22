. $GAUDIONLINEROOT/tests/cmt/preamble.sh
python -c "from hlt_evacuate import *;submit(2,15);"
sleep 1
export UTGID=Prod_0
# start_mbmdump;
# start_meps;
/usr/bin/tail -n 3 ./hlt_evacuate.sh
