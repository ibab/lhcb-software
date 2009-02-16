// $Id: OnlineHistoOnPage.h,v 1.1 2009-02-16 10:37:42 ggiacomo Exp $
#ifndef ONLINEHISTDB_ONLINEHISTOONPAGE_H 
#define ONLINEHISTDB_ONLINEHISTOONPAGE_H 1

#include "OnlineHistDB/OnlineHistogram.h"

/** @class OnlineHistoOnPage OnlineHistoOnPage.h OnlineHistDB/OnlineHistoOnPage.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2009-02-06
 */

class OnlineHistoOnPage 
  {
  public:
    OnlineHistoOnPage(OnlineHistogram* h) :
      histo(h), xmin(0), ymin(0), xmax(1), ymax(1),
      m_mother(NULL), m_ovIx(0)
    { instance=h->instance();}
    virtual ~OnlineHistoOnPage() {}
    /// set pad size (min and max x and y coordinates relative to canvas size)
    inline void setPadSize(float x1, float y1, float x2, float y2) {
      xmin=x1; ymin=y1; xmax=x2; ymax=y2;
    }
    /// declare that histogram must be overlapped on histogram m with an optional order index
    inline void setOverlap(OnlineHistoOnPage* m, int order=1) {
      m_mother=m;
      xmin=m->xmin; ymin=m->ymin; xmax=m->xmax; ymax=m->ymax;
      m_ovIx = order;
    }
    /// get overlap directions
    inline OnlineHistoOnPage* getOverlap(int *order=NULL) {
      if (order)
        *order=m_ovIx;
      return m_mother;
    }
    /// unset overlap
    inline void unsetOverlap() {
      m_mother=NULL;
      m_ovIx=0;
    }
    /// true if histogram must be overlapped
    inline bool isOverlap() {
      return (NULL != m_mother);
    }
    inline void save(std::string& Page) {
      histo->setPage(Page, instance);
    }

    OnlineHistogram* histo;
    unsigned int instance;
    float xmin;
    float ymin;
    float xmax;
    float ymax;
  private:
    OnlineHistoOnPage* m_mother;  
    int m_ovIx;
};

#endif // ONLINEHISTDB_ONLINEHISTOONPAGE_H
