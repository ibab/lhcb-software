
1. First you need an evironment that combines LHCB and LHCbDirac software. This
   is sometimes a little fiddly to get working. The following currently works :-

export COOL_IGNORE_LFC=1 # Probably not needed anymore

> SetupProject LHCbDirac v7r10p11
> SetupProject Brunel v43r2p2 pytools --use LCG_Interfaces/COOL --tag_add NEEDS_PYCOOL --runtime LHCbDirac --use LHCbDiracSys

Just for reference, (and not recommended) to run with the nightlies use.

> SetupProject LHCbDirac
> SetupProject Brunel pytools --nightly lhcb-head --use LCG_Interfaces/COOL --tag_add NEEDS_PYCOOL --runtime LHCbDirac --use LHCbDiracSys
#> SetupProject Brunel pytools --nightly lhcb-head --use LCG_Interfaces/COOL --tag_add NEEDS_PYCOOL

2. The script here to use is pyRunByRunCalibration.py. The first thing you need
   though is a list of ROOT files, as a plain text file. This can be created by
   using the RichCKCalibrate module in Rich/RichAlignment. Just run at the ganga prompt

 > RichCKCalibrate.dumpRootFileNamesToText(RichCKCalibrate.getCalibrationJobList(name='MyName'),'RootFiles.txt')

Replacing 'MyName' with whatever name you gave the calibration jobs you
submitted there. The files are then saved as a list in 'RootFiles.txt'

3. Now, pack to this package. Start 'ipython' and run

In [1]: import pyRunByRunCalibration

In [2]: pyRunByRunCalibration.initialise() 

You can then run whatever calibration you want to run. The list is

  hpdImageShiftsFollow - The main method to produce the run by run HPD image
                         calibratiions.

  hpdImageShiftsAverage - Similar to the above, but produces a single average
                          calibration for each HPD. Useful for prompt processings (i.e. the future).

  hpdOccupancies - Averatge HPD occupancy values.

To run, just do

In [3]: pyRunByRunCalibration.hpdImageShiftsFollow('RootFiles.txt')

and leac it going. This will spit out some PDF files, some plain text files and
the calibration DB itself.
