#include <boost/lexical_cast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include "TcpClient.h"
TcpClient::TcpClient(boost::asio::io_service& ioService,size_t nHeaderLen)
    :mIsInWriting(false)
    ,mSocket(ioService)
    ,mHeaderLength(nHeaderLen)
    ,mHeaderBuffer(new char[nHeaderLen])
{
}

TcpClient::~TcpClient()
{
    mSocket.close();
    delete[] mHeaderBuffer;
}

void TcpClient::Connect(const std::string& host,int port)
{
    using namespace boost::asio::ip;
    ip::tcp::resolver resolver(mSocket.get_io_service());
    ip::tcp::resolver::query query(host,boost::lexical_cast<std::string>(port));
    auto endpointIt= resolver.resolve(query);
    asio::async_connect(mSocket,endpointIt,
	    [this](const system::error_code& ec,tcp::resolver::iterator)
	    {
	        if(!ec)
		{
		    ReadHeader();
                    OnConnected();
		}
		else
		{
		    OnError(ESocket_Connect,ec);
		}
	    });
}

void TcpClient::ReadHeader()
{
    asio::async_read(mSocket,
	    asio::buffer(mHeaderBuffer,mHeaderLength),
	    [this](const system::error_code& ec,std::size_t len)
	    {
	       if(!ec) 
	       {
                   mReadBuffers.clear();
	           if(OnHeader(static_cast<void*const>(mHeaderBuffer),mReadBuffers))
		   {
                       if(0 != mReadBuffers.size())
                       {
		           ReadBody();
                       }
                       else
                       {
                           Close4Error(ESocket_BodyBuffer,ec);
                       }
		   }
		   else
		   {
		       ReadHeader();
		   }
	       }
	       else
	       {
	           Close4Error(ESocket_ReadHeader,ec);
	       }
	    });
}

void TcpClient::ReadBody()
{
    asio::async_read(mSocket,
	    mReadBuffers,
	    [this](const system::error_code& ec,std::size_t len)
	    {
	        if(!ec)
		{
		    OnBody(static_cast<void*const>(mHeaderBuffer),mReadBuffers);
                    ReadHeader();
		}
		else
		{
		    Close4Error(ESocket_ReadBody,ec);
		}
	    });
}

void TcpClient::Send(const ConstBuffer& buf)
{
    mSocket.get_io_service().post(
        [this,buf]()
        {
            mPendingBuffers.push_back(buf);

	    if(mWriteBuffers.empty())
	    {
                Send();
	    }
        });
}

void TcpClient::Send(const SVConstBuffer& vbuf)
{
     mSocket.get_io_service().post(
        [this,vbuf]()
        {
            mPendingBuffers.insert(mPendingBuffers.end(),vbuf->begin(),vbuf->end());

	    if(mWriteBuffers.empty())
	    {
                Send();
	    }
        });
   
}

void TcpClient::Send()
{
    mPendingBuffers.swap(mWriteBuffers);
    asio::async_write(mSocket,
        mWriteBuffers,
	[this](const system::error_code& ec,std::size_t len)
	{
	    mWriteBuffers.clear();
	    if(!ec)
	    {
		if(!mPendingBuffers.empty())
		{
		    Send();
		}
	    } 
	    else
	    {
	        Close4Error(ESocket_Send,ec);
	    }
	});
}

void TcpClient::Close( void )
{
    mSocket.get_io_service().post(
	[this]
	{
	    mSocket.close();
	    OnClosed();
        });
}

void TcpClient::Close4Error(ESocketError type,const system::error_code err)
{
    mSocket.close();
    OnError(type,err);
}

