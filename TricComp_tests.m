% Single Rigid Component
G1 = [
    0 1 1 1;
    1 0 1 1;
    1 1 0 1;
    1 1 1 0
];  
C1 = TricComp(sparse(G1));
assert(all(C1 == [1;2;2;3;3;1;3;4;4;2;4;1]))

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
[C2, T] = TricComp(sparse(G2));
assert(all(C2(:,1) == [2,3,3,1,3,4,4,2,4,1,2,1]'))
assert(all(C2(:,2) == [6,5,5,1,7,6,1,2,2,7,0,0]'))
assert(all(T == [2,1]))
