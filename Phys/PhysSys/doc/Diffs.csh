#!/bin/tcsh

if ( $# != 1 ) then
 echo "Diffs to which version?"
 exit 127
endif

cd $PHYSSYSROOT/cmt
set version = $1

set tmpfile = diffs.tmp
if ( -e $tmpfile ) then
  rm $tmpfile
endif  
cvs diff -r $version requirements | grep use | grep -v "#" > $tmpfile

set newpacks = `grep ">" $tmpfile | awk '{print $3}'`

echo "####"
echo $newpacks
echo "####"
foreach p ( $newpacks )
  set oldv = `grep $p $tmpfile | grep "<" | awk '{print $4}'`
  set newv = `grep $p $tmpfile | grep ">" | awk '{print $4}'`
  set hat = ""
  if ( $oldv == "" ) then
    echo New package $p
     set hat = `grep $p $tmpfile | grep ">" | awk '{print $5}'`
  else if ( $newv == "" ) then
    echo Removed package $p
  else if ( $newv != $oldv ) then
    echo Package $p incremented from $oldv to $newv
     set hat = `grep $p $tmpfile | grep ">" | awk '{print $5}'`
  endif
  if ( $hat != "" ) then
    nedit $PHYSSYSROOT/../../$hat/$p/$newv/doc/release.notes
  endif
end
