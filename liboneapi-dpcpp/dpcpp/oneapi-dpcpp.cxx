#include <dpcpp/oneapi-dpcpp.hxx>

#include <ostream>
#include <stdexcept>

using namespace std;

namespace oneapi_dpcpp
{
  void say_hello (ostream& o, const string& n)
  {
    if (n.empty ())
      throw invalid_argument ("empty name");

    o << "Hello, " << n << '!' << endl;
  }
}
