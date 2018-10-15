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
    bits64 shuffled = firstShuffle(data); // first shuffle here. Checked once - OK

    bits64 res;
    return QString(res.to_string().c_str());
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
            qDebug() << " writing"  << startBit - j * 8 << " th bit to " <<
                   8*i+j << " th result\'s bit\n";
            result[8*i+j] = data[startBit - j * 8];
        }
        startBit += 2;
        if(startBit > 63)
            startBit = 56;
    }

    return result;
}
