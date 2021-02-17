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

  Signal_handler( sighandler_t cur_action_ = nullptr , Signum sign_ = SIGINT );
  sighandler_t call_signal();
  sighandler_t get_current_action();
  sighandler_t get_old_action();
  sighandler_t set_new_action( sighandler_t action  );
  Signum       set_new_signal( Signum       signum_ );
  Signum       get_signal();

};

int main()
{
  Signal_handler handler;
  Handlers &handlers = *Handlers::get_instance();
  handler.set_new_signal( SIGINT  );
  handler.set_new_action( handlers[ Services::SIG_INT ] );

  handler.call_signal();


  handler.set_new_signal( SIGABRT );

  handler.set_new_action( []( int signum )
  {
    std::cout << "SIGABRT: signum: "
              << signum
              << std::endl;
  });

  handler.call_signal();

  while (1) {}

  return 0;
}

Signal_handler::Signal_handler(sighandler_t cur_action_, Signum sign_ ) :
  _old_action( nullptr ),
  _cur_action( cur_action_ ),
  _signal( sign_ ) { }

sighandler_t Signal_handler::call_signal()
{
  if( _cur_action == nullptr )
  {
    std::cerr << "ERROR: cur action wasn't setted!" << std::endl;
    return _cur_action;
  }
  return signal( _signal, _cur_action );
}

sighandler_t Signal_handler::get_current_action()
{
  return _cur_action;
}

sighandler_t Signal_handler::get_old_action()
{
  if( _old_action == nullptr )
    std::cerr << "action is setted in first time. Isn't old action"
              << std::endl;
  return _old_action;
}

sighandler_t Signal_handler::set_new_action( sighandler_t action_ )
{
  _old_action = _cur_action;
  return _cur_action = action_;
}

Signum Signal_handler::set_new_signal( Signum signum_ )
{
  return _signal = signum_;
}

Signum Signal_handler::get_signal()
{
  return _signal;
}



