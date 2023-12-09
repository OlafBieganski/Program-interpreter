#ifndef COM_CHANNEL_HH
#define COM_CHANNEL_HH

#include"AbstractComChannel.hh"

class ComChannel : public AbstractComChannel
{
    private:
        int _Socket;
    public:
        void Init(int Socket) override;
        int GetSocket() const override;
        void LockAccess() override;
        void UnlockAccess() override;
        std::mutex &UseGuard() override;
};

void ComChannel::Init(int Socket)
{

}
    
int ComChannel::GetSocket() const
{

}

void ComChannel::LockAccess()
{

}

void ComChannel::UnlockAccess()
{

}

std::mutex& ComChannel::UseGuard()
{

}

#endif