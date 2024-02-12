/* LSystem.i */
// Name of the module should match the project name
%module LSystem

// Includes so SWIG knows how to handle some of the types in the C++ standard library.
%include "carrays.i"
%include "std_vector.i"
%include "std_string.i"
%include <typemaps.i>

%apply const std::string& {std::string* foo};

// Tell SWIG that this value should be passed by reference.
%apply std::vector<std::vector<float> >& INOUT { std::vector<std::vector<float> >& branches };

%{
/* Put header files here or function declarations */
#include "LSystem.h"
#include "vec.h"
#include "matrix.h"
#include <vector>
%}

// Define relationships between C++ data and Python data.
namespace std {
	%template(VecFloat) vector<float>;

	// In Python, use VectorPyBranch for a vector<vector<float> >
	%template(VectorPyBranch) vector<vector<float> >;
}

%include "LSystem.h"
%include "vec.h"
%include "matrix.h"
