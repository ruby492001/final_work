#include "cred_validator.h"
#include "db_wrapper.h"

QMutex CredValidator::mutex_;
QHash< quint32, QByteArray >  CredValidator::clients_;
quint32 CredValidator::maxClientId_ = 1;
bool CredValidator::inited_ = false;


bool CredValidator::init()
{
     SqlWrapperProtection sql( SqlConnectionManager::getReadOnlyConnection() );

     auto existsClients = sql->existClient();
     foreach( const auto& existClient, existsClients )
     {
          clients_.insert( existClient.first, existClient.second );
          if( existClient.first > maxClientId_ )
          {
               maxClientId_ = existClient.first;
          }
     }
     inited_ = true;
     return inited_;
}


bool CredValidator::validateCreds( quint32 clientId, const QByteArray& uuid, bool& validateCredsRes )
{
     if( !inited_ )
     {
          return false;
     }
     auto client = clients_.find( clientId );
     if( client == clients_.end() )
     {
          validateCredsRes = false;
     }
     else
     {
          if( client.value() != uuid )
          {
               validateCredsRes = false;
          }
          else
          {
               validateCredsRes = true;
          }

     }
     return true;
}


bool CredValidator::createNewClient( const QByteArray& uuid, quint32& newClientId )
{
     if( !inited_ )
     {
          return inited_;
     }
     SqlWrapperProtection sql( SqlConnectionManager::getWriteConnection( true ) );
     if( !sql->beginTransaction() )
     {
          return false;
     }

     newClientId = ++maxClientId_;
     if( !sql->addCreds( newClientId, uuid ) )
     {
          return false;
     }

     if( !sql->commitTransaction() )
     {
          return false;
     }

     clients_.insert( newClientId, uuid );
     return true;
}