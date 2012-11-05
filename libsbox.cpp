#include <iostream>
#include <algorithm>
#include <memory.h>
#include "libsbox.h"


void random_shuffle(int array[], int length)
{
    std::random_shuffle(array, array + length);
}


SBox::SBox(int n, int m, int input_length, int output_length)
{
    this->n = n;
    this->m = m;
    this->input_length = input_length;
    this->output_length = output_length;
    this->input_combinations = 1 << this->input_length;
    this->output_combinations = 1 << this->output_length;
    this->length = this->n * this->m;
    F = new int[this->length];
    this->byte_length = sizeof(F[0]) * this->length;

    // There are only (2**output_length - 1) non-trivial boolean functions.
    boolean_f = new int*[this->output_combinations - 1];
    for (int i = 1; i < this->output_combinations; ++i)
    {
        // Each boolean function is a vector of (2**input_length) elements.
        boolean_f[i - 1] = new int[this->input_combinations];
    }
}

SBox::SBox(SBox &sbox)
{
    this->n = sbox.n;
    this->m = sbox.m;
    this->input_length = sbox.input_length;
    this->output_length = sbox.output_length;
    this->input_combinations = 1 << this->input_length;
    this->output_combinations = 1 << this->output_length;
    this->length = this->n * this->m;
    F = new int[this->length];
    this->byte_length = sizeof(F[0]) * this->length;
    memcpy(F, sbox.F, this->byte_length);

    // There are only (2**output_length - 1) non-trivial boolean functions.
    boolean_f = new int*[this->output_combinations - 1];
    for (int i = 0; i < this->output_combinations - 1; ++i)
    {
        // Each boolean function is a vector of (2**input_length) elements.
        boolean_f[i] = new int[this->input_combinations];
        memcpy(boolean_f[i], sbox.boolean_f, this->byte_length);
    }
}

SBox::~SBox()
{
    if (F != NULL)
    {
        delete[] F;
        F = NULL;
    }
    if (boolean_f != NULL)
    {
        for (int i = 0; i < this->output_combinations - 1; ++i)
            delete[] boolean_f[i];
        delete[] boolean_f;
        boolean_f = NULL;
    }
}

void SBox::set(SBox &sbox)
{
    memcpy(this->F, sbox.F, this->byte_length);
    for (int i = 0; i < this->output_combinations - 1; ++i)
        memcpy(this->boolean_f[i], sbox.boolean_f[i], this->byte_length);
}

void SBox::set(int* F)
{
    memcpy(this->F, F, this->byte_length);
    this->calculate_boolean_f();
}

int SBox::get_length()
{
    return length;
}

void SBox::generate()
{
    for (int j = 0; j < m; ++j)
        for (int i = 0; i < n; ++i)
            F[n * j + i] = i;
    random_shuffle(F, n * m);
    this->calculate_boolean_f();
}

void SBox::print()
{
    for (int j = 0; j < m; ++j, std::cout << '\n')
        for (int i = 0; i < n; ++i)
            std::cout << F[n * j + i] << '\t';
}

void SBox::print_boolean_f()
{
    for (int i = 0; i < this->output_combinations - 1; ++i, std::cout << '\n')
        for (int j = 0; j < this->length; ++j)
            std::cout << boolean_f[i][j] << ' ';
}

int SBox::get_NL()
{
    int max = 0;
    for (int i = 0; i < this->output_combinations - 1; ++i)
        for (int w = 0; w < this->input_combinations; ++w)
        {
            int sum = 0;
            for (int x = 0; x < this->input_combinations; ++x)
            {
                int wx = w & x;
                int wx_sum = 0;
                while(wx)
                {
                    wx_sum ^= wx & 0x1;
                    wx >>= 1;
                }
                sum += (boolean_f[i][x] ^ wx_sum) ? -1 : 1;
            }
            if (sum > max)
                max = sum;
        }
    return max;
}

int SBox::get_AC()
{
    int max = 0;
    for (int i = 0; i < this->output_combinations - 1; ++i)
        for (int delta = 1; delta < this->input_combinations; ++delta)
        {
            int sum = 0;
            for (int x = 0; x < this->input_combinations; ++x)
                sum += (boolean_f[i][x] ^ boolean_f[i][x ^ delta]) ? -1 : 1;
            if (sum > max)
                max = sum;
        }
    return max;
}

void SBox::swap(int pos1, int pos2)
{
    std::swap(F[pos1], F[pos2]);
    // TODO: Optimize this to change only swapped bits.
    this->calculate_boolean_f();
}

void SBox::calculate_boolean_f()
{
    // TODO: Optimize this to store 32 bits instead on 1 bit in one array element.
    int** full_boolean_f = new int*[this->output_length];
    for (int i = 0; i < this->output_length; ++i)
    {
        full_boolean_f[i] = new int[this->length];
        for (int j = 0; j < this->length; ++j)
        {
            full_boolean_f[i][j] = 0;
            for (int k = 0; k <= j; ++k)
                if ((j & k) == k)
                    full_boolean_f[i][j] ^= (F[k] >> i) & 0x1;
        }
    }
    // Calculate linear combinations of coordinative (full) boolean functions
    for (int i = 0; i < this->output_combinations - 1; ++i)
    {
        memset(boolean_f[i], 0, sizeof(boolean_f[0][0]) * this->input_combinations);
        for (int j = 0, f_index = i + 1; f_index; ++j, f_index >>= 1)
            if (f_index & 1)
                for (int k = 0; k < this->input_combinations; ++k)
                    boolean_f[i][k] ^= full_boolean_f[j][k];
    }
    for (int i = 0; i < this->output_length; ++i)
        delete[] full_boolean_f[i];
    delete[] full_boolean_f;
}
