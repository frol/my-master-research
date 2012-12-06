#ifndef des_criteria
#define des_criteria

//#define S_index(index) (((((index) & 0x20) >> 4) | ((index) & 1)) * 16 + (((index) >> 1) & 0xf))
#define S_index(index) (index)

bool testDES(int *S);

#endif
