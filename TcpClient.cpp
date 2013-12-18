#include <boost/lexical_cast.hpp>
#include "TcpClient.h"
TcpClient::TcpClient(boost::asio::io_service& ioService,size_t nHeaderLen,HeaderReadedEvent onHeader)
    :mIsInWriting(false)
    ,mSocket(ioService)
    ,mHeaderLength(nHeaderLen)
    ,mHeaderBuffer(new char(nHeaderLen))
    ,OnHeader(onHeader)
{
}

void TcpClient::Connect(const std::string& host,int port)
{
    ip::tcp::resolver resolver(mSocket.get_io_service());
    auto endpointIt= resolver.resolve(host,lexical_cast<std::string>(port));
    asio::async_connect(mSocket,endpointIt,
	    [this](system::error_code ec,tcp::resolver::iterator)
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
	    [this](system::error_code ec,std::size_t len)
	    {
	       if(!ec && ((len = OnHeader(static_cast<void*const>(mHeaderBuffer),mReadBuffers)) >= 0) && 0 != mReadBuffers.size())
	       {
	           if(0 != len)
		   {
		       ReadBody();
		   }
		   else
		   {
		       ReadHeader();
		   }
	       }
	       else
	       {
	           if(ec)
		   {
	               Close4Error(ESocket_ReadHeader,ec);
		   }
		   else
		   {
		       Close4Error(ESocket_BodyBuffer,ec);
		   }
	       }
	    });
}

void TcpClient::ReadBody()
{
    asio::async_read(mSocket,
	    mReadBuffers,
	    [this](system::error_code ec,std::size_t len)
	    {
	        if(!ec)
		{
		    OnBody(static_cast<void*const>(mHeaderBuffer),mReadBuffers);
		}
		else
		{
		    Close4Error(ESocket_ReadBody,ec);
		}
	    });
}

void TcpClient::Send(const SharedBuffer& buf)
{
    mSocket.get_io_service().post(
        [this,buf]()
        {
            mWriteBuffers.push_back(buf);
            Send();
        }
}

void TcpClient::Send()
{
    if(!mIsInWriting)
    {
        mIsInWriting = true;

	asio::async_write(mSocket,
		mWriteBuffers,
		[this](system::error_code ec,std::size_t len)
		{
		    mIsInWriting = false;

		    if(!ec)
		    {
		        if(0 != mWriteBuffers.size())
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
}

void TcpClient::Close( void )
{
    mSocket.close();
    OnClosed();
}

void TcpClient::Close4Error(ESocketError type,const system::error_code err)
{
    mSocket.close();
    OnError(type,err);
}

