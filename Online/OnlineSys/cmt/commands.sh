tag() { 
    echo " ====  Tagging ONLINE package Online/$1 with tag $2  ==== ";
    cd ../../Online/$1; 
    svn commit -m "New tag for package $1 -- $2"; 
    tag_package Online/$1 $2; 
    echo $2> cmt/version.cmt; 
    cd ../../OnlineSys/cmt;
}

tag_project()
{
    cd ../../cmt;
    echo " ====  Tagging ONLINE project with tag $2  ==== ";
    svn commit -m "New project release Online $1";
    svn mkdir -m "New release Online $1" svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/ONLINE/ONLINE_$1;
    svn cp -m "New release Online $1"  svn+ssh://svn.cern.ch/reps/lhcb/Online/trunk/cmt svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/ONLINE/ONLINE_$1/;
    cd ../OnlineSys/cmt;
}

alias package_diffs='svn diff cmt/requirements |grep "+use "|cut -b 6-'
