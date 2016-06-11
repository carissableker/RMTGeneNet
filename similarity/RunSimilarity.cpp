#include "RunSimilarity.h"

/**
 * Prints the command-line usage instructions for the similarity command
 */
void RunSimilarity::printUsage() {
  printf("\n");
  printf("Usage: ./rmtgnet similarity [options]\n");
  printf("The list of required options:\n");
  printf("  --ematrix|-e      The tab delimited file name that contains the expression matrix.\n");
  printf("                    The rows must be genes or probe sets and columns are samples.\n");
  printf("                    If a header row is present it must only contain the list of\n");
  printf("                    genes (i.e. will be one column shorter than all other rows).\n");
  printf("  --rows|-r         The number of lines in the ematrix file including the header\n");
  printf("                    row if it exists\n");
  printf("  --cols|-c         The number of columns in the input file\n");
  printf("  --method|-m       The correlation methods to use. Supported methods include\n");
  printf("                    Pearson's correlation ('pc'), Spearman's rank ('sc')\n");
  printf("                    and Mutual Information ('mi').\n");
  printf("\n");
  printf("Optional Filtering Arguments:\n");
  printf("  --set1|-1         The path to a file that contains a set of genes to limit\n");
  printf("                    for similarity analaysis.  The genes in this file will\n");
  printf("                    be compared to all other genes. Each gene must be on a \n");
  printf("                    separate line\n");
  printf("  --set2|-2         The path to a file that contains a set of genes to limit\n");
  printf("                    similarity analysis. The genes in this file will be\n");
  printf("                    compared with the genes in the file specified by --set1.\n");
  printf("                    set2 cannot be used by itself.  It must be used with set1.\n");
  printf("                    Each gene must be on a spearate line\n");
  printf("\n");
  printf("Optional Expression Matrix Arguments:\n");
  printf("  --omit_na         Provide this flag to ignore missing values.\n");
  printf("  --na_val|-n       A string representing the missing values in the input file\n");
  printf("                    (e.g. NA or 0.000)\n");
  printf("  --func|-f         A transformation function to apply to elements of the ematrix.\n");
  printf("                    Values include: log, log2 or log10. Default is to not perform\n");
  printf("                    any transformation.\n");
  printf("  --headers         Provide this flag if the first line of the matrix contains\n");
  printf("                    headers.\n");
  printf("\n");
  printf("Optional Similarity Arguments:\n");
  printf("  --min_obs|-o      The minimum number of observations (after missing values\n");
  printf("                    removed) that must be present to calculate a simililarity score.\n");
  printf("                    Default is 30.\n");
  printf("  --th|s            The minimum expression level to include. Anything below is excluded\n");
  printf("\n");
  printf("Optional Mutual Information Arguments:\n");
  printf("  --mi_bins|-b      Use only if the method is 'mi'. The number of bins for the\n");
  printf("                    B-spline estimator function for MI. Default is 10.\n");
  printf("  --mi_degree|-d    Use only if the method is 'mi'. The degree of the\n");
  printf("                    B-spline estimator function for MI. Default is 3.\n");
  printf("\n");
  printf("For Help:\n");
  printf("  --help|-h       Print these usage instructions\n");
  printf("\n");
  printf("Note: similarity values are set to NaN if there weren't enough observations\n");
  printf("to perform the calculation.\n");
}
/**
 * The function to call when running the 'similarity' command.
 */
RunSimilarity::RunSimilarity(int argc, char *argv[]) {

  // Initialize some of the program parameters.
  min_obs = 30;

  // Defaults for mutual information B-spline estimate.
  mi_bins = 10;
  mi_degree = 3;

  // Set the default threshold for expression values.
  threshold  = -INFINITY;

  // Initialize the array of method names. We set it to 10 as max. We'll
  // most likely never have this many of similarity methods available.
  method = (char **) malloc(sizeof(char *) * 10);

  // The concatenated method.
  char * cmethod = NULL;

  // loop through the incoming arguments until the
  // getopt_long function returns -1. Then we break out of the loop
  // The value returned by getopt_long.
  int c;
  while(1) {
    int option_index = 0;

    // specify the long options. The values returned are specified to be the
    // short options which are then handled by the case statement below
    static struct option long_options[] = {
      {"help",         no_argument,       0,  'h' },
      {"method",       required_argument, 0,  'm' },
      {"min_obs",      required_argument, 0,  'o' },
      {"th",           required_argument, 0,  's' },
      // Filtering options.
      {"set1",         required_argument, 0,  '1' },
      {"set2",         required_argument, 0,  '2' },
      // Mutual information options.
      {"mi_bins",      required_argument, 0,  'b' },
      {"mi_degree",    required_argument, 0,  'd' },
      // Expression matrix options.
      {"rows",         required_argument, 0,  'r' },
      {"cols",         required_argument, 0,  'c' },
      {"headers",      no_argument,       &headers,  1 },
      {"omit_na",      no_argument,       &omit_na,  1 },
      {"func",         required_argument, 0,  'f' },
      {"na_val",       required_argument, 0,  'n' },
      {"ematrix",      required_argument, 0,  'e' },
      // Last element required to be all zeros.
      {0, 0, 0,  0 }
    };

    // get the next option
    c = getopt_long(argc, argv, "m:o:b:d:j:i:t:a:l:r:c:f:n:e:s:h", long_options, &option_index);

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
      case 'o':
        min_obs = atoi(optarg);
        break;
      case 's':
        threshold = atof(optarg);
        break;
      // Mutual information options.
      case 'b':
        mi_bins = atoi(optarg);
        break;
      case 'd':
        mi_degree = atoi(optarg);
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
      // Help and catch-all options.
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
        exit(-1);
    }
  }

  // Make sure the similarity method is valid.
  if (!cmethod) {
    fprintf(stderr,"Please provide the method (--method option).\n");
    exit(-1);
  }
  parseMethods(cmethod);

  // make sure the required arguments are set and appropriate
  if (!infilename) {
    fprintf(stderr,"Please provide an expression matrix (--ematrix option).\n");
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
  // make sure the input file exists
  if (access(infilename, F_OK) == -1) {
    fprintf(stderr,"The input file does not exists or is not readable.\n");
    exit(-1);
  }

  // Create and initialize the histogram for the distribution of coefficients.
  histogram = (int *) malloc(sizeof(int) * HIST_BINS + 1);
  for (int m = 0; m < HIST_BINS + 1; m++) {
    histogram[m] = 0;
  }

  if (headers) {
    printf("  Skipping header lines\n");
  }
  printf("  Performing transformation: %s \n", func);
  if (omit_na) {
    printf("  Missing values are: '%s'\n", na_val);
  }
  printf("  Required observations: %d\n", min_obs);
  for(int i = 0; i < this->num_methods; i++) {
    printf("  Using similarity method: '%s'\n", method[i]);
    if (strcmp(method[i], "mi") ==0) {
      printf("  Bins for B-Spline estimate of MI: %d\n", mi_bins);
      printf("  Degree for B-Spline estimate of MI: %d\n", mi_degree);
    }
  }
  printf("  Minimal observed value: %f\n", threshold);

  // Retrieve the data from the EMatrix file.
  printf("  Reading expression matrix...\n");
  ematrix = new EMatrix(infilename, rows, cols, headers, omit_na, na_val, func);

}
/**
 *
 */
void RunSimilarity::parseMethods(char * methods_str) {
  // Split the method into as many parts
  char * tmp;
  int i = 0;
  tmp = strstr(methods_str, ",");
  while (tmp) {
    // Get the method and make sure it's valid.
    method[i] = (char *) malloc(sizeof(char) * (tmp - methods_str + 1));
    strncpy(method[i], methods_str, (tmp - methods_str));
    methods_str = tmp + 1;
    tmp = strstr(methods_str, ",");
    i++;
  }
  // Get the last element of the methods_str.
  method[i] = (char *) malloc(sizeof(char) * strlen(methods_str) + 1);
  strcpy(method[i], methods_str);
  i++;

  this->num_methods = i;

  for (i = 0; i < this->num_methods; i++) {
    if (strcmp(method[i], "pc") != 0 &&
        strcmp(method[i], "mi") != 0 &&
        strcmp(method[i], "sc") != 0 ) {
      fprintf(stderr,"Error: The method (--method option) must contain only 'pc', 'sc' or 'mi'.\n");
      exit(-1);
    }
    // Make sure the method isn't specified more than once.
    for (int j = 0; j < i; j++) {
      if (strcmp(method[i], method[j]) == 0) {
        fprintf(stderr,"Error: You may only specify a similarity method once (--method option).\n");
        exit(-1);
      }
    }
  }
}

/**
 * Implements the destructor.
 */
RunSimilarity::~RunSimilarity() {
  delete ematrix;
  free(histogram);
  for (int i = 0; i < this->num_methods; i++) {
    free(method[i]);
  }
  free(method);
}

/**
 *
 */
void RunSimilarity::execute() {

  // The output file name.
  char outfilename[1024];
  // Used for pairwise comparison of the same gene.
  float one = 1.0;
  // The number of binary files needed to store the matrix.
  int num_bins;
  // The current binary file number.
  int curr_bin;
  // Holds the number of rows in the file.
  int bin_rows;
  // The total number of pair-wise comparisons to be made.
  long long int total_comps;
  // The number of comparisions completed during looping.
  long long int n_comps;
  // The number of genes.
  int num_genes = ematrix->getNumGenes();
  // The binary output file prefix
  char * fileprefix = ematrix->getFilePrefix();
  char outdir[100];
  // Hold an array of output files (one per each method).
  FILE ** outfiles = NULL;

  // calculate the number of binary files needed to store the similarity matrix
  num_bins = (num_genes - 1) / ROWS_PER_OUTPUT_FILE;

  // Make sure the output directory exists
  for (int i = 0; i < this->num_methods; i++) {
    if (strcmp(method[i], "sc") == 0) {
      strcpy((char *)&outdir, "./Spearman");
    }
    if (strcmp(method[i], "pc") == 0) {
      strcpy((char *)&outdir, "./Pearson");
    }
    if (strcmp(method[i], "mi") == 0) {
      strcpy((char *)&outdir, "./MI");
    }
    struct stat st = {0};
    if (stat(outdir, &st) == -1) {
      mkdir(outdir, 0700);
    }
  }

  total_comps = ((long long int)num_genes * ((long long int)num_genes - 1)) / 2;
  n_comps = 0;

  // each iteration of m is a new output file
  printf("Calculating correlations...\n");
  for (curr_bin = 0; curr_bin <= num_bins; curr_bin++) {

    // calculate the limit on the rows to output based on where we are in the calculation
    if (curr_bin < num_bins) {
      bin_rows = (curr_bin + 1) * ROWS_PER_OUTPUT_FILE;
    }
    else {
      bin_rows = num_genes;
    }

    // the output file will be located in the  directory and named based on the input file info
    outfiles = (FILE **) malloc(sizeof(FILE *) * this->num_methods);
    for (int i = 0; i < this->num_methods; i++) {
      // Open the file for storing the binary results.
      sprintf(outfilename, "%s/%s.%s%d.bin", outdir,fileprefix, method[i], curr_bin);
      printf("Writing file %d of %d: %s... \n", curr_bin + 1, num_bins + 1, outfilename);
      outfiles[i] = fopen(outfilename, "wb");

      // write the size of the matrix.
      fwrite(&num_genes, sizeof(num_genes), 1, outfiles[i]);
      // write the number of lines in this file
      int num_lines = bin_rows - (curr_bin * ROWS_PER_OUTPUT_FILE);
      fwrite(&num_lines, sizeof(num_lines), 1, outfiles[i]);
    }


    // iterate through the genes that belong in this file
    for (int j = curr_bin * ROWS_PER_OUTPUT_FILE; j < bin_rows; j++) {
      // iterate through all the genes up to j (only need lower triangle)
      for (int k = 0; k <= j; k++) {
        n_comps++;
        if (n_comps % 1000 == 0) {
          statm_t * memory = memory_get_usage();
          printf("Percent complete: %.2f%%. Mem: %ldb. \r", (n_comps / (float) total_comps) * 100, memory->size);
          free(memory);
        }
        if (j == k) {
          // correlation of an element with itself is 1
          for (int i = 0; i < this->num_methods; i++) {
            fwrite(&one, sizeof(one), 1, outfiles[i]);
          }
          continue;
        }

        float score;
        PairWiseSet * pwset = new PairWiseSet(ematrix, j, k);

        // Perform the appropriate calculation based on the method
        for (int i = 0; i < this->num_methods; i++) {
          if (strcmp(method[i], "pc") == 0) {
            PearsonSimilarity * pws = new PearsonSimilarity(pwset, min_obs);
            pws->run();
            score = (float) pws->getScore();
            delete pws;
          }
          else if(strcmp(method[i], "mi") == 0) {
            MISimilarity * pws = new MISimilarity(pwset, min_obs, mi_bins, mi_degree);
            pws->run();
            score = (float) pws->getScore();
            delete pws;
          }
          else if(strcmp(method[i], "sc") == 0) {
            SpearmanSimilarity * pws = new SpearmanSimilarity(pwset, min_obs);
            pws->run();
            score = (float) pws->getScore();
            delete pws;
          }
        }
        delete pwset;
        fwrite(&score, sizeof(float), 1, outfiles[0]);

//        // if the historgram is turned on then store the value in the correct bin
//        if (score < 1 && score > -1) {
//          if (score < 0) {
//            score = -score;
//          }
//          histogram[(int)(score * HIST_BINS)]++;
//        }
      }
    }
    for (int i = 0; i < this->num_methods; i++) {
      fclose(outfiles[i]);
    }
    free(outfiles);
  }

  // Write the historgram
//  writeHistogram();

  printf("\nDone.\n");
}

/**
 * Prints the histogram to a file.
 *
 * @param CCMParameters params
 *   An instance of the CCM parameters structsimilarity
 *
 * @param int * histogram
 *   An integer array created by the init_histogram function and
 *   populated by calculate_MI or calculate_person.
 *
 */
//void RunSimilarity::writeHistogram() {
//  char outfilename[1024];  // the output file name
//
//  // output the correlation histogram
//  sprintf(outfilename, "%s.%s.corrhist.txt", ematrix->getFilePrefix(), method);
//  FILE * outfile = fopen(outfilename, "w");
//  for (int m = 0; m < HIST_BINS; m++) {
//    fprintf(outfile, "%lf\t%d\n", 1.0 * m / (HIST_BINS), histogram[m]);
//  }
//  fclose(outfile);
//}
