import os,sys, subprocess

file_list = []

maindirectory = os.path.abspath(sys.argv[1])

ls = subprocess.Popen("ls "+sys.argv[1],stdout=subprocess.PIPE, shell=True).communicate()[0]

ls = ls.split()

print maindirectory, ls

for subdir in ls:
	subdir_files = subprocess.Popen("ls "+maindirectory+"/"+subdir+"/*.root",stdout=subprocess.PIPE, shell=True).communicate()[0]
	subdir_files = subdir_files.split()
	#print subdir_files

	for subdir_file in subdir_files:
		cp_command = "cp -v "+subdir_file + " " +maindirectory+"/"+subdir_file.split("/")[-1].split(".")[0]+subdir+".root"
		print cp_command
		cp = subprocess.Popen(cp_command,stdout=subprocess.PIPE, shell=True).communicate()[0]

