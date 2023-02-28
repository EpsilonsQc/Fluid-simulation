#pragma once
#include <iostream>
#include <string>
#include <array>
#include <random>
#include <iomanip>
#include <thread>
#include <chrono>

#if defined(_WIN32) || defined(_WIN64)
#include "conio.h"
#else
#include <ncurses.h>
#endif
using namespace std;

namespace BdB
{
    // Reference: ANSI ESC code convention -> https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

    // CMD
    const string ESC = "\x1b[";

    // CURSOR
    const string HOME    = "H";
    const string UP      = "1A";
    const string DOWN    = "1B";
    const string RGHT    = "1C";
    const string LEFT    = "1D";
    const string NEXT_L  = "1E"; // go to beginning of NEXT line
    const string PREV_L  = "1F"; // go to beginning of PREVIOUS line

    const string SAVE    = "s";  // save current position
    const string LOAD    = "u";  // restore last position

    // MODE
    const string BOLD    = "1;"; // accentuer -> OK
    const string FAINT   = "2;"; // atténuer -> KO
    const string ITALC   = "3;"; // -> KO
    const string ULINE   = "4;"; // soulign� -> OK
    const string BLINK   = "5;"; // -> KO 
    const string REVRS   = "7;"; // reverse FGRND/BGRND color -> OK
    const string SWTCH   = "8;"; // show/hide -> KO
    const string STRKE   = "9;"; // barr� -> KO

    // ERASE
    const string ERASE_DWN = "0J\r";   // efface du curseur à la fin de l'écran
    const string ERASE_UP  = "1J\r";   // efface du curseur au début de l'écran
    const string ERASE_ALL = "2J";
    const string ERASE_L   = "2K\r";   // efface toute la ligne

    // COLOR
    const string RESET_ALL   = "\x1b[0m";
    const string RESET_COLOR = "\x1b[39m";

    // normal text
    const string Black       = "30m";
    const string Red         = "31m";
    const string Green       = "32m";
    const string Yellow      = "33m";
    const string Blue        = "34m";
    const string Magenta     = "35m";
    const string Cyan        = "36m";
    const string White       = "37m";

    // Light text
    const string LBlack      = "90m";
    const string LRed        = "91m";
    const string LGreen      = "92m";
    const string LYellow     = "93m";
    const string LBlue       = "94m";
    const string LMagenta    = "95m";
    const string LCyan       = "96m";
    const string LWhite      = "97m";

    // normal BG
    const string BG_Black    = "40m";
    const string BG_Red      = "41m";
    const string BG_Green    = "42m";
    const string BG_Yellow   = "43m";
    const string BG_Blue     = "44m";
    const string BG_Magenta  = "45m";
    const string BG_Cyan     = "46m";
    const string BG_White    = "47m";

    // Light BG
    const string BG_LBlack    = "40;1m";
    const string BG_LRed      = "41;1m";
    const string BG_LGreen    = "42;1m";
    const string BG_LYellow   = "43;1m";
    const string BG_LBlue     = "44;1m";
    const string BG_LMagenta  = "45;1m";
    const string BG_LCyan     = "46;1m";
    const string BG_LWhite    = "47;1m";

    // SHORCUT
    const string CLS = ESC + ERASE_ALL + ESC + HOME;

    // SPECIAL KEY NAME
    const char NB_SPECIAL_KEY = 32;
    const array<string, NB_SPECIAL_KEY> specialKeyName = {
        "NUL",  // null character
        "SOH",  // start of heading
        "STX",  // start of text
        "ETX",  // end of text
        "EOT",  // end of transmission
        "ENQ",  // enquiry
        "ACK",  // acknowledgment
        "BEL",  // audible bell
        "BS",   // back space
        "HT",   // horizontal tab
        "LF",   // line feed
        "VT",   // vertical tab
        "FF",   // form feed
        "CR",   // carriage return
        "SO",   // shift out
        "SI",   // shift in
        "DLE",  // data link escape
        "DC1",  // device control 1
        "DC2",  // device control 2
        "DC3",  // device control 3
        "DC4",  // device control 4
        "NAK",  // negative acknowledge
        "SYN",  // synchronous idle
        "ETB",  // end of transmit block
        "CAN",  // cancel
        "EM",   // end of medium
        "SUB",  // substitution
        "ESC",  // escape
        "FS",   // file separator
        "GS",   // group separator
        "RS",   // record separator
        "US"    // unit separator
    };

#if !(defined(_WIN32) || defined(_WIN64))
    // Unix equivalent to conio _kbhit() function
    // Reference: https://invisible-island.net/ncurses/NCURSES-Programming-HOWTO.html
    inline int kbhit()
    {
        int ch;
        wtimeout(stdscr, 0);
        ch = wgetch(stdscr);
        nodelay(stdscr, FALSE);
        if (ch == ERR)
            return 0;
        ungetch(ch);
        return 1;
    }

    inline void initializeNCurses()
    {
        initscr(); // initialiser le programme en utilisant ncurses
        cbreak(); // rend disponible le caractère saisi immédiatement.
        noecho(); // désactive l'affichage sur l'écran d'une touche frappée au clavier
        nodelay(stdscr, TRUE); //empêche getch() d'être bloquant
        keypad(stdscr, TRUE); // active les touche étendu du clavier (au delà de 256)
    }

    inline void releaseNCurses()
    {
        keypad(stdscr, FALSE); // active les touche étendu du clavier (au delà de 256)
        nodelay(stdscr, FALSE); //empêche getch() d'être bloquant
        echo(); // rétablit l'affichage du caractère saisi
        nocbreak(); // rétablit le mode par défaut.
        endwin(); // doit être appelée avant de quitter le programme en mode curses. Elle permet de rétablir l'ancien mode du terminal.
    }
#endif

    // INPUT SECTION
    inline char getSpecialKeyCode(const string& keyCodeName)
    {
        for (int i = 0; i < NB_SPECIAL_KEY; ++i)
            if (specialKeyName[i] == keyCodeName)
                return (char)i;

        return 0;
    }

    inline string getSpecialKeyCodeName(char keyCode)
    {
        if (keyCode >= 0 && keyCode < NB_SPECIAL_KEY)
            return specialKeyName[keyCode];
        return "";
    }
    
    inline int getKeyPressed()
    {
        int keyPressed = {};
#if _WIN32 || _WIN64

        if (_kbhit())
            keyPressed = _getch();
#else
        initializeNCurses();
        if (kbhit())
            keyPressed = getch();
        releaseNCurses();
#endif
        return keyPressed;
    }

    inline int waitKeyToPress(char keyToPress = {})
    {
        int keyPressed = {};

#if _WIN32 || _WIN64
        for (;;)
        {
            if (_kbhit())
            {
                keyPressed = _getch();
                if (keyPressed && keyPressed == keyToPress || !keyToPress)
                    break;
            }
        }
#else
        initializeNCurses();
        for (;;)
        {
            if (kbhit())
            {
                keyPressed = getch();
                if (keyPressed && keyPressed == keyToPress || !keyToPress)
                    break;
            }
        }
        releaseNCurses();
#endif

        return keyPressed;
    }

    inline void pressToContinue(string s = "")
    {
        if (s == "")
            s = "Appuie sur une touche pour continuer ... \n";
        cout << s;
        waitKeyToPress();
    }

    // TIME SECTION
    inline void sleepFor(int delay)
    {
        if (delay > 0)
            this_thread::sleep_for(chrono::milliseconds(delay));
    }

    // RANDOM SECTION
    inline default_random_engine& getRand()
    {
        static default_random_engine ran;
        return ran;
    };

    inline void srandInt(int s) { getRand().seed(s); }
    inline int randInt(int min, int max) { return uniform_int_distribution<>{min, max}(getRand()); }
    inline int randInt(int max) { return randInt(0, max); }
}
