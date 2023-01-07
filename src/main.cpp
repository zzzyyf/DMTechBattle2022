#include "common.h"
#include "client.h"

using namespace dm;

const u32 nn = 16;
Client clients[nn];

asio::io_context io_ctx;

void divideInputToClients(const String& filename)
{
    FILE* input = fopen(filename.c_str(), "r");
    // get file size
    fseek(input, 0, SEEK_END);
    i64 len = ftell(input);

    i64 start_offsets[nn];
    u32 i = 0;
    do
    {
        Client& cli = clients[i];
        cli.mId = i;
        FILE* f = fopen(filename.c_str(), "r");

        cli.mStartOffset = start_offsets[i];
        fseek(f, cli.mStartOffset, SEEK_SET);
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

        if (rslt == 1)
        {
            assert(i == nn);
            cli.mEndOffsetEx = len;
        }
        else
        {
            cli.mEndOffsetEx = ftell(input);
            start_offsets[i] = cli.mEndOffsetEx;
        }
    } while (i < nn);
}

void connectTCPClient(const String& remote)
{
    asio::error_code c;
    tcp::resolver resolver(io_ctx);
    tcp::resolver::results_type endpoints =
      resolver.resolve(remote, "daytime");

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
    return 0;
}
