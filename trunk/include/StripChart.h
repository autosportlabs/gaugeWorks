/////////////////////////////////////////////////////////////////////////////
// The GaugeWorks project is released under the GNU Lesser Public License
// with modifications. See the License.txt file for the full text
// Copyright (c) 2007 Brent Picasso http://picasso.org
// Name:        StripChart
// Purpose:     wxLed implementation
/////////////////////////////////////////////////////////////////////////////
#ifndef STRIPCHART_H_
#define STRIPCHART_H_


#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/dynarray.h>
#include <wx/hashmap.h>



class ChartScale{

	public:
		enum UNITS_DISPLAY_ORIENTATION{
			ORIENTATION_LEFT,
			ORIENTATION_RIGHT
		};

		ChartScale(	double min,
					double max,
					double step,
					UNITS_DISPLAY_ORIENTATION orientation,
					wxString label)
			: 	minValue(min),
				maxValue(max),
				stepInterval(step),
				displayOrientation(orientation),
				scaleLabel(label)
		{}
		ChartScale(const ChartScale &rhs){
			minValue = rhs.minValue;
			maxValue = rhs.maxValue;
			stepInterval = rhs.stepInterval;
			displayOrientation = rhs.displayOrientation;
			scaleLabel = rhs.scaleLabel;
		}
		ChartScale & operator=(const ChartScale &rhs){
			minValue = rhs.minValue;
			maxValue = rhs.maxValue;
			scaleLabel = rhs.scaleLabel;
			stepInterval = rhs.stepInterval;
			displayOrientation = rhs.displayOrientation;

			return *this;
		}


		double minValue;
		double maxValue;
		double stepInterval;
		UNITS_DISPLAY_ORIENTATION displayOrientation;
		wxString scaleLabel;
};


class LogItemType{

	public:
		LogItemType()
			: scaleId(0),
				lineColor(wxColor(0,0,0)),
				typeLabel(""),
				typeKey("")
		{}
		LogItemType(int scaleId, wxColor color, wxString label, wxString key)
			: 	scaleId(scaleId),
				lineColor(color),
				typeLabel(label),
				typeKey(key)
		{}
		LogItemType(const LogItemType &rhs){
			scaleId = rhs.scaleId;
			lineColor = rhs.lineColor;
			typeLabel = rhs.typeLabel;
			typeKey = rhs.typeKey;
		}
		LogItemType& operator=(const LogItemType &rhs){
			scaleId = rhs.scaleId;
			lineColor = rhs.lineColor;
			typeLabel = rhs.typeLabel;
			typeKey = rhs.typeKey;
			return *this;
		}
		int 		scaleId;
		wxColor 	lineColor;
		wxString 	typeLabel;
		wxString 	typeKey;
};

WX_DECLARE_STRING_HASH_MAP(int, LogItem);

WX_DECLARE_OBJARRAY(ChartScale, ChartScales);
WX_DECLARE_STRING_HASH_MAP(LogItemType*, LogItemTypes);


class StripChartLogItem : public LogItem{

	public:

		StripChartLogItem(wxDateTime timestamp,size_type size = 10) : LogItem(size),_timestamp(timestamp),_mark(false){}

		StripChartLogItem(size_type size = 10) :
				LogItem(size),
				_timestamp(wxDateTime::UNow()),
				_mark(false)
				{}

		StripChartLogItem(const StripChartLogItem& logItem) :
				LogItem(logItem),
				_timestamp(logItem.GetTimestamp()),
				_mark(false)
				{}

		bool IsMarked(){ return _mark;}
		void SetMark(bool mark){ _mark = mark;}
		wxDateTime GetTimestamp() const { return _timestamp; }

		void SetTimestamp(wxDateTime timestamp){ _timestamp = timestamp;}

	private:
		wxDateTime 	_timestamp;
		bool		_mark;
};

WX_DECLARE_OBJARRAY(StripChartLogItem, LogItemBuffer);

class StripChart : public wxWindow
{
	public:
		StripChart();
		StripChart(wxWindow *parent,
			wxWindowID id = -1,
			const wxPoint &pos = wxDefaultPosition,
			const wxSize &size = wxDefaultSize);

		~StripChart();

		static const int TIMESPAN_FROM_NOW = 0;
		static const int TIMESPAN_FROM_LAST_LOG_ENTRY = 1;

		const static unsigned int DEFAULT_DATA_BUFFER_SIZE = 1000000;

		void SetChartHistorySize(unsigned int chartHistorySize);

		int AddScale(ChartScale *scale);
		ChartScale *GetScale(int id);
		void ClearScales();
		void OnEraseBackground(wxEraseEvent& event);
		int AddLogItemType(LogItemType *logItemType);
		LogItemType *GetLogItemType(wxString typeKey);
		void RemoveLogItemType(wxString typeKey);
		void ClearLogItemTypes();

		void LogData(StripChartLogItem *values);
		void ClearLog();

		void SetLogBufferSize(int size);
		int GetLogBufferSize();

		void SetZoom(int zoomPercentage);
		int GetZoom();

		void SetTimespanMode(int mode);
		int GetTimespanMode();

		bool GetShowScale();
		void ShowScale(bool showScale);

		int GetOffsetFromEndSeconds();
		void SetOffsetFromEndSeconds(int seconds);



		DECLARE_EVENT_TABLE()

	protected:
		void OnPaint( wxPaintEvent &event );
		void OnSize( wxSizeEvent &event );
		void OnMouseEnter(wxMouseEvent &event);
		void OnMouseExit(wxMouseEvent &event);
		void OnMouseMove(wxMouseEvent &event);

	private:

		void DrawGrid(wxMemoryDC &dc);
		void DrawScale(wxMemoryDC &dc);
		void DrawCurrentValues(wxMemoryDC &dc);
		void UpdateCurrentDataBufferUBound();

		ChartScales			_chartScales;
		LogItemBuffer		_dataBuffer;
		LogItemTypes		_logItemTypes;
		unsigned int		_dataBufferSize;

		int					_zoomPercentage;
		int					_currentWidth;
		int					_currentHeight;
		wxBitmap 			*_memBitmap;
		wxColor				_backgroundColor;
		bool				_showScale;

		bool				_showData;
		int					_mouseX;
		int					_mouseY;
		int					_timespanMode;
		int					_offsetFromEndSeconds;
		int					_currentDataBufferUBound;
};

#endif /*STRIPCHART_H_*/
