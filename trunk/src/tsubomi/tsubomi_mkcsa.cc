// $Id$
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "tsubomi_compressor.h"

using namespace std;
using namespace tsubomi;

// tsubomi_mkcsa: tsubomi make compressed suffix array
int main(int argc, char **argv) {
  try {
   // read command line parameters
    const char *textname   = "";
    sa_index   step        = 100;
    bool       is_help     = false;
    bool       is_progress = false;
    char       param       = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'p': is_progress = true;       break;
          case 'h': is_help     = true;       break;
          default : param       = argv[i][1]; break;
        }
      } else if (param) {
        switch (param) {
          case 's': step = atoi(argv[i]); break;
        }
        param = '\0';
      } else {
        textname = argv[i];
      }
    }

    // write help message and exit
    if (textname[0] == '\0' || is_help) {
      cerr << "[USAGE] tsubomi_mkcsa [params] textfile" << endl;
      cerr << "[OPTIONS]" << endl;
      cerr << "  s   : sampling rate of suffix array." << endl;
      cerr << "        if rate is big, *.csa is small but search time is slow." << endl;
      cerr << "  p   : print progress bar while making suffix array." << endl;
      cerr << "  h   : print help message." << endl;
      return 0;
    }

    compressor tbm;
    tbm.mkcsa(textname, step, is_progress);
    tbm.write(textname);
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
