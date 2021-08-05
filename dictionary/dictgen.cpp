/* Problem: Noisy Substring Matching
 * Authors: Seth Baunach, Jason Tran
 * Date: 5/9/2020
 * Class: CS485-004
 * How to compile: g++ dictgen.cpp -o dictgen
 * How to run:
 * ./ dictgen [ARGS] < dictionary.txt > out.txt
 *
 * Args:
 *  -ll [num]: lower bound on length of words to pull (1 by default) (inclusive)
 *  -ul [num]: upper bound on length of words to pull (INF by default) (inclusive)
 *  -n [num]: maximum number of words to pull  (INF by default)
 *  -tolower: convert all words to lowercase (off by default)
 *  -rmpunc: remove all punctuation (besides apostrophes) (off by default)
 *  -ignore [str]: ignore words beginning with str (none by default)
 *  -indelim [char]: set input delimiting character ('\n' by default)
 *  -outdelim [char]: set output delimiting character ('\n' by default)
 *  -nl [len] [num]: maximum number of words of length len to pull
 *  -nn: words of length not specified by nl are not included at all (off by default)
 * NOTE: There is NOT error handling for this program. Please note argument syntax well!
*/

#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string.h>

using namespace std;

const int INF = numeric_limits<int>::max();

bool opt_tolower = 0;
bool opt_rmpunc = 0;
size_t ll = 0;
size_t ul = INF;
size_t n = INF;
unordered_set<string> ignores;
char indelim = '\n';
char outdelim = '\n';
unordered_map<int, int> maxL;
bool opt_nn = 0;

int main(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ll") == 0)
            ll = stoi(argv[++i]);
        else if (strcmp(argv[i], "-ul") == 0)
            ul = stoi(argv[++i]);
        else if (strcmp(argv[i], "-n") == 0)
            n = stoi(argv[++i]);
        else if (strcmp(argv[i], "-tolower") == 0)
            opt_tolower = 1;
        else if (strcmp(argv[i], "-rmpunc") == 0)
            opt_rmpunc = 1;
        else if (strcmp(argv[i], "-ignore") == 0)
            ignores.insert(argv[++i]);
        else if (strcmp(argv[i], "-indelim") == 0)
            indelim = argv[++i][0];
        else if (strcmp(argv[i], "-outdelim") == 0)
            outdelim = argv[++i][0];
        else if (strcmp(argv[i], "-nl") == 0) {
            maxL[stoi(argv[i + 1])] = stoi(argv[i + 2]);
            i += 2;
        } else if (strcmp(argv[i], "-nn") == 0)
            opt_nn = 1;
        else
            cout << "Unrecognized argument: " << argv[i] << "\n";
    }

    for(string x; n && !cin.eof();) {

        getline(cin, x, indelim);
        bool skip = 0;
        for (string ignore: ignores) {
            if (x.find(ignore) != string::npos) {
                skip = 1;
                break;
            }
        }
        if (skip)
            continue;

        if (opt_rmpunc) {
            vector<char> xnew;
            for (char c: x)
                if (!ispunct(c) || c == '\'')
                    xnew.push_back(c);
            x = string(xnew.begin(), xnew.end());
        }

        if (opt_tolower) {
            vector<char> xnew;
            for (char c: x)
                xnew.push_back(tolower(c));
            x = string(xnew.begin(), xnew.end());
        }

        if (x.size() < ll || x.size() > ul)
            continue;

        if (maxL.find(x.size()) != maxL.end()) {
            if (maxL[x.size()] != 0)
                maxL[x.size()]--;
            else
                continue;
        } else if (opt_nn)
            continue;

        cout << x << outdelim;
        n--;
    }
}
