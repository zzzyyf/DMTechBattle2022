#include "common.h"
#include "client.h"

using namespace dm;

const u32 nn = 4;
Client clients[nn];

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

    i64 start_offsets[nn];
    start_offsets[0] = 0;

    u32 i = 0;
    do
    {
        Client& cli = clients[i];
        cli.mId = i;
        FILE* f = fopen(filename.c_str(), "r");
        if (!f)
        {
            std::cout << "error opening file" << filename << ".\n";
            return false;
        }
        fseek(f, start_offsets[i], SEEK_SET);
        cli.mInput = f;

        i++;

        // find end offset (exclusive) of this client (i.e. begin offset of the next client)
        fseek(input, i * (len / nn), SEEK_SET);
        char c;
        i32 rslt;
        do
        {
            rslt = fread(&c, 1, 1, input);
        } while (rslt == 1 && c != '\n');

        if (rslt != 1)
        {
            std::cout << "bad input file, please check the endline.\n";
            return false;
        }

        if (i == nn)
        {
            cli.mTotalLen = len - start_offsets[i-1];
        }
        else
        {
            start_offsets[i] = ftell(input);
            cli.mTotalLen = start_offsets[i] - start_offsets[i-1];
        }

        std::cout << "client #" << i << " get " << cli.mTotalLen << " bytes.\n";
    } while (i < nn);

    return true;
}

void connectTCPClient(const String& remote)
{
    asio::error_code c;
    tcp::resolver resolver(io_ctx);
    tcp::resolver::results_type endpoints =
      resolver.resolve(remote, "dmtb2022");

    std::shared_ptr<dm::socket> sock = std::make_shared<dm::socket>(io_ctx);
    sock->open(stream_protocol(AF_INET, IPPROTO_TCP));
    
    for (auto &ep : endpoints)
    {
        sock->connect(ep.endpoint(), c);
        if (!c) break;
    }
}

void connectLocalClient(const String& remote)
{
    asio::error_code c;
    dm::socket s(io_ctx); 
    
    std::shared_ptr<dm::socket> sock = std::make_shared<dm::socket>(io_ctx);
    sock->open(stream_protocol(AF_UNIX, 0), c);
    
    auto ep = asio::local::stream_protocol::endpoint(remote);
    sock->connect(ep, c);
}


int main(int argc, char* argv[])
{
    bool rslt;
    /*
    rslt = generateInput("test.txt");
    if (!rslt)
        return 1;
    */

    rslt = divideInputToClients("test.txt");
    if (!rslt)
        return 1;

    for (auto &cli : clients)
    {
        cli.run();
        std::cout << "processed " << cli.mReqCount << " requests.\n";
    }
    return 0;
}
