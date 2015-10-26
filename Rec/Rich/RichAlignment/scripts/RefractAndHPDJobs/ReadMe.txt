
This directory contains the following scripts

1. GetLFNsByRun.py

This script is used to get for a given period of time a list of offline runs, 
and for each run an LFN list for the raw data in the express stream. 
It runs under the LHCbDirac environment, so 

 > SetupProject LHCbDirac
 > lhcb-proxy-init

It requires 6 arguments, e.g.

 > ./GetLFNsByRun.py
Usage: GetLFNsByRun <year> <startmonth> <startday> <lastmonth> <lastday>

So for example

 > ./GetLFNsByRun.py 2011 1 1 5 8

This script will produce output bzipped and pickled python files in the RunData 
sub-directory. Note that files for each full month are stored in SVN, so you
will not need to run the script in these cases. These files are needed for the
next steps. 

( Note, if you have it, use the lhcb_calibration role

> lhcb-proxy-init -g lhcb_calibration -v 120:00

as this give you priority over 'average' grid users. If you are working on a
calibration task, and don't have this role, drop me an email ) 

For testing, I suggest editing the dates to a short period with only a few runs....

2. RichCKCalibrate.py

This is a python module for use with ganga. So start ganga (clean terminal) in
the same dir as this file and import it

 > SetupProject Ganga
 > lhcb-proxy-init -g lhcb_calibration -v 120:00
 > ganga
 ...
 > import RichCKCalibrate

If you are going to use the calibration role, its useful to use a different gangadir and
proxy location to your normal work, to isolate it. You can do this with

 - Set X509_USER_PROXY to put your calibration proxy in a different place
    > export X509_USER_PROXY=/usera/jonesc/.grid.proxy.calibration
   You must run 'SetupProject Ganga' after this, to pick up the new setting

 - Start ganga using something like
    > ganga -o[Configuration]gangadir=/var/nwork/pclx/jonesc/gangadir-calibration

The file has a series of methods to submit the calibration jobs, and analyse the
results. For submission just run at the ganga prompt 

 > RichCKCalibrate.submitCalibrationJobs(name="MyName",pickledRunsList=['XYZ.pck.bz2'])

where 'name' is just a name you can give the set of jobs, and pickledRunsList is
a list of pickled python files, containing the run data, produced in step 1.

If you don't give the pickledRunsList argument, the method will just use all
files it finds in the RunData directory.

This will submit a load of jobs, which with the grid gods willing, will eventually run...

Once the jobs are finished (or even if a few aren't) you can analyse the results with

 > calibJobs = RichCKCalibrate.getCalibrationJobList(name="MyName")

which returns a list of (completed) calibration jobs and

 > RichCKCalibrate.refractiveIndexCalib(calibJobs,'Rich1Gas')
 > RichCKCalibrate.refractiveIndexCalib(calibJobs,'Rich2Gas')

which runs the calibration on the jobs, for the given radiator. This process
will spit out another XYZ.pck.bz2 file, which contains the calibration data for the
next step. It also will produce a PDF showing all the fit results, and a plain
text file which can be imported into any spreadsheet application

3. ./CreateRefScaleFactCondDBLayer.py <name>

This script creates a new DB slice from the XYZ.pck.bz2 data spat out in the last
step. It needs to run under a particular environment 

 > SetupProject LHCbDirac
 > SetupProject LHCb --runtime LHCbDirac --use LHCbDiracSys
 > lhcb-proxy-init -g lhcb_calibration
 > ./CreateCondDBLayer.py

(you may need to give an explicit LHCb version). 

The above script takes one argument, which is the name of the calibration files
you want to use. This is a little different frm the name above, in that it alsom
contains the Brunel version. Just run 'ls *.pck.bz2' to see what pickled outoput
files have been created, e.g.

pciy ~/cmtuser/Rec_HEAD/Rich/RichAlignment/scripts/RefractAndHPDJobs > ls *.pck.bz2
Rich1Gas_RefInCalib-Final2010RePro-V2_BR-v37r8.pck.bz2  RunInfoCache.pck.bz2
Rich2Gas_RefInCalib-Final2010RePro-V2_BR-v37r8.pck.bz2

So for me, I would run

3. ./CreateRefScaleFactCondDBLayer.py "RefInCalib-Final2010RePro-V2_BR-v37r8"

This step finally spits out a new NewRichCKRefIndexCalib.db DB file, which can
be used in any Gaudi job as a DB slice. 

4. Once you have a new DB, you can if you want go back to ganga to resubmit some
   verification jobs, to prove it has done the job.

 - First, copy the new NewRichCKRefIndexCalib.db slice into the 'databases'
   sub-dir

Then

 > RichCKCalibrate.submitVerificationJobs(name="MyName",pickledRunsList=['XYZ.pck.bz2'])

where the pck file is the same one used for the calibration jobs. 

Once these jobs are finished you can run 

 > veriJobs = RichCKCalibrate.getVerificationJobList(name="MyName")

to get the job list, then rerun the calibration on these jobs

 > RichCKCalibrate.refractiveIndexCalib(veriJobs,'Rich1Gas')
 > RichCKCalibrate.refractiveIndexCalib(veriJobs,'Rich2Gas')

Which will spit out PDFs and text files as before. If it has work, the n-1
corrections should now be 1 ... 
