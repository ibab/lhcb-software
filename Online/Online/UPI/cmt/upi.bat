start /b xterm  -132 -im -sb -sl 500 -geometry 132x44 -fn littlefont -n %USERNAME%@%COMPUTERNAME% -title %USERNAME%@%COMPUTERNAME% -e /bin/bash upi.sh gentest.exe UPI %*
