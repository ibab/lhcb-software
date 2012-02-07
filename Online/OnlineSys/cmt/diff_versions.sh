#!/bin/bash
diff -r --brief ONLINE_${1} ONLINE_${2} | grep -v ".svn" | grep -v "setup." | grep -v "cleanup." | grep -v ".history" | grep -v "x86_64-slc5-gcc43" | grep -v ".pyc" | grep -v ".ph"
