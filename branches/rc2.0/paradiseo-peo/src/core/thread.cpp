/*
* <thread.cpp>
* Copyright (C) DOLPHIN Project-Team, INRIA Futurs, 2006-2008
* (C) OPAC Team, LIFL, 2002-2008
*
* Sebastien Cahon, Alexandru-Adrian Tantar, Clive Canape
*
* This software is governed by the CeCILL license under French law and
* abiding by the rules of distribution of free software.  You can  use,
* modify and/ or redistribute the software under the terms of the CeCILL
* license as circulated by CEA, CNRS and INRIA at the following URL
* "http://www.cecill.info".
*
* As a counterpart to the access to the source code and  rights to copy,
* modify and redistribute granted by the license, users are provided only
* with a limited warranty  and the software's author,  the holder of the
* economic rights,  and the successive licensors  have only  limited liability.
*
* In this respect, the user's attention is drawn to the risks associated
* with loading,  using,  modifying and/or developing or reproducing the
* software by the user in light of its specific status of free software,
* that may mean  that it is complicated to manipulate,  and  that  also
* therefore means  that it is reserved for developers  and  experienced
* professionals having in-depth computer knowledge. Users are therefore
* encouraged to load and test the software's suitability as regards their
* requirements in conditions enabling the security of their systems and/or
* data to be ensured and,  more generally, to use and operate it in the
* same conditions as regards security.
* The fact that you are presently reading this means that you have had
* knowledge of the CeCILL license and that you accept its terms.
*
* ParadisEO WebSite : http://paradiseo.gforge.inria.fr
* Contact: paradiseo-help@lists.gforge.inria.fr
*
*/

#include <map>

#include "thread.h"

static std :: vector <Thread *> threads;

unsigned num_act = 0;


Thread :: Thread ()
{

  threads.push_back (this);
  act = false;
}

Thread :: ~ Thread ()
{

  /* Nothing ! */
}

void Thread :: setActive ()
{

  if (! act)
    {

      act = true;
      num_act ++;
    }
}

void Thread :: setPassive ()
{

  if (act)
    {

      act = false;
      num_act --;
    }
}

void initThreadsEnv ()
{

  threads.clear ();
  num_act = 0;
}

bool atLeastOneActiveThread ()
{

  return num_act;
}

static void * launch (void * __arg)
{

  Thread * thr = (Thread *) __arg;
  thr -> start ();

  return 0;
}

void addThread (Thread * __hl_thread, std :: vector <pthread_t *> & __ll_threads)
{

  pthread_t * ll_thr = new pthread_t;
  __ll_threads.push_back (ll_thr);
  pthread_create (ll_thr, 0, launch, __hl_thread);
}

void joinThreads (std :: vector <pthread_t *> & __threads)
{

  for (unsigned i = 0; i < __threads.size (); i ++)
    {
      pthread_join (* __threads [i], 0);
      delete __threads [i];
    }
  __threads.clear();
}
