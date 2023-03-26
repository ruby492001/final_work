#include "view_cmd.h"
#include "common.h"


ViewArgs::ViewArgs( const QVariantList& variables )
:QObject( nullptr ), variables_( variables )
{
}


ViewArgs::ViewArgs( const ViewArgs& rhs )
:QObject( nullptr ), variables_( rhs.variables_ )
{
}


ViewArgs::~ViewArgs()
{
}


void ViewArgs::addVariable( const QVariant& variable )
{
     variables_.push_back( variable );
}


quint32 ViewArgs::count() const
{
     return variables_.count();
}


const QVariant* ViewArgs::at( quint32 idx ) const
{
     return &variables_.at( idx );
}


QDataStream& operator<<( QDataStream& out, const ViewArgs& myObj )
{
     out << QVariant::fromValue( myObj.variables_.count() );
     foreach( const auto& var, myObj.variables_ )
     {
          out << var;
     }
     return out;
}


QDataStream& operator>>( QDataStream& in, ViewArgs& myObj )
{
     QVariant variantCount;
     in >> variantCount;
     quint32 varCount = variantCount.toULongLong();
     for( quint32 cur = 0; cur < varCount; cur++ )
     {
          QVariant tmp;
          in >> tmp;
          myObj.variables_.push_back( tmp );
     }
     return in;
}


QDebug& operator<<( QDebug& d, const ViewArgs& cmd )
{
     d << cmd.variables_;
     return d;
}


ViewArgs& ViewArgs::operator=( const ViewArgs& rhs )
{
     variables_ = rhs.variables_;
     return *this;
}


ViewCommand::ViewCommand( quint32 pluginId, quint32 command, const QList<ViewArgs>& args )
:QObject( nullptr ), plgId_( pluginId ), command_( command ), arguments_( args )
{
}


ViewCommand::ViewCommand( const ViewCommand& other )
:QObject( nullptr ), plgId_( other.plgId_ ), command_( other.command_ ), arguments_( other.arguments_ )
{
}


ViewCommand::~ViewCommand()
{
}


quint32 ViewCommand::plgId() const
{
     return plgId_;
}


quint32 ViewCommand::cmd() const
{
     return command_;
}


void ViewCommand::addArg( const ViewArgs& arg )
{
     arguments_.push_back( arg );
}


quint32 ViewCommand::count() const
{
     return arguments_.count();
}


const ViewArgs* ViewCommand::at( quint32 idx ) const
{
     return &arguments_.at( idx );
}


ViewCommand& ViewCommand::operator=( const ViewCommand& rhs )
{
     plgId_ = rhs.plgId_;
     command_ = rhs.command_;
     arguments_ = rhs.arguments_;
     return *this;
}


QDataStream& operator<<( QDataStream& out, const ViewCommand& myObj )
{
     out << QVariant::fromValue( myObj.plgId_ ) << QVariant::fromValue( myObj.command_ ) <<
               QVariant::fromValue( myObj.arguments_ );

     return out;
}


QDataStream& operator>>( QDataStream& in, ViewCommand& myObj )
{
     QVariant plgIdVar;
     QVariant commandVar;
     QVariant argCountVar;
     QVariant argsVar;

     in >> plgIdVar >> commandVar >> argsVar;

     myObj.plgId_ = plgIdVar.toUInt();
     myObj.command_ = commandVar.toUInt();
     myObj.arguments_ = argsVar.value< QList< ViewArgs > >();
     return in;
}


bool ViewCommand::isValid() const
{
     return plgId_ != INVALID_PLUGIN_ID;
}


QByteArray serializeViewCmd( const ViewCommand& cmd )
{
     QByteArray cmdData;
     QDataStream stream( &cmdData, QIODevice::WriteOnly );
     stream << cmd;

     qsizetype size = cmdData.size() + sizeof( size );
     QByteArray data;
     data.resize( sizeof( size ) );
     memcpy( data.data(), &size, sizeof( size ) );
     data += cmdData;
     return data;
}


ViewCommand deserializeViewCmd( QByteArray& data )
{
     qsizetype packMinSize = sizeof( NetworkPackage );
     if( data.size() < packMinSize )
     {
          // не пришёл даже заголовок
          return {};
     }
     NetworkPackage* package = reinterpret_cast< NetworkPackage* >( data.data() );
     if( package->size > data.size() )
     {
          // не все данные пришли
          return {};
     }
     QByteArray packageData( reinterpret_cast< const char* >( package->data ), package->size - sizeof( NetworkPackage::size ) );
     QDataStream stream( &packageData, QIODevice::ReadOnly );
     ViewCommand cmd;
     stream >> cmd;
     data.remove( 0, package->size );
     return cmd;
}


QDebug& operator<<( QDebug& d, const ViewCommand& cmd )
{
     d << "Plugin id:" << cmd.plgId_ << "command:" << cmd.command_ << "args:" << cmd.arguments_;
     return d;
}