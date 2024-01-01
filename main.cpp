#include <Screen/include/Screen.h>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <future>
#include <memory>

int main(int argc, char *argv[]) {
    auto w1 = zyx_tui::make_widget({0, 0}, {10000, 20000});
    w1->setBackgrandColor(0xfe, 0xe5, 0xe1);

    auto a = zyx_tui::make_textbox({3000, 3000}, {7000, 7000}, w1);
    //   a->setBackgrandColor(0xb7, 0xd2, 0xf6);
    a->setBackgrandColor(20, 20, 20);

    auto b = zyx_tui::make_widget({10000, 0}, {20000, 20000});
    b->setBackgrandColor(0xf9, 0xf8, 0xf4);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    //    a->expire();

    while (1)
        getchar();
    return 0;
}
