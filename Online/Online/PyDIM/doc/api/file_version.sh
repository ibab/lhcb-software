rev=`svn info $1 | grep "Changed Rev" | awk '{print $4}'`
date=`svn info $1 | grep "Changed Date" | awk '{print $4}'`
echo "Revision $rev, commited at $date" 
