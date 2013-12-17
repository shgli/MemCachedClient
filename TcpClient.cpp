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
	       Buffer* pBodyBuf = nullptr;
	       if(!ec && ((len = OnHeader(static_cast<void*const>(mHeaderBuffer),pBodyBuf)) >= 0) && nullptr != pBodyBuf)
	       {
	           if(0 != len)
		   {
		       ReadBody(*pBodyBuf);
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

void TcpClient::ReadBody(const Buffer& bodyBuf)
{
    asio::async_read(mSocket,
	    bodyBuf,
	    [this](system::error_code ec,std::size_t len)
	    {
	        if(!ec)
		{
		    OnBody(bodyBuf);
		}
		else
		{
		    Close4Error(ESocket_ReadBody,ec);
		}
	    });
}

void TcpClient::Send(const SharedBuffer& buf)
{
    {
	boost::scoped_lock<boost::mutex> lock(mWriteBufMut);
        mPendingBuffers.push_back(buf);
    }
    Send(); 
}

void TcpClient::Send()
{
    if(!mIsInWriting.test_and_set())
    {
	{
	    boost::scoped_lock<boost::mutex> lock(mWriteBufMut);
            mPendingBuffers.swap(mWritingBuffers);
	}

	asio::async_write(mSocket,
		mWritingBuffers,
		[this](system::error_code ec,std::size_t len)
		{
		    mIsInWriting.clear();
		    if(!ec)
		    {
		        if(0 != mPendingBuffers.size())
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

