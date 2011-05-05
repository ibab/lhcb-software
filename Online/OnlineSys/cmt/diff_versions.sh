#!/bin/bash
diff -r --brief ONLINE_HEAD ONLINE_v4r55 | grep -v ".svn" | grep -v "setup." | grep -v "cleanup." | grep -v ".history" | grep -v "x86_64-slc5-gcc43"
