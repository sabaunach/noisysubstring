/* Problem: Noisy Substring Matching
 * Authors: Seth Baunach, Jason Tran
 * Date: 5/9/2020
 * Class: CS485-004
 * How to compile: g++ noisysubstring.cpp -o noisysubstring
 * RECOMMEND COMPILING WITH -O3 for optimization.
 * How to run:
 * ./ noisysubstring [dictionary] [args]
 * Input >
 * -q: exit the program
 * [str]: run noisy substring algorithm on str
 * -t [n] [l_l] [l_u] [e_l] [e_u] [opts]:
 *      - do n times:
 *          - randomly choose a substring of length between l_l and l_u,
 *					inclusive, from a random word in the dictionary
 *              - if l_u >= the str.size(), the substring is guaranteed to be
 *					of length less than str.size() - 1.
 *          - make it noisy by doing betweem e_l and e_u, inclusive, random 
 *					edits
 *              - LIMITATION: the string may be underedited by performing
 *						certain edits that cancel each other out (i.e. delete
 *						then insert same character)
 *          - run noisy substring algorithm on noisy version of substring
 *      - output frequency of noisy substring correctly estimating original
 *				substring's set
 *  Options:
 *      -o: output each test case and whether it succeeded or failed
 *      -s [seed]: randomize using seed, rather than time(NULL)
 *      -g [param]: sets parameter for geometric distrubtion for number of edits
 *
 * Args:
 *  -l: For [str] command, disable display of space-separated list of matches (on by default)
 *  -t: For [str] command, display sorted newline-separated list of all words 
 *			paired with their minimum LD.
 *  -h: For [str] command, display the dictionary file with matches highlighted (in console)
 *  -wf: For each step of k_dist, print the Wagner-Fischer Matrix
 *  -cnt: Output total number of character comparisons for each command
 *  -o: Optimize by only computing WF matrices for suffixes of x of length >= |y|.
*/
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <set>
#include <random>
#include <string.h>
#include <time.h>

using namespace std;

const int INF = numeric_limits<int>::max();
bool opt_l = 1;
bool opt_t = 0;
bool opt_h = 0;
bool opt_wf = 0;
bool opt_o = 0;
bool opt_cnt = 0;
int cntGlobalCharComp = 0;

int d_subst(char a, char b) {
    if (a == b)
        return 0;
    return 1;
}

int d_insrt() {
    return 1;
}

int d_delet() {
    return 1;
}

// compute Wagner-Fischer for all suffixes of x up to length k,
// computing the LD between all prefixes of those suffixes and y
// return the mininum LD of these substrings
int k_dist(string x, string y, size_t k) {
    int cntOuterCharComp = 0;
    int minDist = INF;
    size_t n = x.size();
    size_t m = y.size();

    /* WAGNER-FISCHER */

    // wagner-fischer matrix
    vector<vector<int>> p(n+1, vector<int>(m+1, 0));

    // if we were to delete every character of x to get from x to blank string
    for (size_t i = 1; i <= n; i++)
        p[i][0] = p[i - 1][0] + d_delet();

    // if we were to insert every character of y to get from blank string to y
    for (size_t j = 1; j <= m; j++)
        p[0][j] = p[0][j - 1] + d_insrt();

    for (size_t q = n - k + 1; q <= n; q++) {
        int cntInnerCharComp = 0;
        // v takes the values of the suffixes of x
        string v = x.substr(n - q, q);

        // compute Wagner-Fischer matrix using v and y
        for (size_t i = 1; i <= q; i++) {
            for (size_t j = 1; j <= m; j++) {
                cntInnerCharComp++;
                int r1 = p[i - 1][j - 1] + d_subst(v[i - 1], y[j - 1]);
                int r2 = p[i][j - 1] + d_insrt();
                int r3 = p[i - 1][j] + d_delet();
                p[i][j] = min(r1, min(r2, r3));
            }
            minDist = min(minDist, p[i][m]);
        }
        cntOuterCharComp += cntInnerCharComp;

        if (opt_wf) {
            cout << "\e[0;32;40m    ";
            for (size_t j = 0; j < m; j++)
                cout << "  " << y[j];
            cout << "\n ";
            for (size_t j = 0; j <= m; j++)
                cout << "  " << p[0][j];
            for (size_t i = 0; i < q; i++) {
                cout << "\n" << v[i];
                for (size_t j = 0; j < m; j++)
                    cout << "  " << p[i + 1][j];
                cout << "  \e[0;30;47m" << p[i + 1][m] << "\e[0;32;40m";
            }
            cout << "\n\e[0m  Min = " << minDist << "";
            if (opt_cnt) {
                cout << "\nComparisons: " << cntInnerCharComp;
            }
            cout << "\n\n";
        }
    }
    cntGlobalCharComp += cntOuterCharComp;
    return minDist;
}

vector<int> computeDistances(vector<string> dictionary, string y) {
    /* NOISY SUBSTRING MATCHING ALGORITHM */

    // Compute all minimum LD distances between words x in dictionary and y
    vector<int> distances;

    // k = x.size() with default algorithm, or max(1, |x|-|y|+1) with optimized version
    int k;
    for (string x: dictionary) {
        if (opt_o)
            k = max((int)x.size() - (int)y.size() + 1, 1);
        else
            k = x.size();
        distances.push_back(k_dist(x, y, k));
    }
    return distances;
}

set<string> answerSet(vector<string> dictionary, string str) {
    /* Get all strings in dictionary which have str as a substring */
    set<string> res;
    for (string x: dictionary) {
        if (x.find(str) != string::npos) {
            res.insert(x);
        }
    }
    return res;
}

string randomEdit(string str, size_t numEdits) {
    if (numEdits == 0)
        return str;
    vector<tuple<size_t, int, char>> edits; // {location, type (insrt, delet, subst), char}
    // avoid repeat deletion / substitution on same index
    vector<bool> openToEdit(str.size(), 1);
    size_t totalEditedIndices = 0;
    /* Compute list of edits to perform */
    size_t e = numEdits;
    while (e--) {
        // pick type of edit
        int type = (totalEditedIndices == str.size()) ? 0 : rand() % 3;
        // pick edit location
        int loc;
        if (type == 0) {
            loc = rand() % (str.size() + 1); // could insert after last char
        } else {
            loc = rand() % (str.size() - totalEditedIndices);
            for (; !openToEdit[loc]; loc++); // move to next valid position
            openToEdit[loc] = 0;
            totalEditedIndices++;
        }
        // pick character
        char c = (rand() % ('z' - 'a' + 1)) + 'a';
        if (type == 2)
            for (; c == str[loc]; c = rand()); // make sure c != char already in place
        edits.push_back(make_tuple(loc, type, c));
    }
    /* Compute noisy version of str */
    e = 0;
    vector<char> y;
    sort(edits.begin(), edits.end()); // sort by location
    for (size_t i = 0; i < str.size(); i++) {
        int action = 0; // 0 for none, 1 for deleted, 2 for substituted
        char subChar;   // character if substituted
        for (auto edit = edits[e]; get<0>(edit) < i; edit = edits[++e]);
        for (auto edit = edits[e]; get<0>(edit) == i; edit = edits[++e]) {
            if (get<1>(edit) == 0) // insert
                y.push_back(get<2>(edit));
            else if (get<1>(edit) == 1) // delete
                action = 1;
            else { // substitute
                action = 2;
                subChar = get<2>(edit);
            }
        }
        if (action == 0)
            y.push_back(str[i]);
        else if (action == 2)
            y.push_back(subChar);
    }
    // inserts past-end
    for (auto edit = edits[e]; e < edits.size(); e++) {
        y.push_back(get<2>(edit));
    }
    // convert to string
    string noisy(y.begin(), y.end());
    return noisy;
}

int main(int argc, char **argv) {

    if (argc <= 1 || argv[1][0] == '-') {
        cerr << "Please call with input file name as first argument!\n";
        exit(0);
    }

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            opt_l = 0;
        else if (strcmp(argv[i], "-t") == 0)
            opt_t = 1;
        else if (strcmp(argv[i], "-hl") == 0)
            opt_h = 1;
        else if (strcmp(argv[i], "-wf") == 0)
            opt_wf = 1;
        else if (strcmp(argv[i], "-o") == 0)
            opt_o = 1;
        else if (strcmp(argv[i], "-cnt") == 0)
            opt_cnt = 1;
        else
            cout << "Unrecognized argument: " << argv[i] << "\n";
    }

    ifstream in_file;
    in_file.open(argv[1]);
    if (in_file.fail()) {
        perror("Failed to open input file\n");
        exit(1);
    }
    vector<string> dictionary;
    for (string x; !in_file.eof();) {
        in_file >> x;
        transform(x.begin(), x.end(), x.begin(), ::tolower);
        if (x.length() > 0)
            dictionary.push_back(x);
    }
    in_file.close();

    while (true) {

        int oldCntGlobalCharComp = cntGlobalCharComp;
        cout << "Input > ";
        string in;
        getline(cin, in);
        istringstream iss(in);
        vector<string> command;
        for (string s; iss >> s;) {
            command.push_back(s);
        }
        if (command.size() == 0)
            continue;

        if (command[0] == "-q")
            exit(0);

        if (command[0] == "-t") {
            size_t tNumArgs = 5;
            // do test
            if (command.size() < tNumArgs + 1) {
                cout << "Not enough inputs specified for command " << command[0] << "\n";
                continue;
            }
            bool topt_o = 0;
            int seed = time(NULL);
            float geom_param = 0.5;
            if (command.size() > tNumArgs + 1) {
                for (size_t i = tNumArgs + 1; i < command.size(); i++) {
                    if (command[i] == "-o")
                        topt_o = 1;
                    else if (command[i] == "-g") {
                        try {
                            geom_param = stof(command[++i]);
                        } catch (int e) {
                            cout << "-g must be followed by a floating-point number representing a seed\n";
                            continue;
                        }
                    }
                    else if (command[i] == "-s") {
                        try {
                                seed = stoi(command[++i]);
                        } catch (int e) {
                            cout << "-r must be followed by an integer representing a seed\n";
                            continue;
                        }
                    }
                    else
                        cout << "Unrecognized argument: " << command[i] << "\n";
                }
            }

            int tArgs[tNumArgs];
            for (size_t i = 0; i < tNumArgs; i++) {
                try {
                    tArgs[i] = stoi(command[i+1]);
                } catch (int e) {
                    cout << "Arguments 1-5 of " << command[0] << " must be positive integers\n";
                    continue;
                }
            }
            for (size_t i = 0; i < tNumArgs; i++) {
                if (tArgs[i] < 0) {
                    cout << "Arguments 1-5 of " << command[0] << " must be positive integers\n";
                    continue;
                }
            }
            size_t n, l_l, l_u, e_l, e_u;
            n = tArgs[0];
            l_l = tArgs[1];
            l_u = tArgs[2];
            e_l = tArgs[3];
            e_u = tArgs[4];
            // sort dictionary by size of strings
            vector<string> sortedDict = dictionary;
            sort(sortedDict.begin(), sortedDict.end(),
                [](const string &a, const string &b) -> bool {
                    return a.size() < b.size();
                });
            if (l_l > l_u) {
                cout << "Argument 2: l_l of " << command[0] << " must be <= argument 3, l_u\n";
                continue;
            }
            if (e_l > e_u) {
                cout << "Argument 4: e_l of " << command[0] << " must be <= argument 5, e_u\n";
                continue;
            }

            /* Dictionary properties */
            size_t longest = sortedDict[sortedDict.size() - 1].size();
            size_t smallestValidIndex = 0;
            for (; sortedDict[smallestValidIndex].size() <= l_l; smallestValidIndex++);
            size_t rangeSize = sortedDict.size() - smallestValidIndex;

            /* Output things */
            int totalCases = n;
            int totalSuccesses = 0;
            double avgRatio = 0;
            double avgEstSize = 0;
            int outw = max((int)longest + 1, 13);
            if (topt_o) {
                cout << setprecision(6);
                cout << setw(outw) << "Word" << setw(outw) << "Substr"
                     << setw(outw) << "Noisy" << setw(9) << "Est Size"
                     << setw(9) << "Ans Size" << setw(8) << "Ratio"
                     << setw(8) << "Success" << "\n";
            }

            /* RNG things */
            srand(seed);
            geometric_distribution<int> geom_dist(geom_param);
            default_random_engine generator(seed);

            while (n--) {
                /* DO TEST */
                // pick random word
                string word = sortedDict[(rand() % rangeSize) + smallestValidIndex];
                // pick length of substring of word
                int l = rand() % (min(l_u, word.length() - 1) - l_l + 1) + l_l;
                // pick location of substring
                int substrloc = rand() % (word.length() - l + 1);

                string str = word.substr(substrloc, l);
                // pick number of edits
                size_t e = (geom_dist(generator) % (e_u - e_l + 1)) + e_l;
                // edit word
                string y = randomEdit(str, e);
                // Run noisy substring algorithm on y
                vector<int> distances = computeDistances(dictionary, y);
                int minDist = INF;
                for (int dist: distances)
                    minDist = min(dist, minDist);
                // Get set of strings which are closest
                set<string> closest;
                for (size_t i = 0; i < dictionary.size(); i++)
                    if (distances[i] == minDist)
                        closest.insert(dictionary[i]);
                // Get set of strings which are the "answer"
                set<string> answer = answerSet(dictionary, str);
                // check if answer is a subset of closest
                bool success = 1;
                for (string x: answer) {
                    if (closest.find(x) == closest.end()) {
                        success = 0;
                        break;
                    }
                }
                if (success)
                    totalSuccesses++;
                double ratio = (double)((double)closest.size() / (double)answer.size());
                avgRatio += ratio;
                avgEstSize += closest.size();
                if (topt_o) {
                    cout << setw(outw) << word << setw(outw) << str << setw(outw) << y
                         << setw(9) << closest.size() << setw(9) << answer.size()
                         << setw(8) << ratio << setw(8) << (success ? "true" : "false") << "\n";
                }
            }
            avgRatio /= totalCases;
            avgEstSize /= totalCases;
            cout << "\nTotal tests: " << totalCases;
            cout << "\nTotal successes: " << totalSuccesses;
            cout << "\nAverage estimate-size: " << avgEstSize;
            cout << "\nAverage estimate-size to answer-size ratio: " << avgRatio;
            cout << "\nFrequency: " << (double)totalSuccesses / (double)totalCases << "\n\n";
        }
        else {
            // Run noisy substring algorithm on command[0] (str input)
            string y = command[0];
            vector<int> distances = computeDistances(dictionary, y);

            int minDist = INF;
            for (int dist: distances) {
                if (dist < minDist)
                    minDist = dist;
            }

            /* OUTPUT */
            cout << "\n";
            if (opt_h) {
                cout << "\e[0;32;40m ";
                for (size_t i = 0; i < dictionary.size(); i++) {
                    if (distances[i] == minDist)
                        cout << "\e[0;30;47m";
                    cout << dictionary[i];
                    cout << "\e[0;32;40m ";
                }
                cout << "\e[0m\n\n";
            }
            if (opt_l) {
                for (size_t i = 0; i < dictionary.size(); i++)
                    if (distances[i] == minDist)
                        cout << dictionary[i] << " ";
                cout << "\n\n";
            }
            if (opt_t) {
                vector<pair<int, int>> distDict(dictionary.size());
                for (size_t i = 0; i < dictionary.size(); i++)
                    distDict[i] = {distances[i], i};
                sort(distDict.begin(), distDict.end());
                for (pair<int, int> w: distDict)
                    cout << w.first << "\t" << dictionary[w.second] << "\n";
                cout << "\n\n";
            }
        }
        if (opt_cnt) {
            cout << "Total comparisons during last command: "
                 << cntGlobalCharComp - oldCntGlobalCharComp << "\n";
            cout << "Total comparisons since beginning of program: "
                 << cntGlobalCharComp << "\n";
        }
        cout << "\n";
    }
}
