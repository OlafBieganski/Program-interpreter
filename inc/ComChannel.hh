#ifndef COM_CHANNEL_HH
#define COM_CHANNEL_HH

#include"AbstractComChannel.hh"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>


class ComChannel : public AbstractComChannel
{
    private:
        int _Socket;
        int _Port;    
    public:
        bool Init();
        int GetSocket() const;
        bool Send(const char *msg);
        //void LockAccess();
        //void UnlockAccess();
        //std::mutex &UseGuard();
        ~ComChannel() { close(_Socket); };
        ComChannel() = default;
        ComChannel(int Port): _Port(Port) {};
};

bool ComChannel::Send(const char *msg)
{
    ssize_t sent_count;
    ssize_t to_send = (ssize_t)strlen(msg);
    while((sent_count = write(_Socket, (const void*)msg, to_send)) > 0)
    {
        to_send -=sent_count;
        msg += sent_count;
    }
    if(sent_count < 0)
    {
        std::cerr << "ERROR sending" << std::endl;
        return false;
    }
    return true;
}

bool ComChannel::Init()
{
    struct sockaddr_in serverAdress;
    bzero((char *)&serverAdress, sizeof(serverAdress));
    serverAdress.sin_family=AF_INET;
    serverAdress.sin_addr.s_addr=inet_addr("127.0.0.1");
    serverAdress.sin_port=htons(_Port);

    _Socket = socket(AF_INET, SOCK_STREAM, 0);

    if(_Socket < 0)
    {
        std::cerr << "Cant open socket" << std::endl;
        return false;
    }

    if(connect(_Socket, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) < 0)
    {
        std::cerr << "Cant establish connection" << std::endl;
        return false;
    }
    return true;
}
    
int ComChannel::GetSocket() const
{
    return _Socket;
}

/*void ComChannel::LockAccess()
{

}*/

/*void ComChannel::UnlockAccess()
{

}*/

/*std::mutex& ComChannel::UseGuard()
{

}*/

#endif