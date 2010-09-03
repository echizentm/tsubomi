// $Id$
#include <iostream>
#include <string>
#include "tsubomi_basic_searcher.h"
#include "tsubomi_compressor.h"

using namespace std;
using namespace tsubomi;

// tsubomi_search: tsubomi search
int main(int argc, char **argv) {
 try {
    // read command line parameters
    const char *textname = "";
    const char *keyword  = "";
    bool is_help         = false;
    bool is_csa          = false;
    char param           = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'c': is_csa     = true;       break;
          case 'h': is_help    = true;       break;
          default : param      = argv[i][1]; break;
        }
      } else if (param) {
        param = '\0';
      } else {
        if (textname[0] == '\0') {
          textname = argv[i];
        } else {
          keyword  = argv[i];
        }
      }
    }

    // write help message and exit
    if (keyword[0] == '\0' || is_help) {
      cout << "[USAGE] tsubomi_csass [params] textfile keyword" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  c   : compressed suffix array mode." << endl;
      cout << "        use not *.ary, but*.csa" << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // search matched strings
    searcher *ptbm;
    if (is_csa) { ptbm = new compressor(textname); }
    else        { ptbm = new basic_searcher(textname); }

    sa_range p = ptbm->search(keyword);
    if (p.first < 0) { return 0; }

    for (sa_index i = p.first; i <= p.second; i++) {
      char     buf[1024];
      sa_index pos;
      ptbm->get_line(i, buf, 1024, &pos);
      cout << buf << "(" << pos << ")" << endl;
    }
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}

