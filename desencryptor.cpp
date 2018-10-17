#include "desencryptor.h"

DESencryptor::DESencryptor()
{

}
template<size_t N>
void DESencryptor::printBitset(bitset<N> set)
{
    qDebug() << set.to_string().c_str();
}

template<size_t N>
bitset<N> DESencryptor::getBinary(std::string str)
{
    bitset<N> res;
    for(int i = N/8-1; i >= 0; i--)
    {
        //QChar ch = str[i];
        bitset<N> bit(str[i]);
        bit <<= N/8 * (N/8 - 1 - i);
        //printBitset<64>(bit);
        res |= bit;
    }
    //printBitset<N>(res);
    return res; //res is tested only with N == 8
}

QString DESencryptor::encrypt(QString src, QString key)
{
    bits64 data = getBinary<64>(src.toStdString());
    bits64 origKey = getBinary<64>(key.toStdString());

    data = firstShuffle(data); // first shuffle here. Checked once - OK

    bitset<32> R, L;

    for(uint8_t i = 0; i < 32; i++)
    {
        R[i] = data[i];
        L[i] = data[i+32];
    }

    printBitset<32>(R);
    printBitset<32>(L);

    for(uint8_t i = 0; i < 16; i++)
    {
        bitset<48> currKey = getKey(origKey, i+1);
        bitset<32> encryptRes = encryptFunction(R, currKey);
        L ^= encryptRes;
        bitset<32> buff = R;
        R = L;
        L = buff;
    }

    for(uint8_t i = 0; i < 32; i++)
    {
        data[i] = L[i];
        data[i+32] = R[i];
    }

    bits64 res;
    for(uint8_t i = 0; i < 64; i++)
        res[i] = data[final_shuffle_[i]-1];

    QString message = convertToString<64>(res);
    return message;
}

bits64 DESencryptor::firstShuffle(bits64 data)
{
    bits64 result;
    // here is zero-based bitset. Look through DES documentations and write "minus 1"
    unsigned int startBit = 57; // taken from first shuffle table
    for(unsigned int i = 0; i < 8; i++)
    {
        for(unsigned int j = 0; j < 8; j++)
        {
            //qDebug() << " writing"  << startBit - j * 8 << " th bit to " <<
              //     8*i+j << " th result\'s bit\n";
            result[8*i+j] = data[startBit - j * 8];
        }
        startBit += 2;
        if(startBit > 63)
            startBit = 56;
    }

    return result;
}

bitset<32> DESencryptor::encryptFunction(bitset<32> R, bitset<48> k)
{
    bitset<48> widedR = widenFunction(R); // widing R up to 48 bits
    widedR ^= k; // XOR for wided R and k

    vector<bitset<4>> Svector;
    // take 8 sets of 6-bit B bitsets
    for(unsigned int i = 0; i < 8; i++)
    {
        bitset<6> currB;
        for(unsigned int j = 0; j < 6; j++)
            currB[j] = widedR[i*6 + j];

        Svector.push_back(S(currB, i));
    }

    bitset<32> res;
    for(unsigned int i = 0; i < 8; i++)
        for(unsigned int j = 0; j < 4; j++)
            res[i*4+j] = Svector[i][j];

    res = shuffleP(res);

    return res;
}

bitset<48> DESencryptor::widenFunction(bitset<32> B)
{
    bitset<48> result;
    result[0] = B[31];
    result[47] = B[0];

    unsigned int currIndex = 2;

    // the formula seems to be al right
    for(unsigned int i = 0; i < 8; i++)
    {
        currIndex -= 2;
        for(unsigned int j = 0; j < 6; j++)
        {
            if((i == 0 && j == 0) ||
               (i == 7 && j == 5))
                continue;

            //qDebug() << " writing"  << currIndex << " th bit to " <<
              //     6*i+j << " th result\'s bit\n";

            result[i*6+j] = result[currIndex++];
        }
    }

        return result;
}

bitset<4> DESencryptor::S(bitset<6> B, unsigned int index)
{
    unsigned long line = 0, column = 0;

    bitset<2> lineOrig;
    lineOrig[0] = B[0];
    lineOrig[1] = B[5];

    bitset<4> columnOrig;
    for(unsigned int i = 0; i < 4; i ++)
        columnOrig[i] = B[i+1];

    line = lineOrig.to_ulong();
    column = columnOrig.to_ulong();

    uint8_t res = S_table_[index][line][column];

    bitset<4> result(res);

    return result;
}

bitset<32> DESencryptor::shuffleP(bitset<32> S)
{
    bitset<32> result;
    for(unsigned int i = 0; i < 32; i++)
        result[i] = S[P_shuffle_table_[i]-1];

    return result;
}

bitset<48> DESencryptor::getKey(bitset<64> originalKey, uint8_t iteration)
{
    bitset<56> shuffledKey = shuffleB(originalKey);
    printBitset<56>(shuffledKey);
    if(iteration == 1)
    {
        for(unsigned int i = 0; i < 28; i++)
        {
            oldC[i] = shuffledKey[i];
            oldD[i] = shuffledKey[i+28];
        }

        oldC = cycleShiftLeft<28>(oldC);
        oldD = cycleShiftLeft<28>(oldD);
    }
    else
    {
        for(uint8_t i = 0; i < S_shift_table_[iteration]; i++)
        {
            oldC = cycleShiftLeft<28>(oldC);
            oldD = cycleShiftLeft<28>(oldD);
        }
    }

    for(uint8_t i = 0; i < 28; i++)
    {
        shuffledKey[i] = oldC[i];
        shuffledKey[i+28] = oldD[i];
    }

    bitset<48> currentKey;
    for(uint8_t i = 0; i < 48; i++)
        currentKey[i] = shuffledKey[K_shuffle_table_[i]-1];

    return currentKey;

}

bitset<56> DESencryptor::shuffleB(bitset<64> key)
{
    bitset<56> result;
    printBitset<64>(key);


    for(unsigned int i = 0; i < 56; i++)
    {
        qDebug() << " writing"  << B_shuffle_table_[i]-1 << "th bit that to " <<
                    i << " th result\'s bit\n";
        result[i] = key[B_shuffle_table_[i]-1];
        qDebug() << "Result [" << i << "] now equals to " << result[i];
    }
    printBitset<56>(result);
    return result;
}

template<size_t N>
bitset<N> DESencryptor::cycleShiftLeft(bitset<N> bset)
{
    bitset<1> buffer;
    buffer[0] = bset[0];
    bset <<= 1;
    bset[N-1] = buffer[0];
    return bset;
}

template<size_t N>
QString DESencryptor::convertToString(bitset<N> msg)
{
    QString res(8,'.');

    uint8_t size = N /8;

    for(uint8_t i = 0; i < size; i++)
    {
        bitset<8> letter;
        for(uint8_t j = 0; j < 8; j++)
        {
            letter[j] = msg[i*8 + j];
            res[i] = (int)(letter.to_ulong());
        }
    }

    return res;
}
