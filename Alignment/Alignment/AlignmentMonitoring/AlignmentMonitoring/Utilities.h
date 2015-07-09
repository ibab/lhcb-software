#ifndef UTILITIES_H
#define UTILITIES_H

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
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
// ROOT
#include "TString.h"

#define MAX_SIZE 100

namespace Alignment
{
  namespace AlignmentMonitoring
  {
    class Utilities {
    public:
      // Constructor
      Utilities() {}
      // Destructor
      ~Utilities() {}
      // Methods
      // command line option processing
      //
      int FindOption(int argc, char **argv, const char *option);
      int ReadIntOption(int argc, char **argv, const char *option, int default_value);
      double ReadDoubleOption(int argc, char **argv, const char *option, double default_value);
      char *ReadStringOption(int argc, char **argv, const char *option, char *default_value);
      
      double to_double ( const char *p );
      std::string GetEnvVar( const char* key);
      int findChar(const char str[], char ch);
      std::vector<std::string> splitString(std::string str, std::string tokens);
      std::string getStringBetween(const char* first, const char* second, const char* str);
      // option processing from a string
      //
      
      std::vector<std::string> getFilesFromList(const char* list);
      std::string find_and_replace(std::string& source, std::string const& find, std::string const& replace);
    protected:
    private:
    };
    
  }// namespace AlignmentMonitoring
}// namespace Alignment


#endif
    
