// $Id: OnlineEvtSelector.h,v 1.4 2006-01-11 12:43:29 jost Exp $
//====================================================================
//	OnlineEvtSelector.h
//--------------------------------------------------------------------
//
//	Package    : GaudiOnline
//
//  Description: The OnlineEvtSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria".
//
//  Author     : M.Frank
//  Created    : 4/01/99
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/OnlineEvtSelector.h,v 1.4 2006-01-11 12:43:29 jost Exp $

#ifndef GAUDIONLINE_EVTSELECTOR_H
#define GAUDIONLINE_EVTSELECTOR_H 1

// Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/DataObject.h"
#include "OnlineKernel/OnlineKernel/MBM/Consumer.h"

/*
 *  GaudiOnline namespace declaration
 */
namespace GaudiOnline  {

  /** @class GaudiOnline::EvtSelector
  */
  class BMRequirement
  {
  public:
	  int evtype;
	  int trmask[4];
	  int vetomask[4];
	  int UserType;
	  int FreqType;
	  int MaskType;
	  float freq;
  private:
	  std::string reqstring;
  public:
	  BMRequirement (std::string s)
	{
		reqstring = s;
	}
	void parse(void)
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
					MaskType = BM_MASK_ANY;
				}
				else
				{
					MaskType = BM_MASK_ALL;
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
					UserType = BM_REQ_USER;
				}
				else if (strcmp(values,"VIP") == 0)
				{
					UserType = BM_REQ_VIP;
				}
				else
				{
					UserType = BM_REQ_ONE;
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
					FreqType = BM_FREQ_MANY;
				}
				else
				{
					FreqType = BM_FREQ_PERC;
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
  };


	class EvtSelector : public Service, virtual public IEvtSelector  {
  public:
    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    /// IService implementation: Service finalization
    virtual StatusCode finalize();

    // IInterface implementation: query interfaces
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const;

    /** Access last item in the iteration
      * @param refContext [IN/OUT] Reference to the Context object.
      */
    virtual StatusCode last(Context& refContext) const;

    /// Get next iteration item from the event loop context
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode next(Context& refCtxt) const;

    /// Get next iteration item from the event loop context, but skip jump elements
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode next(Context& refCtxt,int jump) const;

    /// Get previous iteration item from the event loop context
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * @param jump      [IN]      Number of events to be skipped
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode previous(Context& refCtxt) const;

    /// Get previous iteration item from the event loop context, but skip jump elements
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * @param jump      [IN]      Number of events to be skipped
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode previous(Context& refCtxt,int jump) const;

    /// Rewind the dataset
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode rewind(Context& refCtxt) const;

    /// Create new Opaque address corresponding to the current record
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createAddress(const Context& refCtxt, IOpaqueAddress*&) const;

    /// Release existing event iteration context
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode releaseContext(Context*& refCtxt) const;

    /** Will set a new criteria for the selection of the next list of events and will change
        the state of the context in a way to point to the new list.
        
        @param cr The new criteria string.
        @param c  Reference pointer to the Context object.
    */
    virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;

    /// Service Constructor
    EvtSelector( const std::string& name, ISvcLocator* svcloc );

    /// Standard destructor
    virtual ~EvtSelector();

  protected:
    // Data Members
    CLID                          m_rootCLID;
    std::string                   m_cnvSvcName;
    std::string                   m_rootName;
    std::string                   m_buffLen;
    std::string                   m_dataTypeName;
    int                           m_dataType;
    /// Number of circular data buffers
    int                           m_numBuffers;
	MBM::Consumer*                m_MyConsumer;
    std::string                   m_MyUTGID;
    std::string                   m_Rqs[8];
    std::string                   m_BMIDs;
    std::string                   m_PIDs;
	BMRequirement*				  m_Reqs[8];
	int							  m_nreqs;
  };
}
#endif  // GAUDIONLINE_EVTSELECTOR_H
