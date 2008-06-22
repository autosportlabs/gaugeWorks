/////////////////////////////////////////////////////////////////////////////
// The GaugeWorks project is released under the GNU Lesser Public License
// with modifications. See the License.txt file for the full text
// Copyright (c) 2007 Brent Picasso http://picasso.org
// Name:        AngularMeter.cpp
// Purpose:     Analog meter
// Portions of code adapted by:
// AngularMeter is based in part on the work of
// the KWIC project (http://www.koansoftware.com/kwic/index.htm).
/////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/image.h>
#include <math.h>
#include <wx/log.h>

#include "AngularMeter.h"
#include <wx/event.h>


//IMPLEMENT_DYNAMIC_CLASS(kwxAngularMeter, wxWindow)

BEGIN_EVENT_TABLE(AngularMeter,wxWindow)
	EVT_PAINT(AngularMeter::OnPaint)
	EVT_SIZE(AngularMeter::OnSize)
	
	EVT_ERASE_BACKGROUND(AngularMeter::OnEraseBackGround)
END_EVENT_TABLE()

AngularMeter::AngularMeter(wxWindow* parent,
		                   const wxWindowID id,
		                   const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size,
		                   const long int style)
	: wxWindow(parent, id, pos, size, 0)
{

  if (parent)
    SetBackgroundColour(parent->GetBackgroundColour());
  else
    SetBackgroundColour(*wxBLACK);      

    //SetSize(size);
    SetAutoLayout(TRUE);
	Refresh();

	_windowId = id;

	//valori di default
	_majorTickDivisor = 1;
	_majorTicks = 0;
	_minorTicks = 0;
	_scaledValue = 0;		//gradi
	_realVal = 0;
	_scaledWarningValue = 0;
	_realWarningValue = 0;
	_scaledAlertValue = 0;
	_realAlertValue = 0;
	_rangeStart = 0;
	_rangeEnd = 220;
	_angleStart = -70;
	_angleEnd = 250;
	_dialColor = wxColor(0x40,0x40,0x40);
	_backgroundColor = GetBackgroundColour() ;		//default sfondo applicazione
	_needleColor = *wxRED;	//indicatore
	_alertColor = wxColor(225, 0, 0);
	_warningColor = wxColor(254, 216, 1);
	
	_labelTextColor = *wxWHITE;
	_valueTextColor = *wxWHITE;
	_majorTickColor = *wxWHITE;
	_minorTickColor = *wxWHITE;
	_majorTickTextColor = *wxWHITE;
	_borderColor = GetBackgroundColour() ;
	_PI = 4.0 * atan(1.0);
	_majorTickFont = *wxSWISS_FONT;	//font
	_labelFont = *wxSWISS_FONT;
	_valueFont = *wxSWISS_FONT;
	
	_shouldDrawCurrent = true ;

	_currentWidth = size.GetWidth();
	_currentHeight = size.GetHeight();
	_memBitmap = new wxBitmap(_currentWidth, _currentHeight);
	m_pPreviewBmp = NULL;
	_label = "";


	/////////////// TODO : Test for BMP image loading /////////////////
/*

	m_pPreviewBmp = NULL ;

    wxBitmap bitmap( 300, 300 );
    wxImage image = bitmap.ConvertToImage();

    image.Destroy();

    if ( !image.LoadFile( "thumbnail.bmp", wxBITMAP_TYPE_BMP ) )
        wxLogError(wxT("Can't load BMP image"));
    else
        m_pPreviewBmp = new wxBitmap( image );
*/

}

AngularMeter::~AngularMeter()
{
	delete _memBitmap;
}

double AngularMeter::GetScaledValue( int value ){

	int deltarange = _rangeEnd - _rangeStart;
//	int rangezero = deltarange - m_nRangeStart;
	int deltaangle = _angleEnd - _angleStart;
	double coeff = (double)deltaangle / (double)deltarange;

	double scaledValue = (int)((double)(value - _rangeStart) * coeff); 
	return scaledValue;
}

void AngularMeter::SetValue(int val) 
{ 
	_scaledValue = GetScaledValue( val );
	_realVal = val;
	Refresh();
}

void AngularMeter::SetWarningThreshold(int val){

	_scaledWarningValue = GetScaledValue( val );
	_realWarningValue = val;
	Refresh();
}

void AngularMeter::SetAlertThreshold(int val){
	_scaledAlertValue = GetScaledValue( val );
	_realAlertValue = val;	
	Refresh();
}


void AngularMeter::OnSize(wxSizeEvent &event){

	//Layout();
	Refresh();
}

void AngularMeter::OnPaint(wxPaintEvent &event)
{
	
	wxPaintDC old_dc(this);
	
//	wxPaintDC dc((wxWindow *) this);
	
	int w,h ;
	GetClientSize(&w,&h);
	
	if (w != _currentWidth || h != _currentHeight){
		delete (_memBitmap);
		_currentWidth = w;
		_currentHeight = h;
		_memBitmap = new wxBitmap(_currentWidth, _currentHeight);
	}
	/////////////////
	// Create a memory DC
	wxMemoryDC dc;
	dc.SelectObject(*_memBitmap);

	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(_backgroundColor,wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(_backgroundColor,wxSOLID));
	dc.Clear();

	///////////////////
	//Rettangolo

	dc.SetPen(*wxThePenList->FindOrCreatePen(_borderColor, 1, wxSOLID));
	dc.DrawRectangle(0,0,w,h);
	
	//settori
	DrawSectors(dc) ;

	//tacche
	DrawTicks( dc );

	//indicatore lancetta

	DrawNeedle(dc);

	DrawLabel(dc);

	//testo valore
	if (_shouldDrawCurrent) DrawValue(dc);
	
	//blit into the real DC
	old_dc.Blit(0,0,_currentWidth,_currentHeight,&dc,0,0);
}

void AngularMeter::ClearTicks(){
	
	_majorTicks = 0;
	_minorTicks = 0;	
}

void AngularMeter::AddMajorTick(int value){

	double scaledValue = GetScaledValue(value);
	if (_majorTicks < MAX_TICKS - 1){
		_majorTickScaledValues[_majorTicks] = scaledValue;
		_majorTickRealValues[_majorTicks] = value;
		_majorTicks++;
	}
}

void AngularMeter::AddMinorTick(int value){
	
	double scaledValue = GetScaledValue(value);
	if (_minorTicks < MAX_TICKS -1){
		_minorTickScaledValues[_minorTicks] = scaledValue;
		_minorTickRealValues[_minorTicks] = value;
		_minorTicks++;	
	}
}


void AngularMeter::DrawNeedle(wxDC &dc) 
{
	//indicatore triangolare
	double dxi,dyi, val;
	wxPoint ppoint[6];
	int w, h ;

	GetClientSize(&w,&h);
	
	int hOffset = 0; 
	int wOffset = 0;
	if (w > h) wOffset = (w - h) / 2;
	if (h > w) hOffset = (h - w) / 2;
	//square out the size
	if (w > h) w=h;
	if (h > w) h=w;

	dc.SetPen(*wxThePenList->FindOrCreatePen(_needleColor, 1,wxSOLID));

	val = (_scaledValue + _angleStart) * _PI / 180; //radianti parametro angolo

	dyi = sin(val - 90) * 2; //coordinate base sinistra
	dxi = cos(val - 90) * 2;

	ppoint[0].x = (int)((w / 2) - dxi);	//base sinistra
	ppoint[0].y = (int)((h / 2) - dyi);

	dxi = cos(val) * ((h / 2) - 4); //coordinate punta indicatore
	dyi = sin(val) * ((h / 2) - 4);

	ppoint[2].x = (int)((w / 2) - dxi);	//punta
	ppoint[2].y = (int)((h / 2) - dyi);

	dxi = cos(val + 90) * 2; //coordinate base destra
	dyi = sin(val + 90) * 2;

	ppoint[4].x = (int)((w / 2) - dxi);	//base destra
	ppoint[4].y = (int)((h / 2) - dyi);

	ppoint[5].x = ppoint[0].x;	//ritorno base sinistra
	ppoint[5].y = ppoint[0].y;

//////////////////////////
	val = (_scaledValue + _angleStart + 1) * _PI / 180;

	dxi = cos(val) * ((h / 2) - 10); //coordinate medio destra
	dyi = sin(val) * ((h / 2) - 10);

	ppoint[3].x = (int)((w / 2) - dxi);	//base media destra
	ppoint[3].y = (int)((h / 2) - dyi);

	val = (_scaledValue + _angleStart - 1) * _PI / 180;

	dxi = cos(val) * ((h / 2) - 10); //coordinate medio sinistra
	dyi = sin(val) * ((h / 2) - 10);

	ppoint[1].x = (int)((w / 2) - dxi);	//base media sinistra
	ppoint[1].y = (int)((h / 2) - dyi);

/////////////////////////


	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(_needleColor,wxSOLID));

	dc.DrawPolygon(6, ppoint, wOffset, hOffset, wxODDEVEN_RULE);

	//cerchio indicatore
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(_needleColor,wxSOLID));
	dc.DrawCircle((w / 2) + wOffset, (h / 2) + hOffset, 10);
}



void AngularMeter::DrawSectors(wxDC &dc)
{
	int w,h ;

	
	GetClientSize(&w,&h);

	int hOffset = 0; 
	int wOffset = 0;
	if (w > h) wOffset = (w - h) / 2;
	if (h > w) hOffset = (h - w) / 2;

	//square out the size
	if (w > h) w=h;
	if (h > w) h=w;

	//Draw the dial
	dc.SetPen(*wxThePenList->FindOrCreatePen(_dialColor,1 , wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(_dialColor,wxSOLID));		
	dc.DrawEllipticArc(0 + wOffset, 0 + hOffset, w, h, 0,  360);

	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxBLACK, wxTRANSPARENT));

	dc.SetPen(*wxThePenList->FindOrCreatePen(wxColor(0x25,0x25,0x25), 2, wxSOLID));
	dc.DrawEllipticArc(0 + wOffset, 0 + hOffset, w, h, 45, -135);

	dc.SetPen(*wxThePenList->FindOrCreatePen(wxColor(0x55,0x55,0x55), 2, wxSOLID));
	dc.DrawEllipticArc(0 + wOffset, 0 + hOffset, w, h, -135, 45); 
	//Done drawing dial
	
	//Draw warning range
	
	if (_realWarningValue > 0){
		dc.SetPen(*wxThePenList->FindOrCreatePen(_warningColor,2, wxSOLID));
		dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(_warningColor, wxFDIAGONAL_HATCH));
		double warningEnd = _angleEnd - _scaledWarningValue;
		
		double warningStart = (_realAlertValue > 0 ? _angleEnd - _scaledAlertValue : _angleStart); 
		
		dc.DrawEllipticArc(0 + wOffset + 1, 0 + hOffset + 1, w - 2, h - 2, warningStart, warningEnd);
	}
	if (_realAlertValue > 0){
		dc.SetPen(*wxThePenList->FindOrCreatePen(_alertColor, 3, wxSOLID));
		dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(_alertColor, wxBDIAGONAL_HATCH));
		dc.DrawEllipticArc(0 + wOffset + 1, 0 + hOffset + 1, w - 2, h - 2, _angleStart, _angleEnd - _scaledAlertValue);
	}
}

void AngularMeter::DrawTicks(wxDC &dc)
{
	
	int w,h;
	GetClientSize(&w, &h);

	int hOffset = 0; 
	int wOffset = 0;
	if (w > h) wOffset = (w - h) / 2;
	if (h > w) hOffset = (h - w) / 2;

	//square out the size
	if (w > h) w=h;
	if (h > w) h=w;

	wxPen *pen = wxThePenList->FindOrCreatePen(_majorTickColor, 4, wxSOLID); 
	pen->SetCap(wxCAP_BUTT);
	dc.SetPen(*pen);
	
	int majorTickLineLength = w / 20;
	dc.SetFont(_majorTickFont);
	dc.SetTextForeground(_majorTickTextColor);
	
	for (int i = 0; i < _majorTicks; i++){
	
		double val = ( (_majorTickScaledValues[i] + _angleStart) * _PI) / 180;
		
		double dx = cos(val) * ((h/2) - 1 );	//punto sul cerchio
		double dy = sin(val) * ((h/2) - 1 );

		double tx = cos(val) * ((h / 2) - majorTickLineLength);	//punto nel cerchio
		double ty = sin(val) * ((h / 2) - majorTickLineLength);

		dc.DrawLine((w / 2) - (int)tx + wOffset, (h / 2) - (int)ty + hOffset, (w / 2) - (int)dx + wOffset, (h / 2) - (int)dy + hOffset);
		
		wxString s;		
		s.Printf("%d", _majorTickRealValues[i] / _majorTickDivisor);

		int tw,th;
		dc.GetTextExtent(s, &tw, &th);

		int textX = (int)(cos(val) * ((h / 2) - (majorTickLineLength * 2)));	//punto testo
		int textY = (int)(sin(val) * ((h / 2) - (majorTickLineLength * 2)));
		
		dc.DrawText(s, (w/2) - textX - (tw /2) + wOffset, (h/2) - textY - (th / 2) + hOffset);
		
	}
	
	dc.SetPen(*wxThePenList->FindOrCreatePen(_minorTickColor, 1, wxSOLID));
	int minorTickLineLength = w / 25;
	for (int i = 0; i < _minorTicks; i++){
		double val = ( (_minorTickScaledValues[i] + _angleStart) * _PI) / 180;
		
		double dx = cos(val) * ((h/2) - 1);	//punto sul cerchio
		double dy = sin(val) * ((h/2) - 1);

		double tx = cos(val) * ((h / 2) - minorTickLineLength);	//punto nel cerchio
		double ty = sin(val) * ((h / 2) - minorTickLineLength);

		dc.DrawLine((w / 2) - (int)tx + wOffset, (h / 2) - (int)ty + hOffset, (w / 2) - (int)dx + wOffset, (h / 2) - (int)dy + hOffset);
	}
	
}

void AngularMeter::DrawValue(wxDC &dc){
	
	dc.SetTextForeground(_valueTextColor);
	dc.SetFont(_valueFont);
	
	int w,h;
	GetClientSize(&w, &h);

	//draw label
	int vw,vh;
	wxString valuetext;
	valuetext.Printf("%d",_realVal);
	dc.GetTextExtent(valuetext, &vw, &vh);
	dc.DrawText(valuetext, (w / 2) - (vw / 2), (h / 2) - (int)(((float)vh * 1.3f)));	
}

void AngularMeter::DrawLabel(wxDC &dc){
	
	dc.SetTextForeground(_labelTextColor);
	dc.SetFont(_labelFont);
	
	int w,h;
	GetClientSize(&w, &h);
	
	//draw label
	int lw,lh;
	dc.GetTextExtent(_label, &lw, &lh);
	dc.DrawText(_label, (w / 2) - (lw / 2), (h / 2) + (int)(((float)lh * 0.7f)));
}
