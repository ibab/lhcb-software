/*
 * FileDeleter.cpp
 *
 *  Created on: Jul 20, 2015
 *      Author: beat
 */

#include "dic.hxx"
#include <string>
#include <vector>

class dyn_string : public std::vector<std::string >
{
public:
  dyn_string() {};
  virtual ~dyn_string()
  {
  }
};
dyn_string *doStrsplit(char *s, char *del)
{
  char *pch;
  pch = strtok(s,del);
  dyn_string *v=new dyn_string();
  int nel=0;
  while (pch != 0)
  {
    v->push_back(std::string(pch));
    if (pch != s) *(pch-1) = *del;
    nel++;
    pch = strtok(0,del);
  }
  return v;
}
dyn_string *Strsplit(const char *s, const char *del)
{
  return doStrsplit((char*)s, (char*)del);
}

dyn_string *Strsplit(const char *s, char *del)
{
  return doStrsplit((char*)s,del);
}

dyn_string *Strsplit(char *s, const char *del)
{
  return doStrsplit(s,(char*)del);
}

size_t StringReplace(std::string &in, const char *patt, std::string &repl)
{
  size_t res = in.find(patt);
  if (res != std::string::npos)
  {
    in.replace(res,strlen(patt),repl);
  }
  return res;
}
class CommandInfo : public DimUpdatedInfo
{
  public:
    std::string m_cmd;
    CommandInfo(char *name):DimUpdatedInfo((const char*)name,(char*)"????")
    {
      m_cmd = "";
    }
    void infoHandler()
    {
      char *input;
      input = getString();
      if (strcmp(input,"????") == 0) return;
      dyn_string *splt1 = Strsplit(input," ");
      std::string cmd = splt1->at(0);
      m_cmd = input;
      if (cmd == "delete")
      {
        std::string args;
        size_t i;
        for (i =1;i<splt1->size();i++)
        {
          args += splt1->at(i);
        }
        size_t blk = 0;
        std::string nill="";
        while (blk != std::string::npos)
        {
          blk = StringReplace(args," ",nill);
        }
        delete splt1;
        dyn_string *splt2 = Strsplit(args.c_str(),",");
        std::vector<int> runs;
        std::string shcomm;
        for (i=0;i<splt2->size();i++)
        {
          shcomm = "sudo rm -rf /localdisk/hlt1/Run_*"+splt2->at(i)+"_*.mdf";
          printf("executing command %s\n",shcomm.c_str());
          int status = ::system(shcomm.c_str());
        }
      }
      return;
    }
};
int main(int argc, char** argv)
{
  char *srvnam = (char*)"/Farm/FileDeleter";
  if (argc >1)
  {
    srvnam = argv[1];
  }
  CommandInfo cmd(srvnam);
  ::sleep(0xffffffff);
}
