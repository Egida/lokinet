#ifndef LLARP_NODEDB_H
#define LLARP_NODEDB_H
#include <llarp/common.h>
#include <llarp/crypto.h>
#include <llarp/router_contact.h>

/**
 * nodedb.h
 *
 * persistent storage API for router contacts
 */

#ifdef __cplusplus
extern "C" {
#endif

struct llarp_nodedb;

/** create an empty nodedb */
struct llarp_nodedb *
llarp_nodedb_new(struct llarp_crypto *crypto);

/** free a nodedb and all loaded rc */
void
llarp_nodedb_free(struct llarp_nodedb **n);

/** ensure a nodedb fs skiplist structure is at dir
    create if not there.
 */
bool
llarp_nodedb_ensure_dir(const char *dir);

/** load entire nodedb from fs skiplist at dir */
ssize_t
llarp_nodedb_load_dir(struct llarp_nodedb *n, const char *dir);

/** store entire nodedb to fs skiplist at dir */
ssize_t
llarp_nodedb_store_dir(struct llarp_nodedb *n, const char *dir);

struct llarp_nodedb_iter
{
  void *user;
  struct llarp_rc *rc;
  bool (*visit)(struct llarp_nodedb_iter *);
};

/**
   iterate over all loaded rc with an iterator
 */
void
llarp_nodedb_iterate_all(struct llarp_nodedb *n, struct llarp_nodedb_iter i);

/**
    find rc by rc.k being value k
    returns true if found otherwise returns false
 */
bool
llarp_nodedb_find_rc(struct llarp_nodedb *n, struct llarp_rc *rc,
                     llarp_pubkey_t k);

/**
   return true if we have a rc with rc.k of value k on disk
   otherwise return false
 */
bool
llarp_nodedb_has_rc(struct llarp_nodedb *n, llarp_pubkey_t k);

/**
   put an rc into the node db
   overwrites with new contents if already present
   flushes the single entry to disk
   returns true on success and false on error
 */
bool
llarp_nodedb_put_rc(struct llarp_nodedb *n, struct llarp_rc *rc);

// defined in nodedb.cpp
/*
struct llarp_async_verify_job_context {
    struct llarp_nodedb *nodedb;
    struct llarp_logic *logic;
    struct llarp_crypto *crypto;
    struct llarp_threadpool *cryptoworker;
    struct llarp_threadpool *diskworker;
};
*/

/**
    struct for async rc verification
*/
struct llarp_async_verify_rc;

struct llarp_async_verify_job_context; // forward definition (defined in nodedb.cpp)

typedef void (*llarp_async_verify_rc_hook_func)(struct llarp_async_verify_rc *);

/// verify rc request
struct llarp_async_verify_rc
{
  /// user pointers
  void *user;
  /// context
  llarp_async_verify_job_context *context;
  /// router contact (should this be a pointer?)
  struct llarp_rc rc;
  /// result
  bool valid;
  /// hook
  llarp_async_verify_rc_hook_func hook;
};

/**
    struct for async rc verification
    data is loaded in disk io threadpool
    crypto is done on the crypto worker threadpool
    result is called on the logic thread
*/
void
llarp_nodedb_async_verify(struct llarp_nodedb *nodedb,
                          struct llarp_logic *logic,
                          struct llarp_crypto *crypto,
                          struct llarp_threadpool *cryptoworker,
                          struct llarp_threadpool *diskworker,
                          struct llarp_async_verify_rc *job);

#ifdef __cplusplus
}
#endif
#endif
