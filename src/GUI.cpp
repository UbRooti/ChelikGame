#include "GUI.h"

FontRenderer::FontRenderer(const std::string& path)
{
    if (mFont.loadFromFile(path + "font.ttf"))
    {
        mCharSize = VARS::default_char_size;
        mText.setCharacterSize(VARS::default_char_size);
        mText.setFillColor(sf::Color::White);
        mText.setFont(mFont);

        std::cout << "Loaded font: " << path + "font.ttf" << "\n";
    }
    else
    {
        std::cout << "Can't find font: " << path + "font.ttf" << "\n";
    }
}

void FontRenderer::drawString(sf::RenderWindow& window, const std::string& _text, float x, float y, const sf::Color& color, int _char_size)
{
    mText.setPosition(x, y);
    if (_char_size != mCharSize)
    {
        mText.setCharacterSize(_char_size);
        mCharSize = _char_size;
    }
    if (mText.getFillColor() != color)
    {
        mText.setFillColor(color);
    }
    mText.setString(_text);

    window.draw(mText);
}

int FontRenderer::getStringWidth(const std::string& _text, int _char_size)
{
    mText.setString(_text);
    mText.setCharacterSize(_char_size);
    return mText.getLocalBounds().width;
}

namespace gui
{
    Label::Label()
    : mText(), mX(0), mY(0), mColor(sf::Color::White), mRenderPosX(mX), mRenderPosY(mY)
    {}

    Label::Label(const std::string& text, const sf::Color color)
    : mText(text), mX(0), mY(0), mColor(color), mRenderPosX(mX), mRenderPosY(mY)
    {}

    Label::Label(const std::string& text, float x, float y, const sf::Color color)
    : mText(text), mX(x), mY(y), mColor(color), mRenderPosX(mX), mRenderPosY(mY)
    {}

    void Label::render(sf::RenderWindow& window)
    {
        md::fontRenderer->drawString(window, mText, mX, mY, mColor);
    }

    void Label::setPosition(float x, float y)
    {
        mX = x;
        mY = y;
    }

    void Label::setColor(const sf::Color& color)
    {
        mColor = color;
    }

    void Label::setSize(float size)
    {
        mSize = size;
    }

    float Label::getPositionX()
    {
        return mX;
    }

    float Label::getPositionY()
    {
        return mY;
    }

    sf::Color Label::getColor()
    {
        return mColor;
    }

    float Label::getSize()
    {
        return mSize;
    }

    void Label::setText(const std::string& text)
    {
        mText = text;
    }

    std::string Label::getText()
    {
        return mText;
    }

    void Label::setCentered(bool centered)
    {
        if (mCentered != centered)
        {
            mCentered = centered;
            updateCenter();
        }
    }

    bool Label::getCentered()
    {
        return mCentered;
    }

    void Label::updateCenter()
    {
        if (mCentered)
        {
            mRenderPosX = mX - (md::fontRenderer->getStringWidth(mText, mSize) / 2.f);
            mRenderPosY = mY - (mSize / 2.f);
        }
        else
        {
            mRenderPosX = mX;
            mRenderPosY = mY;
        }
    }



    // TODO: Button class

    // Button::Button(const sf::Color& textColor, const sf::Color& bgColor)
    // : mText(""), mX(0), mY(0), mWidth(0), mHeight(0), mTextColor(textColor), mBgColor(bgColor), mRenderPosX(mX), mRenderPosY(mY)
    // {

    // }

    // Button::Button(const std::string& text, const sf::Color& textColor, const sf::Color& bgColor)
    // : mText(text), mX(0), mY(0), mWidth(0), mHeight(0), mTextColor(textColor), mBgColor(bgColor), mRenderPosX(mX), mRenderPosY(mY)
    // {

    // }

    // Button::Button(const std::string& text, float x, float y, float width, float height, const sf::Color& textColor, const sf::Color& bgColor)
    // : mText(text), mX(x), mY(y), mWidth(width), mHeight(height), mTextColor(textColor), mBgColor(bgColor), mRenderPosX(mX), mRenderPosY(mY)
    // {

    // }

    // void Button::render(sf::RenderWindow& window)
    // {
        
    // }

    // void Button::setPosition(float x, float y)
    // {

    // }

    // float Button::getPositionX()
    // {

    // }

    // float Button::getPositionY()
    // {

    // }

    // void Button::setText(const std::string& text)
    // {

    // }

    // std::string Button::getText()
    // {

    // }

    // void Button::setBgColor(const sf::Color& bgColor)
    // {

    // }

    // sf::Color Button::getBgColor()
    // {

    // }

    // void Button::setTextColor(const sf::Color& textColor)
    // {

    // }

    // sf::Color Button::getTextColor()
    // {

    // }

    // void Button::setSize(int size)
    // {

    // }

    // int Button::getSize()
    // {

    // }

    // void setCentered(bool centered)
    // {

    // }
    
    // bool getCentered()
    // {

    // }
    
    // void updateCenter()
    // {
        
    // }

};