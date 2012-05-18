#! /bin/sh

# you need to run this script from a dir above the 'dirname' dir

destination=$1
tagname=`basename $destination`
text=""
TNsize=400
nx=2

cd ${destination}


  mkdir eps
  mv *.eps eps

  file="index.html"
  blue="#0000FF"
  red="#FF0000"
  black="#000000"
  title="${tagname} - ${dir}"

  mkdir gif
  mv *.gif gif
  mkdir small

  rm -rf ${file}

  echo "<HTML>" >> ${file}
  echo "<TITLE>${title}</TITLE>" >> ${file}
  echo " <BODY>" >> ${file}
  echo "  <CENTER><H1><font color=\"${blue}\">${title}</font></H1></CENTER>" >> ${file}
  echo "<p>" >> ${file}
  echo ${text} >> ${file}
  echo "</p>" >> ${file}

  echo "  <TABLE>" >> ${file}
  echo "   <TR>" >> ${file}

  echo "<a href=\"info.txt\"><H3><font color=\"${blue}\"> Information </font></H3></a>" >> ${file}

  echo "Creating thumbnails..."

  #########################
  i1=0
  j1=0
  echo "    </TR>" >> ${file}
  echo "  </TABLE>" >> ${file}
  echo "<H1><font color=\"${blue}\">  </font></H1>" >> ${file}
  echo "  <TABLE>" >> ${file}
  echo "   <TR>" >> ${file}

  for epsfile1 in `cd eps; ls *.eps` ; do 

    #echo "Converting eps to gif, ${epsfile1} ..."

      eps1=$epsfile1

      #creating scaled thumbnails !!!
      img1=`echo ${eps1} | sed 's/.eps/.gif/'`
      #convert eps/${eps1} gif/${img1}
      convert -scale ${TNsize} gif/${img1} small/TN_${img1}

      echo "    <td align=\"center\">" >> ${file}
      echo "      <a href=\"gif/${img1}\"> <img src=\"small/TN_${img1}\"> </a>" >> ${file}
      eps=`echo $img1 | sed 's/.gif/.eps/'`
      echo "      <br><a href=\"gif/${img1}\">${img1}</a>, <a href=\"eps/${eps1}\">[eps]</a>  "  >> ${file}
      echo "    </td>  "  >> ${file}
      i1=`expr $i1 + 1` 
      j1=`expr ${i1} % ${nx}`
      if [ $j1 -eq 0 ] ; then
         echo "   </TR><TR>" >> ${file}
      fi

  done   # loop over all files


  echo "    </TR>" >> ${file}
  echo "  </TABLE>" >> ${file}
  DATE=`date`
  echo "  <br><br>Created ${DATE} by (c) ${USER} using <a href=\"createHTMLpage.sh\">createHTMLpage.sh</a>"  >> ${file}
  echo " </BODY>" >> ${file}
  echo "</HTML>" >> ${file}

  cd ..


