% Tests need tweaking - hardcoded component id may result in false
% failures if detection algorithm internals changes.

% Single Rigid Component
G1 = [
    0 1 1 1;
    1 0 1 1;
    1 1 0 1;
    1 1 1 0
];  
C1 = TricComp(sparse(G1));
assert(isequal(C1, G1));
[C1, T1] = TricComp(sparse(G1));
assert(isequal(T1, 2));

% One Rigid Component, One Series Component
G2 = [
    0 1 1 1 1 0 0;
    1 0 1 1 0 0 1;
    1 1 0 1 0 0 0;
    1 1 1 0 0 0 0;
    1 0 0 0 0 1 0;
    0 0 0 0 1 0 1;
    0 1 0 0 0 1 0
];
C2Expected = [
    0 2 1 1 3 0 0;
    2 0 1 1 0 0 3;
    1 1 0 1 0 0 0;
    1 1 1 0 0 0 0;
    3 0 0 0 0 3 0;
    0 0 0 0 3 0 3;
    0 3 0 0 0 3 0
];
[C2, T2] = TricComp(sparse(G2));
assert(isequal(C2, C2Expected));
assert(isequal(T2, [2,0,1]));

% Sample from Hopcroft and Tarjan paper
G3 = [
    0 1 0 1 0 0 0 1 0 0 0 1 1;
    1 0 1 0 0 0 0 0 0 0 0 0 1;
    0 1 0 1 0 0 0 0 0 0 0 0 1;
    1 0 1 0 1 0 1 0 0 0 0 0 0;
    0 0 0 1 0 1 1 1 0 0 0 0 0;
    0 0 0 0 1 0 1 0 0 0 0 0 0;
    0 0 0 1 1 1 0 0 0 0 0 0 0;
    1 0 0 0 1 0 0 0 1 0 1 1 0;
    0 0 0 0 0 0 0 1 0 1 1 1 0;
    0 0 0 0 0 0 0 0 1 0 1 1 0;
    0 0 0 0 0 0 0 1 1 1 0 0 0;
    1 0 0 0 0 0 0 1 1 1 0 0 0;
    1 1 1 0 0 0 0 0 0 0 0 0 0;
];
C3Expected = [
    0 12  0 10  0  0  0  4  0  0  0  3 12;
   12  0 12  0  0  0  0  0  0  0  0  0 12;
    0 12  0 11  0  0  0  0  0  0  0  0 12;
   10  0 11  0  9  0  8  0  0  0  0  0  0;
    0  0  0  9  0  6  7  5  0  0  0  0  0;
    0  0  0  0  6  0  6  0  0  0  0  0  0;
    0  0  0  8  7  6  0  0  0  0  0  0  0;
    4  0  0  0  5  0  0  0  1  0  1  2  0;
    0  0  0  0  0  0  0  1  0  1  1  1  0;
    0  0  0  0  0  0  0  0  1  0  1  1  0;
    0  0  0  0  0  0  0  1  1  1  0  0  0;
    3  0  0  0  0  0  0  2  1  1  0  0  0;
   12 12 12  0  0  0  0  0  0  0  0  0  0;
];

[C3, T3] = TricComp(sparse(G3));
assert(isequal(C3, C3Expected));
assert(isequal(T3, [2, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 2]));
