#!/bin/bash
export PATH=/opt/FMC/sbin:/opt/FMC/bin:${PATH};
export LD_LIBRARY_PATH=/opt/FMC/lib;
cd `dirname ${0}`
export PYTHONPATH=`pwd`
eval `/usr/bin/python -c "import dataflowBoot; dataflowBoot.${1}()"`
