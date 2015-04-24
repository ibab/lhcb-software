// $Header:  $
//  ====================================================================
//  RoutingBitsPrescaler.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: $
// Framework include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "MDF/OnlineRunInfo.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "RTL/rtl.h"
#include "MBM/Requirement.h"
#include "MBM/bmdef.h"
#include "MDF/RawDataAddress.h"
#include "MDF/MDFHeader.h"

// C/C++ include files
#include <cstdlib>
#include <vector>
#include <ctime>

/*
 *  LHCb namespace declaration
 */
namespace LHCb
{
  using namespace MBM;
  using namespace std;
  /**@class RoutingBitsPrescaler RoutingBitsPrescaler.cpp
  *
  * Allows to prescale events given a certain trigger type mask.
  * If the mask is exactly ZERO, it is ignored. Otherwise only
  * a configurable fraction of events, with a non-zero and mask
  * with the ODIN triggertype pass.
  *
  *
  * @author:  M.Frank
  * @version: 1.0
  */
  class myRequirement: public MBM::Requirement
  {
    public:
      string m_Name;
      long m_evCounter;
      myRequirement() : MBM::Requirement()
      {
        m_evCounter = 0;
      }
      void parse(const string &reqstr)
      {
        char *items[20];
        int ikey = 0;
        char rstr[1024];
        ::strcpy(rstr, reqstr.c_str());
        char* token = ::strtok(rstr, ";");
        while (token != NULL)
        {
          items[ikey++] = token;
          token = ::strtok(NULL, ";");
        }
        // Requirement format:
        // "NAME=xyz;EvType=x;TriggerMask=0xfeedbabe,0xdeadfeed,0xdeadbabe,0xdeadaffe;
        //  VetoMask=0x,0x,0x,0x;MaskType=ANY/ALL;UserType=USER/VIP/ONE;
        //  Frequency=MANY/PERC;Perc=20.5"
        for (int i = 0; i < ikey; i++)
        {
          char* keyw = ::strtok(items[i], "=");
          if (!keyw)
            break;
          for (unsigned int j = 0; j < strlen(keyw); j++)
          {
            keyw[j] = char(::toupper(keyw[j]));
          }
          char* values = ::strtok(NULL, "=");
          if (::strcmp(keyw, "NAME") == 0)
          {
            m_Name = values;
            continue;
          }
          if (::strcmp(keyw, "EVTYPE") == 0)
          {
            ::sscanf(values, "%d", &evtype);
            continue;
          }
          if (::strcmp(keyw, "TRIGGERMASK") == 0)
          {
            char *v = strtok(values, ",");
            ::memset(trmask, 0, sizeof(trmask));
            for (int j = 0; v != 0 && j < 4; j++)
            {
              ::sscanf(v, "%x", &trmask[j]);
              v = ::strtok(NULL, ",");
            }
            continue;
          }
          if (::strcmp(keyw, "VETOMASK") == 0)
          {
            ::memset(vetomask, 0, sizeof(vetomask));
            char *v = strtok(values, ",");
            for (int j = 0; v != 0 && j < 4; j++)
            {
              ::sscanf(v, "%x", &vetomask[j]);
              v = strtok(NULL, ",");
            }
          }
          if (::strcmp(keyw, "MASKTYPE") == 0)
          {
            for (unsigned int j = 0; j < strlen(values); j++)
            {
              values[j] = char(::toupper(values[j]));
            }
            if (::strcmp(values, "ANY") == 0)
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
            for (unsigned int j = 0; j < strlen(values); j++)
            {
              values[j] = char(::toupper(values[j]));
            }
            if (strcmp(values, "USER") == 0)
            {
              userType = BM_REQ_USER;
            }
            else if (strcmp(values, "VIP") == 0)
            {
              userType = BM_REQ_VIP;
            }
            else if (strcmp(values, "ALL") == 0)
            {
              userType = BM_REQ_ALL;
            }
            else
            {
              int tmp = *(int*) values;
              tmp = tmp >> 8;
              userType = (tmp << 8) + BM_REQ_ONE;
            }
            continue;
          }
          if (strcmp(keyw, "FREQUENCY") == 0)
          {
            for (unsigned int j = 0; j < strlen(values); j++)
            {
              values[j] = char(::toupper(values[j]));
            }
            if (strcmp(values, "MANY") == 0)
              freqType = BM_FREQ_MANY;
            else
              freqType = BM_FREQ_PERC;
            continue;
          }
          if (strcmp(keyw, "PERC") == 0)
          {
            sscanf(values, "%f", &freq);
            freq /= 100.0;
            continue;
          }
        }
      }
  };
  typedef vector<string> Requirements;
  typedef vector<myRequirement> BinRequirements;
  class GAUDI_API RoutingBitsPrescaler: public Algorithm
  {

    public:
      /// Standard Algorithm Constructor(s)
      Requirements m_req;
      BinRequirements m_breq;
      RoutingBitsPrescaler(const std::string& nam, ISvcLocator* svc) :
          Algorithm(nam, svc)
      {
        unsigned int seed = InterfaceID::hash32(
            (RTL::processName() + "@" + RTL::nodeName()).c_str());
        ::srand(seed);
        declareProperty("Requirements", m_req, "Requirements List");
      }

      /// Default Destructor
      virtual ~RoutingBitsPrescaler()
      {
      }

      /// Algorithm overload: Initialize the algorithm
      virtual StatusCode initialize()
      {
        Algorithm::initialize();
        for (auto i = m_req.begin(); i != m_req.end(); i++)
        {
          myRequirement r;
          r.parse(*i);
          m_breq.insert(m_breq.end(), r);
        }
        for (auto i = m_breq.begin(); i != m_breq.end(); i++)
        {
          declareInfo((*i).m_Name,(*i).m_evCounter,"Accepted Events");
        }
        return StatusCode::SUCCESS;
      }

      /// Algorithm overload: Finalize the algorithm
      virtual StatusCode finalize()
      {
        return StatusCode::SUCCESS;
      }

      bool matchTrMask(myRequirement *r, const unsigned int *trm)
      {
        bool tmatch, vmatch;
        tmatch = false;
        vmatch = false;
        for (int i = 0; i < 4; i++)
        {
          tmatch = tmatch | ((r->trmask[i] & trm[i]) != 0);
          vmatch = vmatch | ((r->vetomask[i] & trm[i]) != 0);
        }
        if (!tmatch || vmatch)
        {
          return false;
        }
        return true;
      }

      /// Algorithm overload: Event execution routine
      virtual StatusCode execute()
      {
        std::vector<int>::const_iterator vi;
        MsgStream log(msgSvc(), name());
        DataObject* pDO = 0;
        double frac = double(rand());

        frac /= double(RAND_MAX);
        StatusCode sc = eventSvc()->retrieveObject("/Event", pDO);
        if (sc.isSuccess())
        {
          RawDataAddress *add = (RawDataAddress*) (pDO->registry()->address());
          pair<char*, int> data = add->data();
          RawBank *b = (RawBank*) data.first;
          MDFHeader *MDFHd = (MDFHeader *) b->data();
          const unsigned int *trm = MDFHd->subHeader().H1->triggerMask();
          bool trmok = false;
          for (auto it = m_breq.begin(); it != m_breq.end(); it++)
          {
            myRequirement *r = &(*it);
            trmok = trmok || matchTrMask(r, trm);
            if (trmok)
            {
              if (r->freq >= frac)
              {
                r->m_evCounter++;
                setFilterPassed(true);
                break;
              }
              else
              {
                trmok = false;
              }
            }
          }
          if (!trmok)
          {
            setFilterPassed(false);
          }
        }
        return StatusCode::SUCCESS;
      }
  };
}

#include "GaudiKernel/AlgFactory.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb, RoutingBitsPrescaler)
