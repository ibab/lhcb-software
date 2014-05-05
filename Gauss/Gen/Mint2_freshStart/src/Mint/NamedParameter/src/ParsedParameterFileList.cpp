// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/ParsedParameterFileList.h"

#include "Mint/ParsedParameterFile.h"
#include <string>
#include <cassert>

using namespace std;
using namespace MINT;

ParsedParameterFileList* ParsedParameterFileList::ptr=0;

ParsedParameterFileList* ParsedParameterFileList::getMe(){
  if(0 == ptr){
    ptr = new ParsedParameterFileList;
  }
  return ptr;
}

ParsedParameterFileList::ParsedParameterFileList(){}

ParsedParameterFile*
ParsedParameterFileList::getParsedFile(const std::string& id){
  map<string, ParsedParameterFile* >::iterator it =  _fileMap.find(id);
  if(it == _fileMap.end()){
    ParsedParameterFile* ppf = new ParsedParameterFile(id);
    assert(0 != ppf);
    _fileMap[id] = ppf;
    return ppf;
  }else{
    return it->second;
  }
}

ParsedParameterFile*
ParsedParameterFileList::getParsedFile(std::istream& id){
  map<const istream*, ParsedParameterFile* >::iterator it = 
    _streamMap.find(&id);
  if(it == _streamMap.end()){
    ParsedParameterFile* ppf = new ParsedParameterFile(id);
    assert(0 != ppf);
    _streamMap[&id] = ppf;
    return ppf;
  }else{
    return it->second;
  }
}

void ParsedParameterFileList::deleteFile(const std::string& id){
  map<string, ParsedParameterFile* >::iterator it =  _fileMap.find(id);
  if(it == _fileMap.end()) return;
  delete (it->second);
  _fileMap.erase(it);
}

ParsedParameterFile* ParsedParameterFileList::reloadFile(const std::string& id){
  deleteFile(id);
  return getParsedFile(id);
}

void ParsedParameterFileList::deleteStream(const std::istream& id){
  map<const istream*, ParsedParameterFile* >::iterator it 
    =  _streamMap.find(&id);
  if(it == _streamMap.end()) return;
  delete (it->second);
  _streamMap.erase(it);
}
void ParsedParameterFileList::deleteFileMap(){
  for(map<string, ParsedParameterFile* >::iterator it=_fileMap.begin();
      it != _fileMap.end();
      it++){
    delete (it->second);
  }
  _fileMap.clear();
}
void ParsedParameterFileList::deleteStreamMap(){
  for(map<const istream*, ParsedParameterFile* >::iterator it=
	_streamMap.begin();
      it != _streamMap.end();
      it++){
    delete (it->second);
  }
  _streamMap.clear();
}
void ParsedParameterFileList::deleteAll(){
  deleteFileMap();
  deleteStreamMap();
}
//

