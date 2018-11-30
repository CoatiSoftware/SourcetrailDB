//sourcetraildb.i

%module sourcetraildb
%include "std_string.i"
%feature("autodoc", "1");

%{
#include "sourcetraildb.h"
%}

//double-check that this is indeed %include !!!
%include "sourcetraildb.h"