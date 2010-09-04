// $Id$
#include <iostream>
#include "tsubomi_indexer.h"

using namespace std;

// tsubomi_mkary: tsubomi make suffix array
int main(int argc, char **argv) {
  try {
   // read command line parameters
    const char *textname = "";
    const char *seps     = "";
    bool is_help         = false;
    bool is_utf8         = false;
    bool is_progress     = false;
    bool is_makeonly     = false;
    bool is_sortonly     = false;
    char param           = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'l': seps        = "\n";       break;
          case 'w': seps        = " \t\n";    break;
          case 'u': is_utf8     = true;       break;
          case 'p': is_progress = true;       break;
          case 'm': is_makeonly = true;       break;
          case 's': is_sortonly = true;       break;
          case 'h': is_help     = true;       break;
          default : param       = argv[i][1]; break;
        }
      } else if (param) {
        switch (param) {
          case 't': seps = argv[i]; break;
        }
        param = '\0';
      } else {
        textname = argv[i];
      }
    }

    // write helo message and exit
    if (textname[0] == '\0' || is_help) {
      cerr << "[USAGE] tsubomi_mkary [params] textfile" << endl;
      cerr << "[OPTIONS]" << endl;
      cerr << "  l   : index for each line." << endl;
      cerr << "  w   : index for each word." << 
                      "(word is separated by \" \\t\\n\")" << endl;
      cerr << "  t(s): index for pointer after separators (s)." << endl;
      cerr << "  u   : index for each utf8 character." << endl;
      cerr << "  m   : make-onky mode. make unsorted \"*.ary\"." << endl;
      cerr << "  s   : sort-only mode. make suffix array from unsorted \"*.ary\"." << endl;
      cerr << "  p   : print progress bar while making suffix array." << endl;
      cerr << "  h   : print help message." << endl;
      return 0;
    }

    // make aryfile and write
    bool is_make = true;
    if (is_sortonly) { is_make = false; }
    bool is_sort = true;
    if (is_makeonly) { is_sort = false; }

    tsubomi::indexer tbm(textname);
    tbm.mkary(seps, is_utf8, is_progress, is_make, is_sort);
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}

