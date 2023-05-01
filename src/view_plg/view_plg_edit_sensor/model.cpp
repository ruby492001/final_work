#include "model.h"
#include "agent_event.h"
#include "delegate.h"


int EditSensorModel::columnCount( const QModelIndex& ) const
{
     return 7;
}


QVariant EditSensorModel::data( const QModelIndex& index, int role ) const
{
     if( role != Qt::DisplayRole || !index.isValid() )
     {
          return {};
     }
     switch( index.column() )
     {
          case 0:
          {
               return sensors_.at( index.row() ).clientSensorId >> 32;
          }
          case 1:
          {
               return ( sensors_.at( index.row() ).clientSensorId & 0xffffffff );
          }
          case 2:
          {
               return sensors_.at( index.row() ).userName;
          }
          case 3:
          {
               return toString( sensors_.at( index.row() ).eventType );
          }
          case 4:
          {
               return sensors_.at( index.row() ).place;
          }
          case 5:
          {
               return sensors_.at( index.row() ).type;
          }
          case 6:
          {
               return sensors_.at( index.row() ).measure;
          }
          default:
          {
               break;
          }
     }
     return {};
}


QModelIndex EditSensorModel::index( int row, int column, const QModelIndex& parent ) const
{
     if( parent.isValid() )
     {
          return {};
     }
     return createIndex( row, column, ( void* )( &sensors_.at( row ) ) );
}


QModelIndex EditSensorModel::parent( const QModelIndex& ) const
{
     return {};
}


int EditSensorModel::rowCount( const QModelIndex& parent ) const
{
     if( parent.isValid() )
     {
          return 0;
     }
     return sensors_.count();
}


QVariant EditSensorModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
     if( role != Qt::DisplayRole || orientation != Qt::Horizontal )
     {
          return {};
     }
     switch( section )
     {
          case 0:
          {
               return tr( "ID Клиента" );
          }
          case 1:
          {
               return tr( "ID Датчика" );
          }
          case 2:
          {
               return tr( "Название датчика" );
          }
          case 3:
          {
               return tr( "Тип данных" );
          }
          case 4:
          {
               return tr( "Расположение" );
          }
          case 5:
          {
               return tr( "Тип датчика" );
          }
          case 6:
          {
               return tr( "Единица измерения" );
          }
          default:
          {
               return QVariant();
          }
     }
}


Qt::ItemFlags EditSensorModel::flags( const QModelIndex& index ) const
{
     int col = index.column();
     if( col != 2 && col != 4 && col != 5 && col != 6 )
     {
          return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
     }
     return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


bool EditSensorModel::setData( const QModelIndex& index, const QVariant& value, int )
{

     SensorParam* t = static_cast< SensorParam* >( index.internalPointer() );
     if( !t )
     {
          return false;
     }
     if( value.toString().isEmpty() )
     {
          return true;
     }
     switch( index.column() )
     {
          case 2:
          {
               t->userName = value.toString();
               break;
          }
          case 4:
          case 5:
          case 6:
          {
               if( index.column() == 4 )
               {
                    t->place = value.toString();
               }
               if( index.column() == 5 )
               {
                    t->type = value.toString();
               }
               if( index.column() == 6 )
               {
                    t->measure = value.toString();
               }
               updateDelegateValues();
               break;
          }
          default:
          {
               return false;
          }
     }
     emit sDataChanged( *t );
     dataChanged( index, index );
     return true;
}


void EditSensorModel::onSetSensorList( const QList<SensorParam>& sensors )
{
     beginResetModel();
     sensors_ = sensors;
     endResetModel();
     updateDelegateValues();
}


void EditSensorModel::updateDelegateValues()
{
     foreach( const auto& sensor, sensors_ )
     {
          if( places.indexOf( sensor.place ) == -1 && !sensor.place.isEmpty() )
          {
               places.push_back( sensor.place );
          }
          if( types.indexOf( sensor.type ) == -1 && !sensor.type.isEmpty() )
          {
               types.push_back( sensor.type );
          }
          if( measure.indexOf( sensor.measure ) == -1 && !sensor.measure.isEmpty() )
          {
               measure.push_back( sensor.measure );
          }
     }
     places.sort();
     types.sort();
     measure.sort();
}
