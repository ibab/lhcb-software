#include "Gaucho/MonBuffer.h"

MonBuffer::MonBuffer(IMessageSvc* msgSvc, const std::string& source, int version)
{
  m_msgSvc = msgSvc;
  m_source = source;
  m_typeName = s_monBuffer;
  m_dimPrefix = s_pfixMonBuffer;
  m_comments = "";
  m_endOfRun = false;
  m_serviceActive = true;
}

MonBuffer::~MonBuffer(){

}

void MonBuffer::save(TBufferFile* buffer, const unsigned int version){
  if (version != m_version) m_version = version;
//   buffer->WriteString(m_typeName);
//   buffer->WriteUInt(m_version);
//   buffer->WriteString(m_comments);
//   buffer->WriteBool(m_endOfRun);
}

void MonBuffer::load(TBufferFile* buffer, const unsigned int version){
  if (version != m_version) m_version = version;
/*   buffer->ReadTString(m_typeName);
   buffer->ReadUInt(m_version);
   buffer->ReadTString(m_comments);
   buffer->ReadBool(m_endOfRun);*/
}

void MonBuffer::combine(MonBuffer* monBuffer) {
  if (monBuffer==NULL) return; // to hide warning
}

void MonBuffer::subtract(){
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "MonBuffer::subtract";
  
}
void MonBuffer::print(){
  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO << " type:" << typeName()<<endreq;
  msgStream <<MSG::INFO << " version:"<< version()<<endreq;
  msgStream <<MSG::INFO << " comments:"<< comments()<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  
}

void MonBuffer::copyFrom(MonBuffer* monBuffer){
  if (monBuffer==NULL) return;
}

void MonBuffer::reset(){
  return;
}

MsgStream MonBuffer::createMsgStream(){
  MsgStream msg(m_msgSvc, m_source + ":" + m_typeName);
  return msg;
}

