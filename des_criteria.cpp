#include <stdio.h>
#include "des_criteria.h"

const int S_LENGTH = 64;
/*
int S[S_LENGTH] = {
	// DES S1. Passed: S-2, S-3, S-4, S-5, S-6
    //*
	14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7, 
    0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8, 
    4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0, 
    15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13, 
    // */

	// DES S2. Passed: S-2, S-3, S-4, S-5, S-6
    /*
	15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10, 
    3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5, 
    0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15, 
    13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9, 
    // */
 
	// Splent. Passed: S-3, S-6
    /*
	3,8,15,1,10,6,5,11,14,13,4,2,7,0,9,12,
	15,12,2,7,9,0,5,10,1,11,14,8,6,13,3,4,
	8,6,7,9,3,12,10,15,13,1,14,4,0,11,5,2,
	0,15,11,8,12,9,6,3,13,1,2,4,10,7,5,14,
    // */

	// Gold. Passed: S-6
    /*
	0,3,5,8,6,9,0xc,7,0xd,0xa,0xe,4,1,0xf,0xb,2,
	0,3,5,8,6,0xa,0xf,4,0xe,0xd,9,2,1,7,0xc,0xb,
	0,3,5,8,6,0xc,0xb,7,9,0xe,0xa,0xd,0xf,2,1,4,
	0,3,5,8,6,0xc,0xb,7,0xa,4,9,0xe,0xf,1,2,0xd,
    // */
//};

bool s2(int *S)
{
    /*
    The S-boxes were chosen to minimize the difference between the number of
    1's and 0's when any single bit is held constant.
    */
    printf("s2 ");
    int mask, one_bits_count, output;
    for (int bit = 1; bit < S_LENGTH; bit <<= 1)
    {
        for (int fixed_bit = 0; fixed_bit <= 1; ++fixed_bit)
        {
            mask = 0x3f ^ bit;
            one_bits_count = 0;
            for (int x = 0; x < S_LENGTH; ++x)
            {
                if (x & mask == 0)
                    continue;
                output = fixed_bit ? S[S_index(x | bit)] : S[S_index(x & mask)];
                while(output != 0)
                {
                    if (output & 1 == 1)
                        ++one_bits_count;
                    output >>= 1;
                }
            }
            //printf("%d\n", one_bits_count);
            // Constants 120 and 136 was given experimentally for S1 and S2 DES S-boxes
            if (one_bits_count < 120 || one_bits_count > 136)
                return false;
        }
    }
    return true;
}

bool s3(int *S)
{
    /*
    S(x) != S(x ^ 0abcd0) for any a, b, c, d and abcd != 0000
    */
    printf("s3 ");
    for (int x = 0; x < S_LENGTH; ++x)
    {
        for (int abcd = 1; abcd < 16; ++abcd)
        {
            if (S[S_index(x)] == S[S_index(x ^ (abcd << 1))])
                return false;
        }
    }
    return true;
}

bool s4(int *S)
{
    /*
    Changing one bit of input of S-box results in changing at least two output bits.
    */
    printf("s4 ");
    int output_delta, bits_count;
    for (int input = 0; input < S_LENGTH; ++input)
    {
        for (int d = 1; d < S_LENGTH; d <<= 1)
        {
            output_delta = S[S_index(input)] ^ S[S_index(input ^ d)];
            bits_count = 0;
            while(output_delta != 0)
            {
                if (output_delta & 1 == 1)
                    ++bits_count;
                output_delta >>= 1;
            }
            if (bits_count < 2)
                return false;
        }
    }
    return true;
}

bool s5(int *S)
{
    /*
    S(x) and S(x ^ 001100) differ at least two bits.
    */
    printf("s5 ");
    int output_delta, bits_count;
    for (int input = 0; input < S_LENGTH; ++input)
    {
        output_delta = S[S_index(input)] ^ S[S_index(input ^ 0xC)];
        bits_count = 0;
        while(output_delta != 0)
        {
            if (output_delta & 1 == 1)
                ++bits_count;
            output_delta >>= 1;
        }
        if (bits_count < 2)
            return false;
    }
    return true;
}

bool s6(int *S)
{
    /*
    S(x) != S(x ^ 11ef00) for any e and f
    */
    printf("s6 ");
    for (int x = 0; x < S_LENGTH; ++x)
    {
        for (int ef = 0; ef < 4; ++ef)
        {
            if (S[S_index(x)] == S[S_index(x ^ (ef << 2) ^ 0x30)])
                return false;
        }
    }
    return true;
}


/*
bool s7(int *S)
{
    return true;
}
*/

bool testDES(int* S)
{
    return s2(S) && s3(S) && s4(S) && s5(S) && s6(S);
}
