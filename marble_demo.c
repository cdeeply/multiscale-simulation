//
//  marble_demo.c
//  marble
//
//  Created by Brian Ross on 4/25/24.
//

#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "marble.h"
#include "marble_demo.h"
#include "userfn.h"
#include "CDNN_cicada.h"
#include "cdeeply_neural_network.h"



ccBool downscaleTriggerF(ccInt level, ccInt ct, ccFloat t, ccInt numX, ccInt numSamples, ccFloat *x, ccFloat *storedX, ccFloat *storedY)
{
    int cx, cs;
    const ccInt checkInterval = 30;
    const ccFloat maxSpacing = 1.;
    
    if (ct % checkInterval == 0)  {
    for (cx = 0; cx < numX; cx++)  {
        for (cs = 0; cs <= numSamples; cs++)  {
            if (cs == numSamples)  return ccTrue;
            else if (fabs(x[cx]-storedX[numX*cs+cx]) < maxSpacing)  break;
    }}  }
    
    return ccFalse;
}


int callSimH1(int argc, char **argv)  {  return runSim(argc, argv, callH1);  }


int callH1(int argc, char **argv)
{
    ccInt numTerms, cTerm, cv, *varNos, numVarsInTerm;
    ccFloat *x, prefactor, *coefs, *gradCoefs, gradMult, *gradH;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &gradH, &x, endArgs);
    
    numTerms = (argc-2)/5;
    
    for (cTerm = 0; cTerm < numTerms; cTerm++)  {
        numVarsInTerm = argInfo[4+5*cTerm].argIndices;
        getArgs(argc, argv, fromArg(2+5*cTerm), byValue(&prefactor), byValue(&gradMult), &varNos, &coefs, &gradCoefs, endArgs);
        for (cv = 0; cv < numVarsInTerm; cv++)  gradMult += coefs[cv] * x[varNos[cv]];
        gradMult *= prefactor;
        
        for (cv = 0; cv < numVarsInTerm; cv++)  {
            gradH[varNos[cv]] += gradMult * gradCoefs[cv];
    }   }
    
    return passed;
}
