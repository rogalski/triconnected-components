#include "mex.h"
#include "matrix.h"
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/internal/decomposition/TricComp.h>

#define ADJ_MX (prhs[0])

void buildGraph(ogdf::Graph &G, const mxArray *adjacencyMatrix, const mwIndex n) {
    mwIndex i, j, *jc, *ir;

    // Create nodes
    std::vector<ogdf::node> nodes;
    nodes.reserve(n);
    for (i = 0; i < n; i++) {
        nodes.push_back(G.newNode());
    }

    // Find MATLAB's matrix structure to iterate over egdes
    jc = mxGetJc(adjacencyMatrix);
    ir = mxGetIr(adjacencyMatrix);

    // Iterate over upper triangular matrix.
    // TODO: Refactor.
    for (i = 0; i < n; i++) {
        for (j = jc[i]; j < jc[i+1]; j++) {
            if (ir[j] <= i) {
                continue;    
            }
            G.newEdge(nodes.at(i), nodes.at(ir[j]));
        }
    }
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:nargin",
                          "Function requires single input argument.");
    } else if (nlhs != 0) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:nargout",
                          "Function requires no output arguments.");
    }
    mwIndex n = mxGetN(ADJ_MX);
    if (!mxIsSparse(ADJ_MX) || n != mxGetM(ADJ_MX)) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:invarg",
                          "First argument must be symmetric sparse matrix.");
    }

    ogdf::Graph G;
    buildGraph(G, ADJ_MX, n);
    for (auto edge : G.edges){
        std::cout << edge;
    }
    std::cout << std::endl;
    if (!ogdf::isBiconnected(G)) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:invarg",
                          "First argument is not a biconnected graph.");
    }

    ogdf::TricComp tricComp(G);

    // TODO: Convert to MATLAB matrices

    for (unsigned int i = 0; i < tricComp.m_numComp; i++) {
        ogdf::TricComp::CompStruct component = tricComp.m_component[i];
        ogdf::List<ogdf::edge> L = component.m_edges;
        if (L.size() == 0) {
            continue;
        }
        std::cout << "type: " << component.m_type << ", ";
        for (auto ei : L) {
            std::cout << ei;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    if (!tricComp.checkComp()) {
        mexWarnMsgIdAndTxt("MATLAB:triccomp:invout",
                           "TricComp(G).checkComp() returned false.");
    }
}
