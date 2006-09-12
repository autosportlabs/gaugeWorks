#ifndef STRIPCHART_H_
#define STRIPCHART_H_


#include <wx/wx.h>
#include <wx/dynarray.h>



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
			scaleLabel = rhs.scaleLabel;
			stepInterval = rhs.stepInterval;
			displayOrientation = rhs.displayOrientation;	
		}
		ChartScale & operator=(const ChartScale &rhs){
			minValue = rhs.minValue;
			maxValue = rhs.maxValue;
			scaleLabel = rhs.scaleLabel;
			stepInterval = rhs.stepInterval;
			displayOrientation = rhs.displayOrientation;
			
			return *this;	
		}
		

		
		UNITS_DISPLAY_ORIENTATION displayOrientation;
		double stepInterval;
		double minValue;
		double maxValue;
		wxString scaleLabel;
};


class LogItemType{
	
	public: 
		LogItemType(int id, wxColor color, wxString label)
			: scaleId(id), lineColor(color), typeLabel(label)
		{}
		LogItemType(const LogItemType &rhs){
			scaleId = rhs.scaleId;
			lineColor = rhs.lineColor;
			typeLabel = rhs.typeLabel;	
		}
		LogItemType& operator=(const LogItemType &rhs){
			scaleId = rhs.scaleId;
			lineColor = rhs.lineColor;
			typeLabel = rhs.typeLabel;
			return *this;
		}
		int scaleId;
		wxColor lineColor;
		wxString typeLabel;
};

WX_DEFINE_ARRAY(int, StripChartLogItem);

WX_DECLARE_OBJARRAY(ChartScale, ChartScales);
WX_DECLARE_OBJARRAY(LogItemType, LogItemTypes);
WX_DECLARE_OBJARRAY(StripChartLogItem, LogItemBuffer);



class StripChart : public wxWindow
{
	public:
		StripChart(wxWindow *parent,
			wxWindowID id = -1,
			const wxPoint &pos = wxDefaultPosition,
			const wxSize &size = wxDefaultSize);
	
		~StripChart();
		
		const static unsigned int DEFAULT_DATA_BUFFER_SIZE = 1000;
		
		void SetChartHistorySize(unsigned int chartHistorySize);
		
		int AddScale(ChartScale *scale);
		ChartScale *GetScale(int id);
		
		int AddLogItemType(LogItemType *logItemType);
		LogItemType *GetLogItemType(int id);
		
		void LogData(StripChartLogItem *values);
		
		DECLARE_EVENT_TABLE()
	
	protected:
		void OnPaint( wxPaintEvent &event );
		void OnSize( wxSizeEvent &event );
		
	private:
	
		void DrawGrid(wxMemoryDC &dc);
		void DrawUnits(wxMemoryDC &dc);
		
		ChartScales			_chartScales;
		LogItemBuffer		_dataBuffer;
		LogItemTypes		_logItemTypes;
		unsigned int		_dataBufferSize;
		
		int					_currentWidth;
		int					_currentHeight;
		wxColor				_backgroundColor;
		wxBitmap 			*_memBitmap ;
		
	
		
};

#endif /*STRIPCHART_H_*/