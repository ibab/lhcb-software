#pragma once
#include <string>
#include <vector>
#include <map>
class CCPCObj;
typedef std::map<std::string,CCPCObj*> ObjMap;
typedef std::pair<std::string, CCPCObj*> ObjPair;
typedef ObjMap::iterator SysIter;
class ObjSerializer
{
protected:
  int buffersize;
  void *buffer;
public:
  ObjMap *m_objmap;
  ObjSerializer(void);
  ObjSerializer(ObjMap *ObjMap);
  virtual ~ObjSerializer(void)=0;
  virtual void *SerializeHist(void * &ptr,int &siz, bool clear=false) =0;
  virtual void *SerializeHist(char *name,void * &ptr,int &siz, bool clear=false)=0;
  virtual void *SerializeHist(std::vector<std::string> &nams,void * &ptr,int &siz, bool clear=false)=0;
  virtual void *SerializeDir(void *&ptr, int &siz)=0;
  virtual void DeSerializeHist(char *name,void *&ptr)=0;
  virtual void DeSerializeHist(std::vector<std::string> &nams,void *&ptr)=0;

};
