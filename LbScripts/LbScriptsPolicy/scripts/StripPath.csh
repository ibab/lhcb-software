

set pth=`python $LBSCRIPTSPOLICYROOT/scripts/PathStripper.py $PATH`
setenv PATH $pth

set pth=`python $LBSCRIPTSPOLICYROOT/scripts/PathStripper.py $LD_LIBRARY_PATH`
setenv LD_LIBRARY_PATH $pth

set pth=`python $LBSCRIPTSPOLICYROOT/scripts/PathStripper.py $PYTHONPATH`
setenv PYTHONPATH $pth

set pth=`python $LBSCRIPTSPOLICYROOT/scripts/PathStripper.py $JOBOPTSEARCHPATH`
setenv JOBOPTSEARCHPATH $pth

set pth=`python $LBSCRIPTSPOLICYROOT/scripts/PathStripper.py $HPATH`
setenv HPATH $pth


unset pth
