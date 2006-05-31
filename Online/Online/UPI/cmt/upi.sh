#/bin/sh
export LINES=`stty size | awk '{ print $1 }'`
export COLUMNS=`stty size | awk '{ print $2 }'`
echo "Terminal setup is: $LINES lines and $COLUMNS columns "
stty -icanon -echo
$*
stty icanon echo
