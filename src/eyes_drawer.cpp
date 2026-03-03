/*
 * Copyright (C) 2025 Artem Savkin
 
 */
#include "eyes_drawer.hpp"

//*********************************************************************************************
//  GENERAL METHODS
//*********************************************************************************************

void LuLuEyes::begin(int width, int height, LGFX_Sprite* _sprite)
{
    sprite = _sprite;
    
    screenWidth = width;     // OLED display width, in pixels
    screenHeight = height;   // OLED display height, in pixels
    eyeLheightCurrent = 1;   // start with closed eyes
    eyeRheightCurrent = 1;   // start with closed eyes
}

void LuLuEyes::update()
{
    // Limit drawing updates to defined max framerate
    // if (millis() - fpsTimer >= frameInterval)
    // {
    drawEyes();
    // fpsTimer = millis();
    // }
}

//*********************************************************************************************
//  SETTERS METHODS
//*********************************************************************************************

void LuLuEyes::setFramerate(byte fps)
{
    frameInterval = 1000 / fps;
}

void LuLuEyes::setWidth(byte leftEye, byte rightEye)
{
    eyeLwidthNext = leftEye;
    eyeRwidthNext = rightEye;
    eyeLwidthDefault = leftEye;
    eyeRwidthDefault = rightEye;
}

void LuLuEyes::setHeight(byte leftEye, byte rightEye)
{
    eyeLheightNext = leftEye;
    eyeRheightNext = rightEye;
    eyeLheightDefault = leftEye;
    eyeRheightDefault = rightEye;
}

void LuLuEyes::setBorderradius(byte leftEye, byte rightEye)
{
    eyeLborderRadiusNext = leftEye;
    eyeRborderRadiusNext = rightEye;
    eyeLborderRadiusDefault = leftEye;
    eyeRborderRadiusDefault = rightEye;
}

void LuLuEyes::setSpacebetween(int space)
{
    spaceBetweenNext = space;
    spaceBetweenDefault = space;
}

void LuLuEyes::setMood(unsigned char mood)
{
    switch (mood)
    {
    case TIRED:
        tired = 1;
        angry = 0;
        happy = 0;
        break;
    case ANGRY:
        tired = 0;
        angry = 1;
        happy = 0;
        break;
    case HAPPY:
        tired = 0;
        angry = 0;
        happy = 1;
        break;
    default:
        tired = 0;
        angry = 0;
        happy = 0;
        break;
    }
}

void LuLuEyes::setPosition(unsigned char position)
{
    switch (position)
    {
    case N:
        // North, top center
        eyeLxNext = getScreenConstraint_X() / 2;
        eyeLyNext = 0;
        break;
    case NE:
        // North-east, top right
        eyeLxNext = getScreenConstraint_X();
        eyeLyNext = 0;
        break;
    case E:
        // East, middle right
        eyeLxNext = getScreenConstraint_X();
        eyeLyNext = getScreenConstraint_Y() / 2;
        break;
    case SE:
        // South-east, bottom right
        eyeLxNext = getScreenConstraint_X();
        eyeLyNext = getScreenConstraint_Y();
        break;
    case S:
        // South, bottom center
        eyeLxNext = getScreenConstraint_X() / 2;
        eyeLyNext = getScreenConstraint_Y();
        break;
    case SW:
        // South-west, bottom left
        eyeLxNext = 0;
        eyeLyNext = getScreenConstraint_Y();
        break;
    case W:
        // West, middle left
        eyeLxNext = 0;
        eyeLyNext = getScreenConstraint_Y() / 2;
        break;
    case NW:
        // North-west, top left
        eyeLxNext = 0;
        eyeLyNext = 0;
        break;
    default:
        // Middle center
        eyeLxNext = getScreenConstraint_X() / 2;
        eyeLyNext = getScreenConstraint_Y() / 2;
        break;
    }
}

void LuLuEyes::setAutoblinker(bool active, int interval, int variation)
{
    autoblinker = active;
    blinkInterval = interval;
    blinkIntervalVariation = variation;
}

void LuLuEyes::setAutoblinker(bool active)
{
    autoblinker = active;
}

void LuLuEyes::setIdleMode(bool active, int interval, int variation)
{
    idle = active;
    idleInterval = interval;
    idleIntervalVariation = variation;
}

void LuLuEyes::setIdleMode(bool active)
{
    idle = active;
}

void LuLuEyes::setCuriosity(bool curiousBit)
{
    curious = curiousBit;
}

void LuLuEyes::setCyclops(bool cyclopsBit)
{
    cyclops = cyclopsBit;
}

void LuLuEyes::setHFlicker(bool flickerBit, byte Amplitude)
{
    hFlicker = flickerBit;         // turn flicker on or off
    hFlickerAmplitude = Amplitude; // define amplitude of flickering in pixels
}

void LuLuEyes::setHFlicker(bool flickerBit)
{
    hFlicker = flickerBit; // turn flicker on or off
}

void LuLuEyes::setVFlicker(bool flickerBit, byte Amplitude)
{
    vFlicker = flickerBit;         // turn flicker on or off
    vFlickerAmplitude = Amplitude; // define amplitude of flickering in pixels
}

void LuLuEyes::setVFlicker(bool flickerBit)
{
    vFlicker = flickerBit; // turn flicker on or off
}

//*********************************************************************************************
//  GETTERS METHODS
//*********************************************************************************************

int LuLuEyes::getScreenConstraint_X()
{
    return screenWidth - eyeLwidthCurrent - spaceBetweenCurrent - eyeRwidthCurrent;
}

int LuLuEyes::getScreenConstraint_Y()
{
    return screenHeight - eyeLheightDefault; // using default height here, because height will vary when blinking and in curious mode
}

//*********************************************************************************************
//  BASIC ANIMATION METHODS
//*********************************************************************************************

void LuLuEyes::close()
{
    eyeLheightNext = 1; // closing left eye
    eyeRheightNext = 1; // closing right eye
    eyeL_open = 0;      // left eye not opened (=closed)
    eyeR_open = 0;      // right eye not opened (=closed)
}

void LuLuEyes::open()
{
    eyeL_open = 1; // left eye opened - if true, drawEyes() will take care of opening eyes again
    eyeR_open = 1; // right eye opened
}

void LuLuEyes::blink()
{
    close();
    open();
}

void LuLuEyes::close(bool left, bool right)
{
    if (left)
    {
        eyeLheightNext = 1; // blinking left eye
        eyeL_open = 0;      // left eye not opened (=closed)
    }
    if (right)
    {
        eyeRheightNext = 1; // blinking right eye
        eyeR_open = 0;      // right eye not opened (=closed)
    }
}

void LuLuEyes::open(bool left, bool right)
{
    if (left)
    {
        eyeL_open = 1; // left eye opened - if true, drawEyes() will take care of opening eyes again
    }
    if (right)
    {
        eyeR_open = 1; // right eye opened
    }
}

void LuLuEyes::blink(bool left, bool right)
{
    close(left, right);
    open(left, right);
}

//*********************************************************************************************
//  MACRO ANIMATION METHODS
//*********************************************************************************************

void LuLuEyes::anim_confused()
{
    confused = 1;
}

void LuLuEyes::anim_laugh()
{
    laugh = 1;
}

void LuLuEyes::anim_hearts()
{
    hearts = 1;
    heartsToggle = 0;
    heartsAnimationTimer = millis();
}

//*********************************************************************************************
//  PRE-CALCULATIONS AND ACTUAL DRAWINGS
//*********************************************************************************************

void LuLuEyes::calcCleanEyes()
{
    // Заделываем область чуть большего размера, чтобы удалить любые следы
    cleanupLX = eyeLx - cleanupPadding;
    cleanupLY = eyeLy - cleanupPadding;
    cleanupLWidth = eyeLwidthCurrent + 2 * cleanupPadding;
    cleanupLHeight = eyeLheightCurrent + 2 * cleanupPadding;
    cleanupRX = eyeRx - cleanupPadding;
    cleanupRY = eyeRy - cleanupPadding;
    cleanupRWidth = eyeRwidthCurrent + 2 * cleanupPadding;
    cleanupRHeight = eyeRheightCurrent + 2 * cleanupPadding;
    // Убедитесь, что параметры находятся в пределах допустимого диапазона
    cleanupLX = max(0, cleanupLX);
    cleanupLY = max(0, cleanupLY);
    cleanupLWidth = min(screenWidth - cleanupLX, cleanupLWidth);
    cleanupLHeight = min(screenHeight - cleanupLY, cleanupLHeight);
    cleanupRX = max(0, cleanupRX);
    cleanupRY = max(0, cleanupRY);
    cleanupRWidth = min(screenWidth - cleanupRX, cleanupRWidth);
    cleanupRHeight = min(screenHeight - cleanupRY, cleanupRHeight);
}

void LuLuEyes::cleanEyes()
{
    // Очищаем область левого глаза
    // fillRect_noTr(cleanupLX, cleanupLY, cleanupLWidth, cleanupLHeight, BGCOLOR);
    // // Очищаем область правого глаза, если режим Cyclops отключен
    // if (!cyclops)
    // {
    //     fillRect_noTr(cleanupRX, cleanupRY, cleanupRWidth, cleanupRHeight, BGCOLOR);
    // }
    // fillRect_noTr(cleanupRX, cleanupRY, cleanupRWidth, cleanupRHeight, BGCOLOR);
    // fillRect_noTr(cleanupLX, cleanupLY, cleanupRX + cleanupRWidth, cleanupRY+cleanupRHeight, BGCOLOR);
    sprite->clear();
}

void LuLuEyes::drawHeart(int x, int y, int size, uint16_t color)
{
    // Draw the heart using triangles and circles
    // Top left circle
    sprite->fillCircle(x + size/4, y + size/4, size/4, color);
    // Top right circle
    sprite->fillCircle(x + 3*size/4, y + size/4, size/4, color);
    // Bottom triangle
    sprite->fillTriangle(
        x, y + size/3,
        x + size, y + size/3,
        x + size/2, y + size,
        color
    );
}

void LuLuEyes::drawEyes()
{
    // display.startWrite();
    //// PRE-CALCULATIONS - EYE SIZES AND VALUES FOR ANIMATION TWEENINGS ////
    calcCleanEyes();
    
    // Handle hearts animation
    if (hearts)
    {
        unsigned long elapsed = millis() - heartsAnimationTimer;
        if (elapsed >= heartsAnimationDuration)
        {
            hearts = 0; // Animation finished
            // Reset eye positions after hearts animation
            eyeLheightNext = eyeLheightDefault;
            eyeRheightNext = eyeRheightDefault;
            eyeLwidthNext = eyeLwidthDefault;
            eyeRwidthNext = eyeRwidthDefault;
            // Reset eye positions to default
            eyeLxNext = eyeLxDefault;
            eyeLyNext = eyeLyDefault;
            eyeRxNext = eyeRxDefault;
            eyeRyNext = eyeRyDefault;
        }
    }
    
    // Vertical size offset for larger eyes when looking left or right (curious gaze)
    if (curious && !hearts)
    {
        if (eyeLxNext <= 10)
        {
            eyeLheightOffset = 8;
        }
        else if (eyeLxNext >= (getScreenConstraint_X() - 10) && cyclops)
        {
            eyeLheightOffset = 8;
        }
        else
        {
            eyeLheightOffset = 0;
        } // left eye
        if (eyeRxNext >= screenWidth - eyeRwidthCurrent - 10)
        {
            eyeRheightOffset = 8;
        }
        else
        {
            eyeRheightOffset = 0;
        } // right eye
    }
    else
    {
        eyeLheightOffset = 0; // reset height offset for left eye
        eyeRheightOffset = 0; // reset height offset for right eye
    }
    // Left eye height
    eyeLheightCurrent = (eyeLheightCurrent + eyeLheightNext + eyeLheightOffset) / 2;
    eyeLy += ((eyeLheightDefault - eyeLheightCurrent) / 2); // vertical centering of eye when closing
    eyeLy -= eyeLheightOffset / 2;
    // Right eye height
    eyeRheightCurrent = (eyeRheightCurrent + eyeRheightNext + eyeRheightOffset) / 2;
    eyeRy += (eyeRheightDefault - eyeRheightCurrent) / 2; // vertical centering of eye when closing
    eyeRy -= eyeRheightOffset / 2;
    // Open eyes again after closing them
    if (eyeL_open && !hearts)
    {
        if (eyeLheightCurrent <= 1 + eyeLheightOffset)
        {
            eyeLheightNext = eyeLheightDefault;
        }
    }
    if (eyeR_open && !hearts)
    {
        if (eyeRheightCurrent <= 1 + eyeRheightOffset)
        {
            eyeRheightNext = eyeRheightDefault;
        }
    }
    // Left eye width
    eyeLwidthCurrent = (eyeLwidthCurrent + eyeLwidthNext) / 2;
    // Right eye width
    eyeRwidthCurrent = (eyeRwidthCurrent + eyeRwidthNext) / 2;
    // Space between eyes
    spaceBetweenCurrent = (spaceBetweenCurrent + spaceBetweenNext) / 2;
    // Left eye coordinates
    eyeLx = (eyeLx + eyeLxNext) / 2;
    eyeLy = (eyeLy + eyeLyNext) / 2;
    // Right eye coordinates
    eyeRxNext = eyeLxNext + eyeLwidthCurrent + spaceBetweenCurrent; // right eye's x position depends on left eyes position + the space between
    eyeRyNext = eyeLyNext;                                          // right eye's y position should be the same as for the left eye
    eyeRx = (eyeRx + eyeRxNext) / 2;
    eyeRy = (eyeRy + eyeRyNext) / 2;
    // Left eye border radius
    eyeLborderRadiusCurrent = (eyeLborderRadiusCurrent + eyeLborderRadiusNext) / 2;
    // Right eye border radius
    eyeRborderRadiusCurrent = (eyeRborderRadiusCurrent + eyeRborderRadiusNext) / 2;
    //// APPLYING MACRO ANIMATIONS ////
    if (autoblinker && !hearts)
    {
        if (millis() >= blinktimer)
        {
            blink();
            blinktimer = millis() + (blinkInterval * 1000) + (random(blinkIntervalVariation) * 1000); // calculate next time for blinking
        }
    }
    // Laughing - eyes shaking up and down for the duration defined by laughAnimationDuration (default = 500ms)
    if (laugh && !hearts)
    {
        if (laughToggle)
        {
            setVFlicker(1, 5);
            laughAnimationTimer = millis();
            laughToggle = 0;
        }
        else if (millis() >= laughAnimationTimer + laughAnimationDuration)
        {
            setVFlicker(0, 0);
            laughToggle = 1;
            laugh = 0;
        }
    }
    // Confused - eyes shaking left and right for the duration defined by confusedAnimationDuration (default = 500ms)
    if (confused && !hearts)
    {
        if (confusedToggle)
        {
            setHFlicker(1, 20);
            confusedAnimationTimer = millis();
            confusedToggle = 0;
        }
        else if (millis() >= confusedAnimationTimer + confusedAnimationDuration)
        {
            setHFlicker(0, 0);
            confusedToggle = 1;
            confused = 0;
        }
    }
    // Idle - eyes moving to random positions on screen
    if (idle && !hearts)
    {
        if (millis() >= idleAnimationTimer)
        {
            eyeLxNext = random(getScreenConstraint_X());
            eyeLyNext = random(getScreenConstraint_Y());
            idleAnimationTimer = millis() + (idleInterval * 1000) + (random(idleIntervalVariation) * 1000); // calculate next time for eyes repositioning
        }
    }
    // Adding offsets for horizontal flickering/shivering
    if (hFlicker && !hearts)
    {
        if (hFlickerAlternate)
        {
            eyeLx += hFlickerAmplitude;
            eyeRx += hFlickerAmplitude;
        }
        else
        {
            eyeLx -= hFlickerAmplitude;
            eyeRx -= hFlickerAmplitude;
        }
        hFlickerAlternate = !hFlickerAlternate;
    }
    // Adding offsets for horizontal flickering/shivering
    if (vFlicker && !hearts)
    {
        if (vFlickerAlternate)
        {
            eyeLy += vFlickerAmplitude;
            eyeRy += vFlickerAmplitude;
        }
        else
        {
            eyeLy -= vFlickerAmplitude;
            eyeRy -= vFlickerAmplitude;
        }
        vFlickerAlternate = !vFlickerAlternate;
    }
    // Cyclops mode, set second eye's size and space between to 0
    if (cyclops && !hearts)
    {
        eyeRwidthCurrent = 0;
        eyeRheightCurrent = 0;
        spaceBetweenCurrent = 0;
    }
    //// ACTUAL DRAWINGS ////
    // display.clearDisplay(); // start with a blank screen
    // fillRect_noTr(cleanupLX, cleanupLY, cleanupLWidth, cleanupLHeight, BGCOLOR);
    if (cleanupLX != eyeLx || cleanupLY != eyeLy || cleanupLWidth != eyeLwidthCurrent 
        || cleanupLHeight != eyeLheightCurrent || cleanupRX != eyeRx || cleanupRY != eyeRy 
        || cleanupRWidth != eyeRwidthCurrent || cleanupRHeight != eyeRheightCurrent) // if the left eye has changed, clear it and redraw it
        cleanEyes();
        
    // Handle hearts animation drawing
    if (hearts)
    {
        unsigned long elapsed = millis() - heartsAnimationTimer;
        float progress = (float)elapsed / heartsAnimationDuration;
        
        if (progress <= 0.5f)
        {
            // Transforming to hearts (0% to 50% of animation)
            float transformProgress = progress * 2.0f; // Scale to 0.0 - 1.0
            
            // Calculate heart positions - keep them centered where eyes are
            int centerX = screenWidth / 2;
            int centerY = screenHeight / 2;
            
            // Heart size based on eye size and transformation progress
            int heartSize = eyeLwidthDefault * transformProgress;
            
            // Position hearts at the same location as eyes would be
            int heartLx = eyeLx + (eyeLwidthCurrent - heartSize) / 2;
            int heartLy = eyeLy + (eyeLheightCurrent - heartSize) / 2;
            int heartRx = eyeRx + (eyeRwidthCurrent - heartSize) / 2;
            int heartRy = eyeRy + (eyeRheightCurrent - heartSize) / 2;
            
            // Draw hearts
            drawHeart(heartLx, heartLy, heartSize, HEARTCOLOR);
            if (!cyclops)
            {
                drawHeart(heartRx, heartRy, heartSize, HEARTCOLOR);
            }
        }
        else
        {
            // Transforming back from hearts (50% to 100% of animation)
            float transformProgress = 1.0f - ((progress - 0.5f) * 2.0f); // Scale from 1.0 to 0.0
            
            // Calculate heart positions
            int centerX = screenWidth / 2;
            int centerY = screenHeight / 2;
            
            // Heart size based on transformation progress
            int heartSize = eyeLwidthDefault * transformProgress;
            
            // Position hearts at the same location as eyes
            int heartLx = eyeLx + (eyeLwidthCurrent - heartSize) / 2;
            int heartLy = eyeLy + (eyeLheightCurrent - heartSize) / 2;
            int heartRx = eyeRx + (eyeRwidthCurrent - heartSize) / 2;
            int heartRy = eyeRy + (eyeRheightCurrent - heartSize) / 2;
            
            // Draw hearts with decreasing size
            drawHeart(heartLx, heartLy, heartSize, HEARTCOLOR);
            if (!cyclops)
            {
                drawHeart(heartRx, heartRy, heartSize, HEARTCOLOR);
            }
        }
    }
    else
    {
        // Draw basic eye rectangles
        sprite->fillRoundRect(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, eyeLborderRadiusCurrent, MAINCOLOR); // left eye
        if (!cyclops)
        {
            sprite->fillRoundRect(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, eyeRborderRadiusCurrent, MAINCOLOR); // right eye
        }
        // Prepare mood type transitions
        if (tired)
        {
            eyelidsTiredHeightNext = eyeLheightCurrent / 2;
            eyelidsAngryHeightNext = 0;
        }
        else
        {
            eyelidsTiredHeightNext = 0;
        }
        if (angry)
        {
            eyelidsAngryHeightNext = eyeLheightCurrent / 2;
            eyelidsTiredHeightNext = 0;
        }
        else
        {
            eyelidsAngryHeightNext = 0;
        }
        if (happy)
        {
            eyelidsHappyBottomOffsetNext = eyeLheightCurrent / 2;
        }
        else
        {
            eyelidsHappyBottomOffsetNext = 0;
        }
        // Draw tired top eyelids
        eyelidsTiredHeight = (eyelidsTiredHeight + eyelidsTiredHeightNext) / 2;
        if (!cyclops)
        {
            sprite->fillTriangle(eyeLx, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx, eyeLy + eyelidsTiredHeight - 1, BGCOLOR);                    // left eye
            sprite->fillTriangle(eyeRx, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy + eyelidsTiredHeight - 1, BGCOLOR); // right eye
        }
        else
        {
            // Cyclops tired eyelids
            sprite->fillTriangle(eyeLx, eyeLy - 1, eyeLx + (eyeLwidthCurrent / 2), eyeLy - 1, eyeLx, eyeLy + eyelidsTiredHeight - 1, BGCOLOR);                                       // left eyelid half
            sprite->fillTriangle(eyeLx + (eyeLwidthCurrent / 2), eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy + eyelidsTiredHeight - 1, BGCOLOR); // right eyelid half
        }
        // Draw angry top eyelids
        eyelidsAngryHeight = (eyelidsAngryHeight + eyelidsAngryHeightNext) / 2;
        if (!cyclops)
        {
            sprite->fillTriangle(eyeLx, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy + eyelidsAngryHeight - 1, BGCOLOR); // left eye
            sprite->fillTriangle(eyeRx, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy - 1, eyeRx, eyeRy + eyelidsAngryHeight - 1, BGCOLOR);                    // right eye
        }
        else
        {
            // Cyclops angry eyelids
            sprite->fillTriangle(eyeLx, eyeLy - 1, eyeLx + (eyeLwidthCurrent / 2), eyeLy - 1, eyeLx + (eyeLwidthCurrent / 2), eyeLy + eyelidsAngryHeight - 1, BGCOLOR);                    // left eyelid half
            sprite->fillTriangle(eyeLx + (eyeLwidthCurrent / 2), eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx + (eyeLwidthCurrent / 2), eyeLy + eyelidsAngryHeight - 1, BGCOLOR); // right eyelid half
        }
        // Draw happy bottom eyelids
        eyelidsHappyBottomOffset = (eyelidsHappyBottomOffset + eyelidsHappyBottomOffsetNext) / 2;
        sprite->fillRoundRect(eyeLx - 1, (eyeLy + eyeLheightCurrent) - eyelidsHappyBottomOffset + 1, eyeLwidthCurrent + 2, eyeLheightDefault, eyeLborderRadiusCurrent, BGCOLOR); // left eye
        if (!cyclops)
        {
            sprite->fillRoundRect(eyeRx - 1, (eyeRy + eyeRheightCurrent) - eyelidsHappyBottomOffset + 1, eyeRwidthCurrent + 2, eyeRheightDefault, eyeRborderRadiusCurrent, BGCOLOR); // right eye
        }
    }
    // display.startWrite();
    
    sprite->pushSprite(0,EYEBORDER);
    // display.display(); // show drawings on display
    // display.endWrite();
}