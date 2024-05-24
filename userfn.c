/*
 *  userfn.c(pp) - for the user's own C/C++ functions
 *
 *  Cicada
 *  Copyright (C) 2006 Brian C. Ross
 *
 *  This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2 of the License, or any later version.

 *  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "intrpt.h"
#include "ccmain.h"
#include "userfn.h"


// **********************************************
// #include any user-defined header files here

#include "CDNN_cicada.h"
#include "marble.h"
#include "marble_demo.h"
//#include "engine.h"



extern ccInt callMinMax(ccInt, char **);
extern ccInt callSum(ccInt, char **);
extern ccInt callMakeLinkList(ccInt, char **);
extern ccInt sortpart(const ccInt, const ccInt, const ccInt, const ccFloat *, ccInt *);
extern ccInt callSort(ccInt, char **);




// **********************************************
// UserFunctions defines the { names, function addresses } of user's C routines.
// Each routine must be of the form:  int RoutineName(int argc, char **argv)


userFunction UserFunctions[] = { { "cicada", &runCicada }, { "makeLinkList", &callMakeLinkList },
            { "minmax", &callMinMax }, { "sum", &callSum }, { "makeLinkList", &callMakeLinkList }, { "sort", &callSort },
            { "ptrSize", &callPtrSize }, { "CDNNsize", &callCDNNsize },         // CDNN_cicada.h
            { "getNNdata1", &callGetNNdata1 }, { "getNNdata2", &callGetNNdata2 }, { "getNNdata3", &callGetNNdata3 },
            { "getTabularRegressor", &callGetTabularRegressor }, { "getTabularEncoder", &callGetTabularEncoder },
            { "runCDNN", &callRunCDNN }, { "freeCDNN", &callFreeCDNN },
            { "simCDNN", &callSimCDNN },                                        // marble.h
            { "H1", &callH1 }, { "simH1", &callSimH1 }                          // marble_demo.h
/*                        { "RunModel", &call_run_model },
                        { "InitRandom", &call_InitRandom }, 
                        { "save movie", &save_movie }, { "writeAudioStream", &writeAudioStream }*/ };


const ccInt userFunctionsNum = (ccInt) (sizeof(UserFunctions)/sizeof(userFunction));      // for Cicada's own records




// C routines used to speed up array operations min/max/sum/mean/sort() in user.cicada

ccInt callMinMax(ccInt argc, char **argv)
{
    ccInt c1, listTop, *bestIdx, mult;
    ccFloat *theList, *bestValue;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &theList, byValue(&mult), &bestIdx, &bestValue);
    listTop = argInfo[0].argIndices;
    
    *bestIdx = 1;
    *bestValue = mult * theList[0];
    
    for (c1 = 0; c1 < argInfo[0].argIndices; c1++)  {
    if (mult*theList[c1] > *bestValue)  {
        *bestIdx = c1 + 1;
        *bestValue = mult * theList[c1];
    }}
    
    return passed;
}



ccInt callSum(ccInt argc, char **argv)
{
    ccInt c1;
    ccFloat *theList, *theSum;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &theList, &theSum);
    
    *theSum = 0.;
    for (c1 = 0; c1 < argInfo[0].argIndices; c1++)  *theSum += theList[c1];
    
    return passed;
}



ccInt callMakeLinkList(ccInt argc, char **argv)
{
    ccInt *linkList, firstIndex, direction;
    ccFloat *sortingList;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &sortingList, &linkList, byValue(&direction));
    
    firstIndex = sortpart(0, argInfo[0].argIndices-1, direction, sortingList, linkList);
    
    return firstIndex;
}


ccInt sortpart(const ccInt left, const ccInt right, const ccInt direction, const ccFloat *sortingList, ccInt *linklist)
{
    ccInt middle, smallest, toWrite, side, nextSide, idx[2];
    
    if (right > left)  {
        
        middle = floor((right + left)/2);
        idx[0] = sortpart(left, middle, direction, sortingList, linklist);
        idx[1] = sortpart(middle + 1, right, direction, sortingList, linklist);
        
        if ((sortingList[idx[0]] - sortingList[idx[1]])*direction <= 0)  side = 0;
        else  side = 1;
        
        smallest = toWrite = idx[side];
        idx[side] = linklist[idx[side]];
        
        while ((idx[0] != -1) && (idx[1] != -1))  {
            
            if ((sortingList[idx[0]] - sortingList[idx[1]])*direction <= 0)  nextSide = 0;
            else  nextSide = 1;
            
            if (side != nextSide)  {
                linklist[toWrite] = idx[nextSide];
                side = nextSide;       }
            
            toWrite = idx[side];
            idx[side] = linklist[idx[side]];    }
        
        linklist[toWrite] = idx[1-side];
        
        return smallest;    }
        
    else  return left;
}


ccInt callSort(ccInt argc, char **argv)
{
    ccInt numIndices, cl, ci, idx, *linkList, numLists, firstIndex, rtrn = passed;
    arg_info *argInfo = (arg_info *) argv[argc];
    
    getArgs(argc, argv, &linkList, byValue(&firstIndex), endArgs);
    numIndices = argInfo[0].argIndices;
    numLists = (argc-2)/2;
    
    for (cl = 0; cl < numLists; cl++)  {
        if (argInfo[cl+2].argType != argInfo[cl+2+numLists].argType)  rtrn = 1;
        else if (argInfo[cl+2].argIndices != numIndices)  rtrn = 2;
        else if ((argInfo[cl+2].argType != int_type) && (argInfo[cl+2].argType != double_type))  rtrn = 3;
        if (rtrn != passed)  break;         }
    
    if (rtrn != passed)  {
    for (ci = 0; ci < numIndices; ci++)  {
        linkList[ci]++;        // convert to Cicada indexing
    }}
    
    else  {
    for (cl = 0; cl < numLists; cl++)  {
        idx = firstIndex;
        if (argInfo[cl+2].argType == int_type)  {
            ccInt *list = (ccInt *) (argv[cl+2]), *sortedList = (ccInt *) (argv[cl+2+numLists]);
            for (ci = 0; ci < numIndices; ci++)  {
                sortedList[ci] = list[idx];
                idx = linkList[idx];
        }   }
        else if (argInfo[cl+2].argType == double_type)  {
            ccFloat *list = (ccFloat *) (argv[cl+2]), *sortedList = (ccFloat *) (argv[cl+2+numLists]);
            for (ci = 0; ci < numIndices; ci++)  {
                sortedList[ci] = list[idx];
                idx = linkList[idx];
    }}  }   }
    
    return rtrn;
}



// Misc -- useful for loading argument pointers into the C code

void getArgs(int argc, char **argv, ...)
{
    ccInt loopArg;
    va_list theArgs;
    char **nextarg;
    arg_info *myArgInfo = (arg_info *) argv[argc];
    
    va_start(theArgs, argv);
    for (loopArg = 0; loopArg < argc; loopArg++)  {
        nextarg = va_arg(theArgs, char **);
        if (nextarg != NULL)
            *nextarg = argv[loopArg];
        else  {
            nextarg = va_arg(theArgs, char **);
            if (nextarg == NULL)  {
                loopArg = ((ccInt) va_arg(theArgs, int)) - 1;
                if (loopArg < -1)  break;       }
            else  {
                size_t numBytes = (size_t) myArgInfo[loopArg].argIndices*typeSizes[myArgInfo[loopArg].argType];
                if (numBytes > 0)  memcpy((void *) nextarg, (void *) argv[loopArg], numBytes);
    }   }   }
    va_end(theArgs);
    
    return;
}
