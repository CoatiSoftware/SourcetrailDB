
/*
 * MyType
 *
 * This is a multiline comment
 */

namespace api
{

class MyType
    : public BaseType
{
public:
    void my_method(bool do_send_signal) const
    {
        if (do_send_signal)
        {
            Client::send_signal();
        }
    }
}

}
