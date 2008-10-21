#include "GaudiUPI/DialogItem.h"
#include "RTL/strdef.h"
#include <cstring>

template<typename T> static inline void release(T*& x) { if ( x != 0 ) { delete x; x = 0; } }

static inline ClientData _cnv(int i)  { return (ClientData)(long)i; }
static inline ClientData _cnv(float f)  { return (ClientData)(long) *(int*)&f; }

DialogItem::DialogItem (const std::string& fmt,const std::string& text,const std::string& def,const std::string& lo,const std::string& hi,bool list_only)  {
  init(fmt,text,(const ClientData)&def,(const ClientData)&lo,(const ClientData)&hi,list_only);
}

DialogItem::DialogItem (const std::string& fmt,const std::string& text,int def,int lo,int hi,bool list_only)  {
  init(fmt,text,_cnv(def),_cnv(lo),_cnv(hi),list_only);
}

DialogItem::DialogItem (const std::string& fmt,const std::string& text,float def,float lo,float hi,bool list_only)  {
  init(fmt,text,_cnv(def),_cnv(lo),_cnv(hi),list_only);
}

void DialogItem::init (const std::string& fmt,const std::string& text,const ClientData def,
                       const ClientData lo,const ClientData hi,bool list_only)
{
  setText(text);
  m_list     = 0;
  m_dlgList  = 0;
  m_id       = 0;
  m_fmt      = fmt;
  m_listOnly = list_only;
  m_listSize = m_currSize = 0;
  m_low      = create(lo);
  m_high     = create(hi);
  m_def      = create(def);
}

DialogItem::~DialogItem() {
  deleteList();
}

bool DialogItem::isString() const   {
  if      ( strstr(m_fmt.c_str(),"s") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"S") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"a") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"A") > 0 ) 
    return 1;
  else
    return 0;
}

bool DialogItem::isReal() const {
  if      ( strstr(m_fmt.c_str(),"f") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"F") > 0 ) 
    return 1;
  else
    return 0;
}

bool DialogItem::isInteger() const    {
  if      ( strstr(m_fmt.c_str(),"d") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"D") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"i") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"I") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"x") > 0 ) 
    return 1;
  else if ( strstr(m_fmt.c_str(),"X") > 0 ) 
    return 1;
  else
    return 0;
}

bool DialogItem::isEqual (DialogItemContainer* it1,DialogItemContainer* it2)  {
  if      ( isString()  ) 
    return strncmp(it1->data()->_char,it2->data()->_char,sizeof(display_container)) == 0;
  else if ( isInteger() ) 
    return it1->data()->_int[0] == it2->data()->_int[0];
  else if ( isReal()   ) 
    return it1->data()->_float[0] == it2->data()->_float[0];
  return false;
}

ItemStatus DialogItem::addList (const void* item)    {
  // Check if this entry is already in the list:
  if ( find((void*)item) != 0 )      return ITEM_ERROR;

  DialogItemContainer *it = new DialogItemContainer;
  *it = create(item);
  // Check if list is big enough. If not allocate more storage
  if ( m_listSize + 1 > m_currSize )  {
    DialogItemContainer **new_list = new DialogItemContainer*[m_currSize + 32];
    for (size_t i = 0; i < m_currSize; i++ ) new_list[i] = m_list[i];
    release(m_list);
    release(m_dlgList);
    m_list      = new_list;
    m_currSize += 32;
    m_dlgList   = new ClientData[m_currSize];
  }
  m_list[m_listSize++] = it;
  return ITEM_SUCCESS;
}

ItemStatus DialogItem::removeList (const void* item)   {
  size_t i, j;
  for ( i = 0; i < m_listSize; i++ )  {
    bool found = 0;
    if ( isString() && strcmp(m_list[i]->data()->_char,(const char*)item) == 0 ) found = 1;
    else if ( isInteger() && m_list[i]->data()->_int[0]   == *(int*)item       ) found = 1;
    else if ( isReal()    && m_list[i]->data()->_float[0] == *(float*)item     ) found = 1;
    if ( found )    {
      for ( m_listSize--, j = i; j < m_listSize; j++ ) m_list[j] = m_list[j+1];
      return ITEM_SUCCESS;
    }
  }
  return ITEM_SUCCESS;
}

ItemStatus DialogItem::setCurrent (const void* v){
  if      ( isString()  )    {
    const char* val = (const char*)v;
    char *tar = m_def.data()->_char;
    int len = std::min(sizeof(display_container)-1,strlen(val));
    strncpy(tar,val,len);
    tar[len] = 0;
  }
  else if ( isInteger() )    {
    m_def.data()->_int[0]   = (long)v;
  }
  else if ( isReal()   )    {
    m_def.data()->_float[0] = *(float*)v;
  }
  else  {
    return ITEM_ERROR;
  }
  return ITEM_SUCCESS;
}

ClientData DialogItem::value (const DialogItemContainer* cont) const  {
  // ptr is non const, but the internal value has to be converted...
  DialogItemContainer* container = (DialogItemContainer*)cont;
  if      ( isString()  )      {
    char *ptr = (char*)container->data()->_char;
    size_t siz = sizeof(display_container) - 1;
    // First do some Cleanup before actually returning the pointer
    ::sscanf(m_fmt.c_str(),"%%%zdd",&siz);
    ptr[std::min(siz,strlen(ptr))] = 0;
    str_trim(ptr,ptr,&siz);
    return (ClientData)ptr;
  }
  else if ( isReal()   )  {
    unsigned int *ivalue = (unsigned int*)&container->data()->_float[0];
    long val = *ivalue;
    return (ClientData)val;
  }
  else if ( isInteger() )  
    return (ClientData)(long)container->data()->_int[0];
  else
    return (ClientData)(long)container->data()->_int[0];
}

ClientData DialogItem::buffer()  {
  return (ClientData)&m_def.data()->_char[0];
}

ClientData DialogItem::list()  {
  if ( m_listSize > 0 )  {
    sort();
    for ( size_t i=0; i < m_listSize; i++) 
      m_dlgList[i] = value(m_list[i]);
    return m_dlgList;
  }
  else  {
    return NULL;
  }
}

DialogItem& DialogItem::deleteList()  {
  for ( size_t i=0; i < m_listSize; i++ ) release(m_list[i]);
  release( m_dlgList );
  release( m_list );
  m_listSize = m_currSize = 0;
  return *this;
}

DialogItemContainer* DialogItem::find (const void* entry)  {
  DialogItemContainer it = create(entry);
  for ( size_t i = 0; i < m_listSize; i++ )  {
    if ( isEqual(&it, m_list[i]) ) return m_list[i];
  }
  return (DialogItemContainer*)ITEM_ERROR;
}

DialogItemContainer DialogItem::create (const void* item)   {
  DialogItemContainer it;
  if      ( isString()  )    {
    const std::string* val = (const std::string*)item;
    char *tar   = it.data()->_char;
    size_t len = std::min(sizeof(display_container)-1,val->length());
    strncpy(tar,val->c_str(),len);
    tar[len] = 0;
    return it;
  }
  else if ( isInteger() )  {
    it.data()->_int[0] = *(int*)&item;
  }
  else if ( isReal() )  {
    it.data()->_float[0] = *(float*)&item;
  }
  return it;
}

DialogItem::operator char*()  {
  static char v[64];
  if      ( isString()  )   {
    size_t siz = sizeof(m_def.data()->_char);
    str_trim(m_def.data()->_char,m_def.data()->_char,&siz);
    sprintf(v,m_def.data()->_char);
  }
  else if ( isInteger() )
    sprintf(v,"%d",m_def.data()->_int[0]);
  else if ( isReal() )
    sprintf(v,"%f",m_def.data()->_float[0]);
  return &v[0];
}

DialogItem::operator std::string()   {
  static char v[64];
  if      ( isString()  )   {
    size_t siz = sizeof(m_def.data()->_char);
    str_trim(m_def.data()->_char,m_def.data()->_char,&siz);
    sprintf(v,m_def.data()->_char);
  }
  else if ( isInteger() )
    sprintf(v,"%d",m_def.data()->_int[0]);
  else if ( isReal() )
    sprintf(v,"%f",m_def.data()->_float[0]);
  return &v[0];
}

DialogItem::operator int()  {
  int v = 0;
  if      ( isString()  ) {
    size_t siz = sizeof(m_def.data()->_char);
    str_trim(m_def.data()->_char,m_def.data()->_char,&siz);
    v = atoi(m_def.data()->_char);
  }
  else if ( isInteger() )
    v = m_def.data()->_int[0];
  else if ( isReal() )
    v = (int)m_def.data()->_float[0];
  else
    v = m_def.data()->_int[0];
  return v;
}

DialogItem::operator float()    {
  double v = 0.0;
  if      ( isString()  )    {
    size_t siz = sizeof(m_def.data()->_char);
    str_trim(m_def.data()->_char,m_def.data()->_char,&siz);
    v = atof(m_def.data()->_char);
  }
  else if ( isInteger() )
    v = (double)m_def.data()->_int[0];
  else if ( isReal() )
    v = m_def.data()->_float[0];
  else
    v = (double)m_def.data()->_int[0];
  return float(v);
}

ItemStatus DialogItem::mark (ClientData entry)   {
  DialogItemContainer it = create(entry);
  for ( size_t i = 0; i < m_listSize; i++ )  {
    if ( isEqual(&it, m_list[i]) )  {
      m_list[i]->mark();
      return ITEM_SUCCESS;
    }
  }
  return ITEM_ERROR;
}

ItemStatus DialogItem::unmark (ClientData entry)   {
  DialogItemContainer it = create(entry);
  for ( size_t i = 0; i < m_listSize; i++ )  {
    if ( isEqual(&it, m_list[i]) )   {
      m_list[i]->unMark();
      return ITEM_SUCCESS;
    }
  }
  return ITEM_ERROR;
}

ItemStatus DialogItem::markDelete()    {
  for ( size_t i = 0; i < m_listSize; i++ ) m_list[i]->mark();
  return ITEM_SUCCESS;
}

ItemStatus DialogItem::deleteMarked()  {
  size_t i;
Again:
  for (i=0; i < m_listSize; i++ )   {
    if ( m_list[i]->isMarked() )    {
      removeList(value(m_list[i]));
      break;
    }
  }
  if ( i >= m_listSize) return ITEM_SUCCESS;
  goto Again;
  return ITEM_SUCCESS;
}

DialogItem& DialogItem::sort()  {
  if ( isString() )
    qsort (m_list, m_listSize, sizeof(DialogItemContainer*), cmpString);
  else if ( isInteger() )
    qsort (m_list, m_listSize, sizeof(DialogItemContainer*), cmpInt);
  else if ( isReal() )
    qsort (m_list, m_listSize, sizeof(DialogItemContainer*), cmpFloat);
  return *this;
}

int DialogItem::cmpString (const void* a,const void* b)  {
  DialogItemContainer *d1 = *(DialogItemContainer**)a;
  DialogItemContainer *d2 = *(DialogItemContainer**)b;
  const char *c1 = d1->data()->_char;
  const char *c2 = d2->data()->_char;
  int value = strncmp (c1, c2, std::min(strlen(c1),strlen(c2)));
  return value;
}

int DialogItem::cmpInt (const void* a,const void* b)  {
  DialogItemContainer *d1 = *(DialogItemContainer**)a;
  DialogItemContainer *d2 = *(DialogItemContainer**)b;
  return d1->data()->_int[0] > d2->data()->_int[0];
}

int DialogItem::cmpFloat (const void* a,const void* b)  {
  DialogItemContainer *d1 = *(DialogItemContainer**)a;
  DialogItemContainer *d2 = *(DialogItemContainer**)b;
  return d1->data()->_float[0] > d2->data()->_float[0];
}

DialogItem& DialogItem::setText (const std::string& text)   {
  m_text = text;
  return *this;
}

bool DialogItem::operator== (const DialogItem& cmp) const  {
  return m_id == cmp.m_id;
}
