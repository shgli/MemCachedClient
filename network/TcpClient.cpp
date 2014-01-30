#include <boost/lexical_cast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include "network/TcpClient.h"
#include "log/LogManager.h"
TcpClient::TcpClient(boost::asio::io_service& ioService,size_t nHeaderLen)
    :mIsInWriting(false)
    ,mSocket(ioService)
    ,mHeaderLength(nHeaderLen)
    ,mHeaderBuffer(new char[nHeaderLen])
    ,mLog(LogManager::Instance().GetLogger("Network.TcpClient"))
{
}

TcpClient::~TcpClient()
{
    mSocket.close();
    delete[] mHeaderBuffer;
}

void TcpClient::Connect(const std::string& host,int port)
{
    INFO(mLog)<<"Try connect to "<<host<<":"<<port;
    using namespace boost::asio::ip;
    ip::tcp::resolver resolver(mSocket.get_io_service());
    ip::tcp::resolver::query query(host,boost::lexical_cast<std::string>(port));
    auto endpointIt= resolver.resolve(query);
    asio::async_connect(mSocket,endpointIt,
        [this](const system::error_code& ec,tcp::resolver::iterator)
    {
        if(!ec)
        {
            INFO(mLog)<<"Connect successed.";
            ReadHeader();
            OnConnected();
        }
        else
        {
            WARN(mLog)<<"Connect failed, reason is "<<ec.message();
            OnError(ESocket_Connect,ec);
        }
    });
}

void TcpClient::ReadHeader()
{
    INFO(mLog)<<"Try to read "<<mHeaderLength<<" bytes header";
    asio::async_read(mSocket,
        asio::buffer(mHeaderBuffer,mHeaderLength),
        [this](const system::error_code& ec,std::size_t len)
    {
        if(!ec) 
        {
            INFO(mLog)<<"Read header successed";
            mReadBuffers.clear();
            if(OnHeader(static_cast<void*const>(mHeaderBuffer),mReadBuffers))
            {
                if(0 != mReadBuffers.size())
                {
                    INFO(mLog)<<"Try to read body. result will be retrived in "<<mReadBuffers.size()<<" buffers.";
                    ReadBody();
                }
                else
                {
                    Close4Error(ESocket_BodyBuffer,ec);
                }
            }
            else
            {
                INFO(mLog)<<"No body need read";
                ReadHeader();
            }
        }
        else
        {
            WARN(mLog)<<"Read header failed, reason is "<<ec.message();
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
            INFO(mLog)<<len<<" bytes body has been readed.";
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
            INFO(mLog)<<len<<" bytes has been sent.";
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
    INFO(mLog)<<"Connect will be closed.";
    mSocket.get_io_service().post(
        [this]
    {
        INFO(mLog)<<"Close successed.";
        mSocket.close();
        OnClosed();
    });
}

void TcpClient::Close4Error(ESocketError type,const system::error_code err)
{
    ERROR(mLog)<<"Some problem detectd when "<<SocketError2String(type)<<",connect will be closed. reason is "<<err.message();
    mSocket.close();
    OnError(type,err);
}

const std::string& TcpClient::SocketError2String(ESocketError err)
{
    static const std::string ERROR[] = {
        "connect",
        "read header",
        "read body",
        "send",
        "collect body buffer"
    };

    return ERROR[err];
}

