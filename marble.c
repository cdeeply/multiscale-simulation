//
//  marble_demo.c
//  marble
//
//  Created by Brian Ross on 4/29/24.
//

#include "userfn.h"
#include "marble.h"
#include "CDNN_cicada.h"
#include "cdeeply_neural_network.h"


int callSimCDNN(int argc, char **argv)  {  return runSim(argc, argv, callRunCDNN);  }


int runSim(int argc, char **argv, int(*timeStepF)(int, char **))
{
    ccInt ct, cy, level, numXs, numSamples, *numTimeSteps, argOffset;
    int rtrn;
    ccFloat t0, dt, *storedX, *storedY;
    ccFloat *x, *dxdt;
    arg_info **argInfo = (arg_info **) &argv[argc];
    
    getArgs(argc, argv, byValue(&level), &numTimeSteps, byValue(&t0), byValue(&dt), &storedX, &storedY, &dxdt, &x, endArgs);
    numXs = (*argInfo)[6].argIndices;
    numSamples = (*argInfo)[4].argIndices / numXs;
    
    argOffset = 6;
    if (timeStepF == callRunCDNN)  argOffset++;
    argc -= argOffset;  argv += argOffset;  (*argInfo) += argOffset;
    
    for (ct = 0; ct < *numTimeSteps; ct++)  {
        ccFloat *y = x + numXs;
        
        if (level > 1)  {
        if (downscaleTriggerF(level, ct, t0+ct*dt, numXs, numSamples, x, storedX, storedY))  {
            *numTimeSteps = ct+1;
            return 1;
        }}
        
        rtrn = timeStepF(argc, argv);
        if (rtrn != passed)  return rtrn;
        
        for (cy = 0; cy < numXs; cy++)  {
            y[cy] = x[cy] + dt * dxdt[cy];      }
        
        x = y;
    }
    
    return passed;
}
