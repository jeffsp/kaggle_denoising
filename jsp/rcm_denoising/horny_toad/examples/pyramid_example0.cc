// Pyramid Example
//
// Copyright (C) 2007 Jeffrey S. Perry
//
// This program is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//
// contact: jeffsp@gmail.com

#include "horny_toad/pyramid.h"
#include <iomanip>
#include <iostream>

using namespace horny_toad;
using namespace std;

// Output a pyramid to a stream
template<typename T>
std::ostream& operator<< (std::ostream &s, const pyramid<T> &p)
{
    for (size_t l = p.levels (); l != 0; --l) {
        for (size_t i = 0; i < p[l - 1].rows (); ++i) {
            for (size_t j = 0; j < p[l - 1].cols (); ++j)
                s << setw (4) << p[l - 1](i, j);
            s << endl; }
        s << endl; }
    return s;
}

int main ()
{
    try
    {
        pyramid<int> a (4, 8, 9);
        cout << a;
        // 9   9
        //
        // 9   9   9   9
        // 9   9   9   9
        //
        // 9   9   9   9   9   9   9   9
        // 9   9   9   9   9   9   9   9
        // 9   9   9   9   9   9   9   9
        // 9   9   9   9   9   9   9   9
        //
        pyramid<int> b (4, 8);
        b[0].assign (1);
        cout << b;
        // 0   0
        //
        // 0   0   0   0
        // 0   0   0   0
        //
        // 1   1   1   1   1   1   1   1
        // 1   1   1   1   1   1   1   1
        // 1   1   1   1   1   1   1   1
        // 1   1   1   1   1   1   1   1
        //
        b.reduce2x2 ();
        cout << b;
        // 1   1
        //
        // 1   1   1   1
        // 1   1   1   1
        //
        // 1   1   1   1   1   1   1   1
        // 1   1   1   1   1   1   1   1
        // 1   1   1   1   1   1   1   1
        // 1   1   1   1   1   1   1   1
        //
        pyramid<int> c (6, 12, 0);
        c[0].assign (7);
        c.top ().assign (3);
        cout << c;
        // 3   3
        //
        // 0   0   0
        // 0   0   0
        //
        // 0   0   0   0   0   0
        // 0   0   0   0   0   0
        // 0   0   0   0   0   0
        //
        // 7   7   7   7   7   7   7   7   7   7   7   7
        // 7   7   7   7   7   7   7   7   7   7   7   7
        // 7   7   7   7   7   7   7   7   7   7   7   7
        // 7   7   7   7   7   7   7   7   7   7   7   7
        // 7   7   7   7   7   7   7   7   7   7   7   7
        // 7   7   7   7   7   7   7   7   7   7   7   7
        //
        c.expand3x3 (3);
        c.expand3x3 (2);
        c.expand3x3 (1);
        cout << c;
        // 3   3
        //
        // 3   3   3
        // 3   3   3
        //
        // 3   3   3   3   3   3
        // 3   3   3   3   3   3
        // 3   3   3   3   3   3
        //
        // 3   3   3   3   3   3   3   3   3   3   3   3
        // 3   3   3   3   3   3   3   3   3   3   3   3
        // 3   3   3   3   3   3   3   3   3   3   3   3
        // 3   3   3   3   3   3   3   3   3   3   3   3
        // 3   3   3   3   3   3   3   3   3   3   3   3
        // 3   3   3   3   3   3   3   3   3   3   3   3
        //
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

