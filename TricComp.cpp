#include "mex.h"
#include "matrix.h"
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/internal/decomposition/TricComp.h>

#define ADJ_MX (prhs[0])
#define CMP_MX (plhs[0])
#define TYP_MX (plhs[1])
#define VIR_MX (plhs[2])

static const bool checkComp = false;

void buildGraph(ogdf::Graph &G, const mxArray *adjacencyMatrix, const mwIndex n) {
    mwIndex i, j, *jc, *ir;

    ogdf::node *nodes = (ogdf::node *) mxMalloc(n * sizeof(ogdf::node));
    for (unsigned int i = 0; i < n; i++) {
        nodes[i] = G.newNode(i + 1);
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
            G.newEdge(nodes[i], nodes[ir[j]]);
        }
    }
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:nargin",
                          "Function requires single input argument.");
    } else if (nlhs < 1 || nlhs > 3) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:nargout",
                          "Function requires one, two or three output arguments.");
    }
    mwIndex n = mxGetN(ADJ_MX);
    if (!mxIsDouble(ADJ_MX) || !mxIsSparse(ADJ_MX) || n != mxGetM(ADJ_MX)) {
        mexErrMsgIdAndTxt("MATLAB:triccomp:invarg",
                          "First argument must be symmetric sparse double matrix.");
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
        if (!component.m_edges.empty()) {
            numTriconnectedComponents += 1;
        }
    }

    CMP_MX = mxDuplicateArray(ADJ_MX);
    mwIndex *jc = mxGetJc(CMP_MX);
    mwIndex *ir = mxGetIr(CMP_MX);
    double *pr = mxGetPr(CMP_MX);

    double *typPr;
    if (nlhs >= 2) {
        TYP_MX = mxCreateDoubleMatrix(1, numTriconnectedComponents, mxREAL);
        typPr = mxGetPr(TYP_MX);
    }
    double *virPr;
    if (nlhs >= 3) {
        VIR_MX = mxCreateDoubleMatrix(2, numTriconnectedComponents, mxREAL);
        virPr = mxGetPr(VIR_MX);
    }

    unsigned int componentNum = 0;
    for (unsigned int i = 0; i < tricComp.m_numComp; i++) {
        ogdf::TricComp::CompStruct component = tricComp.m_component[i];
        ogdf::List <ogdf::edge> edges = component.m_edges;
        if (component.m_edges.empty()) {
            continue;
        }
        for (auto &e: edges) {
            if (tricComp.m_pGC->original(e) == 0) {
                if (nlhs >= 3) {
                    // This would lie for series component build from one standard edge and multiple virtual edges.
                    // I'm not yet sure how to handle this.
                    virPr[2 * componentNum] = e->source()->index() + 1;
                    virPr[2 * componentNum + 1] = e->target()->index() + 1;
                }
                continue;
            }
            mwIndex row = (mwIndex) e->source()->index();
            mwIndex col = (mwIndex) e->target()->index();
            // Set componentNum in output matrix for edges
            for (mwIndex j = jc[col]; j < jc[col + 1]; j++) {
                if (ir[j] == row) {
                    pr[j] = componentNum + 1;
                    break;
                }
            }
            for (mwIndex j = jc[row]; j < jc[row + 1]; j++) {
                if (ir[j] == col) {
                    pr[j] = componentNum + 1;
                    break;
                }
            }
        }
        if (nlhs > 1) {
            typPr[componentNum] = component.m_type;
        }
        componentNum++;
    }
    if (checkComp && !tricComp.checkComp()) {
        mexWarnMsgIdAndTxt("MATLAB:triccomp:invout",
                           "TricComp(G).checkComp() returned false.");
    }
}
