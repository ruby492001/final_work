#include "view_cmd.h"


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


const ViewArgs* ViewCommand::at( quint32 idx )
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

               //<< QVariant::fromValue( myObj.arguments_.count() );
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