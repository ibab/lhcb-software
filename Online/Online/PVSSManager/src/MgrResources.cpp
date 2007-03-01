// Our Resources administration
#include "PVSSManager/MgrResources.h"
#include "ErrHdl.hxx"

using namespace PVSS;

// Our static Variable
CharString  MgrResources::m_dllName  = "";
CharString  MgrResources::m_callName = "";


// Wrapper to read config file
void  MgrResources::init(int &argc, char *argv[])   {
  begin(argc, argv);
  while ( readSection() || generalSection() );
  end(argc, argv);
}


// Read the config file.
// Our section is [demo] or [demo_<num>], 
// our keywords are "dpNameSet" and dpNameConnect
PVSSboolean  MgrResources::readSection()    {
  // Is it our section ? 
  // This will test for [demo] and [demo_<num>]
  if (!isSection("demo"))
    return PVSS_FALSE;

  // Read next entry
  getNextEntry();
  CharString val;
  while ( (cfgState != CFG_SECT_START) && (cfgState != CFG_EOF) )  {
    // Compare keyword with "dpNameSet" and dpNameConnect
    if (!keyWord.icmp("DLL"))
      cfgStream >> m_dllName;
    else if (!keyWord.icmp("Call"))
      cfgStream >> m_callName;           
    else if (!readGeneralKeyWords())  {
      ErrHdl::error(ErrClass::PRIO_WARNING,     // not that bad
                    ErrClass::ERR_PARAM,        // wrong parametrization
                    ErrClass::ILLEGAL_KEYWORD,  // illegal Keyword in Res.
                    keyWord);
      // Signal error, so we stop later
      cfgError = PVSS_TRUE;
    }
    getNextEntry();
  }
  // So the loop will stop at the end of the file
  return cfgState != CFG_EOF;
}

