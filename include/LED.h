/////////////////////////////////////////////////////////////////////////////
// The GaugeWorks project is released under the GNU Lesser Public License
// with modifications. See the License.txt file for the full text
// Copyright (c) 2007 Brent Picasso http://picasso.org
// Name:        led.cpp
// Purpose:     wxLed implementation
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LED_H_
#define _WX_LED_H_

#include <wx/window.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/thread.h>

class wxLed : public wxWindow
{
	public :

		wxLed (wxWindow * parent, wxWindowID id, const char * disabledColor = "000000", const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize) ;
		~wxLed () ;

		bool Enable (bool enable = true) ;
		void SetColor (const char * rgb) ;

	protected :

		char m_enabledColor [7] ;
		char m_disabledColor [7] ;
		char m_color [7] ;
		wxBitmap * m_bitmap ;
		wxMutex m_mutex ;

		void OnPaint (wxPaintEvent & event) ;
		virtual void SetBitmap (const char * color) ;

	private :

		DECLARE_EVENT_TABLE ()
} ;

#endif // _WX_LED_H_
