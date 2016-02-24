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
    std::vector <ogdf::node> nodes;
    nodes.reserve(n);
    for (i = 0; i < n; i++) {
        nodes.push_back(G.newNode());
    }

    // Find MATLAB's matrix structure to iterate over egdes
    jc = mxGetJc(adjacencyMatrix);
    ir = mxGetIr(adjacencyMatrix);

    // Iterate over upper triangular matrix.
    for (i = 0; i < n; i++) {
        for (j = jc[i]; j < jc[i + 1]; j++) {
            if (ir[j] <= i) {
                continue;
            }
            G.newEdge(nodes.at(i), nodes.at(ir[j]));
        }
    }
}

inline bool shouldTriconnectedComponentBeSkipped(ogdf::TricComp::CompStruct &c) {
    // TODO: Some components has zero edges, why?
    return c.m_edges.empty();
}

inline bool isEdgeVirtual(ogdf::TricComp &comp, ogdf::edge e) {
    return comp.m_pGC->original(e) == 0;
}

void setValueInSparseMatrix(const mwIndex row, const mwIndex col, const double value, const mxArray *matrix) {
    mwIndex *jc = mxGetJc(matrix);
    mwIndex *ir = mxGetIr(matrix);
    double *pr = mxGetPr(matrix);
    for (mwIndex i = jc[col]; i < jc[col + 1]; i++) {
        if (ir[i] == row) {
            pr[i] = value;
        }
    }
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

    // Get valid component count.
    unsigned int numTriconnectedComponents = 0;
    for (unsigned int i = 0; i < tricComp.m_numComp; i++) {
        ogdf::TricComp::CompStruct component = tricComp.m_component[i];
        if (!shouldTriconnectedComponentBeSkipped(component)) {
            numTriconnectedComponents += 1;
        }
    }

    CMP_MX = mxDuplicateArray(ADJ_MX);
    double *typPr;
    if (nlhs == 2) {
        TYP_MX = mxCreateDoubleMatrix(1, numTriconnectedComponents, mxREAL);
        typPr = mxGetPr(TYP_MX);
    }

    unsigned int componentNum = 0;
    for (unsigned int i = 0; i < tricComp.m_numComp; i++) {
        ogdf::TricComp::CompStruct component = tricComp.m_component[i];
        ogdf::List <ogdf::edge> edges = component.m_edges;
        if (shouldTriconnectedComponentBeSkipped(component)) {
            continue;
        }
        for (auto &e: edges) {
            if (isEdgeVirtual(tricComp, e)) {
                continue;
            }
            mwIndex row = (mwIndex) e->source()->index();
            mwIndex col = (mwIndex) e->target()->index();
            setValueInSparseMatrix(row, col, componentNum + 1, CMP_MX);
            setValueInSparseMatrix(col, row, componentNum + 1, CMP_MX);
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
