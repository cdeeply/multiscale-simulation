//
//  CDNN_cicada.c
//  marble
//
//  Created by Brian Ross on 4/23/24.
//

#include <stdio.h>
#include "userfn.h"
#include "CDNN_cicada.h"
#include "cdeeply_neural_network.h"



int callPtrSize(int argc, char **argv)  {  return sizeof(void *);  }

int callCDNNsize(int argc, char **argv)  {  return sizeof(CDNN);  }


int callGetNNdata1(int argc, char **argv)
{
    CDNN *NN;
    ccInt *numLayers, *encoderLayer, *variationalLayer;
    
    getArgs(argc, argv, &NN, &numLayers, &encoderLayer, &variationalLayer);
    
    *numLayers = NN->numLayers;
    *encoderLayer = NN->encoderLayer;
    *variationalLayer = NN->variationalLayer;
    
    return passed;
}


int callGetNNdata2(int argc, char **argv)
{
    CDNN *NN;
    ccInt cl, *layerSize, *layerAFs, *numLayerInputs;
    
    getArgs(argc, argv, &NN, &layerSize, &layerAFs, &numLayerInputs);
    
    for (cl = 0; cl < NN->numLayers; cl++)  {
        layerSize[cl] = (ccInt) NN->layerSize[cl];
        layerAFs[cl] = (ccInt) NN->layerAFs[cl];
        numLayerInputs[cl] = (ccInt) NN->numLayerInputs[cl];    }
    
    return passed;
}


int callGetNNdata3(int argc, char **argv)
{
    CDNN *NN;
    ccInt l, cil, cw, numWeights, *layerInputs;
    ccFloat *weights;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &NN, byValue(&l), &layerInputs, endArgs);
    
    for (cil = 0; cil < NN->numLayerInputs[l]; cil++)  {
        layerInputs[cil] = (ccInt) NN->layerInputs[l][cil];
        weights = (ccFloat *) argv[3+cil];
        numWeights = argInfo[3+cil].argIndices;
        for (cw = 0; cw < numWeights; cw++)  {
            weights[cw] = (ccFloat) NN->weights[l][cil][cw];
    }   }
    
    return passed;
}


int callGetTabularRegressor(int argc, char **argv)
{
    CDNN *NN;
    ccInt numInputs, numOutputs, numSamples, transpose, *outputIndices, maxWeights, maxHiddenNeurons, maxLayers, maxLayerSkips, rtrn;
    ccBool hasBias, allowIOconnections;
    ccFloat *trainingSamples, *importances;
    int hasBiasArg, allowIOconnectionsArg;
    char *errMsg;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &NN, byValue(&numInputs), byValue(&numOutputs), byValue(&numSamples), &trainingSamples,
                    byValue(&transpose), &outputIndices, &importances,
                    byValue(&maxWeights), byValue(&maxHiddenNeurons), byValue(&maxLayers), byValue(&maxLayerSkips),
                    byValue(&hasBias), byValue(&allowIOconnections));
    
    if (argInfo[7].argIndices == 0)  importances = NULL;
    
    if (hasBias)  hasBiasArg = HAS_BIAS;
    else  hasBiasArg = NO_BIAS;
    
    if (allowIOconnections)  allowIOconnectionsArg = ALLOW_IO_CONNECTIONS;
    else  allowIOconnectionsArg = NO_IO_CONNECTIONS;
    
    rtrn = CDNN_tabular_regressor(NN, numInputs, numOutputs, numSamples, trainingSamples, transpose, outputIndices, importances,
            maxWeights, maxHiddenNeurons, maxLayers, maxLayerSkips, hasBiasArg, allowIOconnectionsArg, NULL, &errMsg);
    
    if (rtrn != passed)  printf("getTabularRegressor() error:  %s\n", errMsg);
    
    return rtrn;
}


int callGetTabularEncoder(int argc, char **argv)
{
    CDNN *NN;
    ccInt numFeatures, numSamples, transpose, numEncodingFeatures, numVariationalFeatures, variationalDist;
    ccInt maxWeights, maxHiddenNeurons, maxLayers, maxLayerSkips, rtrn;
    ccBool doEncoder, doDecoder, hasBias;
    ccFloat *trainingSamples, *importances;
    int hasBiasArg, doEncoderArg, doDecoderArg;
    char *errMsg;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &NN, byValue(&numFeatures), byValue(&numSamples), &trainingSamples, byValue(&transpose), &importances,
                    byValue(&doEncoder), byValue(&doDecoder),
                    byValue(&numEncodingFeatures), byValue(&numVariationalFeatures), byValue(&variationalDist),
                    byValue(&maxWeights), byValue(&maxHiddenNeurons), byValue(&maxLayers), byValue(&maxLayerSkips), byValue(&hasBias));
    
    if (argInfo[5].argIndices == 0)  importances = NULL;
    
    if (doEncoder)  doEncoderArg = DO_ENCODER;
    else  doEncoderArg = NO_ENCODER;
    if (doDecoder)  doDecoderArg = DO_DECODER;
    else  doDecoderArg = NO_DECODER;
    
    if (hasBias)  hasBiasArg = HAS_BIAS;
    else  hasBiasArg = NO_BIAS;
    
    rtrn = CDNN_tabular_encoder(NN, numFeatures, numSamples, trainingSamples, transpose, importances,
                    doEncoderArg, doDecoderArg, numEncodingFeatures, numVariationalFeatures, variationalDist,
                    maxWeights, maxHiddenNeurons, maxLayers, maxLayerSkips, hasBiasArg, NULL, &errMsg);
    
    if (rtrn != passed)  printf("getTabularEncoder() error:  %s\n", errMsg);
    
    return rtrn;
}



// this requires that ccInt == int and ccFloat == double

int callRunCDNN(int argc, char **argv)
{
    CDNN NN;
    ccInt cl, cil;
    double *inputs, **weightArray;
    
    getArgs(argc, argv, &inputs, byValue(&NN.numLayers), byValue(&NN.variationalLayer),
            &NN.layerSize, &NN.layerAFs, &NN.numLayerInputs, &NN.y, &NN.layerInputs, &NN.weights, endArgs);
    NN.encoderLayer = -1;           // irrelevant to run_CDNN()
    
    weightArray = (double **) (argv + 9 + 3*NN.numLayers);
    for (cl = 0; cl < NN.numLayers; cl++)  {
        NN.weights[cl] = (double **) argv[9+cl];
        NN.y[cl] = (double *) argv[9+cl + NN.numLayers];
        NN.layerInputs[cl] = (int *) argv[9+cl + 2*NN.numLayers];
        for (cil = 0; cil < NN.numLayerInputs[cl]; cil++)  {
            NN.weights[cl][cil] = *weightArray;
            weightArray++;
    }   }
    
    run_CDNN(&NN, inputs);
    
    return passed;
}


int callFreeCDNN(int argc, char **argv)
{
    CDNN *NN = (CDNN *) argv[0];
    
    free_CDNN(NN);
    
    return passed;
}
