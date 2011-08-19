#ifndef PARSED_PARAMETER_FILE_LIST_HH
#define PARSED_PARAMETER_FILE_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include <map>
#include <string>
#include <iostream>
#include "Mint/ParsedParameterFile.h"

/* It's a singleton - there can only be one instance
   of ParsedParameterFileList in any program.
*/

namespace MINT{

class ParsedParameterFile; // forward declaration.

class ParsedParameterFileList{
  ParsedParameterFileList();
  static ParsedParameterFileList* ptr;
 protected:
  std::map<std::string, ParsedParameterFile*> _fileMap;
  std::map<const std::istream*, ParsedParameterFile*> _streamMap;
  void deleteFileMap();
  void deleteStreamMap();
  void deleteAll();
 public:
  static ParsedParameterFileList* getMe();

  ParsedParameterFile* getParsedFile(const std::string& fname);
  ParsedParameterFile* getParsedFile(std::istream& is);

  void deleteFile(const std::string& fname);
  void deleteStream(const std::istream& istream);

  ParsedParameterFile* reloadFile(const std::string& id);

  ~ParsedParameterFileList(){
    deleteAll();
  }
};
}//namespace MINT
#endif
//
