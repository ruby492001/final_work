#include "add_sensor_model.h"
#include "agent_event.h"


int AddSensorModel::columnCount( const QModelIndex& ) const
{
     return 4;
}


QVariant AddSensorModel::data( const QModelIndex& index, int role ) const
{
     if( ( role != Qt::DisplayRole && role != Qt::UserRole + 1 ) || !index.isValid() )
     {
          return {};
     }
     if( role == Qt::UserRole + 1)
     {
          return sensors_.at( index.row() ).clientSensorId;
     }
     switch( index.column() )
     {
          case 0:
          {
               if( !sensors_.at( index.row() ).userName.isEmpty() )
               {
                    return sensors_.at( index.row() ).userName;
               }
               return (  "ID клиента:" + QString::number( sensors_.at( index.row() ).clientSensorId >> 32 ) +
                         ". ID датчика: " + QString::number( sensors_.at( index.row() ).clientSensorId & 0xffffffff ) );
          }
          case 1:
          {
               return ( sensors_.at( index.row() ).place );
          }
          case 2:
          {
               return ( sensors_.at( index.row() ).type );
          }
          case 3:
          {
               return ( sensors_.at( index.row() ).measure );
          }
          default:
          {
               break;
          }
     }
     return {};
}


QModelIndex AddSensorModel::index( int row, int column, const QModelIndex& parent ) const
{
     if( parent.isValid() )
     {
          return {};
     }
     return createIndex( row, column, ( void* )( &sensors_.at( row ) ) );
}


QModelIndex AddSensorModel::parent( const QModelIndex& ) const
{
     return {};
}


int AddSensorModel::rowCount( const QModelIndex& parent ) const
{
     if( parent.isValid() )
     {
          return 0;
     }
     return sensors_.count();
}


QVariant AddSensorModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
     if( role != Qt::DisplayRole || orientation != Qt::Horizontal )
     {
          return {};
     }
     switch( section )
     {
          case 0:
          {
               return tr( "Название датчика" );
          }
          case 1:
          {
               return tr( "Расположение" );
          }
          case 2:
          {
               return tr( "Тип датчика" );
          }
          case 3:
          {
               return tr( "Единица измерения" );
          }
          default:
          {
               return QVariant();
          }
     }
}


Qt::ItemFlags AddSensorModel::flags( const QModelIndex& ) const
{
     return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


void AddSensorModel::onSetSensorList( const QList<SensorParam>& sensors )
{
     beginResetModel();
     sensors_ = sensors;
     endResetModel();
}
