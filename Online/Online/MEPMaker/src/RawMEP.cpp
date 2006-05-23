//	====================================================================
//  RawMEP.cpp
//	--------------------------------------------------------------------
//
//	Package   : RawMEP
//
//	Author    : Niko Neufeld
//
//	====================================================================

// Framework include files
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <stdio.h>
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "RawMEP.h"

// Event Model related classes
#include "Event/RawBuffer.h"
#include "Event/RawEvent.h"


#include "GaudiKernel/System.h"
#include <sys/types.h>

static const AlgFactory<RawMEP>  Factory;
const IFactory& RawMEPFactory = Factory;

typedef struct {
  union {
    u_int32_t word1;
    struct {
      u_int32_t magic: 16;
      u_int32_t size: 16;
    } w1;
  } bits1;
  union {
    u_int32_t word2;
    struct {
      u_int32_t bankType: 8;
      u_int32_t version: 8;
      u_int32_t srcid: 16;
    } w2;
  } bits2;
} raw_hdr_t __attribute__((packed));
	 
enum BankType{ L0Calo=0,
               L0DU,
               PrsE,
               EcalE,
               HcalE,
               PrsTrig,
               EcalTrig,
               HcalTrig,
               Velo,
               Rich,
               TT,
               IT,
               OT,
               Muon,
               L0PU,
               DAQ,
               ODIN, 
               HLT };


int
RawMEP::inactivetell1(int tell1id) {
    switch (tell1id) {
    case 1: return 1; /*  L0DU */
    case 247: return 1; /* DAQ 2*/
    }
    return 0;
}
     
int
RawMEP::rawtell1id(int type, int src)
{

  switch (type) {
  case RawBuffer::L0Calo:     return 0;
  case RawBuffer::L0DU:     return 1;
  case RawBuffer:: PrsE:     
      if (src < 8) return 2 + src;
      else *m_log << MSG::ERROR << "Severe: illegal src in PRsE " << src << endmsg;
      break;
  case RawBuffer:: EcalE:    return 10 + src;
  case RawBuffer:: HcalE:    return 20 + src;
  case RawBuffer:: PrsTrig:   return 2 + src;
  case RawBuffer:: EcalTrig: return 10 + src;
  case RawBuffer:: HcalTrig: return 20 + src;
  case RawBuffer:: Velo:     
      if ((src >= 42 && src < 64) || src > 105) {
	  *m_log << MSG::ERROR << "Severe: illegal src in Velo " << src << endmsg;
	  break;
      }
      return 24 + ((src < 42) ? src : (src - 64 + 42));
  case RawBuffer:: Rich:     return 108 + src;
  case RawBuffer:: TT:       
    if (src < 16) return 119 + src;
    if (src < 32) return 119 + 11 + (src - 16);
    if (src < 48) return 119 + 22 + (src - 32);
    if (src < 64) return 119 + 35 + (src - 48);
    *m_log << MSG::ERROR << "Severe: illegal src in TT " << src << endmsg;
    break;
  case RawBuffer:: IT:       
    if (src < 16) return 119 + 48 + src;
    if (src < 32) return 119 + 48 + 14 + (src - 16);
    if (src < 64) return 119 + 48 + 28 + (src - 32);
    *m_log << MSG::ERROR << "Severe: illegal src in IT " << src << endmsg; 
    break;
  case RawBuffer:: OT:       return 119 + 90 + (src - 1);
  case RawBuffer:: Muon:     return 209 + 24 + ((src > 99 ) ?  src - 100 : src);
  case RawBuffer:: L0PU:     return 209 + 24 + 12 + src;
  case RawBuffer:: DAQ:      return 209 + 36 + 1 + (src - 1);
  case RawBuffer:: ODIN:     return 209 + 36 + 2 + src;
  case RawBuffer:: HLT:      return 209 + 36 + 2 + 1 + src;
  }
  return -1;
}

static const int id2tpsrc[249 * 2] = {
0,0,
-1,-1,
2,0,
2,1,
2,2,
2,3,
2,4,
2,5,
2,6,
2,7,
3,0,
3,1,
3,2,
3,3,
3,4,
3,5,
3,6,
3,7,
3,8,
3,9,
4,0,
4,1,
4,2,
4,3,
8,0,
8,1,
8,2,
8,3,
8,4,
8,5,
8,6,
8,7,
8,8,
8,9,
8,10,
8,11,
8,12,
8,13,
8,14,
8,15,
8,16,
8,17,
8,18,
8,19,
8,20,
8,21,
8,22,
8,23,
8,24,
8,25,
8,26,
8,27,
8,28,
8,29,
8,30,
8,31,
8,32,
8,33,
8,34,
8,35,
8,36,
8,37,
8,38,
8,39,
8,40,
8,41,
8,64,
8,65,
8,66,
8,67,
8,68,
8,69,
8,70,
8,71,
8,72,
8,73,
8,74,
8,75,
8,76,
8,77,
8,78,
8,79,
8,80,
8,81,
8,82,
8,83,
8,84,
8,85,
8,86,
8,87,
8,88,
8,89,
8,90,
8,91,
8,92,
8,93,
8,94,
8,95,
8,96,
8,97,
8,98,
8,99,
8,100,
8,101,
8,102,
8,103,
8,104,
8,105,
9,0,
9,1,
9,2,
9,3,
9,4,
9,5,
9,6,
9,7,
9,8,
9,9,
9,10,
10,0,
10,1,
10,2,
10,3,
10,4,
10,5,
10,6,
10,7,
10,8,
10,9,
10,10,
10,16,
10,17,
10,18,
10,19,
10,20,
10,21,
10,22,
10,23,
10,24,
10,25,
10,26,
10,32,
10,33,
10,34,
10,35,
10,36,
10,37,
10,38,
10,39,
10,40,
10,41,
10,42,
10,43,
10,44,
10,48,
10,49,
10,50,
10,51,
10,52,
10,53,
10,54,
10,55,
10,56,
10,57,
10,58,
10,59,
10,60,
11,0,
11,1,
11,2,
11,3,
11,4,
11,5,
11,6,
11,7,
11,8,
11,9,
11,10,
11,11,
11,12,
11,13,
11,16,
11,17,
11,18,
11,19,
11,20,
11,21,
11,22,
11,23,
11,24,
11,25,
11,26,
11,27,
11,28,
11,29,
11,32,
11,33,
11,34,
11,35,
11,36,
11,37,
11,38,
11,39,
11,40,
11,41,
11,42,
11,43,
11,44,
11,45,
12,1,
12,2,
12,3,
12,4,
12,5,
12,6,
12,7,
12,8,
12,9,
12,10,
12,11,
12,12,
12,13,
12,14,
12,15,
12,16,
12,17,
12,18,
12,19,
12,20,
12,21,
12,22,
12,23,
12,24,
13,0,
13,101,
13,2,
13,103,
13,4,
13,105,
13,6,
13,7,
13,108,
13,9,
13,110,
13,11,
14,0,
15,1,  /* DAQ */
-1,-1, /* ODIN */
17,0}; /* HLT */

int rawtyp2det[18] = {       0,      1,      2,      3,      4,      2, 
                             3,      4,      5,      6,      7,      8, 
                             9,     10,      11,     12,    13,     14};

char *rawtypstr[18] = { "L0CA", "L0DU", "PRSE", "ECAE", "HCAE", "PRTR",
                        "ECTR", "HCTR", "VELO", "RICH",   "TT",   "IT", 
                          "OT", "MUON", "L0PU",  "DAQ",  "TFC",  "HLT" };
int  rawmaxsrc[] =  {        1,      1,      8,     10,      4,     84,   
                             48,    11,     24,     24,      1,      1,      
                             1,      1};   

char *rawsubdetstr[15] = { "L0CA", "L0DU", "PRSH", "ECAL", "HCAL", "VELO", 
                           "RICH",   "TT",   "IT",   "OT", "MUON", "L0PU",  
                           "DAQ",  "TFC",   "HLT"};


/* map bank-types to TELL1s */


typedef union {
  u_int32_t word;
  struct {
    u_int16_t evtid;
    u_int16_t len;
  } part;
} efg_t;


static u_int8_t frgbuf[300][0x10000];
static int bankpresent[300];

RawMEP::RawMEP(const std::string& name, ISvcLocator* pSvcLocator)  
  : Algorithm(name, pSvcLocator) { 
    memset(m_rawfd, 0, 300 * sizeof(void *));
    memset(m_rawhh, 0, 300 * sizeof(void *));
    memset(m_fgwritten, 0, sizeof(long long) * 300);
    memset(m_missbank,  0, sizeof(long) * 300);
    declareProperty("RAWPFX", m_rawpfx = "");
    declareProperty("RAWHPFX", m_rawhpfx = "raw");
    declareProperty("RAWNTELL1", m_rawntell1 = 249);
    declareProperty("MarkusFile", m_rawbufout = "");
    m_rmout = -1;
    m_nomep = 0;
}

StatusCode RawMEP::initialize() {
    m_log = new MsgStream(msgSvc(), name());
    if (m_rawbufout.compare("")) {
	if ((m_rmout = open(m_rawbufout.c_str(), O_RDWR | O_CREAT | O_TRUNC, 
			    S_IRWXU | S_IROTH | S_IRGRP)) == -1) {
	    *m_log << MSG::ERROR << "open: " << sys_errlist[errno] << endmsg;
	    return StatusCode::FAILURE;
	}
    }
    if (!m_rawpfx.compare("")) {
	m_nomep = 1;
	return StatusCode::SUCCESS;
    }
    for (int i = 0; i < m_rawntell1; ++i) {
	if (inactivetell1(i)) continue;
	int type, src;
	char srcstr[10];
	type = id2tpsrc[2 * i]; src = id2tpsrc[2 * i + 1];
	sprintf(srcstr, "%03d", src);
	std::string filename = m_rawpfx + "-" + 
	    rawsubdetstr[rawtyp2det[type]] + "-" + srcstr + ".mep"; 
	*m_log << MSG::INFO << filename << " TELL1 " << i << endmsg;	
	if (!(m_rawfd[i] = fopen(filename.c_str(), "w"))) {
	    *m_log << MSG::ERROR  << "fopen: " << sys_errlist[errno] << 
		endmsg;
	    return StatusCode::FAILURE;
	}
    }	    
    return StatusCode::SUCCESS;
}

StatusCode
RawMEP::writeraw(raw_int *b, int n)
{
    static int eventid = 0;
    static int first = 1;
    char srcstr[12];
    int len, tell1id;
    int wlen = 0;
    int nbytes = sizeof(raw_int) * n;

    for (int i = 0; i < 300; ++i) {
	efg_t *fgh = (efg_t *) &frgbuf[i][0];
	fgh->part.len = 0;
    }
    if (m_rmout != -1) wlen = write(m_rmout, &nbytes, 4);
    for (int i = 0; i < n; i+= len) {
	raw_hdr_t *hdr = (raw_hdr_t *) &b[i];
	int type, src;
	
	type = hdr->bits2.w2.bankType;
	src  = hdr->bits2.w2.srcid;
	len  = hdr->bits1.w1.size;
	if ((tell1id = rawtell1id(type, src)) == -1) {
	    return StatusCode::FAILURE;
	}
//	std::cout << "GGG " << tell1id << " " << type << " " << rawsubdetstr[rawtyp2det[type]] << " " << src << std::endl;
	bankpresent[tell1id]++;
	efg_t *fgh = (efg_t *) &frgbuf[tell1id][0];
	if (len == 0) {
	    *m_log << MSG::ERROR << "Severe error 0-size frag" << endmsg;
	    return StatusCode::FAILURE;
	}
	sprintf(srcstr, "%03d", src);
	if (!m_rawfd[tell1id]) {
	    std::string filename = m_rawpfx + "-" + 
		rawsubdetstr[rawtyp2det[type]] + "-" + srcstr + ".mep"; 
	    *m_log << MSG::INFO << filename << " TELL1 " << tell1id << endmsg;	
	    if (!(m_rawfd[tell1id] = fopen(filename.c_str(), "w"))) {
		*m_log << MSG::ERROR  << "fopen: " << sys_errlist[errno] << 
		    endmsg;
		return StatusCode::FAILURE;
	    }
	}
#if 1
	if (m_rawhh[tell1id] == NULL) {
	    std::string histname = m_rawhpfx + "-" + 
		rawsubdetstr[rawtyp2det[type]] + "-" + srcstr;
	    *m_log << MSG::DEBUG <<  "histoname: " << histname << endmsg;
	    if ((m_rawhh[tell1id] = histoSvc()->book(histname, histname, 512, 
						     0.0, 8191.0)) == NULL) {
		*m_log << MSG::ERROR << "booking histogram" << endmsg;
		return StatusCode::FAILURE;
	    } 
	}
#endif
	int nb = sizeof(raw_int) * len;
	memcpy(&frgbuf[tell1id][4 + fgh->part.len], &b[i], nb);
	fgh->part.len += nb;
	fgh->part.evtid = eventid;
    }
    
    unsigned int total = 0;
    for (int i = 0; i < m_rawntell1; ++i) {
	if (inactivetell1(i)) {
	    if (first) 
		*m_log << MSG::INFO << "Ignoring inactive TELL1 " << i << endmsg;
	    continue;
	}
	if (!bankpresent[i]) {
	    if (++m_missbank[i] < 6)
		*m_log << MSG::WARNING << "Missing bank from TELL1: " << 
		    i << endmsg;
	    if (m_missbank[i] == 5) 
		*m_log << MSG::WARNING << 
		    "This message Will be supressed from now on" << endmsg;
	}
	efg_t *fgh = (efg_t *) &frgbuf[i][0];
	total += fgh->part.len;
	if (!m_nomep) {
	    if (fwrite(&frgbuf[i][0], sizeof(efg_t) + fgh->part.len, 1, 
		       m_rawfd[i]) != 1) {
		*m_log << MSG::ERROR << "write: " << sys_errlist[errno] << 
		    endmsg;
		return StatusCode::FAILURE;
	    }
	    m_fgwritten[i]++;
	}
	if (m_rmout != -1) 
	    wlen += write(m_rmout, &frgbuf[i][4], fgh->part.len);
//	    *m_log << MSG::DEBUG << "wrote event " << eventid << endmsg;
	if (m_rawhh[i]) m_rawhh[i]->fill((double) fgh->part.len);	    
    }  
    if (n * sizeof(raw_int) != total) { /* should not happen :-) */
	*m_log << MSG::ERROR << "mep len " << total << "!= raw len" << 
	    n * sizeof(raw_int) << endmsg;
	return StatusCode::FAILURE;
    }
    if (m_rmout != -1 && wlen != (n *sizeof(raw_int) + 4)) {
	*m_log << MSG::ERROR << "raw write len mismatch!" << endmsg;
	return StatusCode::FAILURE;
    }
    ++eventid;
    first = 0;
    return StatusCode::SUCCESS;     
}
//--------------------------------------------------------------------
// Execute
//--------------------------------------------------------------------
StatusCode RawMEP::execute() {
  SmartDataPtr<RawBuffer> raw(eventSvc(), RawBufferLocation::Default);

  if ( raw != 0 )  {
    StatusCode sc = writeraw(raw->buffer(), raw->currentSize());
    return sc;
  }
  *m_log << MSG::ERROR << "Unable to retrieve Event object" << endreq;
  return StatusCode::FAILURE;
}

StatusCode
RawMEP::finalize() 
{
    unsigned long long written;
    int locked = 0, err = 0;
    
    *m_log << MSG::INFO << "TELL1:    MEPs written Missing banks" << endmsg;
    for (int i = 0; i < m_rawntell1; ++i) {
	if (m_rawfd[i]) fclose(m_rawfd[i]);       	
	*m_log << MSG::INFO << i << " " << m_fgwritten[i] << "   " << 
	    m_missbank[i];
	if (inactivetell1(i)) *m_log << " INACTIVE ";
	else if (!locked) {
	    locked = 1;
	    written = m_fgwritten[i];
	} else if (!m_nomep && written != m_fgwritten[i]) err = 1;	   
	*m_log	<< endmsg;
    }
    if (err) *m_log << MSG::ERROR << "SEVERE: mismatch in number of written MEP fragments!"
		    << endmsg;
    return StatusCode::SUCCESS;
}
