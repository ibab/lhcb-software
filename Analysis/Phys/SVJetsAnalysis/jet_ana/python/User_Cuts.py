import sys,os

os.system("pwd")

function_name = sys.argv[1]
User_Cut = sys.argv[2]
#File_out = sys.argv[3]
File_out = "include/"+sys.argv[1]

user_bool = ""

first = True
print "getting config file: "+User_Cut
config_file = open( User_Cut,"r")
#print config_file.read()
#print config_file.readlines()

for line in config_file.readlines():
	print line
	line = line.strip()
	if not first:
		user_bool = user_bool + " && p->" + line
	if first:
		user_bool = "p->"+ line
		first=False
	
print "User_bool : "+user_bool
User_Cuts_text = """
#ifndef """+sys.argv[1]+"""_h
#define """+sys.argv[1]+"""_h
#include "ROOTOscar.h"
bool """+function_name+"""(Particle *);
#endif //"""+sys.argv[1]+"""_HPP_
"""

f = open(File_out+".h","w")
f.write(User_Cuts_text)

User_Cuts_text = """
#include \""""+sys.argv[1]+""".h\"
#define """+sys.argv[1]+"""_h
#include "ROOTOscar.h"
bool """+function_name+"""(Particle *p){
return """+user_bool+ """;
}
"""
f = open(File_out+".cc","w")
f.write(User_Cuts_text)
