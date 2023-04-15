#include <algorithm>

#include "main_windows.h"
#include "net_cfg_dialog.h"
#include "viewplg_itf.h"


MainWindow::MainWindow()
{
     NetCfgDialog dlg;
     if( !dlg.exec() )
     {
          exit( 0 );
     }
     setWindowTitle( tr( "Программа просмотра данных ") );
     client_ = new ViewConnectionClient( dlg.addr(), dlg.port() );
     //client_ = new ViewConnectionClient( "localhost", 54321 );

     connect( client_, &ViewConnectionClient::sConnectionError, this, &MainWindow::onConnectionLost, Qt::DirectConnection );
     connect( client_, &ViewConnectionClient::sConnected, this, &MainWindow::onConnected, Qt::DirectConnection );
     client_->onConnect();
}


void MainWindow::onConnectionLost()
{
     if( !connected_ )
     {
          // ошибка при первом подключении
          QMessageBox::critical( nullptr, tr( "Ошибка подключения к серверу" ),
                                 tr( "Невозможно установить соединение с сервером" ) );
     }
     else
     {
          // подключение отвалилось в процессе работы
          QMessageBox::critical( this, tr( "Потеряно подключение к серверу" ), tr( "Соединение с сервером было потеряно" ) );
     }
     exit( 0 );
}


void MainWindow::onConnected()
{
     resize( 1200, 600 );
     connected_ = true;

     QHBoxLayout* lyt = new QHBoxLayout( this );

     QSplitter* splitter = new QSplitter( Qt::Horizontal );
     lyt->addWidget( splitter );

     pluginSelector_ = new QTreeWidget( this );
     connect( pluginSelector_, &QTreeWidget::currentItemChanged, this, &MainWindow::onSelectionChanged );
     pluginSelector_->setRootIsDecorated( false );
     pluginSelector_->setHeaderHidden( true );
     pluginSelector_->setIconSize( QSize( 35, 35 ) );
     splitter->addWidget( pluginSelector_ );

     widgets_ = new QStackedWidget( this );
     splitter->addWidget( widgets_ );

     splitter->setStretchFactor( 1, 10 );

     loadPlugins();
}


void MainWindow::onSelectionChanged( QTreeWidgetItem* current, QTreeWidgetItem* )
{
     widgets_->setCurrentWidget( current->data( 0, ViewPluginWindowRole ).value< QWidget* >() );
}


void MainWindow::loadPlugins()
{
     QDir dir( QApplication::applicationDirPath() + "/../view_plg/");
     if( !dir.exists() )
     {
          qCritical() << "Folder" << dir.path() << "not exist";
          exit( 0 );
     }
     QStringList fileList = dir.entryList( QStringList(), QDir::Files );
     for( const QString& str: fileList )
     {
          if( !str.startsWith( "libview_" ) )
          {
               continue;
          }
          QPluginLoader loader( dir.absolutePath() + QDir::separator() + str );
          if( !loader.load() )
          {
               qCritical() << "Cannot load plugin with name" << str << loader.errorString();
               continue;
          }
          ViewPluginItf* plg = qobject_cast< ViewPluginItf* >( loader.instance() );
          if( !plg )
          {
               qCritical() << "Error cast:" << str;
               continue;
          }
          qDebug() << "Loaded plugin with id:" << plg->id();

          plugins_.append( plg );
     }
     if( plugins_.isEmpty() )
     {
          QMessageBox::critical( this, tr( "Ошибка загрузки плагинов" ),
                                 tr( "Не загружено ни одного плагина. Завершение работы..." ), QMessageBox::Ok );
          exit( 0 );
     }

     // сортируем по приоритету
     std::sort( plugins_.begin(), plugins_.end(),
           [](const ViewPluginItf* a, const ViewPluginItf* b) -> bool { return a->priority() < b->priority(); } );

     foreach( auto currentPlg, plugins_ )
     {
          /// todo: нормальное подключение к БД
          currentPlg->init( nullptr );
          QTreeWidgetItem* item = new QTreeWidgetItem( pluginSelector_ );
          item->setIcon(0, currentPlg->icon() );
          item->setText( 0, currentPlg->name() );
          QFont tmpFont = item->font( 0 );
          tmpFont.setPointSize( 10 );
          item->setFont( 0, tmpFont );
          item->setBackground( 0, Qt::lightGray );
          item->setSizeHint( 0, QSize( 35, 35 ) );

          connect( currentPlg, SIGNAL( sSendNetRequest( const ViewCommand& ) ),
                   client_, SLOT( onSendCommand( const ViewCommand& ) ) );
          connect( currentPlg, SIGNAL( sChangeIcon() ), this, SLOT( onIconChanged() ) );
          connect( client_, SIGNAL( sResponseReceived( const ViewCommand& ) ),
                   currentPlg, SLOT( onNetResponse( const ViewCommand& ) ) );

          QWidget* wgt = currentPlg->widget( widgets_ );
          item->setData( 0, ViewPluginWindowRole, QVariant::fromValue( wgt ) );
          item->setData( 0, ViewPluginIdx, currentPlg->id() );
          pluginSelector_->addTopLevelItem( item );
          widgets_->addWidget( wgt );
     }
}


void MainWindow::onIconChanged()
{
     ViewPluginItf* plugin = qobject_cast< ViewPluginItf* >( sender() );
     if( !plugin )
     {
          qCritical() << "Cast error in MainWindow::onIconChanged";
          return;
     }
     for( int curIdx = 0; curIdx < pluginSelector_->topLevelItemCount(); curIdx++ )
     {
          if( pluginSelector_->topLevelItem( curIdx )->data( 0, ViewPluginIdx ).toUInt() == plugin->id() )
          {
               pluginSelector_->topLevelItem( curIdx )->setIcon( 0, plugin->icon() );
               break;
          }
     }
}
