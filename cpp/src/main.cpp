#include <iostream>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

using namespace std;
using namespace ftxui;

int main() {

    const string hw { "Hello, World! "};
    Element doc = hbox(
        text(hw) | border
    );

    Screen screen = Screen::Create(Dimension::Fixed(hw.length() + 1), Dimension::Fixed(3));

    Render(screen, doc);
    screen.Print();
    cout << '\n';

    return 0;
}
