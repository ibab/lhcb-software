#include "AlignmentMonitoring/Utilities.h"

/*
  Author: Maurizio Martinelli
  Institute: Nikhef
  Email: maurizio.martinelli@nikhef.nl

  Name: Utilities
  Description: a class that defines a few utilities of to perform repetive
               operations faster.

  Date: 5/6/2013
 */

// POSIX
//#include "dirent.h"
// STL
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
// ROOT
#include "TString.h"

#define MAX_SIZE 100

using namespace Alignment::AlignmentMonitoring;

//
// command line option processing
//
int 
Utilities::FindOption(int argc, char **argv, const char *option)
{
  for (int i = 1; i < argc; i++)
    if(strcmp(argv[i],option)==0)
      return i;
  return -1;
}

int 
Utilities::ReadIntOption(int argc, char **argv, const char *option, int default_value)
{
  int iplace = FindOption(argc,argv,option);
  if(iplace >=0 && iplace < argc-1)
    return atoi(argv[iplace+1]);
  return default_value;
}

double 
Utilities::ReadDoubleOption(int argc, char **argv, const char *option, double default_value)
{
  int iplace = FindOption(argc,argv,option);
  if(iplace >=0 && iplace < argc-1) { 
    std::stringstream ss ( argv[iplace+1] );
    double result = 0;
    ss>> result;
    return result;
  }
  return default_value;
}

char * 
Utilities::ReadStringOption(int argc, char **argv, const char *option, char *default_value)
{
  int iplace = FindOption(argc,argv,option);
  if(iplace >=0 && iplace < argc-1)
    return argv[iplace+1];
  return default_value;
}
 
double  
Utilities::to_double ( const char *p )
{
  std::stringstream ss ( p );
  double result = 0;
 
  ss>> result;
 
  return result;
}

std::string
Utilities::GetEnvVar( const char* key)
{
  char *val;
  val = getenv(key);
  std::string retval="";
  if (val!=NULL)
    retval=val;

  return retval;
}

int 
Utilities::findChar(const char str[], char ch)
{
 int rep=0;
 for(int i=0; str[i]!='\0'; i++)
   if(str[i]==ch)
     rep++;
 return rep;
}

std::vector<std::string>  
Utilities::splitString(std::string str, std::string tokens)
{
  std::vector<std::string> splittedStr;
  std::string strCopy(str.c_str()); 
  char* tmpStr = strtok( (char*)strCopy.c_str(), tokens.c_str() );
  while(tmpStr != NULL) {
    splittedStr.push_back(tmpStr); 
    tmpStr = strtok(NULL, tokens.c_str());
  }
  return splittedStr;
}/* dirent creates a mess
std::vector<std::string> getFileList(const char* directory, const char* flag="") {

  std::vector<std::string> fList;
  DIR *dir;
  struct dirent *ent;
  dir = opendir (directory);
  // loop over found directories
  if (dir != NULL) {
    // print all the files and directories within directory 
    while ( (ent = readdir (dir)) != NULL) {
      string fname(ent->d_name);
      if (fname.find(flag) != -1) {
	printf ("%s%s\n", directory, ent->d_name);
	fList.push_back( Form( "%s%s", directory, ent->d_name ) );
      }
    }
    closedir (dir);
  } else {
    // could not open directory 
    perror ("");
    return fList;
  }
  return fList;
}*/

std::vector<std::string>  
Utilities::getFilesFromList(const char* list) 
{
  std::vector<std::string> fList;
  std::ifstream in;
  in.open( list );
  std::string tmpName;
  while (1) {
    in >> tmpName;
    if (!in.good()) break;
    fList.push_back(tmpName);
    tmpName.clear();
  }
  return fList;
}

std::string
Utilities::find_and_replace(std::string& source, std::string const& find, std::string const& replace)
{
  std::string scopy( source );
  for(std::string::size_type i = 0; (i = scopy.find(find, i)) != std::string::npos;)
    {
        scopy.replace(i, find.length(), replace);
        i += replace.length();
    }
  return scopy;
}

std::string 
Utilities::getStringBetween(const char* first, const char* second, const char* str)
{
  std::string extract(str);
  extract =  extract.substr( extract.find(first)+std::string(first).size() ); // extract from 'first' to the end
  unsigned pos = extract.find( second );         // position of "second" in extract
  extract = extract.substr(0,pos);
  return extract;
}
