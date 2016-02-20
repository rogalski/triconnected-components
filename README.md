## About
This code allows computation of triconnected components of graph in MATLAB with help of [Open Graph Drawing Framework](http://www.ogdf.net).

[![Build Status](https://travis-ci.org/rogalski/triconnected-components.svg?branch=master)](https://travis-ci.org/rogalski/triconnected-components)

## TODO
- Performance evaluation
- Selection of best API

## Disclaimer
This project is in **alpha** phase:

- API may be a subject of drastic changes. 
- Currently only undirected graph without loops are supported. 
- Bug reports, feature requests and pull requests are welcome.

## License
GNU General Public License v3.0.

## How to build
1. Download latest snapshot of OGDF.

        wget -O ogdf-snapshot.zip http://www.ogdf.net/lib/exe/fetch.php/tech:ogdf-snapshot-2015-12-04.zip

2. Extract it in current working directory.

        unzip ogdf-snapshot.zip

3. Build libraries

        cd ogdf-snapshot
        cmake .
        make
    
4. Build MATLAB mex wrapper. In MATLAB run:

        >> mex -O -largeArrayDims -IOGDF-snapshot/include/ -LOGDF-snapshot -lOGDF -lCOIN TricComp.cpp -v

5. Run tests. In MATLAB run:

        >> TricComp_tests

## How to use

    >> help TricComp    
