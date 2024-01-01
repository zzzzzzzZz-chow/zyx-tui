#pragma once

#include <Type/include/Object.h>
#include <Type/include/Position.h>
#include <thread>
#include <mutex>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <memory>
#include <algorithm>

namespace zyx_tui {

class Widget;
struct Char {
    struct {
        bool m_isItalic;
        bool m_isBold;
        struct Color {
            enum class ColorEnum { RealColor, VirtualColor, Gray, System, HSV } m_colorStyle;
            union ColorData {
                struct RealColor {
                    uint8_t m_red;
                    uint8_t m_green;
                    uint8_t m_blue;
                } m_realColor;          // ColorEnum::RealColor
                uint8_t m_virtualColor; // ColorEnum::VirtualColor
                uint8_t m_gray;         // ColorEnum::Gray
                uint8_t m_system;       // ColorEnum::System
                struct HSV {
                    uint8_t m_h;
                    uint8_t m_s;
                    uint8_t m_v;
                } m_HSVColor; // ColorEnum::HSV
            } m_colorData;
        } m_color;
    } m_attribute;
    std::string m_data;
    Char() = default;
    Char(std::string str) : m_attribute(), m_data(str) {
    }
};

namespace Frame {
enum class Style { Empty, Round, Light };
enum class Index { Null, Horizontal, Vertical, LeftUpper, RightUpper, LeftBottom, RightBottom };
inline std::map<Style, std::vector<Char>> charSet{
    {Style::Round, std::vector<Char>{{" "}, {"─"}, {"│"}, {"╭"}, {"╮"}, {"╰"}, {"╯"}}},
    {Style::Light, std::vector<Char>{{" "}, {"─"}, {"│"}, {"┌"}, {"┐"}, {"└"}, {"┘"}}},
};

} // namespace Frame

class Screen final : public virtual Object, public virtual PositionType {
    using Type           = Screen;
    using Reference      = Type &;
    using ConstReference = const Type &;
    using RightReference = Type &&;
    using Pointer        = Type *;
    using ConstPointer   = const Type *;

public:
    friend int main(int argc, char *argv[]);
    friend class Widget;
    Screen();
    Screen(ConstReference that) = delete;
    Screen(RightReference that) = default;
    ~Screen();
    Reference operator=(ConstReference that) = delete;
    Reference operator=(RightReference that) = default;

    // static void show(const ID& id, const Position2D& position, const Icon& str);
    static void push(std::shared_ptr<Widget> widget) {
        childList().push_back(widget);
    }

    static auto startGamePosition() noexcept -> Position2D &;
    static auto endGamePosition() noexcept -> Position2D &;
    static auto windowSize() noexcept -> Point2D &;

private:
    static auto screenMutex() noexcept -> std::mutex &;
    static auto screenThread() noexcept -> std::thread &;
    static auto screenThreadFunction() -> void;

    static auto isWindowUpdata() -> Number &;
    static auto openANSIControlChar() -> void;
    static auto initialFrame() -> void;
    static auto changeWindowSize() -> void;

    static auto windowData() -> std::vector<Char> &;
    static auto toString() -> std::string;
    static auto childList() -> std::vector<std::weak_ptr<Widget>> &;
};

inline Screen screen;

class Widget : public virtual Object, public virtual PositionType {
public:
    Widget(const Position2D &start, const Position2D &end, std::shared_ptr<Widget> father = nullptr)
        : m_startGamePosition(start), m_endGamePosition(end), m_windowData(Screen::windowData()), m_father(father) {
        changeWindowSize();
    }

public:
    friend std::shared_ptr<Widget> make_widget(const PositionType::Position2D &start, const PositionType::Position2D &end);
    friend std::shared_ptr<Widget> make_widget(const PositionType::Position2D &start, const PositionType::Position2D &end,
                                               std::shared_ptr<Widget> father);
    friend std::shared_ptr<Widget> make_textbox(const PositionType::Position2D &start, const PositionType::Position2D &end,
                                                std::shared_ptr<Widget> father);
    virtual ~Widget() noexcept {
        Screen::isWindowUpdata() = 2;
    }
    auto changeWindowSize() -> void {
        if (!m_father) {
            auto t_diffPositionX = Screen::endGamePosition().x() - Screen::startGamePosition().x();
            auto t_diffPositionY = Screen::endGamePosition().y() - Screen::startGamePosition().y();
            auto t_windowSizeX   = Screen::windowSize().x();
            auto t_windowSizeY   = Screen::windowSize().y();
            m_startWindow.x()    = (m_startGamePosition.x() - Screen::startGamePosition().x()) * t_windowSizeX / t_diffPositionX;
            m_startWindow.y()    = (m_startGamePosition.y() - Screen::startGamePosition().y()) * t_windowSizeY / t_diffPositionY;
            m_endWindow.x()      = (m_endGamePosition.x() - Screen::startGamePosition().x()) * t_windowSizeX / t_diffPositionX;
            m_endWindow.y()      = (m_endGamePosition.y() - Screen::startGamePosition().y()) * t_windowSizeY / t_diffPositionY;
            return;
        }
        auto t_diffPositionX = m_father->m_endGamePosition.x() - m_father->m_startGamePosition.x();
        auto t_diffPositionY = m_father->m_endGamePosition.y() - m_father->m_startGamePosition.y();
        auto t_windowSizeX   = m_father->m_endWindow.x() - m_father->m_startWindow.x();
        auto t_windowSizeY   = m_father->m_endWindow.y() - m_father->m_startWindow.y();
        m_startWindow.x()    = m_father->m_startWindow.x() +
                            (m_startGamePosition.x() - m_father->m_startGamePosition.x()) * t_windowSizeX / t_diffPositionX;
        m_startWindow.y() = m_father->m_startWindow.y() +
                            (m_startGamePosition.y() - m_father->m_startGamePosition.y()) * t_windowSizeY / t_diffPositionY;
        m_endWindow.x() = m_father->m_startWindow.x() +
                          (m_endGamePosition.x() - m_father->m_startGamePosition.x()) * t_windowSizeX / t_diffPositionX;
        m_endWindow.y() = m_father->m_startWindow.y() +
                          (m_endGamePosition.y() - m_father->m_startGamePosition.y()) * t_windowSizeY / t_diffPositionY;

        if (m_startWindow.x() < 0)
            m_startWindow.x() = 0;
        if (m_startWindow.y() < 0)
            m_startWindow.y() = 0;
        if (m_endWindow.x() > Screen::windowSize().x())
            m_endWindow.x() = Screen::windowSize().x();
        if (m_endWindow.y() > Screen::windowSize().x())
            m_endWindow.y() = Screen::windowSize().y();

        // can't show bold
        const auto t_diff = (m_frameStyle == Frame::Style::Empty) ? 0 : 1;
        if (m_endWindow.x() - m_startWindow.x() <= t_diff || m_endWindow.y() - m_startWindow.y() <= t_diff)
            m_isHide = true;
        else
            m_isHide = false;
    }
    auto initialFrame() -> void {
        if (m_frameStyle == Frame::Style::Empty)
            return;
        /*auto& t_windowData = windowData();
        auto& t_windowSizeX = windowSize().x();
        auto& t_windowSizeY = windowSize().y();
        auto t_windowSizeArea = windowSize().area();*/
        auto FrameStyle = m_frameStyle;
        m_windowData[m_startWindow.x() + Screen::windowSize().x() * m_startWindow.y()].m_data =
            Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::LeftUpper)].m_data;
        m_windowData[m_endWindow.x() - 1 + Screen::windowSize().x() * m_startWindow.y()].m_data =
            Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::RightUpper)].m_data;
        for (int i = 1 + m_startWindow.x() + Screen::windowSize().x() * m_startWindow.y();
             i < m_endWindow.x() - 1 + Screen::windowSize().x() * m_startWindow.y(); ++i) {
            m_windowData[i].m_data = Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Horizontal)].m_data;
            m_windowData[i + (m_endWindow.y() - m_startWindow.y() - 1) * Screen::windowSize().x()].m_data =
                Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Horizontal)].m_data;
        }
        m_windowData[(m_endWindow.y() - 1) * Screen::windowSize().x() + m_startWindow.x()].m_data =
            Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::LeftBottom)].m_data;
        m_windowData[(m_endWindow.y() - 1) * Screen::windowSize().x() + m_endWindow.x() - 1].m_data =
            Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::RightBottom)].m_data;
        for (int i = m_startWindow.y() + 1; i < m_endWindow.y() - 1; ++i) {
            m_windowData[i * Screen::windowSize().x() + m_startWindow.x()].m_data =
                Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Vertical)].m_data;
            m_windowData[i * Screen::windowSize().x() + m_endWindow.x() - 1].m_data =
                Frame::charSet[FrameStyle][static_cast<int>(Frame::Index::Vertical)].m_data;
        }
    }
    auto show() -> void {
        changeWindowSize();
        if (m_isHide == true) // Screen space is too small to show, widget has to hide self;
            return;
        initialFrame();
        showContent();
        auto t_screenX = Screen::windowSize().x();
        for (auto y = m_startWindow.y(); y < m_endWindow.y(); ++y) {
            for (auto x = m_startWindow.x(); x < m_endWindow.x(); ++x) {
                m_windowData[y * t_screenX + x].m_attribute.m_color.m_colorData.m_realColor = {m_red, m_green, m_blue};
            }
        }
        for (auto it = m_childList.begin(), end = m_childList.end(); it != end; ++it) {
            if (it->expired()) {
                m_childList.erase(it);
                --it;
                end = m_childList.end();
            }
            auto t_ptr = it->lock();
            if (t_ptr->m_isExpired) {
                m_childList.erase(it);
                --it;
                end = m_childList.end();
            }
            t_ptr->show();
        }
    }
    virtual void showContent() {
    }
    auto expire() noexcept -> void {
        m_isExpired              = true;
        Screen::isWindowUpdata() = 2;
        for (auto &it : m_childList)
            it.expired();
    }
    auto isExpired() noexcept -> bool {
        return m_isExpired;
    }
    auto frameStyle() noexcept -> Frame::Style & {
        return m_frameStyle;
    }
    auto setBackgrandColor(uint8_t red, uint8_t green, uint8_t blue) -> Widget & {
        m_red   = red;
        m_green = green;
        m_blue  = blue;
        return *this;
    }

protected:
    void push(std::shared_ptr<Widget> widget) {
        m_childList.push_back(widget);
    }

protected:
    std::shared_ptr<Widget>            m_father{nullptr}; // if nullptr, father is screen
    Point2D                            m_startWindow;
    Point2D                            m_endWindow;
    std::vector<Char>                 &m_windowData;
    Position2D                         m_startGamePosition;
    Position2D                         m_endGamePosition;
    std::vector<std::weak_ptr<Widget>> m_childList{};
    bool                               m_isExpired{false};
    bool                               m_isHide{false};
    Frame::Style                       m_frameStyle{Frame::Style::Light};
    uint8_t                            m_red;
    uint8_t                            m_green;
    uint8_t                            m_blue;
};

inline std::shared_ptr<Widget> make_widget(const PositionType::Position2D &start, const PositionType::Position2D &end) {
    auto widget = std::make_shared<Widget>(start, end);
    Screen::push(widget);
    return widget;
}
inline std::shared_ptr<Widget> make_widget(const PositionType::Position2D &start, const PositionType::Position2D &end,
                                           std::shared_ptr<Widget> father) {
    auto widget = std::make_shared<Widget>(start, end, father);
    father->push(widget);
    return widget;
}

class TextBox : public Widget {
public:
    friend std::shared_ptr<Widget> make_textbox(const PositionType::Position2D &start, const PositionType::Position2D &end,
                                                std::shared_ptr<Widget> father);
    TextBox(const Position2D &start, const Position2D &end, std::shared_ptr<Widget> father) : Widget(start, end, father) {
    }
    void showContent() override {
        if (m_text.empty())
            return;
        auto t_framePadding = 0;
        if (m_frameStyle != Frame::Style::Empty) {
            t_framePadding = 1;
        }
        for (auto y = m_startWindow.y() + t_framePadding, endY = m_endWindow.y() - t_framePadding, idxY = 0; y < endY;
             ++y, ++idxY) {
            if (m_text.size() == idxY)
                break;
            for (auto x = m_startWindow.x() + t_framePadding, endX = m_endWindow.x() - t_framePadding, idxX = 0; x < endX;
                 ++x, ++idxX) {
                if (m_text[idxY].size() == idxX)
                    break;
                m_windowData[y * Screen::windowSize().x() + x].m_data = std::string{m_text[idxY][idxX]};
            }
        }
    }

private:
    std::vector<std::vector<std::string>> m_text{
        {" ", "\b这", " ", "\b是", " ", "\b一", " ", "\b个", " ", "\b告", " ", "\b示", " ", "\b板"},
        {"h", "e", "l", "l", "o", " ", "w", "a", "n", "g", "s", "h", "i", "x",
         "i", "n", "g", " ", "z", "h", "e", " ", "s", "h", "i", " ", "y", "i"}};
};
inline std::shared_ptr<Widget> make_textbox(const PositionType::Position2D &start, const PositionType::Position2D &end,
                                            std::shared_ptr<Widget> father) {
    auto t_ptr = std::make_shared<TextBox>(start, end, father);
    father->push(t_ptr);
    return t_ptr;
}
} // namespace zyx_tui
