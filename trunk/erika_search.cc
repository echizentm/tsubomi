// $Id$
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "erika_louds_trie.h"

using namespace std;
using namespace erika;
int main(int argc, char **argv) {
  try {
    trie *pt;
    char *filename = argv[1];
    if (filename[strlen(filename) - 2] == 'l') {
      pt = new louds_trie();
    } else {
      pt = new basic_trie();
    }
    pt->read(filename);
    cout << "> ";


    char *buf = NULL;
    size_t len = 0;
    while (getline(&buf, &len, stdin) != -1) {
      buf[strlen(buf) - 1] = '\0';
      trie_results values;
      pt->search(buf, values);
      trie_results::iterator i = values.begin();
      trie_results::iterator e = values.end();
      while (i != e) {
        cout << i->first << " : " << i->second << endl;
        i++;
      }
      cout << "> ";
    }
    if (buf) { free(buf); }
    if (pt)  { delete pt; }

  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}

