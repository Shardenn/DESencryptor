#ifndef DESENCRYPTOR_H
#define DESENCRYPTOR_H

#include <bitset>
#include <QString>
#include <QDebug>

using namespace std;

#define bits64 bitset<64>

class DESencryptor
{
public:
    DESencryptor();
    QString encrypt(QString src, QString key);
    QString decrypt(QString src, QString key);

private:
    bits64 sourceData_;
    bits64 sourceKey_;

    template<size_t N>
    bitset<N> getBinary(std::string str);

    template<size_t N>
    void printBitset(bitset<N> set);

    bits64 encryptFunction(bitset<32> R, bitset<48> k);
    bits64 firstShuffle(bits64 data);
    bits64 lastShuffle(bits64 data);
};

#endif // DESENCRYPTOR_H
