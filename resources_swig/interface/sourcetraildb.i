//sourcetraildb.i

%module sourcetraildb
%include "std_string.i"
%feature("autodoc", "1");

%{
#include "sourcetraildb.h"
%}

//double-check that this is indeed %include !!!
%include "sourcetraildb.h"

%pragma(java) jniclasscode=%{
  static {
    try {
        System.loadLibrary("_sourcetraildb");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}
