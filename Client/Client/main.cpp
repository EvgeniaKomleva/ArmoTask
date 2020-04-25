#include <echo.h>
#include <QCoreApplication>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

using namespace std;

int mygetch( )
{
struct termios oldt,
newt;
int ch;
tcgetattr( STDIN_FILENO, &oldt );
newt = oldt;
newt.c_lflag &= ~( ICANON | ECHO );
tcsetattr( STDIN_FILENO, TCSANOW, &newt );
ch = getchar();
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
return ch;
}

// This function acts as a client to the object.

static void hello(Echo_ptr e, string word)
{
  if( CORBA::is_nil(e) ) {
    cerr << "hello: The object reference is nil!" << endl;
    return;
  }

  CORBA::String_var src = (const char*) word.c_str();
  // String literals are (char*) rather than (const char*) on some
  // old compilers.  Thus it is essential to cast to (const char*)
  // here to ensure that the string is copied, so that the
  // CORBA::String_var does not attempt to 'delete' the string
  // literal.

  CORBA::String_var dest = e->echoString(src);
}

//////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        if (argc != 2) {
          cerr << "usage:  eg2_clt <object reference>" << endl;
          return 1;
        }

        CORBA::Object_var obj = orb->string_to_object(argv[1]);

        Echo_var echoref = Echo::_narrow(obj);

        if (CORBA::is_nil(echoref)) {
          cerr << "Can't narrow reference to type Echo (or it was nil)." << endl;
          return 1;
        }

        // Петля ввода
        while (true)
        {
            char ch_word;
            ch_word = char(mygetch());
            cout << ch_word;

            string str_word;
            str_word = ch_word;

            hello(echoref, str_word);
        }

        orb->destroy();
      }
      catch (CORBA::TRANSIENT&) {
        cerr << "Caught system exception TRANSIENT -- unable to contact the "
             << "server." << endl;
      }
      catch (CORBA::SystemException& ex) {
        cerr << "Caught a CORBA::" << ex._name() << endl;
      }
      catch (CORBA::Exception& ex) {
        cerr << "Caught CORBA::Exception: " << ex._name() << endl;
      }



    return a.exec();
}
