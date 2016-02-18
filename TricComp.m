% TricComp.m Help file for TricComp MEX-file.
%  Computes triconnected components of Graph G.
% 
%  The calling syntax is:
%
%    C = TricComp(G)
%    [C, T] = TricComp(G)
% 
%  Input G is an N-by-N sparse matrix that represents a graph.
%  Output: 
%    C is a matrix where each column represents single found connected
%    component. It consists of 2*n rows, where each non-zero pair
%    represents single edge belonging to current triconnected component.
%
%    T is vector with types of each triconnected component.
%    0 is parallel component (cannot occur). 
%    1 is series component.
%    2 is rigid compoent.
% 
%  This is a MEX-file for MATLAB
%  Copyright 2016 Lukasz Rogalski
% 
