#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "libsbox.h"
#include "des_criteria.h"

//#define DES 1
//#define MC_GUFFIN 1
#define GOST 1

#ifdef DES
#define CIPHER_N 16
#define CIPHER_M 4
#define CIPHER_INPUT_LENGTH 6
#define CIPHER_OUTPUT_LENGTH 4
#define WISHED_NL 20
#define WISHED_AC 32
#endif

#ifdef MC_GUFFIN
#define CIPHER_N 16
#define CIPHER_M 4
#define CIPHER_INPUT_LENGTH 6
#define CIPHER_OUTPUT_LENGTH 2
#define WISHED_NL 20
#define WISHED_AC 32
#endif

#ifdef GOST
#define CIPHER_N 16
#define CIPHER_M 1
#define CIPHER_INPUT_LENGTH 4
#define CIPHER_OUTPUT_LENGTH 4
#define CIPHER_ROUNDS_COUNT 32
#define ENABLED_WISHED_BREAK
#define WISHED_MD_D 2
#define WISHED_MD_D_ 1
#define WISHED_ML_L 900
//#define WISHED_NL 4
//#define WISHED_AC 8
#endif


class SimulatedAnnealing
{
    int T0;
    int *ar_pos1, *ar_pos2;
    int pos_index;
    int n, m;
    int input_length, output_length;
    int X, R;
    int* precalc_powers;
    bool is_already_initialized;

    void generate_neighbor(SBox &sbox);
    void restore_generated_neighbor(SBox &sbox);

public:
    SimulatedAnnealing(int X, int R);
    ~SimulatedAnnealing();
    long double get_cost(SBox &sbox);
    void init(SBox &sbox);
    bool run(SBox &sbox, int MIL, int MaxIL, int MUL, double alpha);
};

SimulatedAnnealing::SimulatedAnnealing(int X, int R)
{
    T0 = 1000;
    pos_index = 0;
    this->X = X;
    this->R = R;
    this->is_already_initialized = false;
    this->ar_pos1 = this->ar_pos2 = NULL;
    this->precalc_powers = NULL;
}

SimulatedAnnealing::~SimulatedAnnealing()
{
    if (this->ar_pos1 != NULL)
    {
        delete[] this->ar_pos1;
        this->ar_pos1 = NULL;
    }
    if (this->ar_pos2 != NULL)
    {
        delete[] this->ar_pos2;
        this->ar_pos2 = NULL;
    }
    if (this->precalc_powers != NULL)
    {
        delete[] this->precalc_powers;
        this->precalc_powers = NULL;
    }
}

void SimulatedAnnealing::init(SBox &sbox)
{
    if (this->is_already_initialized)
        return;
    this->is_already_initialized = true;
    this->ar_pos1 = new int[sbox.input_combinations];
    this->ar_pos2 = new int[sbox.input_combinations];
    for (int i=0; i < sbox.input_combinations; ++i)
        this->ar_pos1[i] = this->ar_pos2[i] = i;
    random_shuffle(this->ar_pos1, sbox.input_combinations);
    random_shuffle(this->ar_pos2, sbox.input_combinations);

    int precalc_length = sbox.input_combinations * (sbox.output_combinations - 1);
    this->precalc_powers = new int[precalc_length];
    for (int x = 2; x < precalc_length; ++x)
    {
        this->precalc_powers[x] = x;
        for (int power = 1; power < R; ++power)
            this->precalc_powers[x] *= x;
    }
}

long double SimulatedAnnealing::get_cost(SBox &sbox)
{
    int sum = 0;
    for (int w = 0; w < sbox.input_combinations; ++w)
    {
        int WHT = -X;
        for (int i = 0; i < sbox.output_combinations - 1; ++i)
            for (int x = 0; x < sbox.input_combinations; ++x)
            {
                int wx = w & x;
                int wx_sum = 0;
                while(wx)
                {
                    wx_sum ^= wx & 0x1;
                    wx >>= 1;
                }
                WHT += (sbox.boolean_f[i][x] ^ wx_sum) ? -1 : 1;
            }
        sum += this->precalc_powers[WHT];
    }
    return sum;
}

void SimulatedAnnealing::generate_neighbor(SBox &sbox)
{
    // Generate Neighbor function
    while (true)
    {
        if (this->pos_index == sbox.input_combinations) {
            random_shuffle(this->ar_pos1, sbox.input_combinations);
            random_shuffle(this->ar_pos2, sbox.input_combinations);
            this->pos_index = 0;
        }
        if (this->ar_pos1[this->pos_index] != this->ar_pos2[this->pos_index])
            break;
        ++this->pos_index;
    }
    sbox.swap(this->ar_pos1[this->pos_index], this->ar_pos2[this->pos_index]);
}

void SimulatedAnnealing::restore_generated_neighbor(SBox &sbox)
{
    // restore generated neighbor to previous state
    sbox.swap(this->ar_pos1[this->pos_index], this->ar_pos2[this->pos_index]);
}

bool SimulatedAnnealing::run(SBox &sbox, int MIL, int MaxIL, int MUL, double alpha)
{
    this->init(sbox);

    SBox best_solution(sbox);
    SBox temp_F(sbox);
    best_solution.set(sbox);
    long double cost = get_cost(sbox);
    long double temp_cost, cost_delta;
    int best_NL = sbox.get_NL();
    int best_AC = sbox.get_AC();
    int best_MD_d, best_MD_d_, best_ML_l;
    sbox.get_MD_args(best_MD_d, best_MD_d_);
    sbox.get_ML_args(best_ML_l);
    int temp_NL, temp_AC, temp_MD_d, temp_MD_d_, temp_ML_l;

    double T = this->T0;
    int MFC = 0;
    temp_F.set(sbox);
    int loops = 0;
    for (int i = 0; i < MaxIL; ++i)
    {
        bool changed = false;
        for (int j = 0; j < MIL; ++j)
        {
            generate_neighbor(temp_F);
            temp_cost = get_cost(temp_F);

            cost_delta = temp_cost - cost;
            bool accepted = false;
            if (cost_delta < 0)
                accepted = true;
            else
            {
                double U = double(rand()) / double(RAND_MAX);
                if (U < exp(double(-cost_delta / T)))
                    accepted = true;
                else
                    restore_generated_neighbor(temp_F);
            }
            if (accepted)
            {
                changed = true;
                cost = temp_cost;
                temp_NL = temp_F.get_NL();
                temp_AC = temp_F.get_AC();
                temp_F.get_MD_args(temp_MD_d, temp_MD_d_);
                temp_F.get_ML_args(temp_ML_l);
                if (++loops == 1000)
                {
                    temp_F.print();
                    std::cout << "NL = " << temp_NL << "; AC = " << temp_AC << "; best NL = " << best_NL << "; best AC = " << best_AC << '\n';
                    loops = 0;
                }
                #ifdef ENABLED_WISHED_BREAK
                if (0 ||
                    #ifdef WISHED_MD_D
                    (temp_MD_d <= WISHED_MD_D) || (temp_MD_d_ <= WISHED_MD_D_) ||
                    #endif
                    #ifdef WISHED_ML_L
                    (temp_ML_l <= WISHED_ML_L) ||
                    #endif
                    #ifdef WISHED_NL
                    ((temp_NL >= WISHED_NL) && (temp_AC <= WISHED_AC)) ||
                    #endif
                    #ifdef DES_CRITERIA_IN_ANNEALING
                    (testDES(temp_F.F)) ||
                    #endif
                    0)
                {
                    std::cout << temp_MD_d << ' ' << temp_MD_d_ << ' ' << temp_ML_l << '\n';
                    sbox.set(temp_F);
                    return true;
                }
                #endif
                if ((best_NL < temp_NL) || (best_NL == temp_NL && best_AC > temp_AC))
                {
                    best_solution.set(temp_F);
                    best_NL = temp_NL;
                    best_AC = temp_AC;
                }
            }
            ++this->pos_index;
        }//for (MIL)
        if (!changed)
            ++MFC;
        else
            MFC = 0;
        if (MFC == MUL)
            break;
        T *= alpha;
    }//for (MaxIL)
    sbox.set(best_solution);
    return false;
}

int main()
{
    srand(time(NULL));
    #ifdef CIPHER_ROUNDS_COUNT
    SBox sbox(CIPHER_N, CIPHER_M, CIPHER_INPUT_LENGTH, CIPHER_OUTPUT_LENGTH, CIPHER_ROUNDS_COUNT);
    #else
    SBox sbox(CIPHER_N, CIPHER_M, CIPHER_INPUT_LENGTH, CIPHER_OUTPUT_LENGTH);
    #endif
    SimulatedAnnealing simulated_annealing(8, 3);
    double current_diff, current_linear, max_diff = 0, max_linear = 0;
    
    /*
    // Random
    //int F[] = {4, 11, 3, 15, 8, 3, 1, 4, 10, 2, 1, 7, 3, 2, 15, 8, 11, 2, 0, 8, 5, 5, 9, 12, 14, 10, 11, 14, 15, 10, 14, 6, 13, 5, 3, 6, 7, 5, 9, 6, 8, 7, 4, 2, 1, 10, 15, 0, 14, 9, 0, 12, 4, 11, 0, 12, 1, 7, 12, 9, 13, 13, 6, 13};
    // DES S2
    //int F[] = {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10, 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5, 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15, 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9};
    // DES S2 transposed to direct indexing
    //int F[] = {15, 3, 1, 13, 8, 4, 14, 7, 6, 15, 11, 2, 3, 8, 4, 14, 9, 12, 7, 0, 2, 1, 13, 10, 12, 6, 0, 9, 5, 11, 10, 5, 0, 13, 14, 8, 7, 10, 11, 1, 10, 3, 4, 15, 13, 4, 1, 2, 5, 11, 8, 6, 12, 7, 6, 12, 9, 0, 3, 5, 2, 14, 15, 9};
    // GOST test
    int F[] = {1, 2, 7, 10, 3, 4, 11, 14, 6, 15, 5, 9, 8, 12, 13, 0};
    sbox.set(F);
    sbox.print();
    sbox.print_boolean_f();
    double MD = sbox.get_MD();
    double ML = sbox.get_ML();
    std::cout << "AC = " << sbox.get_AC() << '\n';
    std::cout << "NL = " << sbox.get_NL() << '\n';
    #ifdef GOST
    std::cout << "MD = " << MD << " = 2^" << log2(MD) << '\n';
    std::cout << "ML = " << ML << " = 2^" << log2(ML) << '\n';
    #endif
    simulated_annealing.init(sbox);
    std::cout << "cost = " << simulated_annealing.get_cost(sbox) << '\n';
    //std::cout << "Test DES: " << (testDES(sbox.F) ? "Passed" : "Failed") << '\n';
    //*/
    //*
    while (true)
    {
        sbox.generate();
        simulated_annealing.run(sbox, 500, 300, 50, 0.95);
        std::cout << "Annealing finished. ";
        #ifdef DES_CRITERIA_AFTER_ANNEALING
        if (testDES(sbox.F))
        {
            std::cout << "DES passed\n";
        #endif
            sbox.print();
            //sbox.print_boolean_f();
            current_diff = sbox.differential_characteristic();
            if (current_diff > max_diff)
                max_diff = current_diff;
            current_linear = sbox.linear_characteristic();
            if (current_linear > max_linear)
                max_linear = current_linear;
            double MD = sbox.get_MD();
            double ML = sbox.get_ML();
            std::cout << "AC = " << sbox.get_AC() << "; NL = " << sbox.get_NL() \
                << "\nMD = " << MD << " = 2^" << log2(MD) << "\nML = " << ML << " = 2^" << log2(ML) << '\n' \
                << "Diff current/max = " << current_diff << '/' << max_diff \
                << "; Linear current/max = " << current_linear << '/' << max_linear << '\n';
        #ifdef DES_CRITERIA_AFTER_ANNEALING
        }
        else
            std::cout << "DES failed\n";
        #endif
        std::cout << '\n';
        #ifdef ONE_TIME
        break;
        #endif
    }
    //*/
    return 0;
}
