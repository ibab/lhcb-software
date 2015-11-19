from ROOT import *
import sys
import os.path
    
filename=sys.argv[1]
if not (os.path.isfile(filename)):
    print 'File '+filename+' can not be found'
    exit(0)

F=TFile(filename)
DecayTree=F.Get("DecayTreeTuple/DecayTree")
N=DecayTree.GetEntries()
print N
Branches=DecayTree.GetListOfBranches()
EventsPassing={}
for B in Branches:
    BranchName=B.GetName()
    if "Decision" in  BranchName:
        #print BranchName
        EventsPassing[BranchName]=0

for entry in DecayTree:
   for branch,passing in EventsPassing.iteritems():
       checkpass=getattr(entry,branch)
       if checkpass == 1:
           EventsPassing[branch]=EventsPassing[branch]+1

Printed=False
print '{:<30} | {:<15} | {:<10}'.format('LineName','Percent Passing' ,'# passing')
for key,value in EventsPassing.iteritems():
    if value != 0:
        Printed=True
        percentpassing=(float(value)/float(N))*100.0
        LineString=((key.replace('Stripping','')).replace('Decision',''))
        PercentString=('{0:.1f}'.format(percentpassing))
        ValueString=('{0:.0f}'.format(value))
        print '{:<30} | {:^15} |  {:^10}'.format(LineString,PercentString,ValueString)

if not Printed:
    print 'No Correlations Found'
    


