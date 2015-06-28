/// @file test_readlines.cc
/// @brief test readlines
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-15

#include <iostream>
#include <sstream>
#include "horny_toad/readlines.h"
#include "horny_toad/verify.h"

using namespace std;
using namespace horny_toad;

void test_readlines ()
{
    stringstream ss1, ss2;
    ss1 << "one a\n2 b\nthree\nfour a b c d";
    ss2 << "one\n2\nthree\n";
    vector<string> l1 = readlines (ss1);
    VERIFY (l1.size () == 4);
    vector<string> l2 = readlines (ss2);
    VERIFY (l2.size () == 3);
}

void test_readwords ()
{
    stringstream ss1, ss2, ss3;
    ss1 << "one\n2\nthree\nfour";
    ss2 << "one\n2\nthree\n";
    ss3 << "1.1\n2.3 4.5 6.7\n8.9\n";
    vector<string> l1 = readwords<string> (ss1);
    VERIFY (l1.size () == 4);
    vector<string> l2 = readwords<string> (ss2);
    VERIFY (l2.size () == 3);
    vector<float> l3 = readwords<float> (ss3);
    VERIFY (l3.size () == 5);
}

int main ()
{
    try
    {
        test_readlines ();
        test_readwords ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
