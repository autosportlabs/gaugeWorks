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
#ifndef __ANGULARMETER_H__
#define __ANGULARMETER_H__


#include "wx/wx.h"

#define MAX_TICKS 1000

class AngularMeter : public wxWindow
{
public:
    AngularMeter(wxWindow *parent,
				const wxWindowID id         = -1,
				const wxString&  label      = wxEmptyString,
				const wxPoint&   pos        = wxDefaultPosition,
				const wxSize&    size       = wxDefaultSize,
				const long int   style      = 0);

	virtual ~AngularMeter();
	void SetValue(double val);
	
	void SetWarningThreshold(double value);
	void SetAlertThreshold(double value);

	void AddMajorTick(double value);
	void AddMinorTick(double value);
	void ClearTicks();
	void AddTicks(double majorTickIncrement, double minorTickIncrement);
	
	void SetMajorTickDisplayDivisor(int divisor) { _majorTickDivisor = divisor; };
	
	void SetRange(double min, double max) { _rangeStart = min ; _rangeEnd = max ; } ;
	void SetAngle(double min, double max) { _angleStart = min ; _angleEnd = max ; } ;
	
	void SetLabel(wxString label){ _label = label; };
	void SetMajorTickColor(wxColour color){ _majorTickColor = color; };
	void SetMinorTickColor(wxColour color){ _minorTickColor = color; };
	void SetLabelTextColor(wxColour color){ _labelTextColor = color; };
	void SetValueTextColor(wxColour color){ _valueTextColor = color; };
	void SetLabelFont(wxFont &font) { _labelFont = font; };
	void SetValueFont(wxFont &font) { _valueFont = font; };
	void SetMajorTickTextColor(wxColour color){ _majorTickTextColor = color; };
	void SetValuePrecision(unsigned int precision);
	
	void SetDialColor(wxColour colour) { _dialColor = colour; };
	void SetAlertColor(wxColour colour) { _alertColor = colour; };
	void SetWarningColor(wxColour colour){ _warningColor = colour; };
	void SetNeedleColour(wxColour colour) { _needleColor = colour; };
	void SetBackColour(wxColour colour) { _backgroundColor = colour; };
	void SetBorderColour(wxColour colour) { _borderColor = colour; };
	void SetMajorTickFont(wxFont &font) { _majorTickFont = font; };
	void DrawCurrent(bool state) { _shouldDrawCurrent = state; };



private:
    
    double	GetScaledValue( double value );

	void    OnPaint(wxPaintEvent& event);
	void	OnSize(wxSizeEvent& event);
	void	OnEraseBackGround(wxEraseEvent& event) {};
	void	DrawTicks(wxDC &dc) ;
	void	DrawNeedle(wxDC &dc) ;
	void	DrawSectors(wxDC &dc) ;
	void 	DrawLabel(wxDC &dc);
	void	DrawValue(wxDC &dc);
	
	
	wxWindowID	 GetID() { return _windowId ; } ;

private:
	wxString	_label;
	wxWindowID 	_windowId;
	double		_rangeStart ;
	double		_rangeEnd ;
	int			_angleStart ;
	int			_angleEnd ;
	double		_scaledValue ;
	double		_realVal ;
	double		_scaledWarningValue;
	double		_realWarningValue;
	double		_scaledAlertValue;
	double		_realAlertValue;
	
	double		_PI ;
	bool		_shouldDrawCurrent ;
	wxBitmap 	*_memBitmap ;
	wxFont 		_majorTickFont;
	wxFont 		_labelFont;
	wxFont		_valueFont;
	
	wxColour 	_majorTickColor;
	wxColour	_minorTickColor;
	wxColour 	_labelTextColor;
	wxColour	_valueTextColor;
	wxColour 	_majorTickTextColor;
	wxColour 	_needleColor;
	wxColour 	_backgroundColor;
	wxColour	_warningColor;
	wxColour	_alertColor;
	wxColour 	_borderColor;
	wxColour	_dialColor;

	int			_majorTickDivisor;
	double		_majorTickRealValues[MAX_TICKS];
	double		_majorTickScaledValues[MAX_TICKS];
	int			_majorTicks;
	
	double		_minorTickRealValues[MAX_TICKS];
	double		_minorTickScaledValues[MAX_TICKS];
	int			_minorTicks;

    wxBitmap  *m_pPreviewBmp ;

	int			_currentWidth;
	int			_currentHeight;
	unsigned int m_valuePrecision;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

};
#endif // __ANGULARMETER_H__

