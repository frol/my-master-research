#include <iostream>
#include <algorithm>
#include <memory.h>
#include <math.h>
#include "libsbox.h"
#include "des_criteria.h"

#define DEBUG

void random_shuffle(int array[], int length)
{
    std::random_shuffle(array, array + length);
}


SBox::SBox()
{
}

SBox::SBox(int n, int m, int input_length, int output_length, int cipher_rounds_count)
{
    this->set_params(n, m, input_length, output_length, cipher_rounds_count);
}

void SBox::set_params(int n, int m, int input_length, int output_length, int cipher_rounds_count)
{
    this->n = n;
    this->m = m;
    this->input_length = input_length;
    this->output_length = output_length;
    this->cipher_rounds_count = cipher_rounds_count;
    this->input_combinations = 1 << this->input_length;
    this->output_combinations = 1 << this->output_length;
    this->length = this->n * this->m;
    F = new int[this->length];
    this->byte_length = sizeof(F[0]) * this->length;

    // There are only (2**output_length - 1) non-trivial boolean functions.
    boolean_f = new int*[this->output_combinations - 1];
    for (int i = 0; i < this->output_combinations - 1; ++i)
    {
        // Each boolean function is a vector of (2**input_length) elements.
        boolean_f[i] = new int[this->input_combinations];
    }
    this->byte_input_combinations = sizeof(boolean_f[0][0]) * this->input_combinations;
    this->byte_output_combinations = sizeof(boolean_f[0][0]) * this->output_combinations;
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
    this->byte_input_combinations = sizeof(boolean_f[0][0]) * this->input_combinations;
    this->byte_output_combinations = sizeof(boolean_f[0][0]) * this->output_combinations;
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
        memcpy(this->boolean_f[i], sbox.boolean_f[i], this->byte_input_combinations);
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
            F[n * j + i] = i % this->output_combinations;
    random_shuffle(F, n * m);
    this->calculate_boolean_f();
}

void SBox::print()
{
    for (int j = 0; j < m; ++j, std::cout << '\n')
        for (int i = 0; i < n; ++i)
            std::cout << F[n * j + i] << ", ";
}

void SBox::print_boolean_f()
{
    for (int i = 0; i < this->output_combinations - 1; ++i, std::cout << '\n')
        for (int j = 0; j < this->length; ++j)
            std::cout << boolean_f[i][j] << ' ';
}

void SBox::print_MD_ML_by_rounds()
{
    for (int r = 1; r <= this->cipher_rounds_count; ++r)
    {
        double r_MD = this->get_MD(r);
        double r_ML = this->get_ML(r);
        std::cout << "Round #" << r << ": MD = 2^" << log2(r_MD) << ", ML = 2^" << log2(r_ML) << '\n';
    }
}

/* Nonlinearity
Note: Useful for DES like S-boxes */
int SBox::get_NL()
{
    int WHT_max = 0;
    for (int i = 0; i < this->output_combinations - 1; ++i)
        for (int w = 0; w < this->input_combinations; ++w)
        {
            int sum = 0;
            for (int x = 0; x < this->input_combinations; ++x)
            {
                /* // There is the built-in gcc function for count bits - __builtin_popcount
                int wx = w & x;
                int wx_sum = 0;
                while(wx)
                {
                    wx_sum ^= wx & 0x1;
                    wx >>= 1;
                }
                */
                sum += (boolean_f[i][x] ^ __builtin_popcount(w & x) & 0x1) ? -1 : 1;
            }
            if (sum < 0)
                sum = -sum;
            if (sum > WHT_max)
                WHT_max = sum;
        }
    return (this->input_combinations - WHT_max) >> 1;
}

/* Autocorrelation
Note: Useful for DES like S-boxes */
int SBox::get_AC()
{
    int max = 0;
    for (int i = 0; i < this->output_combinations - 1; ++i)
        for (int delta = 1; delta < this->input_combinations; ++delta)
        {
            int sum = 0;
            for (int x = 0; x < this->input_combinations; ++x)
                sum += (boolean_f[i][x] ^ boolean_f[i][x ^ delta]) ? -1 : 1;
            if (sum < 0)
                sum = -sum;
            if (sum > max)
                max = sum;
        }
    return max;
}

/* Mathematical expectation for differential cryptoanalysis
Note: Useful for GOST like S-boxes */
void SBox::get_MD_args(int& d, int& d_)
{
    d = 0;
    d_ = 0;
    for (int alpha = 1; alpha < this->output_combinations; ++alpha)
        for (int beta = 1; beta < this->output_combinations; ++beta)
        {
            int sum_d_a0 = 0;
            int sum_d_a1 = 0;
            for (int k = 0; k < this->output_combinations; ++k)
            {
                // delta(a, b) == (1 if a == b else 0)
                if ((this->F[S_index((k + alpha) & (this->output_combinations - 1))] ^ F[S_index(k)]) == beta)
                    // v(k, alpha) is the carry bit
                    if (k + alpha >= this->output_combinations)
                        ++sum_d_a1;
                    else
                        ++sum_d_a0;
            }
            if (sum_d_a0 > d_)
                d_ = sum_d_a0;
            if (sum_d_a1 > d_)
                d_ = sum_d_a1;
            if (sum_d_a0 + sum_d_a1 > d)
                d = sum_d_a0 + sum_d_a1;
        }
}

double SBox::get_MD(int cipher_round)
{
    int d, d_;
    this->get_MD_args(d, d_);
    if (cipher_round == 0)
        cipher_round = this->cipher_rounds_count;
    return std::max(
        (
            pow(double(d) / this->output_combinations, cipher_round + 1 - 2 * ceil(cipher_round / 3.0))
            * pow(double(d_) / this->output_combinations, ceil(cipher_round / 3.0))
        ),
        pow(double(d) / this->output_combinations, cipher_round - 1)
    );
}

/* Mathematical expectation for linear cryptoanalysis
Note: Useful for GOST like S-boxes */
void SBox::get_ML_args(int& l)
{
    l = 0;
    for (int alpha = 1; alpha < this->output_combinations; ++alpha)
        for (int beta = 1; beta < this->output_combinations; ++beta)
        {
            int k_sum = 0;
            for (int k = 0; k < this->output_combinations; ++k)
            {
                int x_sum = 0;
                for (int x = 0; x < this->output_combinations; ++x)
                    x_sum += ((__builtin_popcount(beta & this->F[S_index((x + k) & (this->output_combinations - 1))]) ^ __builtin_popcount(alpha & x)) & 0x1) ? -1 : 1;
                k_sum += x_sum * x_sum;
            }
            if (k_sum > l)
                l = k_sum;
        }
}

double SBox::get_ML(int cipher_round)
{
    int l;
    this->get_ML_args(l);
    if (cipher_round == 0)
        cipher_round = this->cipher_rounds_count;
    return pow(
        double(l) / this->output_combinations / this->output_combinations / this->output_combinations,
        round(2.0 / 3 * cipher_round)
    );
}

void SBox::swap(int pos1, int pos2)
{
    std::swap(F[pos1], F[pos2]);
    // TODO: Optimize this to change only swapped bits.
    this->calculate_boolean_f();
}

void SBox::calculate_boolean_f()
{
    // TODO: Optimize this to store 32 bits instead of 1 bit in one array element.
    // There is a boolean_optimizations branch!
    int** full_boolean_f = new int*[this->output_combinations - 1];
    for (int i = 0; i < this->output_combinations - 1; ++i)
    {
        full_boolean_f[i] = new int[this->input_combinations];
        memset(full_boolean_f[i], 0, this->byte_input_combinations);
        for (int j = 0; j < this->length; ++j)
        {
            full_boolean_f[i][j] ^= (F[j] >> i) & 0x1;
        }
    }
    // Calculate linear combinations of coordinative (full) boolean functions
    for (int i = 0; i < this->output_combinations - 1; ++i)
    {
        memset(boolean_f[i], 0, this->byte_input_combinations);
        for (int j = 0, f_index = i + 1; f_index; ++j, f_index >>= 1)
            if (f_index & 1)
                for (int k = 0; k < this->input_combinations; ++k)
                    boolean_f[i][k] ^= full_boolean_f[j][k];
    }
    for (int i = 0; i < this->output_combinations - 1; ++i)
        delete[] full_boolean_f[i];
    delete[] full_boolean_f;
}

double SBox::linear_characteristic()
{
    char lat[this->input_combinations][this->output_combinations];
    char max = 0;
    char value;
    memset(lat, char(-(this->input_combinations >> 1)),
        sizeof(lat[0][0]) * this->input_combinations * this->output_combinations);
    for (int mx = 0; mx < this->input_combinations; ++mx)
    {
        for (int my = 0; my < this->output_combinations; ++my)
            for (int x = 0; x < this->input_combinations; ++x)
                if ((__builtin_popcount(x & mx) & 0x1) == (__builtin_popcount(this->F[S_index(x)] & my) & 0x1))
                    ++lat[mx][my];
        for (int my = 1; my < this->output_combinations; ++my)
        {
            value = lat[mx][my];
            if (value < 0)
                value = -value;
            if (max < value)
                max = value;
        }
    }
    #ifdef DEBUG
    std::cout << "Linear:\n";
    for (int y = 0; y < this->input_combinations; ++y, std::cout << '\n')
        for (int x = 0; x < this->output_combinations; ++x)
            std::cout << int(lat[x][y]) << "\t";
    #endif
    return double(max) / (this->input_combinations >> 1);
}

double SBox::differential_characteristic()
{
    char diff_table[this->input_combinations][this->output_combinations];
    char max = 0;
    char value;
    memset(diff_table, 0,
        sizeof(diff_table[0][0]) * this->input_combinations * this->output_combinations);
	for (int deltaX = 0; deltaX < this->input_combinations; ++deltaX)
	{
		for (int x = 0; x < this->input_combinations; ++x)
			++diff_table[deltaX][this->F[S_index(x)] ^ this->F[S_index(x ^ deltaX)]];
        if (deltaX != 0)
            for (int y = 0; y < this->output_combinations; ++y)
            {
                if (diff_table[deltaX][y] > max)
                    max = diff_table[deltaX][y];
            }    
    }
    #ifdef DEBUG
    std::cout << "Diff:\n";
    for (int y = 0; y < this->input_combinations; ++y, std::cout << '\n')
        for (int x = 0; x < this->output_combinations; ++x)
            std::cout << int(diff_table[y][x]) << "\t";
    #endif
    return double(max);
}
