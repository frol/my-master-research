# DES S2
s = [15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
     3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
     0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
     13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9]

def DES_transpose(sbox):
    res_sbox = [0] * 64
    for x in range(64):
        res_sbox[x] = sbox[(((x >> 4) & 0x2) | (x & 0x1)) * 16 + ((x >> 1) & 0xf)] 
    return res_sbox

new_sbox = DES_transpose(s)

print(', '.join([str(x) for x in new_sbox]))
