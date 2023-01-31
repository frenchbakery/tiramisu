#include <iostream>

struct coord
{
private:
    int z;

public:
    int x, y;

    int sum(int _c)
    {
        return x + y + _c;
    }
};

void myfunc(int _a)
{
    std::cout << _a << std::endl;
}

int main()
{
    std::cout << "Hello, World!" << std::endl;

    coord mylocation;

    std::cout << mylocation.sum(85) << std::endl;

    return 0;
}