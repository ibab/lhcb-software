#include <iostream>
#include <string>
#include <cstdio>
#include <cctype>
#include <map>
#include <sstream>

class Command {
public:
  struct Data {
    std::string name;
    std::string file;
    int fileID;
    std::string guid;
    std::string sender;
    int nEvts;
    int sliceID;
  };
  Data data;

  Command ();
  Command (char* cmd, char* f, int fid, char* g, char* s, int n);
  int decodeCommand(std::string msg);
  std::string encodeResponse(int status,int nEvts, const std::string& error);
  void clear();
  void setSliceID( int );
  
private:
  enum StringValue { not_defined,
                     enum_params,
                     enum_command,
                     enum_file,
                     enum_fileID,
                     enum_guid,
                     enum_sender };
  std::map<std::string, StringValue> s_mapStringValues;
  
  int decodeDict(std::string msg, int &pos, Data &out);
  std::string decodeString(std::string msg, int &pos);
  int decodeInt(std::string msg, int &pos);
};  

Command::Command () {
  data.name="";
  data.file="";
  data.fileID=-1;
  data.guid="";
  data.sender="";
  data.nEvts=0;
  data.sliceID=-1;
  
  s_mapStringValues["params"] = enum_params;
  s_mapStringValues["command"] = enum_command;
  s_mapStringValues["raw_file"] = enum_file;
  s_mapStringValues["fileID"] = enum_fileID;
  s_mapStringValues["guid"] = enum_guid;
  s_mapStringValues["sender"] = enum_sender;
}

Command::Command (char* cmd, char* f ,int fid, char* g, char* s, int n=0){
  data.name=cmd;
  data.file=f;
  data.fileID=fid;
  data.guid=g;
  data.sender=s;
  data.nEvts=n;
  
  s_mapStringValues["params"] = enum_params;
  s_mapStringValues["command"] = enum_command;
  s_mapStringValues["raw_file"] = enum_file;
  s_mapStringValues["fileID"] = enum_fileID;
  s_mapStringValues["guid"] = enum_guid;  
  s_mapStringValues["sender"] = enum_sender;
}

int Command::decodeCommand(std::string msg){
  clear();
  Data ret;
  int i=0;
  switch (msg[i]){
    case 'd':
      i=decodeDict(msg,i,ret); // Add error check
      if (!i) return 0;      
      break;
    default:
      return 0;
  }
  data=ret;
  return 1;
}                          

int Command::decodeDict(std::string msg, int &pos, Data &out){
  int i=pos+1;
  bool end=0;
  std::string tok;
  while(!(end)){
    switch (msg[i]){
      case 's':
        tok=decodeString(msg,i);
        // std::cout << "Left to decode: " << msg.substr(i) << std::endl;
        switch (s_mapStringValues[tok]){
          case enum_params:
            // std::cout << "Decode dict again: " << msg.substr(i) << std::endl;
            i=decodeDict(msg,i,out);
            if (!i) return 0;
            break;
          case enum_command:
            // std::cout << "Decode command" << std::endl;
            out.name=decodeString(msg,i);
            break;
          case enum_fileID:
            // std::cout << "Decode fileID" << std::endl;
            out.fileID=decodeInt(msg,i);
            break;
          case enum_file:
            // std::cout << "Decode file" << std::endl;
            out.file=decodeString(msg,i);
            break;
          case enum_guid:
            // std::cout << "Decode GUID" << std::endl;
            out.guid=decodeString(msg,i);
            break;          
          case enum_sender:
            // std::cout << "Sender" << std::endl;
            out.sender=decodeString(msg,i);
            break;          
          default:
            std::cout << "Unknown parameter: " << tok << std::endl;
            switch(msg[i+1]){
              case 's':
                decodeString(msg,i);
                break;
              case 'i':
                decodeInt(msg,i);
                break;
              case 'I':
                decodeInt(msg,i);
                break;
              case 'd':
                decodeDict(msg,i,out);
                break;
            }
            break;
        }
        break;
      case 'e':
        end=1;
        break;
      default:
        return 0;
        break;
    }
  }
  return i+1;
}

std::string Command::decodeString(std::string msg, int &pos){
  size_t colonpos=msg.find(":",pos);
  int len=atoi(msg.substr(pos+1,colonpos-pos-1).c_str());
  std::string tok=msg.substr(colonpos+1,len);
  pos = colonpos + len + 1;
  return tok;
}

int Command::decodeInt(std::string msg, int &pos){
  size_t begpos=msg.find("i",pos);
  size_t endpos=msg.find("e",pos);
  int tok=atoi(msg.substr(begpos+1,endpos-begpos-1).c_str());
  pos = endpos+1;
  return tok;
}

void Command::clear(){
  data.name="";
  data.file="";
  data.fileID=-1;
  data.sender="";
}

void Command::setSliceID( int id ){
  data.sliceID = id;
}

std::string Command::encodeResponse(int status,int nEvts=0, const std::string& error=""){
  std::stringstream outstream;
  std::string out;
  if (status==0){
    outstream << "ds6:statusi" << status << "es6:paramsds7:sliceIDi" << data.sliceID << "es6:fileIDi" << data.fileID << "es5:errors"<< error.length() << ":" << error << "es7:commands" << data.name.length() << ":" << data.name << "e";
  } else if (status==1){
    outstream << "ds6:statusi" << status << "es6:paramsds7:sliceIDi" << data.sliceID << "es6:fileIDi" << data.fileID << "ees7:commands" << data.name.length() << ":" << data.name << "e";
  } else if (status==2) {
    outstream << "ds6:statusi" << status << "es6:paramsds7:sliceIDi" << data.sliceID << "es6:fileIDi" << data.fileID << "es5:nEvtsi" << nEvts << "ees7:commands" << data.name.length() << ":" << data.name << "e";
  } 
  outstream >> out;
  return out;
}

// int main(){
//   Command r;
//   r.decodeCommand("ds7:commands13:newFileToReads6:paramsds8:raw_files58:/daqarea/CCRC/v0/00002090/RAW/0000/00002090_00002563_1.raws6:senders8:whatevers4:files14:5mdfEvents.dats6:fileIDi1000eee");
// 'ds7:commands13:newFileToReads6:paramsds8:raw_files14:5mdfEvents.dats6:senders8:whatevers6:fileIDi1000eee'
// 'ds6:paramsds6:fileIDi1000es6:senders8:whatevers8:raw_files9:/tmp/fifoes7:commands13:newFileToReade'
//   std::cout << "Name: " << r.data.name << std::endl;
//   std::cout << "File: " << r.data.file << std::endl;
//   std::cout << "Sender: " << r.data.sender << std::endl;
//   std::cout << "FileID: " << r.data.fileID << std::endl;
//   std::cout << r.encodeResponse(0) << std::endl;
//   std::cout << r.encodeResponse(0,0,"TestError") << std::endl;  
//   std::cout << r.encodeResponse(1) << std::endl;
//   std::cout << r.encodeResponse(2,100) << std::endl;  
//   return 1;
// }

// Idle: ds6:statusi1es6:paramsdes7:commands4:idlee
// Out: ds6:statusi1es6:paramsds6:fileIDi10000ees7:commands13:newFileToReade
