#ifndef _RUNEXTRACT_
#define _RUNEXTRACT_

#include <getopt.h>
#include "../ematrix/EMatrix.h"
#include "SimMatrixBinary.h"

class RunExtract {
  private:
    // The expression matrix object.
    EMatrix * ematrix;
    // Specifies the method: sc, pc, mi.
    // Specifies the correlation method that was used: pc, mi, sc
    char ** method;
    // Indicates the number of methods.
    int num_methods;
    // The method used for thresholding.
    char * th_method;
    int quiet;

    // Common Filters
    // --------------
    // The threshold for creating the network.
    float th;
    // The user-specified x coordinate to retrieve
    int x_coord;
    // The user-specified y coordinate to retrieve
    int y_coord;
    // The user-specified name of gene1
    char * gene1;
    // the user-specified name of gene2
    char * gene2;


    // Variables for the expression matrix
    // -----------------------------------
    // Indicates if the expression matrix has headers.
    int headers;
    // The input file name
    char *infilename;
    // The number of rows in the input ematrix file (including the header)
    int rows;
    // The number of cols in the input ematrix file.
    int cols;
    // Indicates if missing values should be ignored in the EMatrix file.
    int omit_na;
    // Specifies the value that represents a missing value.
    char *na_val;
    // Specifies the transformation function: log2, none.
    char func[10];


    void parseMethods(char * methods_str);

  public:
    RunExtract(int argc, char *argv[]);
    ~RunExtract();

    static void printUsage();
    void execute();
};

#endif
