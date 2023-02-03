#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <string>
#include <exception>
#include <pqxx/pqxx>
#include <pqxx/except>
#include <pqxx/stream_to>
#include <ranges>
#include <string_view>
#include "dbconnector.h"


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
    explicit http_connection(tcp::socket socket): socket_(std::move(socket)), fsr() {}

    // Initiate the asynchronous operations associated with the connection.
    void
    start()
    {
        read_request();
    }

private:
    FileSystemRepository fsr;

    tcp::socket socket_;

    // The buffer for performing reads.
    beast::flat_buffer buffer_{8192};

    // The request message.
    http::request<http::string_body> request_;

    // The response message.
    http::response<http::dynamic_body> response_;

    // The timer for putting a deadline on connection processing.
    net::steady_timer deadline_{
        socket_.get_executor(), std::chrono::seconds(60)};

    // Asynchronously receive a complete request message.
    void read_request()
    {
        auto self = shared_from_this();
        http::async_read(
                socket_,
                buffer_,
                request_,
                [self](beast::error_code ec,
                       std::size_t bytes_transferred)
                {
                    boost::ignore_unused(bytes_transferred);
                    if(!ec)
                        self->process_request();
                });
    }

    // Determine what needs to be done with the request message.
    void process_request()
    {
        // Submit a lambda object to the pool.


        // TODO
        // pool.join();

        response_.version(request_.version());
        response_.keep_alive(false);

        response_.set(http::field::server, "Beast");
        create_response();
        write_response();
    }

    // Construct a response message based on the program state.
    void create_response()
    {
        response_.set(http::field::content_type, "text/html");
        response_.result(http::status::ok);

        std::string body = request_.body();
        std::string url(request_.target());
        auto method = request_.method();
        std::cout << url << "\n";
        std::vector <std::string> vec;
        boost::split(vec, url, boost::is_any_of("/"));
        int key = stoi(vec[2]);
        if (vec[1] == "get") beast::ostream(response_.body()) << fsr.get(key) << "\n";
        if (vec[1] == "del") fsr.del(key);
        if (vec[1] == "put") fsr.put(key, stoi(vec[3]));
    }

    // Asynchronously transmit the response message.
    void write_response()
    {
//        pool.join();
        auto self = shared_from_this();

        response_.content_length(response_.body().size());

        http::async_write(
                socket_,
                response_,
                [self](beast::error_code ec, std::size_t)
                {
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                    self->deadline_.cancel();
                });
    }
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
                          [&](beast::error_code ec)
                          {
                              if(!ec)
                                  std::make_shared<http_connection>(std::move(socket))->start();
                              http_server(acceptor, socket);
                          });
}

int main()
{
    try
    {
        std::string hostname;
        std::cin >> hostname;
        auto const address = net::ip::make_address(hostname);
        auto port = static_cast<unsigned short>(8080);

        net::io_context ioc{1};

        tcp::acceptor acceptor{ioc, {address, port}};
        tcp::socket socket{ioc};
        http_server(acceptor, socket);

        ioc.run();
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;

}

