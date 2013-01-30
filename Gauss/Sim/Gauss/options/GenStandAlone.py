## 
##  Example on how to run only the generator phase
##  It can be passed as additional argument to gaudirun.py directly
##   > gaudirun.py $APPCONFIGOPTS/Gauss/MC09-b5TeV-md100.py \
##                 $APPCONFIGOPTS/Conditions/MC09-20090602-vc-md100.py \
##                 $DECFILESROOT/options/EVENTTYPE.opts \
##                 $LBPYTHIAROOT/options/Pythia.opts \
##                 $GAUSSOPTS/GenStandAlone.py \
##                 $GAUSSOPTS/Gauss-Job.py
##  or you can set the property in your Gauss-Job.py
##  Port to python of GenStandAlone.opts
## 

from Gauss.Configuration import *

Gauss().Phases = ["Generator","GenToMCTree"] 
