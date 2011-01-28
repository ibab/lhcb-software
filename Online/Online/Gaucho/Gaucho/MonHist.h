/*
 * MonHist.h
 *
 *  Created on: Mar 26, 2010
 *      Author: beat
 */

#ifndef MONHIST_H_
#define MONHIST_H_
#include "Gaucho/MonObj.h"
#include "MonTypes.h"
#include <string>
#include "GaudiKernel/IMessageSvc.h"
#include "TObject.h"
//#include "AIDA/IBaseHistogram.h"
class TObject;
class TAxis;
class CntrMgr;
namespace AIDA { class IBaseHistogram; }
class StatEntity;
class MonHist : public MonObj
{
private:
	MONTYPE m_type;
	TObject *m_rootobj;
	TObject *m_rootdeser;
	CntrMgr *m_cntrmgr;
	int m_xmitbuffersize;
	int m_hdrlen;
	int m_buffersize;
	char *m_name;
	int m_namelen;
	char *m_title;      /** Pointer to Histogram Title **/
	int m_titlen;
	int m_nx;           /** Number of x-bins **/
	double m_xmin;     /** Minimum x **/
	double m_xmax;     /** Maximum y **/
	int m_ny;           /* Number of y bins */
	double m_ymin;     /* Minimum y */
	double m_ymax;     /* Maximum y */
	int m_addoff;

	double *m_hentries;
	double *m_hsumw2;
	double *m_hsumw;
	double *m_hbinsumw2;
	int m_blocksize;
	TAxis *m_Xaxis;
	TAxis *m_Yaxis;
	char **m_Xlabels;
	char **m_Ylabels;
	int m_xlablen;
	int m_ylablen;
  void setup(IMessageSvc* msgSvc, const std::string& source, const AIDA::IBaseHistogram *aidahist);
  void setup(IMessageSvc* msgSvc);
  void setup(IMessageSvc* msgSvc, const std::string& source,const std::string& desc, const StatEntity *se);
  void cpyBinLabels(char *,char**, int);
  IMessageSvc* m_msgsvc;
public:
  MonHist();
  MonHist(IMessageSvc* msgSvc, const std::string& source, const AIDA::IBaseHistogram *aidahist);
  MonHist(IMessageSvc* msgSvc, const std::string& source, const std::string& desc,const StatEntity *se);
  MonHist(IMessageSvc* msgSvc, const std::string& source, CntrMgr *);
	~MonHist();
	int datasize(){return m_buffersize;};
	int xmitbuffersize();
	int hdrlen(){return m_hdrlen;};
	int serialize(void* &ptr);
	char *name(){return m_name;};
	void clear(void);
	int namelength(){return m_namelen+1;};
	int titlen(){return m_titlen+1;};
	int type(){return m_type;};
	void *cpyName(void*);
	void *cpytitle(void *ptr);
  int GetBinLabels(TAxis *ax, char ***labs);
  static void SetBinLabels(TAxis *ax, char *l);
	void makeCounters();
	void List();
	static void *de_serialize(void *ptr, char *nam =0);
	void resetup(void);
	void FreeDeser(){if (m_rootdeser != 0){delete m_rootdeser; m_rootdeser=0;};};
	int BlockSize(void){return m_blocksize;};

};

#endif /* MONHIST_H_ */
