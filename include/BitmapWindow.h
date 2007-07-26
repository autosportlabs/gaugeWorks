/////////////////////////////////////////////////////////////////////////////
// The GaugeWorks project is released under the GNU Lesser Public License
// with modifications. See the License.txt file for the full text
// Copyright (c) 2007 Brent Picasso http://picasso.org
// Name:        BitmapWindow
// Purpose:     wxLed implementation
/////////////////////////////////////////////////////////////////////////////
#ifndef BITMAPWINDOW_H_
#define BITMAPWINDOW_H_
#include "wx/wx.h"

class BitmapWindow : public wxWindow{

	public:
		BitmapWindow(){}
	    BitmapWindow( wxWindow *parent,wxWindowID id, char ** xpm_bitmap, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize );
	    ~BitmapWindow();
	    
	    void OnPaint( wxPaintEvent &event );
	    void OnSize( wxSizeEvent &event);
	    wxSize GetMinSize() const;
	private:
	    DECLARE_EVENT_TABLE()
		wxBitmap *_imageXpm;
};

#endif /*BITMAPWINDOW_H_*/
