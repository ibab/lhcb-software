#include "MBM/Requirement.h"
#include "MBM/bmdef.h"
#include <cctype>
#include <cstdio>
#include <cstring>

/// Default constructor
MBM::Requirement::Requirement () {
  reset();
}

/// Reset requirement to default values
void MBM::Requirement::reset()    {
  vetomask[0] = vetomask[1] = vetomask[2] = vetomask[3] = 0;
  trmask[0] = trmask[1] = trmask[2] = trmask[3] = ~0x0;
  evtype = userType = freqType = maskType = 0;
  freq = 0.;	
}

/// Parse requirement from string
void MBM::Requirement::parse(const std::string& reqstring)
{
  char *items[20];
  int ikey=0;
  char rstr[1024];
  ::strcpy(rstr,reqstring.c_str());
  char* token = ::strtok(rstr, ";");
  while (token!= NULL)  {
    items[ikey++] = token;
    token = ::strtok(NULL,";");
  }
  // Requirement format:
  // "EvType=x;TriggerMask=0xfeedbabe,0xdeadfeed,0xdeadbabe,0xdeadaffe;
  //  VetoMask=0x,0x,0x,0x;MaskType=ANY/ALL;UserType=USER/VIP/ONE;
  //  Frequency=MANY/PERC;Perc=20.5"
  for(int i=0; i<ikey; i++ )  {
    char* keyw = strtok(items[i],"=");
    for (unsigned int j=0;j<strlen(keyw);j++)  {
      keyw[j] = char(::toupper(keyw[j]));
    }
    char* values = strtok(NULL,"=");
    if ( ::strcmp(keyw, "EVTYPE") == 0 )    {
      ::sscanf(values,"%d",&evtype);
      continue;
    }
    if ( ::strcmp(keyw,"TRIGGERMASK") == 0 )  {
      char *v = strtok(values,",");
      ::memset(trmask, 0, sizeof(trmask));
      for (int j=0; v != 0 && j<4; j++)  {
        ::sscanf(v,"%x",&trmask[j]);
        v = ::strtok(NULL,",");
      }
      continue;
    }
    if ( ::strcmp(keyw,"VETOMASK") == 0 )    {
      ::memset(vetomask, 0, sizeof(vetomask));
      char *v = strtok(values,",");
      for (int j=0; v != 0 && j<4; j++)  {
        ::sscanf(v,"%x",&vetomask[j]);
        v = strtok(NULL,",");
      }
    }
    if ( ::strcmp(keyw, "MASKTYPE") == 0 )  {
      for (unsigned int j=0;j<strlen(values);j++)  {
        values[j] = char(::toupper(values[j]));
      }
      if ( ::strcmp(values,"ANY") == 0 )  {
        maskType = BM_MASK_ANY;
      }
      else  {
        maskType = BM_MASK_ALL;
      }
      continue;
    }
    if (strcmp(keyw, "USERTYPE") == 0)   {
      for (unsigned int j=0;j<strlen(values);j++)  {
        values[j] = char(::toupper(values[j]));
      }
      if (strcmp(values,"USER") == 0) {
        userType = BM_REQ_USER;
      }
      else if (strcmp(values,"VIP") == 0) {
        userType = BM_REQ_VIP;
      }
      else if (strcmp(values,"ALL") == 0) {
        userType = BM_REQ_ALL;
      }
      else {
	int tmp = *(int*)values;
	tmp = tmp >> 8;
        userType = (tmp<<8) + BM_REQ_ONE;
      }
      continue;
    }
    if (strcmp(keyw, "FREQUENCY") == 0)  {
      for (unsigned int j=0;j<strlen(values);j++)  {
        values[j] = char(::toupper(values[j]));
      }
      if (strcmp(values,"MANY") == 0)
        freqType = BM_FREQ_MANY;
      else
        freqType = BM_FREQ_PERC;
      continue;
    }
    if (strcmp(keyw, "PERC") == 0)  {
      sscanf(values,"%f",&freq);
      continue;
    }
  }
}
