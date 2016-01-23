//
// stream_server.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdio>
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

namespace asio = boost::asio;

namespace   {
  class session : public boost::enable_shared_from_this<session>  {
  public:
    session(asio::io_service& io_service) : socket_(io_service)
    {  }

    asio::local::stream_protocol::socket& socket()  {  return socket_;  }

    void start()  {
      socket_.async_read_some(asio::buffer(data_),
			      boost::bind(&session::handle_read,
					  shared_from_this(),
					  asio::placeholders::error,
					  asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred)  {
      if (!error)      {
	asio::async_write(socket_,
			  asio::buffer(data_, bytes_transferred),
			  boost::bind(&session::handle_write,
				      shared_from_this(),
				      asio::placeholders::error));
      }
    }

    void handle_write(const boost::system::error_code& error)  {
      if (!error)      {
	socket_.async_read_some(asio::buffer(data_),
				boost::bind(&session::handle_read,
					    shared_from_this(),
					    asio::placeholders::error,
					    asio::placeholders::bytes_transferred));
      }
    }

  private:
    // The socket used to communicate with the client.
    asio::local::stream_protocol::socket socket_;
    // Buffer used to store data received from the client.
    boost::array<char, 1024> data_;
  };

  typedef boost::shared_ptr<session> session_ptr;

  class server   {
  public:
    server(asio::io_service& io_service, const std::string& file)
      : io_service_(io_service), acceptor_(io_service, asio::local::stream_protocol::endpoint(file)) {
      session_ptr new_session(new session(io_service_));
      acceptor_.async_accept(new_session->socket(),
			     boost::bind(&server::handle_accept, this, new_session,
					 asio::placeholders::error));
    }

    void handle_accept(session_ptr new_session, const boost::system::error_code& error)  {
      if (!error)      {
	new_session->start();
      }
      new_session.reset(new session(io_service_));
      acceptor_.async_accept(new_session->socket(),
			     boost::bind(&server::handle_accept, this, new_session,
					 asio::placeholders::error));
    }

  private:
    asio::io_service& io_service_;
    asio::local::stream_protocol::acceptor acceptor_;
  };
}

extern "C" int boost_asio_unix_sockets_server(int argc, char** argv)   {
  using namespace std;
  try  {
    if (argc != 2)    {
      cerr << "Usage: stream_server <file>\n";
      cerr << "*** WARNING: existing file is removed ***\n";
      return 1;
    }

    asio::io_service io_service;

    remove(argv[1]);
    server s(io_service, argv[1]);

    io_service.run();
  }
  catch (std::exception& e)  {
    cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}

//
// stream_client.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
extern "C" int boost_asio_unix_sockets_client(int argc, char** argv)   {
  using namespace std;
  enum { max_length = 1024 };
  try  {
    if (argc != 2)    {
      cerr << "Usage: stream_client <file>\n";
      return 1;
    }

    asio::io_service io_service;

    asio::local::stream_protocol::socket s(io_service);
    s.connect(asio::local::stream_protocol::endpoint(argv[1]));

    using namespace std; // For strlen.
    cout << "Enter message: ";
    char request[max_length];
    cin.getline(request, max_length);
    size_t request_length = strlen(request);
    asio::write(s, asio::buffer(request, request_length));

    char reply[max_length];
    size_t reply_length = asio::read(s,asio::buffer(reply, request_length));
    cout << "Reply is: ";
    cout.write(reply, reply_length);
    cout << "\n";
  }
  catch (std::exception& e)  {
    cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}

#else // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
# error Local sockets not available on this platform.
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
