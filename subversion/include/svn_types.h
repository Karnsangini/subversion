/**
 * @copyright
 * ====================================================================
 * Copyright (c) 2000-2004 CollabNet.  All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at http://subversion.tigris.org/license-1.html.
 * If newer versions of this license are posted there, you may use a
 * newer version instead, at your option.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://subversion.tigris.org/.
 * ====================================================================
 * @endcopyright
 *
 * @file svn_types.h
 * @brief Subversion's data types
 */

#ifndef SVN_TYPES_H
#define SVN_TYPES_H

/* ### this should go away, but it causes too much breakage right now */
#include <stdlib.h>

#include <apr.h>        /* for apr_size_t */
#include <apr_pools.h>
#include <apr_hash.h>
#include <apr_tables.h>
#include <apr_time.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/** Subversion error object.
 *
 * Defined here, rather than in svn_error.h, to avoid a recursive #include 
 * situation.
 */
typedef struct svn_error_t
{
  /** APR error value, possibly SVN_ custom err */
  apr_status_t apr_err;

  /** details from producer of error */
  const char *message;

  /** ptr to the error we "wrap" */
  struct svn_error_t *child;

  /** The pool holding this error and any child errors it wraps */
  apr_pool_t *pool;

  /** Source file where the error originated.  Only used iff @c SVN_DEBUG. */
  const char *file;

  /** Source line where the error originated.  Only used iff @c SVN_DEBUG. */
  long line;

} svn_error_t;



/** index into an apr_array_header_t */
#define APR_ARRAY_IDX(ary,i,type) (((type *)(ary)->elts)[i])

/** easier array-pushing syntax */
#define APR_ARRAY_PUSH(ary,type) (*((type *)apr_array_push (ary)))


/** The various types of nodes in the Subversion filesystem. */
typedef enum
{
  /* absent */
  svn_node_none,

  /* regular file */
  svn_node_file,

  /* directory */
  svn_node_dir,

  /* something's here, but we don't know what */
  svn_node_unknown
} svn_node_kind_t;

/** About Special Files in Subversion
 *
 * Subversion denotes files that cannot be portably created or
 * modified as special files (svn_node_special).  It stores these
 * files in the repository as a plain text file with the svn:special
 * property set.  The file contents contain: a platform-specific type
 * string, a space character, then any information necessary to create
 * the file on a supported platform.  For example, if a symbolic link
 * were being represented, the repository file would have the
 * following contents:
 *
 * "link /path/to/link/target"
 *
 * Where 'link' is the identifier string showing that this special
 * file should be a symbolic link and '/path/to/link/target' is the
 * destination of the symbolic link.
 *
 * Special files are stored in the text-base exactly as they are
 * stored in the repository.  The platform specific files are created
 * in the working copy at EOL/keyword translation time using
 * svn_subst_copy_and_translate2.  If the current platform does not
 * support a specific special file type, the file is copied into the
 * working copy as it is seen in the repository.  Because of this,
 * users of other platforms can still view and modify the special
 * files, even if they do not have their unique properties.
 *
 * New types of special files can be added by:
 *  1. Implementing a platform-dependent routine to create a uniquely
 *     named special file and one to read the special file in
 *     libsvn_subr/io.c.
 *  2. Creating a new textual name similar to
 *     SVN_SUBST__SPECIAL_LINK_STR in libsvn_subr/subst.c.
 *  3. Handling the translation/detranslation case for the new type in
 *     create_special_file and detranslate_special_file, using the
 *     routines from 1.
 */

/** A revision number. */
typedef long int svn_revnum_t;

/** Valid revision numbers begin at 0 */
#define SVN_IS_VALID_REVNUM(n) ((n) >= 0)

/** The 'official' invalid revision num */
#define SVN_INVALID_REVNUM ((svn_revnum_t) -1)

/** Not really invalid...just unimportant -- one day, this can be its
 * own unique value, for now, just make it the same as
 * @c SVN_INVALID_REVNUM.
 */
#define SVN_IGNORED_REVNUM ((svn_revnum_t) -1) 

/** Convert null-terminated C string @a str to a revision number. */
#define SVN_STR_TO_REV(str) ((svn_revnum_t) atol(str))

/** In @c printf()-style functions, format revision numbers using this. */
#define SVN_REVNUM_T_FMT "ld"


/** The size of a file in the Subversion FS. */
typedef apr_int64_t svn_filesize_t;

/** The 'official' invalid file size constant. */
#define SVN_INVALID_FILESIZE ((svn_filesize_t) -1)

/** In @c printf()-style functions, format file sizes using this. */
#define SVN_FILESIZE_T_FMT APR_INT64_T_FMT

/* Parse a base-10 numeric string into a 64-bit unsigned numeric value. */
/* NOTE: Private. For use by Subversion's own code only. See issue #1644. */
/* FIXME: APR should supply a function to do this, such as "apr_atoui64". */
#define svn__atoui64(X) ((apr_uint64_t) apr_atoi64(X))


/** YABT:  Yet Another Boolean Type */
typedef int svn_boolean_t;

#ifndef TRUE
/** uhh... true */
#define TRUE 1
#endif /* TRUE */

#ifndef FALSE
/** uhh... false */
#define FALSE 0
#endif /* FALSE */


/** An enum to indicate whether recursion is needed. */
enum svn_recurse_kind
{
  svn_nonrecursive = 1,
  svn_recursive
};


/** A general subversion directory entry. */
typedef struct svn_dirent_t
{
  /** node kind */
  svn_node_kind_t kind;

  /** length of file text, or 0 for directories */
  svn_filesize_t size;

  /** does the node have props? */
  svn_boolean_t has_props;

  /** last rev in which this node changed */
  svn_revnum_t created_rev;

  /** time of created_rev (mod-time) */
  apr_time_t time;

  /** author of created_rev */
  const char *last_author;

} svn_dirent_t;




/** Keyword substitution.
 *
 * All the keywords Subversion recognizes.
 * 
 * Note that there is a better, more general proposal out there, which
 * would take care of both internationalization issues and custom
 * keywords (e.g., $NetBSD$).  See
 * 
 *<pre>    http://subversion.tigris.org/servlets/ReadMsg?list=dev&msgNo=8921
 *    =====
 *    From: "Jonathan M. Manning" <jmanning@alisa-jon.net>
 *    To: dev@subversion.tigris.org
 *    Date: Fri, 14 Dec 2001 11:56:54 -0500
 *    Message-ID: <87970000.1008349014@bdldevel.bl.bdx.com>
 *    Subject: Re: keywords</pre>
 *
 * and Eric Gillespie's support of same:
 *
 *<pre>    http://subversion.tigris.org/servlets/ReadMsg?list=dev&msgNo=8757
 *    =====
 *    From: "Eric Gillespie, Jr." <epg@pretzelnet.org>
 *    To: dev@subversion.tigris.org
 *    Date: Wed, 12 Dec 2001 09:48:42 -0500
 *    Message-ID: <87k7vsebp1.fsf@vger.pretzelnet.org>
 *    Subject: Re: Customizable Keywords</pre>
 *
 * However, it is considerably more complex than the scheme below.
 * For now we're going with simplicity, hopefully the more general
 * solution can be done post-1.0.
 *
 * @defgroup svn_types_keywords keywords
 * @{
 */

/** The maximum size of an expanded or un-expanded keyword. */
#define SVN_KEYWORD_MAX_LEN    255

/** The most recent revision in which this file was changed. */
#define SVN_KEYWORD_REVISION_LONG    "LastChangedRevision"

/** Short version of LastChangedRevision */
#define SVN_KEYWORD_REVISION_SHORT   "Rev"

/** Medium version of LastChangedRevision, matching the one CVS uses */
#define SVN_KEYWORD_REVISION_MEDIUM  "Revision"

/** The most recent date (repository time) when this file was changed. */
#define SVN_KEYWORD_DATE_LONG        "LastChangedDate"

/** Short version of LastChangedDate */
#define SVN_KEYWORD_DATE_SHORT       "Date"

/** Who most recently committed to this file. */
#define SVN_KEYWORD_AUTHOR_LONG      "LastChangedBy"

/** Short version of LastChangedBy */
#define SVN_KEYWORD_AUTHOR_SHORT     "Author"

/** The URL for the head revision of this file. */
#define SVN_KEYWORD_URL_LONG         "HeadURL"

/** Short version of HeadURL */
#define SVN_KEYWORD_URL_SHORT        "URL"

/** A compressed combination of the other four keywords.
 *
 * (But see comments above about a more general solution to keyword
 * combinations.)
 */
#define SVN_KEYWORD_ID               "Id"

/** @} */


/** A structure to represent a path that changed for a log entry. */
typedef struct svn_log_changed_path_t
{
  /** 'A'dd, 'D'elete, 'R'eplace, 'M'odify */
  char action;

  /** Source path of copy (if any). */
  const char *copyfrom_path;

  /** Source revision of copy (if any). */
  svn_revnum_t copyfrom_rev;

} svn_log_changed_path_t;


/** The callback invoked by log message loopers, such as
 * @c svn_ra_plugin_t.get_log() and @c svn_repos_get_logs().
 *
 * This function is invoked once on each log message, in the order
 * determined by the caller (see above-mentioned functions).
 *
 * @a baton, @a revision, @a author, @a date, and @a message are what you 
 * think they are.  Any of @a author, @a date, or @a message may be @c NULL.
 *
 * If @a date is neither null nor the empty string, it was generated by
 * @c svn_time_to_string() and can be converted to @c apr_time_t with
 * @c svn_time_from_string().
 *
 * If @a changed_paths is non-@c NULL, then it contains as keys every path
 * committed in @a revision; the values are (@c svn_log_changed_path_t *) 
 * structures (see above).
 *
 * ### The only reason @a changed_paths is not qualified with `const' is
 * that we usually want to loop over it, and @c apr_hash_first() doesn't
 * take a const hash, for various reasons.  I'm not sure that those
 * "various reasons" are actually even relevant anymore, and if
 * they're not, it might be nice to change @c apr_hash_first() so
 * read-only uses of hashes can be protected via the type system.
 *
 * Use @a pool for all allocation.  (If the caller is iterating over log
 * messages, invoking this receiver on each, we recommend the standard
 * pool loop recipe: create a subpool, pass it as @a pool to each call,
 * clear it after each iteration, destroy it after the loop is done.)
 */
typedef svn_error_t *(*svn_log_message_receiver_t)
     (void *baton,
      apr_hash_t *changed_paths,
      svn_revnum_t revision,
      const char *author,
      const char *date,  /* use svn_time_from_string() if need apr_time_t */
      const char *message,
      apr_pool_t *pool);


/** Callback function type for commits.
 *
 * When a commit succeeds, an instance of this is invoked on the @a
 * new_revision, @a date, and @a author of the commit, along with the
 * @a baton closure.
 */
typedef svn_error_t * (*svn_commit_callback_t) (
    svn_revnum_t new_revision,
    const char *date,
    const char *author,
    void *baton);


/** The maximum amount we (ideally) hold in memory at a time when
 * processing a stream of data.
 *
 * For example, when copying data from one stream to another, do it in
 * blocks of this size.
 */
#define SVN_STREAM_CHUNK_SIZE 102400

/** The maximum amount we can ever hold in memory. */
/* FIXME: Should this be the same as SVN_STREAM_CHUNK_SIZE? */
#define SVN_MAX_OBJECT_SIZE (((apr_size_t) -1) / 2)



/* ### Note: despite being about mime-TYPES, these probably don't
 * ### belong in svn_types.h.  However, no other header is more
 * ### appropriate, and didn't feel like creating svn_validate.h for
 * ### so little.
 */

/** Validate @a mime_type.
 *
 * If @a mime_type does not contain a "/", or ends with non-alphanumeric
 * data, return @c SVN_ERR_BAD_MIME_TYPE, else return success.
 * 
 * Use @a pool only to find error allocation.
 *
 * Goal: to match both "foo/bar" and "foo/bar; charset=blah", without
 * being too strict about it, but to disallow mime types that have
 * quotes, newlines, or other garbage on the end, such as might be
 * unsafe in an HTTP header.
 */
svn_error_t *svn_mime_type_validate (const char *mime_type,
                                     apr_pool_t *pool);


/** Return false iff @a mime_type is a textual type.
 *
 * All mime types that start with "text/" are textual, plus some special 
 * cases (for example, "image/x-xbitmap").
 */
svn_boolean_t svn_mime_type_is_binary (const char *mime_type);



/** A user defined callback that subversion will call with a user defined 
 * baton to see if the current operation should be continued.  If the operation 
 * should continue, the function should return @c SVN_NO_ERROR, if not, it 
 * should return @c SVN_ERR_CANCELLED.
 */
typedef svn_error_t *(*svn_cancel_func_t) (void *cancel_baton);



/* @since New in 1.2 */

/** A lock object, for client & server to share.
 *
 * A lock represents the exclusive right to add, delete, or modify a
 * path.  A lock is created in a repository, wholly controlled by the
 * repository.  A "lock-token" is the lock's UUID, and can be used to
 * learn more about a lock's fields, and or/make use of the lock.
 * Because a lock is immutable, a client is free to not only cache the
 * lock-token, but the lock's fields too, for convenience.
 *
 * ### Note: in the current implementation, only files are lockable.
 */
typedef struct svn_lock_t
{
  const char *path;             /* the path this lock applies to */
  const char *token;            /* UUID of the lock */
  const char *owner;            /* the username which owns the lock */
  const char *comment;          /* (optional) description of lock  */
  apr_time_t creation_date;     /* when lock was made */
  apr_time_t expiration_date;   /* (optional) when lock will expire;
                                   If value is 0, lock will never expire. */
} svn_lock_t;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SVN_TYPES_H */
