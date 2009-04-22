#ifndef GAUCHO_MONBUFFEROBJECT_H
#define GAUCHO_MONBUFFEROBJECT_H 1

#include "Gaucho/MonBuffer.h"
#include "TObject.h"
#include "TH1D.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

/** @class ClassName MonBufferObject.h 
 *
 *  MonBufferObject class
 *
 *  @author Juan Otalora
 *  @date 16/02/2009
 */

class MonBufferObject: public MonBuffer {
protected:
  TObject* m_object;

public:

  MonBufferObject(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonBufferObject();

  virtual void save(TBufferFile* buffer, const unsigned int version);
  virtual void load(TBufferFile* buffer, const unsigned int version);

  void setObject(TObject* tObject);
  TObject* object();
  std::string objectName(){return m_object->GetName();};
  std::string objectTitle(){return m_object->GetTitle();};

  void setAidaObject(AIDA::IHistogram1D* iHistogram1D);
  void setAidaObject(AIDA::IHistogram2D* iHistogram2D);
  void setAidaObject(AIDA::IProfile1D* iProfile1D);

  virtual void combine(MonBuffer * m2);
  virtual void copyFrom(MonBuffer* m2);
  virtual void print();
  virtual void write();
  virtual void reset();

};

#endif //GAUCHO_MONBUFFEROBJECT_H

