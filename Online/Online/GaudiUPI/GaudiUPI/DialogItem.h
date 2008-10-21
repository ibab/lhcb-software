// DECLARATIONS FOR OMT MODULE dialogitem
#ifndef _dialogitem_H
#define _dialogitem_H 1

#include <string>
typedef void* ClientData;

enum ItemStatus   {
  ITEM_SUCCESS = 1,
  ITEM_ERROR = 0
};
enum ItemTypes  {
  DIALOGITEM_ASCII,
  DIALOGITEM_FLOAT,
  DIALOGITEM_INTEGER
};


class DialogItem;
union display_container;
class DialogItemContainer;

union display_container   {
  char  _char[64];
  int   _int[16];
  float _float[16];
};

class DialogItemContainer
{
 private:
  bool m_mark;
 public:
   DialogItemContainer() : m_mark(0)  {                             }
  virtual ~DialogItemContainer()      {                             }
  display_container* data()           { return &m_data;             }
  void mark()                         { m_mark = 1;                 }
  void unMark()                       { m_mark=0;                   }
  bool isMarked() const               { return m_mark;              }
  char* ascii()                       { return m_data._char;        }
  int integer()                       { return m_data._int[0];      }
  float real()                        { return m_data._float[0];    }
 protected:
  display_container m_data;
};

class DialogItem   {
 private:
  ClientData    m_id;
  std::string   m_fmt;
  std::string   m_text;
  bool          m_listOnly;
  size_t        m_listSize;
  size_t        m_currSize;
  ClientData*   m_dlgList;

  void init ( const std::string& fmt,const std::string& text,
              const ClientData def,  const ClientData lo,
              const ClientData hi,bool list_only);
  bool isString()   const;
  bool isReal()     const;
  bool isInteger()  const;
  ClientData value (const DialogItemContainer* cont) const;

public:
  DialogItem (const std::string& fmt,const std::string& text,const std::string& def,const std::string& lo = "",const std::string& hi = "",bool list_only = 1);
  DialogItem (const std::string& fmt,const std::string& text,int def,int lo,int hi,bool list_only);
  DialogItem (const std::string& fmt,const std::string& text,float def,float lo,float hi,bool list_only);
  virtual ~DialogItem();
  DialogItem& deleteList();
  ItemStatus addList (const char* item)  {  return addList(std::string(item?item:"")); }
  ItemStatus addList (const std::string& it) {  return addList((void*)&it); }
  ItemStatus addList (int         item)  {  return addList(&item); }
  ItemStatus addList (float       item)  {  return addList(&item); }
  ItemStatus addList (const void* item);
  ItemStatus setCurrent (const void* value);
  ItemStatus removeList (const void* item);
  int length()                const {   return m_listSize;          }
  const std::string& format() const {   return m_fmt;               }
  const std::string& text()   const {   return m_text;              }
  bool useList()              const {   return m_listOnly;          }
  ClientData item(int num)    const {   return value(m_list[num]);  }
  ClientData current()        const {   return value(&m_def);       }
  ClientData min()            const {   return value(&m_low);       }
  ClientData max()            const {   return value(&m_high);      }
  ClientData buffer();
  ClientData list();
  bool isEqual (DialogItemContainer* it1,DialogItemContainer* it2);
  operator char*();
  operator std::string();
  operator int();
  operator float();
  DialogItemContainer* find (const void* entry);
  DialogItemContainer create (const void* item);
  ItemStatus mark (ClientData entry);
  ItemStatus unmark (ClientData entry);
  ItemStatus markDelete();
  ItemStatus deleteMarked();
  DialogItem& sort();
  static int cmpString (const void* a,const void* b);
  static int cmpInt (const void* a,const void* b);
  static int cmpFloat (const void* a,const void* b);
  DialogItem& setText (const std::string& text);
  DialogItem& setID (ClientData id) {m_id=id; return *this;}
  ClientData id() {return m_id;}
  bool operator== (const DialogItem& cmp) const;
  operator const std::string&();
 protected:
  DialogItemContainer** m_list;
  DialogItemContainer m_def;
  DialogItemContainer m_low;
  DialogItemContainer m_high;
};
#endif
