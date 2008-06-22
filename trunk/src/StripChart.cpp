/////////////////////////////////////////////////////////////////////////////
// The GaugeWorks project is released under the GNU Lesser Public License
// with modifications. See the License.txt file for the full text
// Copyright (c) 2007 Brent Picasso http://picasso.org
// Name:        StripChart
// Purpose:     wxLed implementation
/////////////////////////////////////////////////////////////////////////////

#include "StripChart.h"


#include <wx/arrimpl.cpp> 

#define UNITS_LABEL_SPACING 	2
#define GRID_SIZE 				20
#define DEFAULT_ZOOM 			100
#define MIN_ZOOM				25
#define DEFAULT_OFFSET_SECONDS	0

//the data buffer
WX_DEFINE_OBJARRAY(ChartScales);
WX_DEFINE_OBJARRAY(LogItemBuffer);




BEGIN_EVENT_TABLE( StripChart, wxWindow )
	EVT_PAINT( StripChart::OnPaint )
	EVT_SIZE( StripChart::OnSize )
	EVT_MOTION(StripChart::OnMouseMove)
	EVT_ENTER_WINDOW(StripChart::OnMouseEnter)
	EVT_LEAVE_WINDOW(StripChart::OnMouseExit)
    EVT_ERASE_BACKGROUND(StripChart::OnEraseBackground)	
END_EVENT_TABLE()


StripChart::StripChart(): wxWindow(), 
	_zoomPercentage(DEFAULT_ZOOM),
	_showScale(true),
	_timespanMode(TIMESPAN_FROM_NOW),
	_offsetFromEndSeconds(DEFAULT_OFFSET_SECONDS)
{}

StripChart::StripChart(		wxWindow *parent,
							wxWindowID id,
							const wxPoint &pos,
							const wxSize &size)
							: wxWindow(parent, id, pos, size),
								_zoomPercentage(DEFAULT_ZOOM),
								_showScale(true),
								_timespanMode(TIMESPAN_FROM_NOW),
								_offsetFromEndSeconds(DEFAULT_OFFSET_SECONDS),
								_currentDataBufferUBound(-1)
							
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

void StripChart::OnMouseEnter(wxMouseEvent &event){

	_showData = true;
	_mouseX = event.GetX();
	_mouseY = event.GetY();
	Refresh();
}

void StripChart::OnMouseMove(wxMouseEvent &event){
	
	_mouseX = event.GetX();
	_mouseY = event.GetY();	
	Refresh();
}

void StripChart::OnMouseExit(wxMouseEvent &event){
	
	_showData = false;
	Refresh();
}

void StripChart::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

int StripChart::GetZoom(){
	return _zoomPercentage;
}

void StripChart::SetZoom(int zoomPercentage){
	_zoomPercentage = zoomPercentage >= MIN_ZOOM ? zoomPercentage : MIN_ZOOM;
	Refresh();
}
	
void StripChart::SetTimespanMode(int mode){
	_timespanMode = mode;
}

int StripChart::GetTimespanMode(){
	return _timespanMode;
}

int StripChart::GetOffsetFromEndSeconds(){
	return _offsetFromEndSeconds;
}
void StripChart::SetOffsetFromEndSeconds(int offsetSeconds){
	_offsetFromEndSeconds = offsetSeconds;
	UpdateCurrentDataBufferUBound();
	Refresh();
}

void StripChart::SetLogBufferSize(int size){
	_dataBufferSize = size;
}

int StripChart::GetLogBufferSize(){
	return _dataBufferSize;
}

int GetLogBufferSize();


void StripChart::ClearScales(){
	_chartScales.Clear();
	ClearLogItemTypes();	
}

int StripChart::AddScale(ChartScale *scale){
	_chartScales.Add(scale);
	return _chartScales.size() - 1;
}

ChartScale * StripChart::GetScale(int id){
	return &_chartScales[id];	
}

int StripChart::AddLogItemType(LogItemType *logItemType){
	_logItemTypes[logItemType->typeKey] = logItemType;
	
	//default everything to minvalue
	ChartScale *scale = &_chartScales[logItemType->scaleId];
	int minValue = (int)scale->minValue;
	
	//pad the existing log data
	int size = _dataBuffer.size();
	for (int i = 0; i < size; i++){
		StripChartLogItem *logItem = &_dataBuffer[i];
		(*logItem)[logItemType->typeKey] = minValue;	
	}
	return _logItemTypes.size() - 1;
}


void StripChart::ClearLogItemTypes(){
	_logItemTypes.clear();
	ClearLog();
}


void StripChart::RemoveLogItemType(wxString typeKey){

	int size = _dataBuffer.size();
	
	//purge the existing log items
	for (int i = 0; i < size; i++){
		StripChartLogItem *logItem = &_dataBuffer[i];
		logItem->erase(typeKey);
	}
	//and remove the log item type
	_logItemTypes.erase(typeKey);	
	
}

LogItemType* StripChart::GetLogItemType(wxString typeKey){
	return _logItemTypes[typeKey];	
}

void StripChart::LogData(StripChartLogItem *values){
	_dataBuffer.Add(values);
	if (_dataBuffer.size() > _dataBufferSize){
		_dataBuffer.RemoveAt(0);
	}
	UpdateCurrentDataBufferUBound();
	Refresh();
}

void StripChart::UpdateCurrentDataBufferUBound(){
	
	int dataBufferSize = _dataBuffer.size();
	if (dataBufferSize > 0){
		wxDateTime lastTimestamp = _dataBuffer[dataBufferSize - 1].GetTimestamp();
		for (int i = dataBufferSize - 1; i >=0;i--){
			wxDateTime timestamp = _dataBuffer[i].GetTimestamp();
			wxTimeSpan span = (lastTimestamp - timestamp);
			if (span.GetSeconds().ToLong() >= _offsetFromEndSeconds){
				_currentDataBufferUBound = i;
				return;
			}
		}
		_currentDataBufferUBound = 0;
	}
	else{
		_currentDataBufferUBound = -1;
	}
}

void StripChart::ClearLog(){
	_dataBuffer.Clear();
	UpdateCurrentDataBufferUBound();
	Refresh();	
}

void StripChart::ShowScale(bool showScale){
	_showScale = showScale;
}

bool StripChart::GetShowScale(){
	return _showScale;
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
	if (_showScale) DrawScale(dc);

	unsigned int dataBufferSize = _dataBuffer.size();
	
	
	float zoomFactor = (float)_zoomPercentage / 100;
	
	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxLIGHT_GREY, 1, wxSHORT_DASH));
	
	int dataBufferUBound = _currentDataBufferUBound;
	
	float currentX = (float)w;
	for (int i = dataBufferUBound; i >= 0; i--){		
		if (_dataBuffer[i].IsMarked()){
			dc.DrawLine((int)currentX,0,(int)currentX,h);					
		}	
		currentX -= zoomFactor;
	}
	
	for (LogItemTypes::iterator it = _logItemTypes.begin(); it != _logItemTypes.end(); ++it){

		wxString key = it->first;
		
		LogItemType *itemType = it->second;
		
		ChartScale scale = _chartScales[itemType->scaleId];
		
		double minValue = scale.minValue;
		double maxValue = scale.maxValue;
		
		currentX = (float)w;
		
		dc.SetPen(*wxThePenList->FindOrCreatePen(itemType->lineColor, 1, wxSOLID));
		
		int lastX = (int)currentX;
		int lastY;
		
		if (dataBufferSize > 0){
			
			StripChartLogItem logItem = _dataBuffer[dataBufferUBound];
			double loggedValue = logItem[key];
			double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);
			lastY = h - (int)(((double)h) * percentageOfMax);
			
			for (int i = dataBufferUBound; i >= 0; i--){
				StripChartLogItem logItem = _dataBuffer[i];
				double loggedValue = logItem[key];
				
				double percentageOfMax = (loggedValue - minValue) / (maxValue - minValue);
				
				int y = h - (int)(((double)h) * percentageOfMax);
				
				dc.DrawLine(lastX, lastY, (int)currentX, y);
				lastX = (int)currentX;
				lastY = y;
				currentX -= zoomFactor;		
			}
		}
	}

	if (_showData) DrawCurrentValues(dc);
	//blit into the real DC
	old_dc.Blit(0,0,_currentWidth,_currentHeight,&dc,0,0);
	
}

void StripChart::DrawCurrentValues(wxMemoryDC &dc){
	
	//see if data was logged for the mouse location
	int dataBufferSize = _dataBuffer.size();
	float zoomFactor = (float)_zoomPercentage / 100.0;
	int dataBufferIndex = dataBufferSize - (int)(((float)(_currentWidth - _mouseX)) / zoomFactor) - 1;
	//adjust for uboundoffset
	dataBufferIndex-=((dataBufferSize - 1) -_currentDataBufferUBound);
	if (dataBufferIndex < 0 || dataBufferIndex >= dataBufferSize) return;
	
	
	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxWHITE, 1, wxSOLID));
	dc.DrawLine(_mouseX, 0, _mouseX, _currentHeight);

	int currentOffset = 0;
	StripChartLogItem logItem = _dataBuffer[dataBufferIndex];
	
	wxDateTime timestamp = logItem.GetTimestamp();
	
	wxDateTime fromTime;
	switch(_timespanMode){
		case TIMESPAN_FROM_LAST_LOG_ENTRY:
		{
			StripChartLogItem lastLogItem = _dataBuffer[dataBufferSize - 1];
			fromTime = lastLogItem.GetTimestamp();
			break;
		}
		case TIMESPAN_FROM_NOW:
			fromTime = wxDateTime::UNow();
			break;
	}
	wxTimeSpan span =  (fromTime - timestamp);
	wxString timeString = wxString::Format("%d seconds back",(int)span.GetSeconds().ToLong());
	
	
	wxFont labelFont = GetFont();
	int labelWidth,labelHeight,descent,externalLeading;
	dc.GetTextExtent(timeString, &labelHeight, &labelWidth, &descent, &externalLeading, &labelFont);
	dc.SetTextForeground(*wxWHITE);
	
	dc.DrawRotatedText(timeString, _mouseX - labelWidth, _mouseY,90);
	
	for (LogItemTypes::iterator it = _logItemTypes.begin(); it != _logItemTypes.end(); ++it){

		wxString key = it->first;		
		LogItemType *logItemType = it->second;
		ChartScale *scale = &_chartScales[logItemType->scaleId];
		
		dc.SetTextForeground(logItemType->lineColor);

		int value = logItem[logItemType->typeKey];
		wxString valueString = logItemType->typeLabel + ": " + wxString::Format("%d",value) + " " +scale->scaleLabel;

		dc.GetTextExtent(valueString, &labelHeight, &labelWidth, &descent, &externalLeading, &labelFont);
		
		currentOffset += labelWidth;
		dc.DrawRotatedText(valueString,_mouseX - labelHeight - 15, _mouseY - currentOffset,0);
	}
}

void StripChart::DrawScale(wxMemoryDC &dc){
		
	int leftOrientationEdge = 0;
	int rightOrientationEdge = _currentWidth - 1;
	
	
	wxFont labelFont = GetFont();

	for (LogItemTypes::iterator it = _logItemTypes.begin(); it != _logItemTypes.end(); ++it){

		wxString key = it->first;		
		LogItemType *logItemType = it->second;
		ChartScale *scale = &_chartScales[logItemType->scaleId];
		
		ChartScale::UNITS_DISPLAY_ORIENTATION orientation = scale->displayOrientation;
		double stepInterval = scale->stepInterval;
		double minValue = scale->minValue;
		double maxValue = scale->maxValue;
		
		dc.SetPen(*wxThePenList->FindOrCreatePen(logItemType->lineColor, 1, wxSOLID));

		wxString chartLabel = logItemType->typeLabel + " ( " + scale->scaleLabel + " )";
		int labelWidth,labelHeight,descent,externalLeading;
		dc.GetTextExtent(chartLabel, &labelHeight, &labelWidth, &descent, &externalLeading, &labelFont);


		dc.SetTextForeground(logItemType->lineColor);
	
		int verticalMiddleOfChart = _currentHeight / 2;
		
		int tickFromX = 0;
		int tickToX = 0; 
		
		bool showLabels;
		showLabels = (_currentHeight > labelHeight * 2  );
		switch (orientation){
			case ChartScale::ORIENTATION_LEFT:
			{
				if (showLabels) dc.DrawRotatedText(chartLabel,leftOrientationEdge, verticalMiddleOfChart + (labelHeight / 2),90);
				leftOrientationEdge += labelWidth;
				tickFromX = leftOrientationEdge;
				leftOrientationEdge += 5;
				tickToX = leftOrientationEdge;
				dc.DrawLine(tickFromX, 0 , tickFromX, _currentHeight);
				break;
			}
			case ChartScale::ORIENTATION_RIGHT:
			{
				if (showLabels) dc.DrawRotatedText(chartLabel, rightOrientationEdge, verticalMiddleOfChart - (labelHeight / 2), 270);
				rightOrientationEdge -= labelWidth;
				tickToX = rightOrientationEdge;
				rightOrientationEdge -= 5;
				tickFromX = rightOrientationEdge;
				dc.DrawLine(tickToX, 0 , tickToX, _currentHeight);
				break;
			}	
		}
		
		
		bool labelOn = false;
		int tickLabelWidth,tickLabelHeight,tickDescent,tickExternalLeading;
		for (double tick = minValue; tick <=maxValue; tick = tick + stepInterval){
			
			double percentageOfMax = (tick - minValue) / (maxValue - minValue);
			int y = _currentHeight - (int)(((double)_currentHeight) * percentageOfMax);
			
			dc.DrawLine(tickFromX, y, tickToX, y);
			
			if (labelOn){
				wxString tickLabel;
				tickLabel.Printf("%d",(int)tick);
				dc.GetTextExtent(chartLabel, &tickLabelHeight, &tickLabelWidth, &tickDescent, &tickExternalLeading, &labelFont);
				if (showLabels){
					switch (orientation){
						case ChartScale::ORIENTATION_LEFT:
						{
							dc.DrawRotatedText(tickLabel, leftOrientationEdge, y + (tickLabelWidth / 2), 90);
							break;				
						}
						case ChartScale::ORIENTATION_RIGHT:
						{
							dc.DrawRotatedText(tickLabel, rightOrientationEdge, y - (tickLabelWidth / 2), 270);
							break;
						}	
					}
				}
			}
			labelOn = !labelOn;
		}
		switch (orientation){
			case ChartScale::ORIENTATION_LEFT:
			{
				leftOrientationEdge += (tickLabelWidth);
				break;	
			}
			case ChartScale::ORIENTATION_RIGHT:
			{
				rightOrientationEdge -= (tickLabelWidth);
				break;
			}
				
		}
	}
}

void StripChart::DrawGrid(wxMemoryDC &dc){

	dc.SetPen(*wxThePenList->FindOrCreatePen(wxColor(40,40,40), 1, wxSOLID));
	
	int width = _currentWidth;
	int height = _currentHeight;
	
	float zoomFactor = (float)_zoomPercentage / 100;
	
	int gridIncrement = (int)(GRID_SIZE * zoomFactor);
	
	for (int x = width; x >=0 ; x -= gridIncrement){
		dc.DrawLine(x, 0, x, height);
	}
	
	float i = 0;
	while (i < 1){
		int y = (int)(((float)height) * i);
		dc.DrawLine(0, y, width, y);
		i = i + 0.1;				
	}	
}
