/////////////////////////////////////////////////////////////////////////////
// The GaugeWorks project is released under the GNU Lesser Public License
// with modifications. See the License.txt file for the full text
// Copyright (c) 2007 Brent Picasso http://picasso.org
// Name:        LCDWindow.cpp
// LCD Window is based in part on the work of
// the KWIC project (http://www.koansoftware.com/kwic/index.htm).
/////////////////////////////////////////////////////////////////////////////

//#include "kprec.h"//
#include <wx/wx.h>

#include "LCDWindow.h"



#define LCD_NUMBER_SEGMENTS 8



BEGIN_EVENT_TABLE( LCDDisplay, wxWindow )
	EVT_PAINT( LCDDisplay::OnPaint )
	EVT_SIZE( LCDDisplay::OnSize )
END_EVENT_TABLE()


LCDDisplay::LCDDisplay( wxWindow *parent, 
						const wxPoint& pos, 
						const wxSize& size ) 
: wxWindow( parent, -1, pos, size )
{
	mSegmentLen = 40;
	mSegmentWidth = 10;
	mSpace = 5;

	mNumberDigits = 6;

	mLightColour = wxColour( 0, 255, 0 );
	mGrayColour = wxColour( 0, 40, 0 );
	_labelTextColor = *wxWHITE;
	_labelFont = *wxSWISS_FONT;
	
	SetBackgroundColour( wxColour( 0, 0, 0 ) );

}


LCDDisplay::~LCDDisplay()
{

}


void LCDDisplay::OnPaint( wxPaintEvent &event )
{
	wxPaintDC dc( this );

	DrawLabel( dc );
	int dw = GetClientSize().GetWidth();
	int dh = GetClientSize().GetHeight();

	int bw = GetBitmapWidth();
	int bh = GetBitmapHeight();

	double xs = ( double ) dw / bw;
	double ys = ( double ) dh / bh;

	double as = xs > ys? ys : xs;

	dc.SetUserScale( as, as );
	dc.SetDeviceOrigin( ( ( dw - bw * as ) / 2 ), ( ( dh - bh * as ) / 2 ) );

	DoDrawing( &dc );
	dc.EndDrawing();
}


void LCDDisplay::OnSize( wxSizeEvent &event )
{
	Refresh();
}

void LCDDisplay::DrawLabel(wxDC &dc){
	
	dc.SetTextForeground(_labelTextColor);
	dc.SetFont(_labelFont);
	
	int w,h;
	GetClientSize(&w, &h);
	
	
	//draw label
	int lw,lh;
	dc.GetTextExtent(_label, &lw, &lh);
	
	dc.DrawText(_label, (w / 2) - (lw / 2), h - lh);
}

void LCDDisplay::DoDrawing( wxDC *dc )
{
	wxString buf;
	buf.Printf( "%s", mValue.c_str() );
	while( buf.Replace( "..", ". ." ) );

	char current;
	char next;
	int buflen = buf.Len();
	int ac = buflen - 1;
	int c = 0 ;

	while (c < mNumberDigits)	//numero cifre display
	{
		ac >= 0? current = buf.GetChar( ac ): current = ' ';
		ac >= 0 && ac < buflen - 1? next = buf.GetChar( ac + 1 ): next = ' ';

		if( current == '.' )
			ac-- ;
		else
		{

			wxDigitData *data = new wxDigitData;

			data->value = current;
			data->comma = false;

			if( next == '.' )
			{
				data->comma = true;
			}

			DrawDigit( dc, c, data  );
			ac--;
			c++ ;

			delete data;
		}
	}
}


void LCDDisplay::DrawDigit( wxDC *dc, int digit, wxDigitData *data )
{
	unsigned char dec = Decode( data->value );

	if( data->value == ':' )	//scrive :
		DrawTwoDots( dc, digit );
	else
	{
		for( int c = 0; c < LCD_NUMBER_SEGMENTS - 1; c++ )
		{
			DrawSegment( dc, digit, c, ( dec >> c ) & 1 );
		}

		DrawSegment( dc, digit, 7, data->comma );	//scrive comma
	}
}


void LCDDisplay::DrawTwoDots( wxDC *dc, int digit )
{
	int sl = mSegmentLen;
	int sw = mSegmentWidth;
//	int sp = mSpace;

	int x = DigitX( digit );
	int y = DigitY( digit );

	wxBrush brushOn( mLightColour, wxSOLID );

	x += ( sl / 2 ) - sw;
	y += ( sl / 2 ) - sw;

	dc->SetBrush( brushOn );
	dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );

	dc->DrawEllipse( x, y, 2 * sw, 2 * sw );

	y += sl;

	dc->DrawEllipse( x, y, 2 * sw, 2 * sw );
}


void LCDDisplay::DrawSegment( wxDC *dc, int digit, int segment, bool state )
{
	int sl = mSegmentLen;
	int sw = mSegmentWidth;
//	int sp = mSpace;

	int x = DigitX( digit );
	int y = DigitY( digit );

	wxBrush brushOn( mLightColour, wxSOLID );
	wxBrush brushOff( mGrayColour, wxSOLID );

	if( state )
		dc->SetBrush( brushOn );
	else
		dc->SetBrush( brushOff );

	dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );

	wxPoint points[4];
	wxPoint p6[6];

	switch( segment )
	{
		case 0:
			{
				points[0].x = x;
				points[0].y = y;
				points[1].x = x + sl;
				points[1].y = y;
				points[2].x = x + sl - sw;
				points[2].y = y + sw;
				points[3].x = x + sw;
				points[3].y = y + sw;
				break;
			}
		case 1:
			{
				points[0].x = x;
				points[0].y = y;
				points[1].x = x;
				points[1].y = y + sl;
				points[2].x = x + sw;
				points[2].y = y + sl - sw / 2;
				points[3].x = x + sw;
				points[3].y = y + sw;
				break;
			}
		case 2:
			{
				x += sl - sw;
				points[0].x = x;
				points[0].y = y + sw;
				points[1].x = x + sw;
				points[1].y = y;
				points[2].x = x + sw;
				points[2].y = y + sl;
				points[3].x = x;
				points[3].y = y + sl - sw / 2;
				break;
			}
		case 3:
			{
				y += sl;
				points[0].x = x;
				points[0].y = y;
				points[1].x = x;
				points[1].y = y + sl;
				points[2].x = x + sw;
				points[2].y = y + sl - sw;
				points[3].x = x + sw;
				points[3].y = y + sw - sw / 2;
				break;
			}
		case 4:
			{
				y += sl;
				x += sl - sw;
				points[0].x = x;
				points[0].y = y + sw / 2;
				points[1].x = x + sw;
				points[1].y = y;
				points[2].x = x + sw;
				points[2].y = y + sl;
				points[3].x = x;
				points[3].y = y + sl - sw;
				break;
			}
		case 5:
			{
				y += 2 * sl - sw;
				points[0].x = x + sw;
				points[0].y = y;
				points[1].x = x + sl - sw;
				points[1].y = y;
				points[2].x = x + sl;
				points[2].y = y + sw;
				points[3].x = x;
				points[3].y = y + sw;
				break;
			}
		case 6:
			{
				y += sl - sw / 2;

				p6[0].x = x;
				p6[0].y = y + sw / 2;
				p6[1].x = x + sw;
				p6[1].y = y;
				p6[2].x = x + sl - sw;
				p6[2].y = y;
				p6[3].x = x + sl;
				p6[3].y = y + sw / 2;
				p6[4].x = x + sl - sw;
				p6[4].y = y + sw;
				p6[5].x = x + sw;
				p6[5].y = y + sw;

				break ;
			}
		default:
			break;
	}

	if( segment < 6 )				//segmento normale
		dc->DrawPolygon( 4, points );
	else if ( segment == 6 )		//segmento centrale a 6 lati
		dc->DrawPolygon( 6, p6 );
	else							//comma
	{
		y += 2 * sl;
		x += sl;

		dc->DrawEllipse( x + 1, y - sw, sw, sw );
	}

}


// Protected functions that calculate sizes.
// Needed by OnPaint

int LCDDisplay::GetDigitWidth( void )
{
	return mSegmentLen + mSegmentWidth + mSpace;
}


int LCDDisplay::GetDigitHeight( void )
{
	return ( 2 * mSegmentLen ) + ( 2 * mSpace );
}


int LCDDisplay::GetBitmapWidth( void )
{
	return ( mNumberDigits * GetDigitWidth() ) + mSpace;
}


int LCDDisplay::GetBitmapHeight( void )
{
	return GetDigitHeight();
}


int LCDDisplay::DigitX( int digit )
{
	return GetBitmapWidth() - ( ( digit + 1 ) * GetDigitWidth() );
}


int LCDDisplay::DigitY( int digit )
{
	return mSpace;
}


// Public functions accessible by the user.

void LCDDisplay::SetNumberDigits( int ndigits )	//numero cifre
{
	mNumberDigits = ndigits;
	Refresh( false );
}


void LCDDisplay::SetValue( wxString value )
{
	if (value == mValue) return;
	
	mValue = value;
	Refresh( false );
}


wxString LCDDisplay::GetValue( void )
{
	return mValue;
}


int LCDDisplay::GetNumberDigits( void )
{
	return mNumberDigits;
}


void LCDDisplay::SetLightColour( wxColour c )
{
	mLightColour = c;
}


void LCDDisplay::SetGrayColour( wxColour c )
{
	mGrayColour = c;
}


wxColour LCDDisplay::GetLightColour( void )
{
	return mLightColour;
}


wxColour LCDDisplay::GetGrayColour( void )
{
	return mGrayColour;
}


int LCDDisplay::GetDigitsNeeded( wxString value )
{
	wxString tst = value;

	while( tst.Replace( ".", "" ) );

	return tst.Len();
}


// The decoder function. The heart of the kwxLCDDisplay.


//      ***0***
//     *       *
//     1       2
//     *       *
//      ***6***
//     *       *  
//     3       4
//     *       *
//      ***5***

// A 10
// B 11
// C 12
// D 13
// E 14
// F 15

//     8421 8421
//     -654 3210
//---------------------
// 0 : 0011.1111 = 0x3F
// 1 : 0001.0100 = 0x14
// 2 : 0110.1101 = 0x6D
// 3 : 0111.0101 = 0x75
// 4 : 0101.0110 = 0x56
// 5 : 0111.0011 = 0x73
// 6 : 0111.1011 = 0x7B
// 7 : 0001,0101 = 0x15
// 8 : 0111.1111 = 0x7F
// 9 : 0111.0111 = 0x77
//   : 0000.0000 = 0x00
// - : 0100.0000 = 0x40

// r : 0100.1000 = 0x48
// o : 0111.1000 = 0x78 
// ^ : 0100.0111 = 0x47 //simbolo gradi

// A : 0101.1111 = 0x2F
// B : 0111.1010 = 0x7A
// C : 0010.1011 = 0x2B
// D : 0111.1100 = 0x7C
// E : 0110.1011 = 0x6B
// F : 0100.1011 = 0x4B

// H : 0101.1110 = 0x5E
// L : 0010.1010 = 0x2A
// P : 0100.1111 = 0x4F
// U : 0011.1110 = 0x3E


unsigned char LCDDisplay::Decode( char c )
{
	unsigned char ret = 0;
	
	struct DecodedDisplay
	{
		char ch;
		unsigned char value;
	};

	DecodedDisplay dec[] = 
	{
		{ '0', 0x3F }, 
		{ '1', 0x14 },
		{ '2', 0x6D },
		{ '3', 0x75 },
		{ '4', 0x56 },
		{ '5', 0x73 },
		{ '6', 0x7B },
		{ '7', 0x15 },
		{ '8', 0x7F },
		{ '9', 0x77 },
		{ ' ', 0x00 },
		{ '-', 0x40 },
		{ 'r', 0x48 },
		{ 'o', 0x78 },
		{ '^', 0x47 },
		{ 'A', 0x5F },
		{ 'B', 0x7A },
		{ 'C', 0x2B },
		{ 'D', 0x7C },
		{ 'E', 0x6B },
		{ 'F', 0x4B },
		{ 'H', 0x5E },
		{ 0, 0 }
	};

	for( int d = 0; dec[d].ch != 0; d++ )
	{
		if( dec[d].ch == c )
		{
			ret = dec[d].value;
			break;
		}
	}

	return ret;
}
