#ifndef _RUNTHRESHOLD_
#define _RUNTHRESHOLD_

#include <getopt.h>
#include "./methods/RMTThreshold.h"

class RunThreshold {
  private:
    // The expression matrix object.
    EMatrix * ematrix;
    // Specifies the methods: sc, pc, mi.
    char ** method;
    // Indicates the number of methods.
    int num_methods;
    // The method used for thresholding.
    char * th_method;
    // The directory where the binary similarity matrix is found
    char * bin_dir;

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

    // RMT Threshold options
    // ---------------------
    // The starting threshold value.
    double thresholdStart;
    // The step size for decreasing threshold value.
    double thresholdStep;
    // The Chi-square value being sought.
    double chiSoughtValue;


    void parseMethods(char * methods_str);

  public:
    RunThreshold(int argc, char *argv[]);
    ~RunThreshold();

    static void printUsage();
    void execute();

};

#endif
