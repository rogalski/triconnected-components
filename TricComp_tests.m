% Single Rigid Component
G1 = [
    0 1 1 1;
    1 0 1 1;
    1 1 0 1;
    1 1 1 0
];  
TricComp(sparse(G1));

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
TricComp(sparse(G2));

% TODO: assertions on output
