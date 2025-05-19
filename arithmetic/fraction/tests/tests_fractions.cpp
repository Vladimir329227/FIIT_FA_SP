#include <fraction.h>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc,char *argv[])
{
    fraction eps (1, 100);

    fraction a(1,2);
    fraction b(12, 16);
    cout << a + b << std::endl;
    cout << a - b << std::endl;
    cout << a * b << std::endl;
    cout << a / b << std::endl;

    if (a > b)
    {
        if (a != b)
            cout << a << " greater than " << b << endl;
    }
    else
    {
        cout << b << " greater than " << a << endl;
    }

    a = fraction(1, 10);
    b = fraction(-121, -1210);
    if (a == b)
    {
        cout << "a and b are equal" << endl;
    }
    try
    {
        a = fraction(1, 0);
        b = fraction(-121, -1210);
        a += b;
    }catch (std::invalid_argument &ex)
    {
        cout << "Getted error : \n";
        cout << ex.what() << endl;
    }
    //std::cin >> a;

    cout << a << endl;
    std::cout << "\n------------------------------------------\n";

    fraction f2 (144, 1);
    std::cout << (f2.root(2, eps)) << std::endl;


    fraction f (271, 100);
    std::cout << (f.pow(2)).ln(eps) << std::endl;

    fraction g (10, 1);
    std::cout << g.lg(eps) << std::endl;

    a = fraction(1, 2);
    cout << a.cos(eps) << endl;

    a = fraction(1, 2);
    cout << a.tg(eps) << endl;

    a = fraction(1, 2);
    cout << a.sec(eps) << endl;

    a = fraction(1, 2);
    cout << a.cosec(eps) << endl;

    a = fraction(1, 2);
    cout << a.sin(eps) << endl;

    a = fraction(1, 2);
    cout << a.cos(eps) << endl;
}