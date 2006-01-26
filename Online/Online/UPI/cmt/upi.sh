#/usr/bin/sh
stty -icanon -echo
`stty size | awk '{ print "export LINES=" $1 " COLUMNS=" $2 }'`
echo "Terminal setup is: $LINES lines and $COLUMNS columns "
$*
stty icanon echo

