% TricComp.m Help file for TricComp MEX-file.
%  Computes triconnected components of Graph G.
% 
%  The calling syntax is:
%
%    C = TricComp(G)
%    [C, T] = TricComp(G)
%    [C, T, V] = TricComp(G)
% 
%  Input G is an N-by-N sparse matrix that represents adjacency graph.
%  Output: 
%    C is a N-by-N symmetric sparse matrix with same sparsity pattern as G,
%    with values from range from 1 to M, where M is number of triconnected
%    components found.
%    Value m of non-zero element in (r, c) indicated, that edge between
%    nodes R and C belongs to triconnected component numbered m.
%
%    T is a 1xM vector.
%    T(m) indicates type of triconnected component numbered m.
%    0 is parallel component. 
%    1 is series component.
%    2 is rigid component.
%
%    V is a 2xM vector, which contains edge numbers related to component
%    virtual edge.
%    Warning! It currently fails for series components build from one
%    standard edge and multiple virtual edges.
%
%  Single edge components may occur as either series or parallel components
%  depending on virtual edges of graph.
%
%  Virtual edges are not included in C matrix.
% 
%  This is a MEX-file for MATLAB
%  Copyright 2016 Lukasz Rogalski
% 
