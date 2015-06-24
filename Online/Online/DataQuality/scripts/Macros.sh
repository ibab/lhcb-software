#!/bin/bash
DQ_UTIL="`which python` ${DATAQUALITYROOT}/python/DataQualityScan/Utils.py";
alias dq_util='${DQ_UTIL}';
alias dq_reschedule='${DQ_UTIL} -r -d ';
alias dq_archive='${DQ_UTIL} -a -d';
alias dq_dump='${DQ_UTIL} -d';
