#include "RunExtract.h"

/**
 * Prints the command-line usage instructions for the similarity command
 */
void RunExtract::printUsage() {
  printf("\n");
  printf("Usage: ./rmtgnet extract [options]\n");
  printf("The list of required options:\n");
  printf("  --ematrix|-e     The file name that contains the expression matrix.\n");
  printf("                   The rows must be genes or probe sets and columns are samples\n");
  printf("  --rows|-r        The number of lines in the ematrix file including the header\n");
  printf("                   row if it exists\n");
  printf("  --cols|-c        The number of columns in the input file\n");
  printf("  --th|-t          The threshold to cut the similarity matrix. Network files will be generated.\n");
  printf("  --method|-m      The correlation methods used. Supported methods include\n");
  printf("                   Pearson's correlation ('pc'), Spearman's rank ('sc')\n");
  printf("                   and Mutual Information ('mi').");
  printf("\n");
  printf("Optional expression matrix arguments:\n");
  printf("  --omit_na         Provide this flag to ignore missing values. Use this option for\n");
  printf("                    RNA-seq expression matricies where counts are zero.\n");
  printf("  --na_val|-n      A string representing the missing values in the input file\n");
  printf("                   (e.g. NA or 0.000)\n");
  printf("  --func|-f        A transformation function to apply to elements of the ematrix.\n");
  printf("                   Values include: log, log2 or log10. Default is to not perform\n");
  printf("                   any transformation.\n");
  printf("  --headers        Provide this flag if the first line of the matrix contains\n");
  printf("                   headers.\n");
  printf("\n");
  printf("Optional filtering arguments:\n");
  printf("  -x               Extract a single similarity value: the x coordinate. Must also use -y\n");
  printf("  -y               Extract a single similarity value: the y coordinate. Must also use -x.\n");
  printf("  --gene1|-1       Extract a single similarity value: The name of the first gene in a singe\n");
  printf("                   pair-wise comparision.  Must be used with --gene2 option.\n");
  printf("  --gene1|-2       Extract a single similarity value: The name of the second gene in a singe\n");
  printf("                   pair-wise comparision.  Must be used with --gene1 option.\n");
  printf("\n");
  printf("For Help:\n");
  printf("  --help|-h        Print these usage instructions\n");
  printf("\n");
}

RunExtract::RunExtract(int argc, char *argv[]) {

  // Set some default values;
  headers = 0;
  rows = 0;
  cols = 0;
  x_coord = -1;
  y_coord = -1;
  gene1 = NULL;
  gene2 = NULL;
  th = 0;
  quiet = 0;

   // The value returned by getopt_long
   int c;

   // loop through the incoming arguments until the
   // getopt_long function returns -1. Then we break out of the loop
   while(1) {
    int option_index = 0;

    // specify the long options. The values returned are specified to be the
    // short options which are then handled by the case statement below
    static struct option long_options[] = {
      {"quiet",        no_argument,       &quiet,  1 },
      {"method",       required_argument, 0,  'm' },
      {"th_method",    required_argument, 0,  'p' },
      {"help",         no_argument,       0,  'h' },
      // Expression matrix options.
      {"rows",         required_argument, 0,  'r' },
      {"cols",         required_argument, 0,  'c' },
      {"headers",      no_argument,       &headers,  1 },
      {"omit_na",      no_argument,       &omit_na,  1 },
      {"func",         required_argument, 0,  'f' },
      {"na_val",       required_argument, 0,  'n' },
      {"ematrix",      required_argument, 0,  'e' },
      // Common fitering options
      {"th",           required_argument, 0,  't' },
      {"gene1",        required_argument, 0,  '1' },
      {"gene2",        required_argument, 0,  '2' },
      {"x",            required_argument, 0,  'x' },
      {"y",            required_argument, 0,  'y' },

      // Last element required to be all zeros.
      {0, 0, 0, 0}
    };
    delete ematrix;

    // get the next option
    c = getopt_long(argc, argv, "m:r:c:f:n:e:t:1:2:x:y:g:d:z:l:h", long_options, &option_index);

    // if the index is -1 then we have reached the end of the options list
    // and we break out of the while loop
    if (c == -1) {
      break;
    }

    // handle the options
    switch (c) {
      case 0:
        break;
      case 'm':
        cmethod = optarg;
        break;
      // Common fitering options
      case 't':
        th = atof(optarg);
        break;
      case '1':
        gene1 = optarg;
        break;
      case '2':
        gene2 = optarg;
        break;
      case 'x':
        x_coord = atoi(optarg);
        break;
      case 'y':
        y_coord = atoi(optarg);
        break;
      // Expression matrix options.
      case 'e':
        infilename = optarg;
        break;
      case 'r':
        rows = atoi(optarg);
        break;
      case 'c':
        cols = atoi(optarg);
        break;
      case 'n':
        na_val = optarg;
        break;
      case 'f':
        strcpy(func, optarg);
        break;
      case 'h':
        printUsage();
        exit(-1);
        break;
      case '?':
        exit(-1);
        break;
      case ':':
        printUsage();
        exit(-1);
        break;
      default:
        printUsage();
     }
   }

   // Make sure the similarity method is valid.
   if (!cmethod) {
     fprintf(stderr,"Please provide the method (--method option).\n");
     exit(-1);
   }

   // make sure we have a positive integer for the rows and columns of the matrix
   if (rows < 0 || rows == 0) {
     fprintf(stderr, "Please provide a positive integer value for the number of rows in the \n");
     fprintf(stderr, "expression matrix (--rows option).\n");
     exit(-1);
   }
   if (cols < 0 || cols == 0) {
     fprintf(stderr, "Please provide a positive integer value for the number of columns in\n");
     fprintf(stderr, "the expression matrix (--cols option).\n");
     exit(-1);
   }

   if (omit_na && !na_val) {
     fprintf(stderr, "Error: The missing value string should be provided (--na_val option).\n");
     exit(-1);
   }

   if (th > 0 && (x_coord > 0 ||  y_coord > 0)) {
     fprintf(stderr, "Please provide a threshold or x and y coordinates only but not both\n");
     exit(-1);
   }

   if ((gene1 && !gene2) || (!gene1 && gene2)) {
     fprintf(stderr, "You must provide both gene1 and gene2 options.\n");
     exit(-1);
   }

   // make sure the required arguments are set and appropriate
   if (!infilename) {
     fprintf(stderr,"Please provide an expression matrix (--ematrix option).\n");
     exit(-1);
   }

   // Load the input expression matrix.
   ematrix = new EMatrix(infilename, rows, cols, headers, omit_na, na_val, func);

   // if the user supplied gene
   if (gene1 && gene2) {
     x_coord = ematrix->getGeneCoord(gene1);
     y_coord = ematrix->getGeneCoord(gene2);

     // Make sure the coordinates are positive integers
     if (x_coord < 0) {
       fprintf(stderr, "Could not find gene %s in the genes list file\n", gene1);
       exit(-1);
     }
     if (y_coord < 0) {
       fprintf(stderr, "Could not find gene %s in the genes list file\n", gene2);
       exit(-1);
     }
   }

   // Make sure we have a positive integer for the x and y coordinates.
   if ((x_coord >= 1 &&  y_coord < 1) ||
       (x_coord < 1  &&  y_coord >= 1)) {
     fprintf(stderr, "Please provide a positive integer greater than 1 for both the x and y coordinates (-x and -y options)\n");
     exit(-1);
   }

   // TODO: make sure the th_method is in the method array.

   // print out some setup details
   if (!quiet) {
     if (th > 0) {
       printf("  Using threshold of %f\n", th);
     }
     else {
       printf("  Using coords (%d, %d)\n", x_coord, y_coord);
     }
   }
}


/**
 *
 */
RunExtract::~RunExtract() {
  delete ematrix;
}

/**
 *
 */
void RunExtract::execute() {

  SimMatrixBinary * smatrix = new SimMatrixBinary(ematrix, quiet, cmethod,
    x_coord, y_coord, gene1, gene2, th);

  // If we have a threshold then we want to get the edges of the network.
  // Otherwise the user has asked to print out the similarity value for
  // two genes.
  if (smatrix->getThreshold() > 0) {
    smatrix->writeNetwork();
  }
  else {
    smatrix->getPosition();
  }
}
