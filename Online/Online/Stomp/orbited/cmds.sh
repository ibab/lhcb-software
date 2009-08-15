#!/bin/sh
export ORB_home=${HOME}/cmtuser/orbited
export PYTHONPATH=${ORB_home}
export PYTHONPATH=${ORB_home}/simplejson-2.0.9:${PYTHONPATH}
export PYTHONPATH=${ORB_home}/morbid-0.8.7.1-py2.5.egg:${PYTHONPATH}
export PYTHONPATH=${ORB_home}/stomper-0.2.2-py2.5.egg:${PYTHONPATH}
###export PYTHONPATH=${ORB_home}/orbited-0.7.10/orbited:${PYTHONPATH}
export PYTHONPATH=${ORB_home}/orbited-0.7.10-py2.5.egg:${PYTHONPATH}
echo cd orbited-0.7.10
echo python orbited/start.py --config=orbited/orbited-debug.cfg
