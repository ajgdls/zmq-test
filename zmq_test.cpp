#include <iostream>
#include <thread>
//#include <string>
//#include <fstream>
//#include <streambuf>
using namespace std;

#include "zmq/zmq.hpp"
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;


class ZMQTest
{
  private:
    std::string pub_;
    std::string sub_;
    int32_t number_;
    int32_t delay_;
    zmq::context_t zmq_context_;
    zmq::socket_t socket_tx_;
    zmq::socket_t socket_rx_;
    int socket_type_;
    std::map<std::string, std::string> bound_endpoints_;
    
  public:
    ZMQTest():
        socket_tx_(zmq_context_, ZMQ_PUB),
        socket_rx_(zmq_context_, ZMQ_SUB)
    {
        // Create ZMQ context, set TX port and RX port
    }

    ~ZMQTest()
    {
    }

    void run()
    {
        // Bind the rx socket
        socket_rx_.connect(sub_.c_str());
        char resolved_endpoint[256];
        std::size_t endpoint_size = sizeof(resolved_endpoint);
        socket_rx_.getsockopt(ZMQ_LAST_ENDPOINT, resolved_endpoint, &endpoint_size);
        std::cout << "Subscribe socket bound to: " << resolved_endpoint << std::endl;
        bound_endpoints_[sub_] = std::string(resolved_endpoint);
        // Set default subscription on subscriber
        socket_rx_.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        socket_tx_.bind(pub_.c_str());
        endpoint_size = sizeof(resolved_endpoint);
        socket_tx_.getsockopt(ZMQ_LAST_ENDPOINT, resolved_endpoint, &endpoint_size);
        std::cout << "Publish socket connected to: " << resolved_endpoint << std::endl;
        bound_endpoints_[pub_] = std::string(resolved_endpoint);

        // Create thread on the stack which subscribes
        std::thread th(&ZMQTest::subscription, this);

        // Now perform the publishing
        this->publish();

        // Wait for the subscription to finish
        th.join();
    }

    void publish()
    {
        // loop number_ of times
        for ( int i = 1; i <= number_; i++ )
        {
            // delay_ send cycle
            usleep(delay_);
            std::stringstream ss;
            ss << "Message " << i << " from " << pub_;
            // Create zmq message
            zmq::message_t request(ss.str().length());
            // Copy contents to zmq message
            memcpy(request.data(), ss.str().c_str(), ss.str().length());
            // Publish the message
            socket_tx_.send( request );
            std::cout << "Publishing: " << i << " from " << pub_ << std::endl;
        }
    }

    void subscription()
    {
        // infinite loop to receive messages
        for ( int i = 1; i <= number_; i++ )
        {
            // Receive the message and convert to string
            zmq::message_t update;
            socket_rx_.recv(&update);
            char msg[256];
            memcpy(msg, update.data(), update.size());
            // Print the message
            std::cout << "Num: " << i << ", message: " << msg << std::endl;
        }
    }
    
    //! Parse command-line arguments.
    //!
    //! \param argc - standard command-line argument count
    //! \param argv - array of char command-line options
    //! \return return code:
    //!   * -1 if option parsing succeeded and we should continue running the application
    //!   *  0 if specific command completed (e.g. --help) and we should exit with success
    //!   *  1 if option parsing failed and we should exit with failure
    int parse_arguments(int argc, char** argv)
    {
        int rc = -1;
        try
        {
            std::string config_file;

            // Declare a group of options that will allowed only on the command line
            po::options_description generic("Generic options");
            generic.add_options()
                ("help,h",
                "Print this help message")
                ;

            po::options_description config("Configuration options");
            config.add_options()
                ("pub,p", po::value<std::string>(),
                "Publisher endpoint (eg tcp://*:10000)")
                ("sub,s", po::value<std::string>(),
                "Subscriber endpoint (eg tcp://localhost:10000)")
                ("number,n", po::value<int32_t>()->default_value(20),
                "Number of messages sent and subscribed to")
                ("delay,d", po::value<int32_t>()->default_value(1000000),
                "Delay before first message and each corresponding message (us)")
                ;

            po::options_description cmdline_options;
            cmdline_options.add(generic).add(config);

            // Parse the command line options
            po::variables_map vm;
            po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
            po::notify(vm);

            // If the command-line help option was given, print help and exit
            if (vm.count("help"))
            {
            std::cout << "usage: zmq_test [options]" << std::endl << std::endl;
            std::cout << cmdline_options << std::endl;
            return 0;
            }

            if (vm.count("pub"))
            {
            pub_ = vm["pub"].as<std::string>();
            std::cout << "Pub channel set to " << pub_ << std::endl;
            }

            if (vm.count("sub"))
            {
            sub_ = vm["sub"].as<std::string>();
            std::cout << "Sub channel set to " << sub_ << std::endl;
            }

            number_ = vm["number"].as<int32_t>();
            std::cout << "Number of messages set to " << number_ << std::endl;

            delay_ = vm["delay"].as<int32_t>();
            std::cout << "Delay in us between messages " << delay_ << std::endl;

        }
        catch (po::unknown_option &e)
        {
            std::cout << "Error parsing command line arguments: " << e.what() << std::endl;
            rc = 1;
        }
        catch (exception &e)
        {
            std::cout << "Got exception during command line argument parsing: " << e.what() << std::endl;
            rc = 1;
        }
        catch (...)
        {
            std::cout << "Got unknown exception during command line argument parsing" << std::endl;
            throw;
        }

        return rc;
    }
};


//! Main application entry point
int main (int argc, char** argv)
{
  int rc = -1;

  // Create the test app instance
  ZMQTest app;

  // Parse command line arguments and set up node configuration
  rc = app.parse_arguments(argc, argv);

  if (rc == -1) {
    // Run the application
    app.run();
  }

  return rc;
}
