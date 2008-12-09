/***************************************************************************
 *   Copyright (C) 2008 by Andrzej Rybczak   *
 *   electricityispower@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef HAVE_WINDOW_H
#define HAVE_WINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ncurses.h"

#include <stack>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>

#ifdef _UTF8
# define UNICODE 1
# define my_char_t wchar_t
# define my_string_t wstring
# define TO_STRING(x) ToString(x)
# define TO_WSTRING(x) ToWString(x)
#else
# define UNICODE 0
# define my_char_t char
# define my_string_t string
# define TO_STRING(x) x
# define TO_WSTRING(x) x
#endif

using std::string;
using std::wstring;
using std::vector;

enum Color { clDefault, clBlack, clRed, clGreen, clYellow, clBlue, clMagenta, clCyan, clWhite, clEnd };
enum Format { fmtNone = 100, fmtBold, fmtBoldEnd, fmtReverse, fmtReverseEnd, fmtAltCharset, fmtAltCharsetEnd };
enum Border { brNone, brBlack, brRed, brGreen, brYellow, brBlue, brMagenta, brCyan, brWhite };
enum Where { wUp, wDown, wPageUp, wPageDown, wHome, wEnd };

typedef void (*GetStringHelper)();

void InitScreen();
void DestroyScreen();

struct Colors
{
	Colors(Color one, Color two = clDefault) : fg(one), bg(two) { }
	Color fg;
	Color bg;
};
	
struct XY
{
	XY(int xx, int yy) : x(xx), y(yy) { }
	int x;
	int y;
};

char *ToString(const wchar_t *);
wchar_t *ToWString(const char *);
std::string ToString(const std::wstring &);
std::wstring ToWString(const std::string &);

class Window
{
	public:
		Window(size_t, size_t, size_t, size_t, const std::string &, Color, Border);
		Window(const Window &);
		virtual ~Window();
		
		WINDOW *Raw() const { return itsWindow; }
		
		size_t GetWidth() const;
		size_t GetHeight() const;
		size_t GetStartX() const;
		size_t GetStartY() const;
		
		const std::string &GetTitle() const;
		Color GetColor() const;
		Border GetBorder() const;
		std::string GetString(const std::string &, size_t = -1, size_t = 0) const;
		string GetString(unsigned int length = -1, int width = 0) const { return GetString("", length, width); }
		void GetXY(int &, int &);
		void GotoXY(int, int);
		const int &X() const;
		const int &Y() const;
		
		void SetGetStringHelper(GetStringHelper helper) { itsGetStringHelper = helper; }
		void SetColor(Color, Color = clDefault);
		void SetBaseColor(Color, Color = clDefault);
		void SetBorder(Border);
		void SetTimeout(int);
		void SetTitle(const string &);
		
		void Hide(char = 32) const;
		void Bold(bool) const;
		void Reverse(bool) const;
		void AltCharset(bool) const;
		
		void Display();
		virtual void Refresh();
		
		virtual void MoveTo(int, int);
		virtual void Resize(int, int);
		virtual void Clear(bool stub = 1);
	
		void ReadKey(int &) const;
		void ReadKey() const;
		
		void Write(const string &s, bool cte = 0) { Write(0xFFFF, s, cte); }
		void Write(int, const string &, bool = 0);
		void WriteXY(int x, int y, const string &s, bool ete = 0) { WriteXY(x, y, 0xFFFF, s, ete); }
		void WriteXY(int, int, int, const string &, bool = 0);
#		ifdef _UTF8
		void Write(const wstring &s, bool cte = 0) { Write(0xFFFF, s, cte); }
		void Write(int, const wstring &, bool = 0);
		void WriteXY(int x, int y, const wstring &s, bool ete = 0) { WriteXY(x, y, 0xFFFF, s, ete); }
		void WriteXY(int, int, int, const wstring &, bool = 0);
#		endif
		
		void Scrollable(bool) const;
		virtual void Scroll(Where);
		
		Window &operator<<(const Colors &);
		Window &operator<<(const Color &);
		Window &operator<<(const Format &);
		Window &operator<<(const XY &);
		Window &operator<<(const char *);
		Window &operator<<(const char &);
		Window &operator<<(const wchar_t *);
		Window &operator<<(const wchar_t &);
		Window &operator<<(const int &);
		Window &operator<<(const double &);
		Window &operator<<(const size_t &);
		
		Window &operator<<(const std::string &);
		Window &operator<<(const std::wstring &);
		
		virtual Window *Clone() const { return new Window(*this); }
		virtual Window *EmptyClone() const;
		
		
		
		// stubs for inherits, ugly shit, needs improvement
		virtual void Select(int, bool) { }
		virtual bool Selected(int) const { return 0; }
		virtual int Size() const { return 0; }
		virtual bool IsAnySelected() const { return 0; }
		virtual void GetSelectedList(vector<int> &) const { }
		virtual bool IsStatic(int = -1) const { return 0; }
		virtual void Highlight(int) { }
		virtual string GetOption(int = -1) const { return ""; }
		virtual int GetChoice() const { return -1; } // for Menu class
		//virtual void Add(string str) { Write(str); } // for Scrollpad class
		
		static size_t Length(const wstring &);
		
	protected:
		
		class BadSize { };
		
		virtual void Recreate();
		virtual void ShowBorder() const;
		
		WINDOW *itsWindow;
		WINDOW *itsWinBorder;
		
		GetStringHelper itsGetStringHelper;
		
		size_t itsStartX;
		size_t itsStartY;
		size_t itsWidth;
		size_t itsHeight;
		
		int itsWindowTimeout;
		int itsX;
		int itsY;
		
		string itsTitle;
		std::stack<Colors> itsColors;
		
		Color itsColor;
		Color itsBaseColor;
		Color itsBgColor;
		Color itsBaseBgColor;
		
		Border itsBorder;
};

#endif
