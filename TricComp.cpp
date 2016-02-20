#include "mex.h"
#include "matrix.h"
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/internal/decomposition/TricComp.h>

#define ADJ_MX (prhs[0])
#define CMP_MX (plhs[0])
#define TYP_MX (plhs[1])

static const bool checkComp = false;

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
        for (j = jc[i]; j < jc[i + 1]; j++) {
            if (ir[j] <= i) {
                continue;
            }
            G.newEdge(nodes.at(i), nodes.at(ir[j]));
        }
    }
}

inline bool shouldTriconnectedComponentBeSkipped(ogdf::TricComp::CompStruct c) {
    // TODO: Some components has zero edges, why?
    return c.m_edges.empty() || c.m_type == ogdf::TricComp::CompType::bond;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:nargin",
                          "Function requires single input argument.");
    } else if (nlhs != 1 && nlhs != 2) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:nargout",
                          "Function requires one or two output arguments.");
    }
    mwIndex n = mxGetN(ADJ_MX);
    if (!mxIsSparse(ADJ_MX) || n != mxGetM(ADJ_MX)) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:invarg",
                          "First argument must be symmetric sparse matrix.");
    }

    ogdf::Graph G;
    buildGraph(G, ADJ_MX, n);
    if (!ogdf::isBiconnected(G)) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:invarg",
                          "First argument is not a biconnected graph.");
    }

    ogdf::TricComp tricComp(G);
    unsigned int foundComponents = tricComp.m_numComp;

    // Get output matrix size. TODO: May it be done cleaner?
    unsigned int maxTriconnectedComponentSize = 0;
    unsigned int numTriconnectedComponents = 0;
    for (unsigned int i = 0; i < foundComponents; i++) {
        ogdf::TricComp::CompStruct component = tricComp.m_component[i];
        if (!shouldTriconnectedComponentBeSkipped(component)) {
            numTriconnectedComponents += 1;
        }
        if (component.m_edges.size() > maxTriconnectedComponentSize) {
            maxTriconnectedComponentSize = component.m_edges.size();
        };
    }

    CMP_MX = mxCreateDoubleMatrix(2 * maxTriconnectedComponentSize, numTriconnectedComponents, mxREAL);
    double *cmpPr = mxGetPr(CMP_MX);
    double *typPr;
    if (nlhs == 2) {
        TYP_MX = mxCreateDoubleMatrix(1, numTriconnectedComponents, mxREAL);
        typPr = mxGetPr(TYP_MX);
    }

    unsigned int componentNum = 0;
    for (unsigned int i = 0; i < foundComponents; i++) {
        ogdf::TricComp::CompStruct component = tricComp.m_component[i];
        ogdf::List <ogdf::edge> edges = component.m_edges;
        if (shouldTriconnectedComponentBeSkipped(component)) {
            continue;
        }
        unsigned j = 0;
        for (auto &e: edges) {
            int row = e->source()->index();
            int col = e->target()->index();
            cmpPr[componentNum * 2 * maxTriconnectedComponentSize + 2 * j] = row + 1;
            cmpPr[componentNum * 2 * maxTriconnectedComponentSize + 2 * j + 1] = col + 1;
            j++;
        }
        if (nlhs == 2) {
            typPr[componentNum] = component.m_type;
        }
        componentNum++;
    }
    if (checkComp && !tricComp.checkComp()) {
        mexWarnMsgIdAndTxt("MATLAB:triccomp:invout",
                           "TricComp(G).checkComp() returned false.");
    }
}
