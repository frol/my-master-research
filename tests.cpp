#include <iostream>
#include "libsbox.h"

int main()
{
    // MG - MacGuffin
    const int MG_sbox_count = 8;
    const int MG_n = 64;
    const int MG_m = 1;
    const int MG_input_length = 6;
    const int MG_output_length = 2;
    SBox *MG_sbox_list = new SBox[MG_sbox_count];
    int MG_sbox_F_list[MG_sbox_count][MG_n] = {
        2,0,0,3,3,1,1,0,0,2,3,0,3,3,2,1,1,2,2,0,0,2,2,3,1,3,3,1,0,1,1,2,
        0,3,1,2,2,2,2,0,3,0,0,3,0,1,3,1,3,1,2,3,3,1,1,2,1,2,2,0,1,0,0,3,

        3,1,1,3,2,0,2,1,0,3,3,0,1,2,0,2,3,2,1,0,0,1,3,2,2,0,0,3,1,3,2,1,
        0,3,2,2,1,2,3,1,2,1,0,3,3,0,1,0,1,3,2,0,2,1,0,2,3,0,1,1,0,2,3,3,

        2,3,0,1,3,0,2,3,0,1,1,0,3,0,1,2,1,0,3,2,2,1,1,2,3,2,0,3,0,3,2,1,
        3,1,0,2,0,3,3,0,2,0,3,3,1,2,0,1,3,0,1,3,0,2,2,1,1,3,2,1,2,0,1,2,

        1,3,3,2,2,3,1,1,0,0,0,3,3,0,2,1,1,0,0,1,2,0,1,2,3,1,2,2,0,2,3,3,
        2,1,0,3,3,0,0,0,2,2,3,1,1,3,3,2,3,3,1,0,1,1,2,3,1,2,0,1,2,0,0,2,

        0,2,2,3,0,0,1,2,1,0,2,1,3,3,0,1,2,1,1,0,1,3,3,2,3,1,0,3,2,2,3,0,
        0,3,0,2,1,2,3,1,2,1,3,2,1,0,2,3,3,0,3,3,2,0,1,3,0,2,1,0,0,1,2,1,

        2,2,1,3,2,0,3,0,3,1,0,2,0,3,2,1,0,0,3,1,1,3,0,2,2,0,1,3,1,1,3,2,
        3,0,2,1,3,0,1,2,0,3,2,1,2,3,1,2,1,3,0,2,0,1,2,1,1,0,3,0,3,2,0,3,

        0,3,3,0,0,3,2,1,3,0,0,3,2,1,3,2,1,2,2,1,3,1,1,2,1,0,2,3,0,2,1,0,
        1,0,0,3,3,3,3,2,2,1,1,0,1,2,2,1,2,3,3,1,0,0,2,3,0,2,1,0,3,1,0,2,

        3,1,0,3,2,3,0,2,0,2,3,1,3,1,1,0,2,2,3,1,1,0,2,3,1,0,0,2,2,3,1,0,
        1,0,3,1,0,2,1,1,3,0,2,2,2,2,0,3,0,3,0,2,2,3,3,0,3,1,1,1,1,0,2,3,
    };
    for (int i = 0; i < MG_sbox_count; ++i)
    {
        MG_sbox_list[i].set_params(MG_n, MG_m, MG_input_length, MG_output_length);
        MG_sbox_list[i].set(MG_sbox_F_list[i]);
        std::cout << "MG[" << (i+1) << "]: AC = " << MG_sbox_list[i].get_AC() << ", NL = " << MG_sbox_list[i].get_NL() << '\n';
        double current_diff = MG_sbox_list[i].differential_characteristic();
        double current_linear = MG_sbox_list[i].linear_characteristic();
        std::cout << "Diff current = " << current_diff << '\n';
        std::cout << "Linear current = " << current_linear << '\n';
    }

    // Anubis, Khazad (dublicate of Anubis)
    /*
    const int Anubis_sbox_count = 1;
    const int Anubis_n = 16;
    const int Anubis_m = 16;
    const int Anubis_input_length = 4;
    const int Anubis_output_length = 4;
    SBox *Anubis_sbox_list = new SBox[Anubis_sbox_count];
    int Anubis_sbox_F_list[Anubis_sbox_count][Anubis_n] = {
    };
    for (int i = 0; i < Anubis_sbox_count; ++i)
    {
        Anubis_sbox_list[i].set_params(Anubis_n, Anubis_m, Anubis_input_length, Anubis_output_length);
        Anubis_sbox_list[i].set(Anubis_sbox_F_list[i]);
        std::cout << "MG[" << (i+1) << "]: AC = " << Anubis_sbox_list[i].get_AC() << ", NL = " << Anubis_sbox_list[i].get_NL() << '\n';
    }
    */

    const int Noeken_sbox_count = 1;
    const int Noeken_n = 16;
    const int Noeken_m = 1;
    const int Noeken_input_length = 4;
    const int Noeken_output_length = 4;
    SBox *Noeken_sbox_list = new SBox[Noeken_sbox_count];
    int Noeken_sbox_F_list[Noeken_sbox_count][Noeken_n] = {
        0x7, 0xa, 0x2, 0xc, 0x4, 0x8, 0xf, 0x0, 0x5, 0x9, 0x1, 0xe, 0x3, 0xd, 0xb, 0x6
    };
    for (int i = 0; i < Noeken_sbox_count; ++i)
    {
        Noeken_sbox_list[i].set_params(Noeken_n, Noeken_m, Noeken_input_length, Noeken_output_length);
        Noeken_sbox_list[i].set(Noeken_sbox_F_list[i]);
        std::cout << "Noeken[" << (i+1) << "]: AC = " << Noeken_sbox_list[i].get_AC() << ", NL = " << Noeken_sbox_list[i].get_NL() << '\n';
    }

    const int SC2000_sbox_count = 1;
    const int SC2000_n = 16;
    const int SC2000_m = 1;
    const int SC2000_input_length = 4;
    const int SC2000_output_length = 4;
    SBox *SC2000_sbox_list = new SBox[SC2000_sbox_count];
    int SC2000_sbox_F_list[SC2000_sbox_count][SC2000_n] = {
        2, 5, 10, 12, 7, 15, 1, 11, 13, 6, 0, 9, 4, 8, 3, 14,
    };
    for (int i = 0; i < SC2000_sbox_count; ++i)
    {
        SC2000_sbox_list[i].set_params(SC2000_n, SC2000_m, SC2000_input_length, SC2000_output_length);
        SC2000_sbox_list[i].set(SC2000_sbox_F_list[i]);
        std::cout << "SC2000[" << (i+1) << "]: AC = " << SC2000_sbox_list[i].get_AC() << ", NL = " << SC2000_sbox_list[i].get_NL() << '\n';
    }

    const int Serpent_sbox_count = 8;
    const int Serpent_n = 16;
    const int Serpent_m = 1;
    const int Serpent_input_length = 4;
    const int Serpent_output_length = 4;
    SBox *Serpent_sbox_list = new SBox[Serpent_sbox_count];
    int Serpent_sbox_F_list[Serpent_sbox_count][Serpent_n] = {
        3,8,15,1,10,6,5,11,14,13,4,2,7,0,9,12,
        15,12,2,7,9,0,5,10,1,11,14,8,6,13,3,4,
        8,6,7,9,3,12,10,15,13,1,14,4,0,11,5,2,
        0,15,11,8,12,9,6,3,13,1,2,4,10,7,5,14,
        1,15,8,3,12,0,11,6,2,5,4,10,9,14,7,13,
        15,5,2,11,4,10,9,12,0,3,14,8,13,6,7,1,
        7,2,12,5,8,4,6,11,14,9,1,15,13,3,10,0,
        1,13,15,0,14,8,2,11,7,4,12,10,9,3,5,6,
    };
    for (int i = 0; i < Serpent_sbox_count; ++i)
    {
        Serpent_sbox_list[i].set_params(Serpent_n, Serpent_m, Serpent_input_length, Serpent_output_length);
        Serpent_sbox_list[i].set(Serpent_sbox_F_list[i]);
        std::cout << "Serpent[" << (i+1) << "]: AC = " << Serpent_sbox_list[i].get_AC() << ", NL = " << Serpent_sbox_list[i].get_NL() << '\n';
    }

    return 0;
}
