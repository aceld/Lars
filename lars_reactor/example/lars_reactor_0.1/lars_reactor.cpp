#include "tcp_server.h"

int main() {

    tcp_server server("127.0.0.1", 7777);
    server.do_accept();

    return 0;
}
