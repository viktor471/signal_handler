#include <QCoreApplication>
#include <QMap>
#include <signal.h>
#include <iostream>
#include <QDebug>

typedef int Signum;
typedef void (*Handler)( int signum );
int a;

namespace Services {

  typedef QString SigName ;
  enum Handler_number
  {
    SIG_INT = SIGINT,
    SIG_FPE = SIGFPE,
  };

  typedef QMap< Handler_number, SigName > SigNames_map;
  SigNames_map _sig_names;

  Handler_number get_signal_number( Signum signum_ )
  {
    return static_cast< Handler_number >( signum_ );
  }

}

class Handlers{

  typedef QMap< Services::Handler_number, Handler > Handlers_map;
  Handlers_map _handlers;

  static Handlers* _object;
  Handlers( Handlers& other )      = delete;

  Handlers(){

    using namespace Services;

    _sig_names.insert( SIG_INT, "SIGINT" );
    _sig_names.insert( SIG_FPE, "SIGFPE" );

    for(
        SigNames_map::Iterator it = _sig_names.begin();
        it != _sig_names.end();
        it++
    )
      _handlers.insert(
          it.key(),
          []( int _signum ) -> void {

          Handler_number handler = get_signal_number( _signum );

          qDebug()  << _sig_names[ handler ]
                    << ", sig number: "
                    << _signum;
          }
      );
  }

 void operator=(const Handlers& ) = delete;

public:

  static Handlers* get_instance()
  {
    if( _object == nullptr )
      _object = new Handlers;

    return _object;
  }

Handler operator[]( Services::Handler_number number )
{
  return *_handlers.find( number );
}

};

Handlers* Handlers::_object = nullptr;

class Signal_handler
{
private:

  sighandler_t _old_action;
  sighandler_t _cur_action;
  Signum       _signal;

public:

  void setHandler( Handler _handler );
  sighandler_t call_signal();
  sighandler_t getCurAction();
  sighandler_t getOldAction();
  sighandler_t setNewAction( sighandler_t action  );
  Signum       setNewSignal( Signum       signum_ );
  Signum       getSignal();

};

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  return a.exec();
}

sighandler_t SignalHandler::getCurAction()
{
  return cur_action;
}

sighandler_t SignalHandler::getOldAction()
{
  return old_action;
}

sighandler_t SignalHandler::setNewAction(sighandler_t action)
{
  old_action = cur_action;
  return cur_action = action;
}

Signum SignalHandler::setNewSignal(Signum signum_)
{

}



