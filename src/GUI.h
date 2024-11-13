#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Consts.h"
#include "Globals.h"

class FontRenderer
{
    sf::Text mText;
    sf::Font mFont;
    int mCharSize;
public:
    FontRenderer(const std::string& path);

    void drawString(sf::RenderWindow& window, const std::string& _text, float x, float y, const sf::Color& color = sf::Color::White, int _char_size = VARS::default_char_size);

    int getStringWidth(const std::string& text, int _char_size = VARS::default_char_size);
};


namespace gui
{
    class Label
    {
        std::string mText;
        sf::Color mColor;
        float mX, mY;
        float mRenderPosX, mRenderPosY;
        int mSize;
        bool mCentered = false;

    public:
        Label();
        Label(const std::string& text, const sf::Color color = sf::Color::White);
        Label(const std::string& text, float x, float y, const sf::Color color = sf::Color::White);

        void render(sf::RenderWindow& window);

        void setColor(const sf::Color& color);
        sf::Color getColor();

        void setSize(float size);
        float getSize();

        void setPosition(float x, float y);
        float getPositionX();
        float getPositionY();

        void setText(const std::string& text);
        std::string getText();

        void setCentered(bool centered);
        bool getCentered();

    private:
        void updateCenter();
    };
    
    class Button // TODO: Button class
    {
        std::string mText;
        sf::Color mBgColor;
        sf::Color mTextColor;
        float mX, mY;
        float mWidth, mHeight;
        float mRenderPosX, mRenderPosY;
        bool mCentered = false;

    public:
        Button(const sf::Color& textColor = sf::Color::White, const sf::Color& bgColor = sf::Color::Black);
        Button(const std::string& text, const sf::Color& textColor = sf::Color::White, const sf::Color& bgColor = sf::Color::Black);
        Button(const std::string& text, float x, float y, float width, float height, const sf::Color& color = sf::Color::White, const sf::Color& bgColor = sf::Color::Black);

        void render(sf::RenderWindow& window);

        void setPosition(float x, float y);
        float getPositionX();
        float getPositionY();

        void setText(const std::string& text);
        std::string getText();

        void setBgColor(const sf::Color& bgColor);
        sf::Color getBgColor();

        void setTextColor(const sf::Color& textColor);
        sf::Color getTextColor();

        void setSize(int size);
        int getSize();

        void setCentered(bool centered);
        bool getCentered();
    
    private:
        void updateCenter();
    };
};