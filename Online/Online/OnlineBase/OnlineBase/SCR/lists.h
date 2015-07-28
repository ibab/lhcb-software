#ifndef LISTS_H
#define LISTS_H

/*---------------------  Linked lists general stuff   -----------------------*/

typedef struct LINKED_LIST Linked_list;
typedef struct LINK  Link;

#define Linked_list_items *first, *last
struct LINKED_LIST {
  Link Linked_list_items;
};

#define Link_items *prev, *next; Linked_list *father
struct LINK {
  Link        Link_items;
};

#ifdef __cplusplus
extern "C"  {
#endif
  /*---------------------------------------------------------------------------*/
#ifdef __LISTS_C__
  /*---------------------------------------------------------------------------*/
  void list_init (Linked_list*);
  Link* list_add_entry (Linked_list*,int);
  void list_connect_entry (Link*,Linked_list*);
  Link* list_insert_entry (Link*,Link*,int);
  void list_remove_entry (Link*);
  void list_move_entry (Link*,Link*,Link*);
  void list_disconnect_entry (Link*);
  void list_transfer_entry (Link*,Linked_list*,Link*,Link*);
  void list_transfer (Linked_list*,Linked_list*);
  void list_convert (Linked_list*);
  Link* list_first (Linked_list*);
  Link* list_last (Linked_list*);
  Link* list_prev (Link*);
  Link* list_next (Link*);
  Link* list_father (Link*);
  char* list_malloc (int);

  void listr_init (Linked_list*);
  Link* listr_add_entry (Linked_list*,int);
  void listr_connect_entry (Link*,Linked_list*);
  Link* listr_insert_entry (Link*,Link*,int);
  void listr_remove_entry (Link*);
  void listr_move_entry (Link*,Link*,Link*);
  void listr_disconnect_entry (Link*);
  void listr_transfer_entry (Link*,Linked_list*,Link*,Link*);
  void listr_transfer (Linked_list*,Linked_list*);
  void listr_convert (Linked_list*);
  Link* listr_first (Linked_list*);
  Link* listr_last (Linked_list*);
  Link* listr_prev (Link*);
  Link* listr_next (Link*);
  Linked_list* listr_father (Link*);
  void listr_set_first (Linked_list* list, Link* entry);
  void listr_set_last (Linked_list* list, Link* entry);
  void listr_set_next (Link* entry, Link* next);
  void listr_set_prev (Link* entry, Link* next);
  void listr_set_father (Link* entry, Linked_list* next);
  char* listr_malloc (int);

#else

  void list_init (void* _list);
  Link* list_add_entry (void* _list, int _size);
  void list_connect_entry (void* _entry,  void* _list);
  Link* list_insert_entry (void* _before, void* _after, int _size);
  void list_remove_entry ( void* _entry );
  void list_move_entry (void*, void*, void*);
  void list_disconnect_entry ();
  void list_transfer_entry (void*, void*, void*,void*);
  void list_transfer (void*, void*);
  void list_convert ();
  Link* list_first ();
  Link* list_last ();
  Link* list_prev ();
  Link* list_next ();
  Link* list_father ();
  char* list_malloc (unsigned long);

  void listr_init (void*);
  void listr_connect_entry (void*,void*);
  void listr_convert (void*);

#endif

#ifdef __cplusplus
}
#endif
/*---------------------------------------------------------------------------*/
#endif
