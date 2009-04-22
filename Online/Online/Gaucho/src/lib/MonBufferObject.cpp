#include "Gaucho/MonBufferObject.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>
#include <GaudiUtils/HistoTableFormat.h>

MonBufferObject::MonBufferObject(IMessageSvc* msgSvc, const std::string& source, int version):
MonBuffer(msgSvc, source, version)
{
  m_typeName  = s_monBufferObject;
  m_dimPrefix = s_pfixMonBufferObject;
  m_object = 0;
}

MonBufferObject::~MonBufferObject(){
}

void MonBufferObject::setAidaObject(AIDA::IHistogram1D* iHistogram1D){
  setObject((TObject*)Gaudi::Utils::Aida2ROOT::aida2root(iHistogram1D));
}
void MonBufferObject::setAidaObject(AIDA::IHistogram2D* iHistogram2D){
  setObject((TObject*)Gaudi::Utils::Aida2ROOT::aida2root(iHistogram2D));
}
void MonBufferObject::setAidaObject(AIDA::IProfile1D* iProfile1D){
  setObject((TObject*)Gaudi::Utils::Aida2ROOT::aida2root(iProfile1D));
}

void MonBufferObject::setObject(TObject* object){
  m_object = object;
//  m_name = Gaudi::Utils::Histos::path(iHistogram1D);
//  m_object->SetName(m_name.c_str());
}

void MonBufferObject::save(TBufferFile* buffer, const unsigned int version){
  MonBuffer::save(buffer, version);
  if (m_object == 0) return;
  buffer->WriteObject(m_object);
}

void MonBufferObject::load(TBufferFile* buffer, const unsigned int version){
  MonBuffer::load(buffer, version);
  m_object = (TObject*)buffer->ReadObjectAny(TObject::Class());
}

TObject* MonBufferObject::object(){  
  return m_object;
}

void MonBufferObject::write(){
  if (m_object == 0) return;
  // No caso de ser H1D
 ((TH1D*)m_object)->Write();
}


void MonBufferObject::combine(MonBuffer * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
  if (H->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }

  if (m_object == 0) {
    copyFrom(H);
    return;
  }

  // add the two histos
  MonBufferObject *HH = (MonBufferObject*)H;
  // No caso de ser H1D
 ((TH1D*)m_object)->Add((TH1D*)HH->object());
}

void MonBufferObject::copyFrom(MonBuffer * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
  MonBufferObject *HH = (MonBufferObject*)H;
  m_endOfRun = HH->endOfRun();
  m_comments = HH->comments();
  m_object = HH->object()->Clone();
}

void MonBufferObject::reset(){
  if (m_object == 0) return;
  // No caso de ser H1D
 ((TH1D*)m_object)->Reset();

}

void MonBufferObject::print(){
  MonBuffer::print();
}

