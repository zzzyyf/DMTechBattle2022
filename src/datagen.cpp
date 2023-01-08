#include "common.h"
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

const u32 nn = 100000;

bool generateInput(const String &filename)
{
    FILE* file = fopen(filename.c_str(), "w");
    if (!file)
    {
        std::cout << "failed to open file!\n";
        return false;
    }

    for (u32 i = 0; i < nn; ++i)
    {
        u64 n_num = getRandomU64() % 1000 + 1;

        for (u32 j = 0; j < n_num; j++)
        {
            String numstr = std::to_string(getRandomU64());
            if (j != n_num - 1)
                numstr += ',';
            fwrite(numstr.data(), numstr.size(), 1, file);
        }

        fwrite("\r\n", 2, 1, file);
    }

    fclose(file);
    return true;
}

}   // end of namespace dm;
