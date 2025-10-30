Simple ZMQ test applications.  Run two instances and point them to each other.  They will initialise, create the binds/connections and then post messages.

zmq_test -h
usage: zmq_test [options]


Generic options:
  -h [ --help ]                  Print this help message

Configuration options:
  -p [ --pub ] arg               Publisher endpoint (eg tcp://*:10000)
  -s [ --sub ] arg               Subscriber endpoint (eg tcp://localhost:10000)
  -n [ --number ] arg (=20)      Number of messages sent and subscribed to
  -d [ --delay ] arg (=1000000)  Delay before first message and each 
                                 corresponding message (us)


Build requires cmake. For example

mkdir -p build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/test_zmq .. && \
    make -j8 VERBOSE=1 && \
    make install
