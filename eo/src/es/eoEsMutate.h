// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-
 
//-----------------------------------------------------------------------------
// eoESMute.h : ES mutation
// (c) Maarten Keijzer 2000 & GeNeura Team, 1998 for the EO part
//     Th. Baeck 1994 and EEAAX 1999 for the ES part
/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Contact: todos@geneura.ugr.es, http://geneura.ugr.es
             marc.schoenauer@polytechnique.fr 
                       http://eeaax.cmap.polytchnique.fr/
 */
//-----------------------------------------------------------------------------


#ifndef _EOESMUTATE_H
#define _EOESMUTATE_H

#include <utils/eoRNG.h>
#include <eoRnd.h>
#include <cmath>		// for exp

#include <es/eoEsMutationInit.h>
#include <es/eoEsSimple.h>
#include <es/eoEsStdev.h>
#include <es/eoEsFull.h>
#include <es/eoEsObjectiveBounds.h>

#include <eoOp.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/** 

\ingroup EvolutionStrategies

  ES-style mutation in the large: Obviously, valid only for eoES*

  It is currently valid for three types of ES chromosomes:

  eoEsSimple:   only 1 std deviation
  eoEsStdev:    as many standard deviations as object variables
  eoEsFull:     The whole guacemole: correlations, stdevs and object variables

  Each of these three variant has it's own operator() in eoEsMutate and intialization
  is also split into three cases (that share some commonalities)
*/

template <class EOT>
class eoEsMutate: public eoMonOp< EOT > {
public:

    typedef typename EOT::Fitness FitT;

    /** Initialization
        parameters:

        @param _init    proxy class for initializating the three parameters eoEsMutate needs
        @param _bounds  the bounds for the objective variables
    */
    eoEsMutate(eoEsMutationInit& _init, eoEsObjectiveBounds& _bounds) : bounds(_bounds)
    {
        init(EOT(), _init); // initialize on actual type used
    }

    /// needed virtual dtor
    virtual ~eoEsMutate() {};
    
  /** Inherited from eoObject 
      @see eoObject
  */
  virtual string className() const {return "eoESMutate";};
  
  /**
    Mutate eoEsSimple
  */
  virtual void operator()( eoEsSimple<FitT>& _eo) const
  {
      _eo.stdev *= exp(TauLcl * rng.normal());	

      if (_eo.stdev < stdev_eps)
	        _eo.stdev = stdev_eps;
      
      // now apply to all

      for (unsigned i = 0; i < _eo.size(); ++i)
      {
          _eo[i] += _eo.stdev * rng.normal();
      }

      keepInBounds(_eo);
  }
  
  /// mutations - standard and correlated
  //  ========= 
  /*
   *	Standard mutation of object variables and standard 	
   *	deviations in ESs. 
   *	If there are fewer different standard deviations available 
   *	than the dimension of the objective function requires, the 
   * 	last standard deviation is responsible for ALL remaining
   *	object variables.
   *	Schwefel 1977: Numerische Optimierung von Computer-Modellen
   *	mittels der Evolutionsstrategie, pp. 165 ff.
   */

  virtual void operator()( eoEsStdev<FitT>& _eo ) const 
  {
    double global = exp(TauGlb * rng.normal());
    for (unsigned i = 0; i < _eo.size(); i++) 
      {
	    double stdev = _eo.stdevs[i];
	    stdev *= global * exp(TauLcl * rng.normal());	

	    if (stdev < stdev_eps)
	        stdev = stdev_eps;

	    _eo.stdevs[i] = stdev; 
	    _eo[i] += stdev * rng.normal();
      }

    keepInBounds(_eo);
  }

  /*
   *	Correlated mutations in ESs, according to the following
   *	sources:
   *	H.-P. Schwefel: Internal Report of KFA Juelich, KFA-STE-IB-3/80
   *	p. 43, 1980
   *	G. Rudolph: Globale Optimierung mit parallelen Evolutions-
   *	strategien, Diploma Thesis, University of Dortmund, 1990
   */
  
  // Code from Thomas Baeck 
  
  virtual void operator()( eoEsFull<FitT> & _eo ) const 
  {

    /*
     *	First: mutate standard deviations (as above).
     */
    
    double global = exp(TauGlb * rng.normal());
    unsigned i;
    for (i = 0; i < _eo.size(); i++) 
    {
	    double stdev = _eo.stdevs[i];
	    stdev *= global * exp(TauLcl * rng.normal());	

	    if (stdev < stdev_eps)
	        stdev = stdev_eps;

	    _eo.stdevs[i] = stdev; 
    }

    
    /*
     *	Mutate rotation angles.
     */
    
    for (i = 0; i < _eo.correlations.size(); i++) 
    {
      _eo.correlations[i] += TauBeta * rng.normal(); 
      if ( fabs(_eo.correlations[i]) > M_PI ) 
      {
	    _eo.correlations[i] -= M_PI * (int) (_eo.correlations[i]/M_PI) ;
      }
    }
    
    /*
     *	Perform correlated mutations.
     */
    unsigned k, n1, n2;
    double d1,d2, S, C;

    vector<double> VarStp(_eo.size());
    for (i = 0; i < _eo.size(); i++) 
      VarStp[i] = _eo.stdevs[i] * rng.normal();

    unsigned nq = _eo.correlations.size() - 1;

    for (k = 0; k < _eo.size()-1; k++) 
    {
      n1 = _eo.size() - k - 1;
      n2 = _eo.size() - 1;
      
      for (i = 0; i < k; i++) 
      {
	    d1 = VarStp[n1];
	    d2 = VarStp[n2];
	    S  = sin( _eo.correlations[nq] );
	    C  = cos( _eo.correlations[nq] );
	    VarStp[n2] = d1 * S + d2 * C;
	    VarStp[n1] = d1 * C - d2 * S;
	    n2--;
	    nq--;
      }
    }
    
    for (i = 0; i < _eo.size(); i++) 
      _eo[i] += VarStp[i];

    keepInBounds(_eo);
  }

  void keepInBounds(eoEsBase<FitT>& _eo) const
  {
      for (unsigned i = 0; i < _eo.size(); ++i)
      {
          if (_eo[i] < bounds.minimum(i))
              _eo[i] = bounds.minimum(i);
          else if (_eo[i] > bounds.maximum(i))
              _eo[i] = bounds.maximum(i);
      }
  }

  private :

    void init(eoEsSimple<FitT>, eoEsMutationInit& _init)
    {
        unsigned size = bounds.chromSize();
        TauLcl = _init.TauLcl();
        TauLcl /= sqrt((double) size);
    }

    void init(eoEsStdev<FitT>, eoEsMutationInit& _init)
    {
        unsigned size = bounds.chromSize();

        TauLcl = _init.TauLcl();
        TauGlb = _init.TauGlb();

        // renormalization
	    TauLcl /= sqrt( 2.0 * sqrt( (double)size ) );
	    TauGlb /= sqrt( 2.0 * ( (double) size ) );
    }
    
    void init(eoEsFull<FitT>, eoEsMutationInit& _init)
    {
        init(eoEsStdev<FitT>(), _init);
        TauBeta = _init.TauBeta();
    }

   // the data 
  //=========
  double TauLcl;	/* Local factor for mutation of std deviations */
  double TauGlb;	/* Global factor for mutation of std deviations */
  double TauBeta;	/* Factor for mutation of correlation parameters  */

  eoEsObjectiveBounds& bounds;

  static const double stdev_eps;
};

template <class EOT>
const double eoEsMutate<EOT>::stdev_eps = 1.0e-40;

/*
 *	Correlated mutations in ESs, according to the following
 *	sources:
 *	H.-P. Schwefel: Internal Report of KFA Juelich, KFA-STE-IB-3/80
 *	p. 43, 1980
 *	G. Rudolph: Globale Optimierung mit parallelen Evolutions-
 *	strategien, Diploma Thesis, University of Dortmund, 1990
 */

#endif

