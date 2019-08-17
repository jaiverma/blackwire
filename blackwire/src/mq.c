#include "mq.h"

#include <zmq.h>

int init_zeromq() {
    mq_context = zmq_ctx_new();
    mq_publisher = zmq_socket(mq_context, ZMQ_PUB);
    int rc = zmq_bind(mq_publisher, "tcp://*:5556");
    return rc;
}

int deinit_zeromq() {
    zmq_close(mq_publisher);
    zmq_ctx_destroy(mq_context);
    return 0;
}
