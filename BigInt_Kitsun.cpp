#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class BigInt {
public:
    BigInt(const std::string& hex);

    BigInt XOR(const BigInt& other) const;

    BigInt INV(const BigInt& other) const;

    BigInt OR(const BigInt& other) const;

    BigInt AND(const BigInt& other) const;

    BigInt shiftR(int n) const;

    BigInt shiftL(int n) const;

    BigInt ADD(const BigInt& other) const;

    BigInt SUB(const BigInt& other) const;

    BigInt MOD(const BigInt& other) const;

    void CheckSize();

    bool isZero() const;

    // Вивід числа у шістнадцятковому форматі.
    std::string toHex() const;

    void setHex(const std::string& hex);

    bool operator>=(const BigInt& other) const {
        if (number.size() > other.number.size()) {
            return true;
        }
        if (number.size() < other.number.size()) {
            return false;
        }
        for (int i = number.size() - 1; i >= 0; i--) {
            if (number[i] > other.number[i]) {
                return true;
            }
            if (number[i] < other.number[i]) {
                return false;
            }
        }
        return true;
    }

private:
    std::vector<unsigned int> number;
};

BigInt::BigInt(const std::string& hex) {
    const int blockSize = 8;  

    number.clear();

    for (int i = hex.length(); i > 0; i -= blockSize) {
        int start = std::max(i - blockSize, 0);
        int end = i;
        std::string hexBlock = hex.substr(start, end - start);
        unsigned int intBlock = std::stoul(hexBlock, 0, 16);
        number.push_back(intBlock);
    }

    CheckSize();
}

BigInt BigInt::XOR(const BigInt& other) const {
    BigInt result(*this); 

    int maxSize = std::max(result.number.size(), other.number.size());
    for (int i = 0; i < maxSize; ++i) {
        if (i >= result.number.size()) { result.number.push_back(0);}
        if (i < other.number.size()) { result.number[i] ^= other.number[i];}
    }
    result.CheckSize();
    return result;
}

BigInt BigInt::ADD(const BigInt& other) const {
    BigInt result(*this);

    int maxSize = std::max(number.size(), other.number.size());
    int carry = 0;

    for (int i = 0; i < maxSize || carry; ++i) {
        if (i == result.number.size()) {
            result.number.push_back(0);
        }

        unsigned long long sum = static_cast<unsigned long long>(carry);
        if (i < number.size()) {
            sum += static_cast<unsigned long long>(number[i]);
        }
        if (i < other.number.size()) {
            sum += static_cast<unsigned long long>(other.number[i]);
        }

        result.number[i] = static_cast<unsigned int>(sum);
        carry = static_cast<unsigned int>(sum >> 32);
    }
    return result;
}

BigInt BigInt::INV(const BigInt& other) const {
    BigInt result(*this); 

    for (int i = 0; i < result.number.size(); ++i) {
        result.number[i] = ~result.number[i];
    }
    result.CheckSize();
    return result;
}

BigInt BigInt::SUB(const BigInt& other) const {
    BigInt result(*this);

    int maxSize = std::max(result.number.size(), other.number.size());
    int borrow = 0; 
    for (int i = 0; i < maxSize; ++i) {
        if (i < other.number.size()) {
            int subtraction = result.number[i] - other.number[i] - borrow;
            if (subtraction < 0) {
                subtraction += 0x100000000; 
                borrow = 1; 
            }
            else { borrow = 0;}
            result.number[i] = subtraction;
        }
        else {
            int subtraction = result.number[i] - borrow;
            if (subtraction < 0) {
                subtraction += 0x100000000;
                borrow = 1;
            }
            else { borrow = 0;}
            result.number[i] = subtraction;
        }
    }
    result.CheckSize();
    return result;
}

void BigInt::CheckSize() {
    while (number.size() > 1 && number.back() == 0) {
        number.pop_back();
    }
}

bool BigInt::isZero() const {
    for (const unsigned int& block : number) {
        if (block != 0) {
            return false;
        }
    }
    return true;
}

std::string BigInt::toHex() const {
    std::string hexStr;

    for (int i = number.size() - 1; i >= 0; --i) {
        std::string blockHex = "0123456789ABCDEF";
        unsigned int block = number[i];
        std::string blockStr;

        while (block > 0) {
            blockStr = blockHex[block % 16] + blockStr;
            block /= 16;
        }
        if (blockStr.empty()) { blockStr = "0";}
        hexStr += blockStr;
    }
    hexStr = "0x" + hexStr;
    return hexStr;
}

void BigInt::setHex(const std::string& hex) {
    const int blockSize = 8;
    number.clear();

    for (int i = hex.length(); i > 0; i -= blockSize) {
        int start = std::max(i - blockSize, 0);
        int end = i;
        std::string hexBlock = hex.substr(start, end - start);
        unsigned int intBlock = std::stoul(hexBlock, 0, 16);
        number.push_back(intBlock);
    }

    CheckSize();
}

BigInt BigInt::OR(const BigInt& other) const {
    BigInt result(*this); 

    int maxSize = std::max(result.number.size(), other.number.size());
    for (int i = 0; i < maxSize; ++i) {
        if (i >= result.number.size()) { result.number.push_back(0); }
        if (i < other.number.size()) { result.number[i] |= other.number[i];}
    }
    result.CheckSize();
    return result;
}

BigInt BigInt::AND(const BigInt& other) const {
    BigInt result(*this); 

    int maxSize = std::max(result.number.size(), other.number.size());
    for (int i = 0; i < maxSize; ++i) {
        if (i >= result.number.size() || i >= other.number.size()) {
            result.number.push_back(0);
        }
        else { result.number[i] &= other.number[i]; }
    }
    result.CheckSize();

    return result;
}

BigInt BigInt::shiftR(int n) const {
    BigInt result(*this);

    if (n > 0) {
        int carry = 0;
        for (int i = 0; i < result.number.size(); ++i) {
            int shiftedBlock = result.number[i] >> n;
            result.number[i] = (result.number[i] >> n) | (carry << (32 - n));
            carry = shiftedBlock;
        }
        result.CheckSize();
    }
    return result;
}

BigInt BigInt::shiftL(int n) const {
    BigInt result(*this);

    if (n > 0) {
        int carry = 0;
        for (int i = result.number.size() - 1; i >= 0; --i) {
            int shiftedBlock = result.number[i] << n;
            result.number[i] = (result.number[i] << n) | carry;
            carry = shiftedBlock >> 32;
        }
    }

    result.CheckSize();
    return result;
}

BigInt BigInt::MOD(const BigInt& other) const {
    if (other.isZero()) { throw std::runtime_error("Division by zero"); }

    BigInt dividend(*this);
    BigInt divisor(other);
    BigInt quotient("0");
    BigInt remainder("0");

    int m = dividend.number.size();
    int n = divisor.number.size();

    std::reverse(divisor.number.begin(), divisor.number.end());

    for (int i = m - 1; i >= 0; i--) {
        remainder = remainder.shiftL(1);
        remainder.number[0] = dividend.number[i];
        int k = n;

        while (remainder >= divisor) {
            BigInt tempDivisor = divisor;
            int j = 0;

            while (remainder >= tempDivisor) {
                remainder = remainder.SUB(tempDivisor);
                j++;
                tempDivisor = tempDivisor.shiftL(1);
            }

            k = k - j;
            quotient.number[i / 32] |= (1 << (i % 32));
        }
    }
    std::reverse(remainder.number.begin(), remainder.number.end());
    remainder.CheckSize();
    return remainder;
}

int main() {

    // XOR
    BigInt numberA("0");
    numberA.setHex("51bf608414ad5726a3c1bec098f77b1b54ffb2787f8d528a74c1d7fde6470ea4");
    BigInt numberB("403db8ad88a3932a0b7e8189aed9eeffb8121dfac05c3512fdb396dd73f6331c");
    BigInt resultXOR = numberA.XOR(numberB);
    std::cout << "XOR Result: " << resultXOR.toHex() << std::endl;

    // ADD
    BigInt numberC("36f028580bb02cc8272a9a020f4200e346e276ae664e45ee80745574e2f5ab80");
    BigInt numberD("70983d692f648185febe6d6fa607630ae68649f7e6fc45b94680096c06e4fadb");
    BigInt resultADD = numberC.ADD(numberD);
    std::cout << "ADD Result: " << resultADD.toHex() << std::endl;

    // SUB
    BigInt numberE("33ced2c76b26cae94e162c4c0d2c0ff7c13094b0185a3c122e732d5ba77efebc");
    BigInt numberF("22e962951cb6cd2ce279ab0e2095825c141d48ef3ca9dabf253e38760b57fe03");
    BigInt resultSUB = numberE.SUB(numberF);
    std::cout << "SUB Result: " << resultSUB.toHex() << std::endl;

    // SHIFTL
    BigInt numberShiftL("0xabcd ");
    BigInt resultShiftL = numberShiftL.shiftL(3);
    std::cout << "ShiftL Result: " << resultShiftL.toHex() << std::endl;

    // SHIFTR
    BigInt numberShiftR("0xabcd ");
    BigInt resultShiftR = numberShiftR.shiftR(3);
    std::cout << "ShiftR Result: " << resultShiftR.toHex() << std::endl;
    
    // INV
    BigInt numberG("51bf608414ad5726a3c1bec098f77b1b54ffb2787f8d528a74c1d7fde6470ea4");
    BigInt resultINV = numberG.INV(numberG);
    std::cout << "INV Result: " << resultINV.toHex() << std::endl;

    // OR
    BigInt numberH("51bf608414ad5726a3c1bec098f77b1b54ffb2787f8d528a74c1d7fde6470ea4");
    BigInt numberI("403db8ad88a3932a0b7e8189aed9eeffb8121dfac05c3512fdb396dd73f6331c");
    BigInt resultOR = numberH.OR(numberI);
    std::cout << "OR Result: " << resultOR.toHex() << std::endl;

    // AND
    BigInt numberJ("51bf608414ad5726a3c1bec098f77b1b54ffb2787f8d528a74c1d7fde6470ea4");
    BigInt numberK("403db8ad88a3932a0b7e8189aed9eeffb8121dfac05c3512fdb396dd73f6331c");
    BigInt resultAND = numberJ.AND(numberK);
    std::cout << "AND Result: " << resultAND.toHex() << std::endl;

    // MOD
    BigInt numberL("123456789abcdef0123456789abcdef0");
    BigInt numberM("1000000000000000000000000000000");
    BigInt resultMOD = numberL.MOD(numberM);
    std::cout << "MOD Result: " << resultMOD.toHex() << std::endl;

    return 0;
}
