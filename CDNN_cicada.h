//
//  CDNN_cicada.h
//  marble
//
//  Created by Brian Ross on 4/23/24.
//

#ifndef CDNN_cicada_h
#define CDNN_cicada_h

#include "userfn.h"


extern int callPtrSize(int, char **);
extern int callCDNNsize(int, char **);
extern int callGetNNdata1(int, char **);
extern int callGetNNdata2(int, char **);
extern int callGetNNdata3(int, char **);
extern int callGetTabularRegressor(int, char **);
extern int callGetTabularEncoder(int, char **);
extern int callRunCDNN(int, char **);
extern int callFreeCDNN(int, char **);

#endif /* CDNN_cicada_h */
