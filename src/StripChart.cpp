
#include "StripChart.h"


#include <wx/arrimpl.cpp> 

//the data buffer
WX_DEFINE_OBJARRAY(ChartScales);
WX_DEFINE_OBJARRAY(LogItemTypes);
WX_DEFINE_OBJARRAY(LogItemBuffer);




BEGIN_EVENT_TABLE( StripChart, wxWindow )
	EVT_PAINT( StripChart::OnPaint )
	EVT_SIZE( StripChart::OnSize )
END_EVENT_TABLE()


StripChart::StripChart(		wxWindow *parent,
							wxWindowID id,
							const wxPoint &pos,
							const wxSize &size)
							: wxWindow(parent, id, pos, size)
							
{
	if (parent){
		SetBackgroundColour(parent->GetBackgroundColour());
	}
	else{
		SetBackgroundColour(*wxBLACK);  
	}
	_dataBufferSize = DEFAULT_DATA_BUFFER_SIZE;
	_currentWidth = size.GetWidth();
	_currentHeight = size.GetHeight();
	_memBitmap = new wxBitmap(_currentWidth, _currentHeight);
}


StripChart::~StripChart(){
	delete (_memBitmap);
}


int StripChart::AddScale(double minValue, double maxValue, wxString label){
	_chartScales.Add(new ChartScale(minValue, maxValue, label));
	return _chartScales.size() - 1;
}

int StripChart::AddLogItemType(int scaleId, wxColor lineColor, wxString label){
	_logItemTypes.Add(new LogItemType(scaleId, lineColor, label));
	return _logItemTypes.size() - 1;	
}

void StripChart::LogData(StripChartLogItem *values){
	_dataBuffer.Add(values);
	if (_dataBuffer.size() > _dataBufferSize){
		_dataBuffer.RemoveAt(0);
	}
	Refresh();
}


void StripChart::OnSize(wxSizeEvent &event){
	Refresh();
}


void StripChart::OnPaint(wxPaintEvent &event){

	wxPaintDC old_dc(this);
	
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

	wxColor backColor = GetBackgroundColour();
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(backColor,wxSOLID));
	dc.Clear();
	
	DrawGrid(dc);

	unsigned int dataBufferSize = _dataBuffer.size();
	unsigned int itemTypeSize = _logItemTypes.size();
	
	for (unsigned int typeId = 0; typeId < itemTypeSize; typeId++){
		
		LogItemType itemType = _logItemTypes[typeId];
		ChartScale scale = _chartScales[itemType.scaleId];
		
		double minValue = scale.minValue;
		double maxValue = scale.maxValue;
		
		int currentX = w - dataBufferSize;
		
		dc.SetPen(*wxThePenList->FindOrCreatePen(itemType.lineColor, 2, wxSOLID));
		
		int lastX = currentX;
		int lastY;
		if (dataBufferSize > 0){
			StripChartLogItem logItem = _dataBuffer[0];
			double loggedValue = logItem[typeId];
			double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);
			lastY = h - (int)(((double)h) * percentageOfMax);
		}
		for (unsigned int i = 1; i < dataBufferSize; i++){
			StripChartLogItem logItem = _dataBuffer[i];
			double loggedValue = logItem[typeId];
			
			double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);
			
			int y = h - (int)(((double)h) * percentageOfMax);
			
			dc.DrawLine(lastX, lastY, currentX, y);
			lastX = currentX;
			lastY = y;
			currentX++;		
		}
	}

	//blit into the real DC
	old_dc.Blit(0,0,_currentWidth,_currentHeight,&dc,0,0);
	
}

void StripChart::DrawGrid(wxMemoryDC &dc){

	dc.SetPen(*wxThePenList->FindOrCreatePen(wxColor(40,40,40), 1, wxSOLID));
	
	int width = _currentWidth;
	int height = _currentHeight;
	
	for (int x = 0; x < width; x+=20){
		dc.DrawLine(x, 0, x, height);
	}
	
	float i = 0;
	while (i < 1){
		int y = (int)(((float)height) * i);
		dc.DrawLine(0, y, width, y);
		i = i + 0.1;				
	}	
}
