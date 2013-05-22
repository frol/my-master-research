#ifndef libsbox
#define libsbox_h

void random_shuffle(int array[], int length);

class SBox
{
    int length;
    int byte_length;
    int byte_input_combinations;
    int byte_output_combinations;
    int cipher_rounds_count;
    
    void calculate_boolean_f();

public:
    int n, m;
    int input_length, output_length;
    int input_combinations, output_combinations;
    int* F;
    int** boolean_f;

    SBox();
    SBox(int n, int m, int input_length, int output_length, int cipher_rounds_count=0);
    SBox(SBox &sbox);
    ~SBox();

    void set_params(int n, int m, int input_length, int output_length, int cipher_rounds_count=0);
    void set(SBox &sbox);
    void set(int* F);
    int get_length();
    void generate();
    void print();
    void print_boolean_f();
    int get_NL();
    int get_AC();
    void get_MD_args(int& d, int& d_);
    double get_MD();
    void get_ML_args(int& l);
    double get_ML();
    void swap(int pos1, int pos2);

    double linear_characteristic();
    double differential_characteristic();
};

#endif
