// Image Window
#include "BitmapWindow.h"

BEGIN_EVENT_TABLE(BitmapWindow,wxWindow)
	EVT_PAINT(BitmapWindow::OnPaint)
	EVT_SIZE(BitmapWindow::OnSize)
	
END_EVENT_TABLE()


BitmapWindow::BitmapWindow( 	wxWindow *parent, 
							wxWindowID id, 
							char ** xpm_bitmap, 
							const wxPoint &pos, 
							const wxSize &size )
: wxWindow(parent, id, pos, size, 0)							
{
	_imageXpm = new wxBitmap(xpm_bitmap);
}

BitmapWindow::~BitmapWindow(){
	delete (_imageXpm);	
}

void BitmapWindow::OnSize(wxSizeEvent &event){
	Refresh();
}

wxSize BitmapWindow::GetMinSize() const{
	return wxSize(_imageXpm->GetWidth(), _imageXpm->GetHeight());
}

void BitmapWindow::OnPaint(wxPaintEvent &event){
	
	int w,h ;
	GetClientSize(&w,&h);

    wxPaintDC dc( this );
    PrepareDC( dc );
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(),wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(),wxSOLID));
	dc.Clear();
	dc.DrawBitmap(*_imageXpm, w -_imageXpm->GetWidth(),0);	
}
