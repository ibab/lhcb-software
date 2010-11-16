#!/bin/bash

python ./RunDatabase/XMLRPC_Dummy_Server.py&
. ./setup.sh
. ./testWriter.sh
. ./injectData.sh /group/online/jc/00004834_00000007_1.mdf
