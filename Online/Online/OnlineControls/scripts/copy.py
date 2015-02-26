import os, sys
inst ='/group/online/dataflow/cmtuser/WINCC_OA_dev/Online/OnlineControls/pvss'
wincc='/localdisk/wincc/'+sys.argv[1]
os.stat(wincc)
print 'Updating', wincc

os.chdir(inst)
os.chdir(inst+'/scripts')
os.system('for i in `ls *.cpp`;do rm -f '+wincc+'/scripts/$i; ln -s `pwd`/$i '+wincc+'/scripts/$i; echo '+wincc+'/scripts/$i;done')
#
os.chdir(inst+'/scripts/libs')
os.system('for i in `ls *.cpp`;do rm -f '+wincc+'/scripts/libs/$i; ln -s `pwd`/$i '+wincc+'/scripts/libs/$i; echo '+wincc+'/scripts/libs/$i;done')
#
#
#
os.chdir(inst+'/panels')
os.system('for i in `ls .`;do rm -rf '+wincc+'/panels/$i; ln -s `pwd`/$i '+wincc+'/panels/$i; echo '+wincc+'/panels/$i; done')
