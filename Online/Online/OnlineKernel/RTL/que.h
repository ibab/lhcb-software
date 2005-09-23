/*
** QUEUE ROUTINES prototypes
*/
#ifndef __QUE_H__
#define __QUE_H__
struct qentry {
	struct qentry *next;
	struct qentry *prev;
#ifdef __cplusplus
  qentry() : next(0), prev(0) {}
  qentry(qentry* n, qentry* p) : next(n), prev(p) {}
#endif
};
typedef qentry qentry_t;
#ifdef __cplusplus
extern "C" {
#endif
int insqti (qentry_t *qent, qentry_t *head);
int insqhi (qentry_t *qent, qentry_t *head);
int remqhi (qentry_t *head, qentry_t **qent);
int remqti (qentry_t *head, qentry_t **qent);
#ifdef __cplusplus
}
#endif
#define QUE_SUCCESS   0
#define QUE_ONEENTQUE   2
#define QUE_QUEISEMPTY  3
#define QUE_I_SUCCESS   0
#define QUE_I_ONEENTQUE 2
#define QUE_I_QUEWASEMP 3
#endif
