/*
 * ra-dav-test.c :  basic test program for the RA/DAV library
 *
 * ================================================================
 * Copyright (c) 2000 CollabNet.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. The end-user documentation included with the redistribution, if
 * any, must include the following acknowlegement: "This product includes
 * software developed by CollabNet (http://www.Collab.Net)."
 * Alternately, this acknowlegement may appear in the software itself, if
 * and wherever such third-party acknowlegements normally appear.
 * 
 * 4. The hosted project names must not be used to endorse or promote
 * products derived from this software without prior written
 * permission. For written permission, please contact info@collab.net.
 * 
 * 5. Products derived from this software may not use the "Tigris" name
 * nor may "Tigris" appear in their names without prior written
 * permission of CollabNet.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL COLLABNET OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by many
 * individuals on behalf of CollabNet.
 *
 * Portions of this software were originally written by Greg Stein as a
 * sourceXchange project sponsored by SapphireCreek.
 */



#include <apr_general.h>
#include <apr_pools.h>

#include "svn_delta.h"
#include "svn_ra.h"
#include "svn_wc.h"


int
main (int argc, char **argv)
{
  apr_pool_t *pool;
  svn_error_t *err;
  svn_ra_session_t *ras;
  const char *url;
  const char *dir;
  const svn_delta_edit_fns_t *editor;
  void *edit_baton;
  svn_string_t *repos;
  svn_string_t *anc_path;
  svn_revnum_t revision;

  apr_initialize ();
  pool = svn_pool_create (NULL);

  if (argc != 3)
    {
      fprintf (stderr, "usage: %s REPOSITORY_URL TARGET_DIR\n", argv[0]);
      return 1;
    }

  url = argv[1];
  dir = argv[2];        /* ### default to the last component of the URL */

  err = svn_ra_open(&ras, url, pool);
  if (err)
    {
      svn_handle_error (err, stdout, 0);
      return 1;
    }

  repos = svn_string_create(url, pool);

  /* ### what the heck does "ancestor path" mean for a checkout? */
  anc_path = svn_string_create("", pool);

  /* ### how can we know this before we start fetching crap? */
  revision = 1;

  err = svn_wc_get_checkout_editor(svn_string_create(dir, pool),
                                   repos, anc_path, revision,
                                   &editor, &edit_baton, pool);
  if (err)
    goto error;

  err = svn_ra_checkout(ras, "", 1, editor, edit_baton);
  if (err)
    goto error;

  err = (*editor->close_edit)(edit_baton);
  if (err)
    goto error;

  svn_ra_close(ras);

  apr_destroy_pool(pool);
  apr_terminate();

  return 0;

 error:
  svn_handle_error (err, stdout, 0);
  return 1;
}





/* -----------------------------------------------------------------
 * local variables:
 * eval: (load-file "../../svn-dev.el")
 * end:
 */
