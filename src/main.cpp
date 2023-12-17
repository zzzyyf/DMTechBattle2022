#include "Common.h"
#include "Client.h"
#include "Debug.h"

using namespace dm;

const u32 nn = 1;
Client clients2[nn];

asio::io_context io_ctx;

bool divideInputToClients(const String& filename)
{
    FILE* input = fopen(filename.c_str(), "r");
    if (!input)
    {
        std::cout << "error opening file" << filename << ".\n";
        return false;
    }

    // get file size
    fseek(input, 0, SEEK_END);
    i64 len = ftell(input);
    if (len == 0)
    {
        std::cout << "input file " << filename << " is empty.\n";
        return false;
    }

    i64 start_offsets[nn];
    start_offsets[0] = 0;

    u32 i = 0;
    do
    {
        Client& cli2 = clients2[i];
        cli2.mId = i;
        FILE* f = fopen(filename.c_str(), "r");
        if (!f)
        {
            std::cout << "error opening file" << filename << ".\n";
            return false;
        }
        fseek(f, start_offsets[i], SEEK_SET);

        if (!cli2.setInput(f, start_offsets[i]))
            assert_r(false, false);

        i++;

        // find end offset (exclusive) of this client (i.e. begin offset of the next client)
        i64 seek_pos = i * (len / nn);
        if (seek_pos == len)    // at least try to read the last char
        {
            seek_pos--;
        }
        fseek(input, seek_pos, SEEK_SET);

        char c = 0;
        i32 rslt;
        do
        {
            rslt = fread(&c, 1, 1, input);
            if (rslt != 1)
            {
                std::cout << "bad input file, please check the endline.\n";
                return false;
            }
        } while (c != '\n');

        u32 part_len;
        if (i == nn)
        {
            part_len = len - start_offsets[i-1];
        }
        else
        {
            start_offsets[i] = ftell(input);
            part_len = start_offsets[i] - start_offsets[i-1];
        }

        std::cout << "client #" << i << " get " << part_len << " bytes.\n";
    } while (i < nn);

    return true;
}


int main(int argc, char* argv[])
{
    bool rslt;

    // rslt = generateInput("test.txt");
    // if (!rslt)
    //     return 1;

    rslt = divideInputToClients("test.txt");
    if (!rslt)
        return 1;

    i64 interval;
    {
        ScopeCounter counter(interval);
        for (u32 i = 0; i < nn; i++)
        {
            clients2[i].run();
            std::cout << "processed " << clients2[i].mReqCount << " requests.\n";
        }
    }
    std::cout << "Client cost " << interval << " milliseconds." << std::endl;

    return 0;
}
