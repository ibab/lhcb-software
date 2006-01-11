#include "MBM/Requirement.h"
#include "MBM/bmdef.h"

/// Default constructor
MBM::Requirement::Requirement () {
  reset();
}

/// Reset requirement to default values
void MBM::Requirement::reset()    {
  vetomask[0] = vetomask[1] = vetomask[2] = vetomask[3] = -1;
  trmask[0] = trmask[1] = trmask[2] = trmask[3] = -1;
  evtype = userType = freqType = maskType = 0;
  freq = 0.;	
}

/// Parse requirement from string
void MBM::Requirement::parse(const std::string& reqstring)
{
  char *token;
  char *items[20];
  int ikey=0;
  char rstr[1024];
  strcpy(rstr,reqstring.c_str ( ));
  token=strtok(rstr, ";");
  while (token!= NULL)
  {
    items[ikey] = token;
    ikey++;
    token = strtok(NULL,";");
  }
  // Requirement format:
  // "EvType=x;TriggerMask=0xfeedbabe,0xdeadfeed,0xdeadbabe,0xdeadaffe;
  //  VetoMask=0x,0x,0x,0x;MaskType=ANY/ALL;UserType=USER/VIP/ONE;
  //  Frequency=MANY/PERC;Perc=20.5"
  for(int i=0;i<ikey;i++)
  {
    char* keyw;
    char *values;
    keyw=strtok(items[i],"=");
    unsigned int j;
    for (j=0;j<strlen(keyw);j++)
    {
      keyw[j] = toupper(keyw[j]);
    }
    values = strtok(NULL,"=");

    if (strcmp(keyw, "EVTYPE") == 0)
    {
      sscanf(values,"%d",&evtype);
      continue;
    }
    if (strcmp(keyw,"TRIGGERMASK") == 0)
    {
      for (i=0;i<4;i++)
      {
        trmask[i] = 0;
      }
      char *v;
      v = strtok(values,",");
      for (i=0;i<4;i++)
      {
        if (v != NULL)
        {
          sscanf(v,"%x",&trmask[i]);
          v = strtok(NULL,",");
        }
        else
        {
          break;
        }
      }
      continue;
    }
    if (strcmp(keyw,"VETOMASK") == 0)
    {
      for (i=0;i<4;i++)
      {
        vetomask[i] = 0;
      }
      char *v;
      v = strtok(values,",");
      for (i=0;i<4;i++)
      {
        if (v != NULL)
        {
          sscanf(v,"%x",&vetomask[i]);
          v = strtok(NULL,",");
        }
        else
        {
          break;
        }
      }
      continue;
    }
    if (strcmp(keyw, "MASKTYPE") == 0)
    {
      for (j=0;j<strlen(values);j++)
      {
        values[j] = toupper(values[j]);
      }
      if (strcmp(values,"ANY") == 0)
      {
        maskType = BM_MASK_ANY;
      }
      else
      {
        maskType = BM_MASK_ALL;
      }
      continue;
    }
    if (strcmp(keyw, "USERTYPE") == 0)
    {
      for (j=0;j<strlen(values);j++)
      {
        values[j] = toupper(values[j]);
      }
      if (strcmp(values,"USER") == 0)
      {
        userType = BM_REQ_USER;
      }
      else if (strcmp(values,"VIP") == 0)
      {
        userType = BM_REQ_VIP;
      }
      else
      {
        userType = BM_REQ_ONE;
      }
      continue;
    }
    if (strcmp(keyw, "FREQUENCY") == 0)
    {
      for (j=0;j<strlen(values);j++)
      {
        values[j] = toupper(values[j]);
      }
      if (strcmp(values,"MANY") == 0)
      {
        freqType = BM_FREQ_MANY;
      }
      else
      {
        freqType = BM_FREQ_PERC;
      }
      continue;
    }
    if (strcmp(keyw, "PERC") == 0)
    {
      sscanf(values,"%f",&freq);
      continue;
    }
  }
}
