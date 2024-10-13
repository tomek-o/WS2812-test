//---------------------------------------------------------------------------

#ifndef RC4H
#define RC4H
//---------------------------------------------------------------------------

class Rc4
{
private:
    char Sbox[256];
    unsigned int i, j, t;
    char temp , k;
public:
	Rc4(void);
    void Init(const unsigned char* const key, unsigned int keylen, unsigned int discard = 0);
    void Process(unsigned char* data, unsigned int inplen);
};

#endif
