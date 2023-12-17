#include "Common.h"
#include <ios>
#include <sstream>
#include <sys/random.h>
#include <random>

namespace dm {

std::random_device rd;
std::mt19937_64 gen(rd());
std::uniform_int_distribution<u64> distrib(0, UINT64_MAX);

u64 getRandomU64()
{
    return distrib(gen);
}

const u32 nn = 1'000'000;

bool generateInput(const String &filename)
{
    FILE* file = fopen(filename.c_str(), "w");
    if (!file)
    {
        std::cout << "failed to open file!\n";
        return false;
    }

    std::stringstream ss;
    for (u32 i = 0; i < nn; ++i)
    {
        u64 n_num = getRandomU64() % (1000 - 16) + 16;

        for (u32 j = 0; j < n_num; j++)
        {
            ss << getRandomU64();
            if (j != n_num - 1)
                ss << ',';
        }
        ss << "\r\n";

        String &&str = std::move(ss).str();
        fwrite(str.data(), str.size(), 1, file);
    }

    fclose(file);
    return true;
}

}   // end of namespace dm;


int main(int argc, char* argv[])
{
    bool rslt = dm::generateInput("test.txt");
    if (!rslt)
        return 1;

    return 0;
}
