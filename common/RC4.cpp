#include "RC4.h"
#include <string.h>
#include <stdio.h>

Rc4::Rc4(void) {
	i = j = t = 0;
	k = 0;
    temp = 0;
}

void Rc4::Init(const unsigned char* const key, unsigned int keylen, unsigned int discard)
{
	char Sbox2[256];
	i = j = t = 0;
	k = 0;
    temp = 0;

    //initialize sbox i
    for(i = 0; i < 256U; i++)
    {
        Sbox[i] = (char)i;
    }

    j = 0;

    //initialize the sbox2 with user key
    for(i = 0; i < 256U ; i++)
    {
        if(j == keylen)
        {
            j = 0;
        }
        Sbox2[i] = key[j++];
    }


    j = 0 ; //Initialize j
    //scramble sbox1 with sbox2
    for(i = 0; i < 256; i++)
    {
        j = (j + (unsigned int) Sbox[i] + (unsigned int) Sbox2[i]) % 256U ;
        temp =  Sbox[i];
        Sbox[i] = Sbox[j];
        Sbox[j] =  temp;
    }

    i = j = 0;

	//discard initial key fragment
	/** \note Ta pêtla po optymalizacji mo¿e wieszaæ na CGT 6.1.18, 6.1.21
	*/
    for(unsigned int x = 0; x < discard; x++)
    {
        //increment i
        i = (i + 1U) % 256U;
        //increment j
        j = (j + (unsigned int) Sbox[i]) % 256U;

        //Scramble SBox #1 further so encryption routine will
        //will repeat itself at great interval
        temp = Sbox[i];
        Sbox[i] = Sbox[j] ;
        Sbox[j] = temp;
    }
}

void Rc4::Process(unsigned char* data, unsigned int inplen)
{

	//	printf("Rc4::Process %d\n", inplen);
	/** \note Ta pêtla po optymalizacji mo¿e wieszaæ na CGT 6.1.18, 6.1.21
	*/
    for(unsigned int x = 0; x < inplen; x++)
    {
        //increment i
        i = (i + 1U) % 256U;
        //increment j
        j = (j + (unsigned int) Sbox[i]) % 256U;

        //Scramble SBox #1 further so encryption routine will
        //will repeat itself at great interval
        temp = Sbox[i];
        Sbox[i] = Sbox[j] ;
        Sbox[j] = temp;

        //Get ready to create pseudo random  byte for encryption key
        t = ((unsigned int) Sbox[i] + (unsigned int) Sbox[j]) %  256U ;

        //get the random byte
        k = Sbox[t];

        //xor with the data and done
        data[x] = static_cast<unsigned char>(data[x] ^ k);
    }
}
