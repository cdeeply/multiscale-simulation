#include "cicada.h"
#include "userfn.h"


extern ccBool downscaleTriggerF(ccInt, ccInt, ccFloat, ccInt, ccInt, ccFloat *, ccFloat *, ccFloat *);

extern int callSimCDNN(int, char **);

extern int runSim(int, char **, int(*)(int, char **));
