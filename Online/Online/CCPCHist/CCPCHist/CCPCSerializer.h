#pragma once
#include "ObjSerializer.h"
#include <map>
class CCPCHisto;
typedef std::map<std::string,CCPCHisto*> HistMap;

class CCPCSerializer : public ObjSerializer
{
  void *Allocate(int);
  CCPCHisto *findhisto(const char *name);
public:
  CCPCSerializer(void);
  CCPCSerializer(ObjMap *);
  virtual ~CCPCSerializer(void);
  void *SerializeHist(void * &ptr,int &siz, bool clear=false);
  void *SerializeHist(char *name,void * &ptr,int &siz, bool clear=false);
  void *SerializeHist(std::vector<std::string> &nams,void * &ptr,int &siz, bool clear=false);
  void *SerializeDir(void *&ptr, int &siz);
  void DeSerializeHist(char *name,void *&ptr);
  void DeSerializeHist(std::vector<std::string> &nams,void *&ptr);
};
