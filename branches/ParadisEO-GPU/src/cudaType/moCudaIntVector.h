/*
 <moCudaIntVector.h>
 Copyright (C) DOLPHIN Project-Team, INRIA Lille - Nord Europe, 2006-2010

 Karima Boufaras, Thé Van LUONG

 This software is governed by the CeCILL license under French law and
 abiding by the rules of distribution of free software.  You can  use,
 modify and/ or redistribute the software under the terms of the CeCILL
 license as circulated by CEA, CNRS and INRIA at the following URL
 "http://www.cecill.info".

 As a counterpart to the access to the source code and  rights to copy,
 modify and redistribute granted by the license, users are provided only
 with a limited warranty  and the software's author,  the holder of the
 economic rights,  and the successive licensors  have only  limited liability.

 In this respect, the user's attention is drawn to the risks associated
 with loading,  using,  modifying and/or developing or reproducing the
 software by the user in light of its specific status of free software,
 that may mean  that it is complicated to manipulate,  and  that  also
 therefore means  that it is reserved for developers  and  experienced
 professionals having in-depth computer knowledge. Users are therefore
 encouraged to load and test the software's suitability as regards their
 requirements in conditions enabling the security of their systems and/or
 data to be ensured and,  more generally, to use and operate it in the
 same conditions as regards security.
 The fact that you are presently reading this means that you have had
 knowledge of the CeCILL license and that you accept its terms.

 ParadisEO WebSite : http://paradiseo.gforge.inria.fr
 Contact: paradiseo-help@lists.gforge.inria.fr
 */

#ifndef _moCudaIntVector_H_
#define _moCudaIntVector_H_

#include <cudaType/moCudaVector.h>

/**
 * Implementation of integer vector representation on CUDA.
 */

template<class Fitness>

class moCudaIntVector: public moCudaVector<int, Fitness> {

public:

	using moCudaVector<int, Fitness>::vect;
	using moCudaVector<int, Fitness>::N;

	/**
	 * Default constructor.
	 */

	moCudaIntVector() :
		moCudaVector<int, Fitness> () {

	}

	/**
	 *Constructor.
	 *@param _size The neighborhood size.
	 */

	moCudaIntVector(unsigned _size):
		moCudaVector<int, Fitness> (_size) {
		create();
	}

	/**
	 *Assignment operator
	 *@param _vector The vector passed to the function determine the new content.
	 *@return a new vector.
	 */

	moCudaIntVector& operator=(const moCudaIntVector & _vector) {
		moCudaVector<int, Fitness> :: operator=(_vector);
	}

	/**
	 *Initializer of random integer vector.
	 */
	void create() {

		unsigned random;
		int temp;
		for (unsigned i = 0; i < N; i++)
			vect[i] = i;
		// we want a random permutation so we shuffle
		for (unsigned i = 0; i < N; i++) {
			random = rng.rand() % (N - i) + i;
			temp = vect[i];
			vect[i] = vect[random];
			vect[random] = temp;
		}
	}

};

#endif