#include <Screen/include/Screen.h>
#include <memory>
#include <regex>
#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif

namespace zyx_tui {

Screen::Screen() {
    openANSIControlChar();
    screenThread();
}

Screen::~Screen() {
}

// void Screen::show(const ID& id, const Position2D& position, const Icon& str) { // 把 在屏幕上的位子和图标保存到ID上
//	Number x = position.x() * windowSize().x() / (endGamePosition().x() - startGamePosition().x());
//	Number y = position.y() * windowSize().y() / (endGamePosition().y() - startGamePosition().y());
//
//	std::lock_guard<std::mutex> lock{ screenMutex() };
//	if (x >= windowSize().x() || x < 0 || y >= windowSize().y() || y < 0) {
//		if (objectPointInfo().find(id) != objectPointInfo().end())
//			objectPointInfo().erase(id);
//		return;
//	}
//	objectPointInfo()[id] = std::make_pair(Point2D{ x, y }, Icon{ str });
// }

auto Screen::startGamePosition() noexcept -> Position2D & {
    static Position2D s_startGamePosition{0, 0};
    return s_startGamePosition;
}

auto Screen::endGamePosition() noexcept -> Position2D & {
    static Position2D s_endGamePosition{20000, 20000};
    return s_endGamePosition;
}

auto Screen::screenMutex() noexcept -> std::mutex & {
    static std::mutex s_screenMutex{};
    return s_screenMutex;
}

auto Screen::screenThread() noexcept -> std::thread & {
    static std::thread s_screenThread{Type::screenThreadFunction};
    return s_screenThread;
}

auto Screen::initialFrame() -> void {
    auto &t_windowData     = windowData();
    auto &t_windowSizeX    = windowSize().x();
    auto &t_windowSizeY    = windowSize().y();
    auto  t_windowSizeArea = windowSize().area();
    auto  FrameStyle       = Frame::Style::Round;

	if (t_windowSizeX < 1 || t_windowSizeY < 1)
		return;

    for (int i = 0; i < t_windowSizeArea; ++i) {
        t_windowData[i].m_data = Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Null)].m_data;
    }
    t_windowData[0].m_data                 = Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::LeftUpper)].m_data;
    t_windowData[t_windowSizeX - 1].m_data = Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::RightUpper)].m_data;
    for (int i = 1; i < t_windowSizeX - 1; ++i) {
        t_windowData[i].m_data = Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Horizontal)].m_data;
        t_windowData[t_windowSizeArea - t_windowSizeX + i].m_data =
            Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Horizontal)].m_data;
    }
    t_windowData[(t_windowSizeY - 1) * t_windowSizeX].m_data =
        Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::LeftBottom)].m_data;
    t_windowData[t_windowSizeArea - 1].m_data = Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::RightBottom)].m_data;
    for (int i = 1; i < t_windowSizeY - 1; ++i) {
        t_windowData[i * t_windowSizeX].m_data = Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Vertical)].m_data;
        t_windowData[(i + 1) * t_windowSizeX - 1].m_data =
            Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Vertical)].m_data;
    }
    for (auto &it : t_windowData)
        it.m_attribute.m_color.m_colorData.m_realColor = {15, 15, 31};
}

auto Screen::screenThreadFunction() -> void {
    std::cout.sync_with_stdio(false);
    while (1) {
        std::lock_guard<std::mutex> lock{screenMutex()};

        // 绘制边框
        initialFrame();

        for (auto it = childList().begin(), end = childList().end(); it != end; ++it) {
            if (it->expired()) {
                childList().erase(it);
                end = childList().end();
                --it;
            }
            auto t_ptr = it->lock();
            if (t_ptr->isExpired()) {
                childList().erase(it);
                end = childList().end();
                --it;
            }
            t_ptr->show();
        }

        // 最终显示
        std::cout << toString() << std::flush;

        // 不设置为bool Type，因为在绘制途中屏幕可能改变，
        // 需连续两次均不改变才算不更新
        if (isWindowUpdata() != 0)
            --isWindowUpdata();

        // 检测屏幕是否有改变大小，使窗口随之适应，并申请内存
        // changeWindowSize()函数检查放在绘制功能之后，
        // 防止在第一次循环时，因没有窗口变化而导致一直跳过第一次绘画
        changeWindowSize();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

auto Screen::windowSize() noexcept -> Point2D & {
    static int x = 40; // unuse 不将使用
    static int y = 20; // unuse 不将使用
#ifdef _WIN32
    static bool flag = [&] { // 初次设置窗口大小为控制台大小
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        x = csbi.srWindow.Right - csbi.srWindow.Left;
        y = csbi.srWindow.Bottom - csbi.srWindow.Top;
        return true;
    }();
#else // Unix
    static bool flag = [&] {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        auto t_right  = size.ws_col;
        auto t_left   = 0;
        auto t_bottom = size.ws_row - 1; // don't know  - 1;
        auto t_top    = 0;
        x             = t_right - t_left;
        y             = t_bottom - t_top;
        return true;
    }();

#endif
    static Point2D s_windowSize{x, y};
    return s_windowSize;
}

auto Screen::windowData() -> std::vector<Char> & {
    const std::size_t        area = windowSize().area();
    static std::vector<Char> s_windowData{area};
    return s_windowData;
}

auto Screen::isWindowUpdata() -> Number & {
    static Number s_windowUpdata{2};
    return s_windowUpdata;
}

auto Screen::changeWindowSize() -> void {
    auto &t_windowSizeY = windowSize().y();
    auto &t_windowSizeX = windowSize().x();
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    auto t_right  = csbi.srWindow.Right;
    auto t_left   = csbi.srWindow.Left;
    auto t_bottom = csbi.srWindow.Bottom;
    auto t_top    = csbi.srWindow.Top;
#else // Unix
    winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    auto t_right  = size.ws_col;
    auto t_left   = 0;
    auto t_bottom = size.ws_row - 1; // don't know  - 1;
    auto t_top    = 0;
#endif
    if (t_windowSizeX != t_right - t_left || t_windowSizeY != t_bottom - t_top) {
		t_windowSizeX = static_cast<int>(t_right) - static_cast<int>(t_left);
		t_windowSizeY = static_cast<int>(t_bottom) - static_cast<int>(t_top);
		if (t_windowSizeX < 0)
			t_windowSizeX = 0;
		if (t_windowSizeY < 0)
			t_windowSizeY = 0;
        if (windowSize().area() > 0)
            windowData().resize(windowSize().area());
        isWindowUpdata() = 2;
        // clear screen and return cursor;
        std::cout << "\x1b[2J\x1b[H";
    }
}

auto Screen::openANSIControlChar() -> void { // 初次输出时，开启ANSI转义序列
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return;
    }
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        return;
    }
#endif
}

auto Screen::toString() -> std::string {
    uint8_t h_red   = 0;
    uint8_t h_green = 0;
    uint8_t h_blue  = 0;
    if (isWindowUpdata() == 0)
        return "\x1b[H";
    auto              &t_windowSizeY = windowSize().y();
    auto              &t_windowSizeX = windowSize().x();
    static std::string str;
    str.erase(str.begin(), str.end());

    str += "\x1b[?25l"; // hide cursor
    auto &t_windowData = windowData();
    for (auto y = 0; y < t_windowSizeY; ++y) {
        for (auto x = 0; x < t_windowSizeX; ++x) {
            if (t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_red != h_red ||
                t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_green != h_green ||
                t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_blue != h_blue) {
                h_red   = t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_red;
                h_green = t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_green;
                h_blue  = t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_blue;
                str +=
                    "\x1b[48;2;" +
                    std::to_string(t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_red) + ";" +
                    std::to_string(t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_green) +
                    ";" + std::to_string(t_windowData[x + t_windowSizeX * y].m_attribute.m_color.m_colorData.m_realColor.m_blue) +
                    "m";
            }
            str += t_windowData[x + t_windowSizeX * y].m_data;
            // str += "\x1b[0m";
        }
        str += "\n";
    }
    str += "\x1b[H"; // return the cursor to (1, 1)
    return str;
}

auto Screen::childList() -> std::vector<std::weak_ptr<Widget>> & {
    static std::vector<std::weak_ptr<Widget>> s_childList{};
    return s_childList;
}

} // namespace zyx_tui
