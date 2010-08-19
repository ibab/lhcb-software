
This directory contains the following scripts

1. GetLFNsByRun.py

This script is used to get for a given period of time (currently hard-coded at
the top of the file itself...) a list of offline runs, and for each run an LFN
list for the raw data in the express stream. It runs under the LHCbDirac
environment, so 

 > SetupProject LHCbDirac
 > lhcb-proxy-init
 > ./GetLFNsByRun.py

( Note, if you have it, use the lhcb_calibration role

> lhcb-proxy-init -g lhcb_calibration

as this give you priority over 'average' grid users. If you are working on a
calibration task, and don't have this role, drop me an email) 

The ./GetLFNsByRun.py script will spit out a pickled python file (.pck)
containing the run and LFN information, which is needed for the next step. 

For testing, I suggest editing the dates to a short period with only a few runs....

2. RichCKCalibrate.py

This is a python module for use with ganga. So start ganga (clean terminal) in
the same dir as this file and import it 

 > SetupProject Ganga
 > lhcb-proxy-init -g lhcb_calibration
 > ganga
 ...
 > import RichCKCalibrate

The file has a series of methods to submit the calibration jobs, and analyse the
results. For submission just run at the ganga prompt 

 > RichCKCalibrate.submitCalibrationJobs(['XYZ.pck'])

where XYZ.pck is a file created in step 1. If you have more than one pickled file you
wish to submit, just extend the list, e.g.

 > RichCKCalibrate.submitCalibrationJobs(['XYZ1.pck','XYZ2.pck'])

This will submit a load of jobs, which with the grid gods willing, will eventually run...

Once the jobs are finished (or even if a few aren't) you can analyse the results with

 > calibJobs = RichCKCalibrate.getCalibrationJobList()

which returns a list of (completed) calibration jobs and

 > RichCKCalibrate.refractiveIndexCalib(calibJobs,'Rich1Gas')
 > RichCKCalibrate.refractiveIndexCalib(calibJobs,'Rich2Gas')

which runs the calibration on the jobs, for the given radiator. This process
will spit out another XYZ.pck file, which contains the calibration data for the
next step, and also a PDF showing the fit results etc. 

3. ./CreateCondDBLayer.py

This script creates a new DB slice from the XYZ.pck data spat out in the last
step. It needs to run under a particular environment 

 > SetupProject LHCb --runtime LHCbDirac --use LHCbDiracSys
 > lhcb-proxy-init -g lhcb_calibration
 > ./CreateCondDBLayer.py

(you may need to give an explicit LHCb version. I know v31r1 works as it was the
last one I used, probably later ones are OK as well). 

This step finally spits out a new NewRichCKRefIndexCalib.db DB file, which can
be used in any Gaudi job as a DB slice. 

4. Once you have a new DB, you can if you want go back to ganga to resubmit some
   verification jobs, to prove it has done the job. From ganga 

 > RichCKCalibrate.submitVerificationJobs('XYZ.pck')

where the pck file is the same one used for the calibration jobs. Once these
jobs are finished you can run 

 > veriJobs = RichCKCalibrate.getVerificationJobList()

to get the job list, then rerun the calibration these

 > RichCKCalibrate.refractiveIndexCalib(veriJobs,'Rich1Gas')
 > RichCKCalibrate.refractiveIndexCalib(veriJobs,'Rich2Gas')

If it has work, the n-1 corrections should now be 1 ...
