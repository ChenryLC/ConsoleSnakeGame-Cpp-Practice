#pragma once
class IGameEventHandler{
    protected:
        IGameEventHandler() = default;
    public:
        virtual ~IGameEventHandler()=default;
        virtual void playerDied(int playerId)=0;
};