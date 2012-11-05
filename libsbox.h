#ifndef libsbox
#define libsbox_h

void random_shuffle(int array[], int length);

class SBox
{
    int length;
    int byte_length;
    
    void calculate_boolean_f();

public:
    int n, m;
    int input_length, output_length;
    int input_combinations, output_combinations;
    int* F;
    int** boolean_f;

    SBox(int n, int m, int input_length, int output_length);
    SBox(SBox &sbox);
    ~SBox();

    void set(SBox &sbox);
    void set(int* F);
    int get_length();
    void generate();
    void print();
    void print_boolean_f();
    int get_NL();
    int get_AC();
    void swap(int pos1, int pos2);
};

#endif
