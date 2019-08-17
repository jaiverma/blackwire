void *context;
void *publisher;

#ifndef BLACKWIRE_MQ_H
#define BLACKWIRE_MQ_H

void *mq_context;
void *mq_publisher;

int init_zeromq();
int deinit_zeromq();

#endif //BLACKWIRE_MQ_H